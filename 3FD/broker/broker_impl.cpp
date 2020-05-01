#include "pch.h"
#include "broker_impl.h"
#include <3fd/core/configuration.h>
#include <3fd/core/logger.h>
#include <3fd/utils/utils_io.h>

namespace _3fd {
namespace broker {

    /// <summary>
    /// Handles any exception caught within a catch block during execution of this module.
    /// </summary>
    void HandleException(const char *when)
    {
        std::ostringstream oss;

        try
        {
            throw;
        }
        catch (core::IAppException &)
        {
            throw; // just forward already handled error
        }
        catch (nanodbc::programming_error &ex)
        {
            oss << "ODBC SQL error when " << when << ": " << ex.what();
            throw core::AppException<std::logic_error>(oss.str());
        }
        catch (nanodbc::database_error &ex)
        {
            oss << "ODBC database error when " << when << ": " << ex.what();
            throw core::AppException<std::runtime_error>(oss.str());
        }
        catch (std::future_error &ex)
        {
            oss << "Synchronization error when " << when << ": " << ex.what();
            throw core::AppException<std::runtime_error>(oss.str(),
                                                         core::StdLibExt::GetDetailsFromFutureError(ex));
        }
        catch (std::system_error &ex)
        {
            oss << "System error when " << when << ": " << ex.what();
            throw core::AppException<std::runtime_error>(oss.str(),
                                                         core::StdLibExt::GetDetailsFromSystemError(ex));
        }
        catch (std::exception &ex)
        {
            oss << "Generic error when " << when << ": " << ex.what();
            throw core::AppException<std::runtime_error>(oss.str());
        }
        catch (...)
        {
            oss << "Unexpected exception when " << when;
            throw core::AppException<std::runtime_error>(oss.str());
        }
    }

    /// <summary>
    /// Log any exception caught within a catch block during execution of the module.
    /// </summary>
    void LogException(const char *when) noexcept
    {
        try
        {
            HandleException(when);
        }
        catch (core::IAppException &appEx)
        {
            core::Logger::Write(appEx, core::Logger::PRIO_ERROR);
        }
    }

    /// <summary>
    /// Converts an enumerated option of backend to a label.
    /// </summary>
    /// <param name="backend">The backend option.</param>
    /// <returns>A label for the backend.</returns>
    const char *ToString(Backend backend)
    {
        switch (backend)
        {
        case Backend::MsSqlServer:
            return "Microsoft SQL Server";
        default:
            _ASSERTE(false);
            return "UNKNOWN";
        }
    }

    /// <summary>
    /// Converts an enumerated type of message content validation to a label.
    /// </summary>
    /// <param name="msgContentValidation">What validation to use in message content.</param>
    /// <returns>A label as expected by T-SQL.</returns>
    const char *ToString(MessageContentValidation msgContentValidation)
    {
        switch (msgContentValidation)
        {
        case MessageContentValidation::None:
            return "NONE";
        case MessageContentValidation::WellFormedXml:
            return "WELL_FORMED_XML";
        default:
            _ASSERTE(false);
            return "UNKNOWN";
        }
    }

    /////////////////////////////////
    // DatabaseSession class
    /////////////////////////////////

    /// <summary>
    /// Gets an ODBC database connection.
    /// Fails with an exception only after timeout and a number
    /// of retries specified in framework configuration file.
    /// </summary>
    /// <param name="connString">The connection string.</param>
    /// <returns>The succesfully established connection.</returns>
    DatabaseSession::DatabaseSession(const string &connString)
        : m_connectionString(connString)
    {
        uint32_t retryCount(0);

        while (true)
        {
            try
            {
                static const auto timeout =
                    core::AppConfig::GetSettings().framework.broker.dbConnTimeoutSecs;

                m_dbConnection = nanodbc::connection(connString, timeout);
            }
            catch (nanodbc::database_error &)
            {
                static const auto maxRetries =
                    core::AppConfig::GetSettings().framework.broker.dbConnMaxRetries;

                if (retryCount >= maxRetries)
                    throw;
            }

            ++retryCount;
        }
    }

