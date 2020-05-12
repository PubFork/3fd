//
// Copyright (c) 2020 Part of 3FD project (https://github.com/faburaya/3fd)
// It is FREELY distributed by the author under the Microsoft Public License
// and the observance that it should only be used for the benefit of mankind.
//
#include "pch.h"
#include "broker_impl.h"
#include <3fd/core/callstacktracer.h>
#include <3fd/core/exceptions.h>
#include <3fd/core/logger.h>
#include <3fd/utils/utils_io.h>
#include <3fd/utils/utils_string.h>

#include <chrono>
#include <sstream>

namespace _3fd {
namespace broker {

    using std::string;

    /// <summary>
    /// Initializes a new instance of the <see cref="QueueReader"/> class.
    /// </summary>
    /// <param name="svcBrokerBackend">The broker backend to use.</param>
    /// <param name="connString">The backend connection string.</param>
    /// <param name="utils::to_ucs2(serviceURL)">The service URL.</param>
    /// <param name="msgTypeSpec">The specification for creation of message type.
    /// Such type is createad in the backend at the first time a reader or writer
    /// for this queue is instantiated. Subsequent instantiations will not effectively
    /// alter the message type by simply using different values in this parameter.</param>
    QueueReader::QueueReader(Backend svcBrokerBackend,
                             const string &connString,
                             const string &serviceURL,
                             const MessageTypeSpec &msgTypeSpec)
    try
        : m_dbSession(nullptr)
        , m_serviceURL(serviceURL)
    {
        CALL_STACK_TRACE;

        _ASSERTE(svcBrokerBackend == Backend::MsSqlServer);
        
        m_dbSession = dbg_new DatabaseSession(connString);

        // Create message type, contract, queue, service and message content data type:
        nanodbc::just_execute(m_dbSession->GetConnection(),
            utils::TextUcs2::in(L'%', LR"(
                if not exists ( select * from sys.service_queues where name = N'%service/v1_0_0/Queue' )
                begin
                    create message type [%service/v1_0_0/Message] validation = %validation;
                    create contract [%service/v1_0_0/Contract] ([%service/v1_0_0/Message] sent by initiator);
                    create queue [%service/v1_0_0/Queue] with poison_message_handling (status = off);
                    create service [%service/v1_0_0] on queue [%service/v1_0_0/Queue] ([%service/v1_0_0/Contract]);
                end;

                if not exists (
                    select * from sys.systypes
                        where name = N'%service/v1_0_0/Message/ContentType'
                )
                begin
                    create type [%service/v1_0_0/Message/ContentType] from varchar(%nbytes);
                end;
            )")
            .Replace(L"service", utils::to_ucs2(serviceURL))
            .Replace(L"validation", ToString(msgTypeSpec.contentValidation))
            .Use(L"nbytes", msgTypeSpec.nBytes)
            .Emit()
        );

        // Create stored procedure to read messages from queue:

        auto result = nanodbc::execute(m_dbSession->GetConnection(),
            utils::TextUcs2::in(L'%', L"select object_id(N'%service/v1_0_0/ReadMessagesProc', N'P');")
                .Replace(L"service", utils::to_ucs2(serviceURL))
                .Emit()
        );

        if (!result.next())
        {
            throw core::AppException<std::runtime_error>(
                "Could not check presence of stored procedure to read from broker queue!", serviceURL);
        }
        
        if (result.is_null(0))
        {
            nanodbc::just_execute(m_dbSession->GetConnection(), utils::TextUcs2::in(L'%', LR"(
                create procedure [%service/v1_0_0/ReadMessagesProc] (
                    @recvMsgCountLimit int
                    ,@recvTimeoutMilisecs int
                ) as
                begin try
                    begin transaction;

                        set nocount on;

                        declare @ReceivedMessages table (
                            queuing_order        bigint
                            ,conversation_handle uniqueidentifier
                            ,message_type_name   sysname
                            ,message_body        [%service/v1_0_0/Message/ContentType]
                        );

                        waitfor(
                            receive top (@recvMsgCountLimit)
                                    queuing_order
                                    ,conversation_handle
                                    ,message_type_name
                                    ,message_body
                                from [%service/v1_0_0/Queue]
                                into @ReceivedMessages
                        )
                        ,timeout @recvTimeoutMilisecs;
        
                        declare @RowsetOut        table (content [%service/v1_0_0/Message/ContentType]);
                        declare @prevDialogHandle uniqueidentifier;
                        declare @dialogHandle     uniqueidentifier;
                        declare @msgTypeName      sysname;
                        declare @msgContent       [%service/v1_0_0/Message/ContentType];

                        declare cursorMsg
                            cursor forward_only read_only
                            for select conversation_handle
                                        ,message_type_name
                                        ,message_body
                                from @ReceivedMessages
                                order by queuing_order;

                        open cursorMsg;
                        fetch next from cursorMsg into @dialogHandle, @msgTypeName, @msgContent;
        
                        while @@fetch_status = 0
                        begin
                            if @dialogHandle <> @prevDialogHandle and @prevDialogHandle is not null
                                end conversation @prevDialogHandle;

                            if @msgTypeName = '%service/v1_0_0/Message'
                                insert into @RowsetOut values (@msgContent);

                            else if @msgTypeName = 'http://schemas.microsoft.com/SQL/ServiceBroker/Error'
                                throw 50001, 'There was an error during conversation with service', 1;

                            else if @msgTypeName <> 'http://schemas.microsoft.com/SQL/ServiceBroker/EndDialog'
                                throw 50000, 'Message received in service broker queue had unexpected type', 1;
            
                            set @prevDialogHandle = @dialogHandle;
                            fetch next from cursorMsg into @dialogHandle, @msgTypeName, @msgContent;
                        end;

                        close cursorMsg;
                        deallocate cursorMsg;

                        save transaction doneReceiving;

                        set @dialogHandle = newid();

                        receive top (1)
                            @dialogHandle = conversation_handle
                            from [%service/v1_0_0/Queue];

                        rollback transaction doneReceiving;

                        if @dialogHandle <> @prevDialogHandle and @prevDialogHandle is not null
                            end conversation @prevDialogHandle;
            
                        select content from @RowsetOut;

                    commit transaction;
                end try
                begin catch

                    rollback transaction;
                    throw;

                end catch;
            )")
            .Replace(L"service", utils::to_ucs2(serviceURL))
            .Emit());
        }

        std::ostringstream oss;
        oss << "Initialized successfully the reader for broker queue '"
            << serviceURL << "/v1_0_0/Queue' backed by "
            << utils::to_utf8(ToString(svcBrokerBackend)) << " via ODBC";

        core::Logger::Write(oss.str(), core::Logger::PRIO_INFORMATION);
    }
    catch (...)
    {
        CALL_STACK_TRACE;
        delete m_dbSession;
        HandleException("creating reader for broker queue");
    }

