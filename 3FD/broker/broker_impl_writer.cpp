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

#include <future>
#include <sstream>

namespace _3fd {
namespace broker {

    using std::string;

    /// <summary>
    /// Initializes a new instance of the <see cref="QueueWriter"/> class.
    /// </summary>
    /// <param name="svcBrokerBackend">The broker backend to use.</param>
    /// <param name="connString">The backend connection string.</param>
    /// <param name="serviceURL">The URL of the service that reads the messages.</param>
    /// <param name="msgTypeSpec">The specification for creation of message type.
    /// Such type is createad in the backend at the first time a reader or writer
    /// for this queue is instantiated. Subsequent instantiations will not effectively
    /// alter the message type by simply using different values in this parameter.</param>
    QueueWriter::QueueWriter(Backend svcBrokerBackend,
                             const string &connString,
                             const string &serviceURL,
                             const MessageTypeSpec &msgTypeSpec)
    try :
        m_dbSession(nullptr),
        m_serviceURL(serviceURL)
    {
        CALL_STACK_TRACE;

        _ASSERTE(svcBrokerBackend == Backend::MsSqlServer);

        m_dbSession = dbg_new DatabaseSession(connString);

        // Create message type, contract, queue, service, message content data type and input stage table:
        nanodbc::just_execute(m_dbSession->GetConnection(),
            utils::TextUcs2::in(L'%', LR"(
                if not exists ( select * from sys.service_queues where name = N'%service/v1_0_0/Queue' )
                begin
                    create message type [%service/v1_0_0/Message] validation = %validation;
                    create contract [%service/v1_0_0/Contract] ([%service/v1_0_0/Message] sent by initiator);
                    create queue [%service/v1_0_0/Queue] with poison_message_handling (status = off);
                    create service [%service/v1_0_0] on queue [%service/v1_0_0/Queue] ([%service/v1_0_0/Contract]);
                end;

                if not exists ( select * from sys.service_queues where name = N'%service/v1_0_0/ResponseQueue' )
                begin
                    create queue [%service/v1_0_0/ResponseQueue];
                    create service [%service/v1_0_0/Sender] on queue [%service/v1_0_0/ResponseQueue];
                end;

                if not exists (
                    select * from sys.systypes
                        where name = N'%service/v1_0_0/Message/ContentType'
                )
                begin
                    create type [%service/v1_0_0/Message/ContentType] from varchar(%nbytes);
                end;

                if not exists (
                    select * from sys.tables
                        where name = N'%service/v1_0_0/InputStageTable'
                )
                begin
                    create table [%service/v1_0_0/InputStageTable] (content [%service/v1_0_0/Message/ContentType]);
                end;
            )")
            .Replace(L"service", utils::to_ucs2(serviceURL))
            .Replace(L"validation", ToString(msgTypeSpec.contentValidation))
            .Use(L"nbytes", msgTypeSpec.nBytes)
            .Emit()
        );

        // Create stored procedure to send messages to service queue:
        auto result = nanodbc::execute(m_dbSession->GetConnection(),
            utils::TextUcs2::in(L'%', L"select object_id(N'%service/v1_0_0/SendMessagesProc', N'P');")
                .Replace(L"service", utils::to_ucs2(serviceURL))
                .Emit()
        );

        if (!result.next())
        {
            nanodbc::just_execute(m_dbSession->GetConnection(), utils::TextUcs2::in(L'%', LR"(
                create procedure [%service/v1_0_0/SendMessagesProc] as
                begin try
                    begin transaction;

                        set nocount on;

                        declare @dialogHandle uniqueidentifier;

                        begin dialog @dialogHandle
                            from service [%service/v1_0_0/Sender]
                            to service '%service/v1_0_0'
                            on contract [%service/v1_0_0/Contract]
                            with encryption = off;

                        declare @msgContent [%service/v1_0_0/Message/ContentType];

                        declare cursorMsg cursor for (
                            select * from [%service/v1_0_0/InputStageTable]
                        );

                        open cursorMsg;
                        fetch next from cursorMsg into @msgContent;

                        while @@fetch_status = 0
                        begin
                            send on conversation @dialogHandle
                                message type [%service/v1_0_0/Message] (@msgContent);

                            fetch next from cursorMsg into @msgContent;
                        end;

                        close cursorMsg;
                        deallocate cursorMsg;
                        delete from [%service/v1_0_0/InputStageTable];

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

        // Create stored procedure to finish conversations in the initiator endpoint:
        result = nanodbc::execute(m_dbSession->GetConnection(),
            utils::TextUcs2::in(L'%', L"select object_id(N'%service/v1_0_0/FinishDialogsOnEndptInitProc', N'P');")
                .Replace(L"service", utils::to_ucs2(serviceURL))
                .Emit()
        );

        if (!result.next())
        {
            nanodbc::just_execute(m_dbSession->GetConnection(), utils::TextUcs2::in(L'%', LR"(
                create procedure [%service/v1_0_0/FinishDialogsOnEndptInitProc] as
                begin try
                    begin transaction;

                        set nocount on;

                        declare @ReceivedMessages table (
                            conversation_handle  uniqueidentifier
                            ,message_type_name   sysname
                        );

                        receive conversation_handle
                                ,message_type_name
                            from [%service/v1_0_0/ResponseQueue]
                            into @ReceivedMessages;
        
                        declare @dialogHandle  uniqueidentifier;
                        declare @msgTypeName   sysname;
        
                        declare cursorMsg
                            cursor forward_only read_only for
                                select conversation_handle
                                       ,message_type_name
                                    from @ReceivedMessages;

                        open cursorMsg;
                        fetch next from cursorMsg into @dialogHandle, @msgTypeName;

                        while @@fetch_status = 0
                        begin
                            if @msgTypeName = 'http://schemas.microsoft.com/SQL/ServiceBroker/EndDialog'
                                end conversation @dialogHandle;

                            fetch next from cursorMsg into @dialogHandle, @msgTypeName;
                        end;

                        close cursorMsg;
                        deallocate cursorMsg;

                    commit transaction;
                end try
                begin catch

                    rollback transaction;
                    throw;

                end catch;

                alter queue [%service/v1_0_0/ResponseQueue]
                    with activation (
                        status = on,
                        max_queue_readers = 1,
                        procedure_name = [%service/v1_0_0/FinishDialogsOnEndptInitProc],
                        execute as owner
                    );
            )")
            .Replace(L"service", utils::to_ucs2(serviceURL))
            .Emit());
        }

        std::ostringstream oss;
        oss << "Initialized successfully the writer for broker queue '" << serviceURL
            << "/v1_0_0/Queue' backed by " << ToString(svcBrokerBackend) << " via ODBC";

        core::Logger::Write(oss.str(), core::Logger::PRIO_INFORMATION);
    }
    catch (...)
    {
        delete m_dbSession;
        HandleException("creating writer for broker queue");
    }


    /// <summary>
    /// Helps synchronizing with an asynchronous write to a broker queue.
    /// </summary>
    /// <seealso cref="notcopiable" />
    class AsyncWriteImpl : public IAsyncWrite
    {
    private:

        AsyncTransactionImpl m_transaction;
        std::future<void> m_future;

    public:

        AsyncWriteImpl(const AsyncWriteImpl &) = delete;

        virtual ~AsyncWriteImpl() = default;

        /// <summary>
        /// Initializes a new instance of the <see cref="AsyncWriteImpl" /> class.
        /// </summary>
        /// <param name="dbSession">The database session.</param>
        /// <param name="serviceURL">The service URL.</param>
        /// <param name="messages">The messages to write.</param>
        AsyncWriteImpl(DatabaseSession &dbSession,
                       const string &serviceURL,
                       const std::vector<string> &messages)
            : m_transaction(dbSession, m_future)
        {
            CALL_STACK_TRACE;

            try
            {
                std::array<wchar_t, 256> buffer;
                size_t length = utils::SerializeTo(buffer,
                    L"insert into [", serviceURL, L"/v1_0_0/InputStageTable] (content) values (?);");

                auto stageInsertStatement =
                    std::make_shared<nanodbc::statement>(m_transaction.GetConnection(),
                                                         nanodbc::string(buffer.data(), length));

                stageInsertStatement->bind_strings(0, messages);

                const size_t batchSize = messages.size();

                length = utils::SerializeTo(buffer,
                                            L"exec [", serviceURL, L"/v1_0_0/SendMessagesProc];");

                auto stoProcStatement =
                    std::make_shared<nanodbc::statement>(m_transaction.GetConnection(),
                                                         nanodbc::string(buffer.data(), length));

                m_future = std::async(std::launch::async, [batchSize, stageInsertStatement, stoProcStatement]()
                {
                    nanodbc::just_execute(*stageInsertStatement, batchSize);
                    nanodbc::just_execute(*stoProcStatement);
                });
            }
            catch_and_handle_exception("launching asynchronous task to write into broker queue")
        }

        /// <inheritdoc/>
        bool TryWait(uint16_t timeout) override
        {
            CALL_STACK_TRACE;
            return m_transaction.TryWait(timeout);
        }

        /// <inheritdoc/>
        virtual void Rethrow() override
        {
            CALL_STACK_TRACE;

            try
            {
                _ASSERTE(m_future.valid()); // not supposed to be called twice!
                m_future.get();
            }
            catch_and_handle_exception("writing messages into broker queue")
        }

        /// <inheritdoc/>
        bool Rollback(uint16_t timeout) override
        {
            CALL_STACK_TRACE;
            return m_transaction.Rollback(timeout);
        }

        /// <inheritdoc/>
        bool Commit(uint16_t timeout) override
        {
            CALL_STACK_TRACE;
            return m_transaction.Commit(timeout);
        }

    };// end of AsyncWriteImpl class

    /// <summary>
    /// Asychonously writes the messages into the queue.
    /// </summary>
    /// <param name="messages">The messages to write.</param>
    /// <return>An object to help synchronizing with the asynchronous operation.</return>
    std::unique_ptr<IAsyncWrite> QueueWriter::WriteMessages(const std::vector<string> &messages)
    {
        CALL_STACK_TRACE;

        try
        {
            return std::unique_ptr<IAsyncWrite>(
                dbg_new AsyncWriteImpl(*m_dbSession, m_serviceURL, messages)
            );
        }
        catch_and_handle_exception("creating new task to write into broker queue")
    }

}// end of namespace broker
}// end of namespace _3fd