    /// <summary>
    /// Checks the state of a database connection.
    /// Fails with an exception only if disconnected and after a
    /// number of retries specified in framework configuration file.
    /// </summary>
    /// <param name="connString">The connection string.</param>
    /// <returns>A refence to the checked connection.</returns>
    nanodbc::connection &DatabaseSession::GetConnection()
    {
        if (m_dbConnection.connected())
            return m_dbConnection;

        static const auto maxRetries =
            core::AppConfig::GetSettings().framework.broker.dbConnMaxRetries;

        uint32_t retryCount(1);

        if (retryCount <= maxRetries)
        {
            std::array<char, 512> bufErrMsg;

            utils::SerializeTo(bufErrMsg,
                "Lost connection to broker queue in database '(",
                m_dbConnection.database_name(),")\\", m_dbConnection.dbms_name(),
                "' - Client will attempt reconnection up to ", maxRetries, " time(s)");

            core::Logger::Write(bufErrMsg.data(), core::Logger::PRIO_WARNING);
        }

        // loop for reconnection:
        while (true)
        {
            try
            {
                static const auto timeout =
                    core::AppConfig::GetSettings().framework.broker.dbConnTimeoutSecs;

                m_dbConnection.connect(m_connectionString, timeout);

                std::array<char, 512> bufErrMsg;

                utils::SerializeTo(bufErrMsg,
                    "Succesfully reconnected to broker queue in database '(",
                    m_dbConnection.database_name(), ")\\", m_dbConnection.dbms_name(), '\'');

                core::Logger::Write(bufErrMsg.data(), core::Logger::PRIO_WARNING);

                return m_dbConnection;
            }
            catch (nanodbc::database_error &)
            {
                if (retryCount >= maxRetries)
                    throw;
            }

            ++retryCount;

        }// end of loop
    }

    /////////////////////////////////
    // AsyncTransactionImpl class
    /////////////////////////////////

    AsyncTransactionImpl::AsyncTransactionImpl(DatabaseSession &dbSession,
                                               const std::future<void> &future)
        : m_dbSession(dbSession)
        , m_future(future)
    {
        CALL_STACK_TRACE;

        try
        {
            m_transaction.reset(
                dbg_new nanodbc::transaction(dbSession.GetConnection())
            );
        }
        catch_and_handle_exception("setting up transaction for queue operation")
    }

    AsyncTransactionImpl::~AsyncTransactionImpl()
    {
        CALL_STACK_TRACE;

        try
        {
            if (m_dbSession.GetConnection().transactions() > 0)
            {
                core::Logger::Write("broker queue under destruction in the middle of a transaction",
                                    core::Logger::PRIO_NOTICE, true);
            }
        }
        catch_and_log_exception("destructing queue transaction")
    }

    bool AsyncTransactionImpl::TryWait(uint16_t timeout)
    {
        CALL_STACK_TRACE;

        try
        {
            return !m_future.valid()
                || m_future.wait_for(std::chrono::milliseconds(timeout)) == std::future_status::ready;
        }
        catch_and_handle_exception("waiting for asynchronous broker queue transaction")
    }

    bool AsyncTransactionImpl::Rollback(uint16_t timeout)
    {
        CALL_STACK_TRACE;

        try
        {
            // must be within a transaction now
            _ASSERTE(m_dbSession.GetConnection().transactions() > 0);

            if (!TryWait(timeout))
                return false;

            m_transaction->rollback();
            return true;
        }
        catch_and_handle_exception("rolling back broker queue transaction")
    }

    bool AsyncTransactionImpl::Commit(uint16_t timeout)
    {
        CALL_STACK_TRACE;

        try
        {
            // must be within a transaction now
            _ASSERTE(m_dbSession.GetConnection().transactions() > 0);

            if (!TryWait(timeout))
                return false;

            m_transaction->commit();
            return true;
        }
        catch_and_handle_exception("committing broker queue transaction")
    }

}// end of namespace broker
}// end of namespace _3fd