    /// <summary>
    /// Controls retrieval of results from asynchronous read of queue.
    /// This implementation is NOT THREAD SAFE!
    /// </summary>
    class AsyncReadImpl : public IAsyncRead
    {
    private:

        AsyncTransactionImpl m_transaction;
        nanodbc::statement m_stoProcExecStmt;
        std::future<void> m_stoProcFuture;
        std::vector<string> m_messages;

    public:

        AsyncReadImpl(const AsyncReadImpl &) = delete;

        virtual ~AsyncReadImpl() = default;

        void CallbackStoredProcedure()
        {
            auto result = nanodbc::execute(m_stoProcExecStmt);

            m_messages.reserve(m_messages.size() + result.rows());

            while (result.next())
                m_messages.push_back(result.get<string>(0));
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="AsyncReadImpl" /> class.
        /// </summary>
        /// <param name="dbSession">The database session.</param>
        /// <param name="msgCountStepLimit">How many messages are to be
        /// retrieved at most at each asynchronous execution step.</param>
        /// <param name="msgRecvTimeout">The timeout (in ms) when the backend awaits for messages.</param>
        /// <param name="utils::to_ucs2(serviceURL)">The service URL.</param>
        AsyncReadImpl(DatabaseSession &dbSession,
                      uint16_t msgCountStepLimit,
                      uint16_t msgRecvTimeout,
                      const string &serviceURL)
            : m_transaction(dbSession, m_stoProcFuture)
        {
            CALL_STACK_TRACE;

            try
            {
                std::array<wchar_t, 256> buffer;
                size_t length = utils::SerializeTo(buffer,
                                                   L"exec [", serviceURL, L"/v1_0_0/ReadMessagesProc] ",
                                                   msgCountStepLimit, L", ", msgRecvTimeout, L";");

                m_stoProcExecStmt = nanodbc::statement(
                    dbSession.GetConnection(),
                    nanodbc::string(buffer.data(), length)
                );

                m_stoProcFuture = std::async(std::launch::async,
                                             &AsyncReadImpl::CallbackStoredProcedure,
                                             this);
            }
            catch_and_handle_exception("setting up to read messages asynchronously from broker queue")
        }

        /// <inheritdoc/>
        bool TryWait(uint16_t timeout) override
        {
            CALL_STACK_TRACE;
            return m_transaction.TryWait(timeout);
        }

        /// <inheritdoc/>
        void StepNext() override
        {
            CALL_STACK_TRACE;

            try
            {
                if (!TryWait(0))
                {
                    throw core::AppException<std::logic_error>("Could not step into execution because the "
                                                               "last asynchronous operation is pending!");
                }

                m_stoProcFuture = std::async(std::launch::async,
                                             &AsyncReadImpl::CallbackStoredProcedure,
                                             this);
            }
            catch_and_handle_exception("stepping into execution of broker queue read procedure")
        }

        /// <inheritdoc/>
        virtual std::vector<string> GetStepResult(uint16_t timeout) override
        {
            CALL_STACK_TRACE;

            try
            {
                if (!TryWait(timeout))
                    return std::vector<string>();

                if (m_stoProcFuture.valid())
                    m_stoProcFuture.get(); // transport exception from exec thread

                std::vector<string> result;
                result.swap(m_messages);
                return result;
            }
            catch_and_handle_exception("retrieving messages read from broker queue")
        }

        /// <inheritdoc/>
        bool Rollback(uint16_t timeout) override
        {
            CALL_STACK_TRACE;
            bool status = m_transaction.Rollback(timeout);
            m_messages.clear();
            return status;
        }

        /// <inheritdoc/>
        bool Commit(uint16_t timeout) override
        {
            CALL_STACK_TRACE;
            return m_transaction.Commit(timeout);
        }

    };// end of class AsyncReadImpl

    /// <summary>
    /// Asynchronously reads the messages from the queue into a vector.
    /// </summary>
    /// <param name="msgCountStepLimit">How many messages are to be retrieved
    /// at most for each asynchronous execution step.</param>
    /// <param name="msgRecvTimeout">The timeout (in ms) when the backend awaits for messages.</param>
    /// <return>An object to control the result of the asynchronous operation.</return>
    std::unique_ptr<IAsyncRead> QueueReader::ReadMessages(uint16_t msgCountStepLimit, uint16_t msgRecvTimeout)
    {
        CALL_STACK_TRACE;

        try
        {
            return std::unique_ptr<IAsyncRead>(
                dbg_new AsyncReadImpl(*m_dbSession,
                                      msgCountStepLimit,
                                      msgRecvTimeout,
                                      m_serviceURL)
            );
        }
        catch (core::IAppException &)
        {
            throw; // just forward exceptions raised for errors known to have been already handled
        }
        catch (std::exception &ex)
        {
            std::ostringstream oss;
            oss << "Generic failure prevented reading broker queue: " << ex.what();
            throw core::AppException<std::runtime_error>(oss.str());
        }
    }

}// end of namespace broker
}// end of namespace _3fd
