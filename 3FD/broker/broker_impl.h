//
// Copyright (c) 2020 Part of 3FD project (https://github.com/faburaya/3fd)
// It is FREELY distributed by the author under the Microsoft Public License
// and the observance that it should only be used for the benefit of mankind.
//
#ifndef BROKER_IMPL_H // header guard
#define BROKER_IMPL_H

#include "broker.h"

#define NANODBC_ENABLE_UNICODE
#include <nanodbc/nanodbc.h>
#include <future>

#define catch_and_handle_exception(WHEN) catch(...) { HandleException(WHEN); }

#define catch_and_log_exception(WHEN) catch(...) { LogException(WHEN); }

namespace _3fd
{
namespace broker
{
    void HandleException(const char *when);

    void LogException(const char *when) noexcept;

    const wchar_t *ToString(Backend backend);

    const wchar_t *ToString(MessageContentValidation msgContentValidation);

    /// <summary>
    /// Provides a resilient database ODBC connection.
    /// </summary>
    class DatabaseSession
    {
    private:

        nanodbc::connection m_dbConnection;
        string m_connectionString;

    public:

        DatabaseSession(const string &connString);

        DatabaseSession(const DatabaseSession &) = delete;

        nanodbc::connection &GetConnection();
    };

    /// <summary>
    /// Implements basic contract for asynchronous database transactions.
    /// </summary>
    class AsyncTransactionImpl : public IAsyncTransaction
    {
    private:

        DatabaseSession &m_dbSession;
        std::unique_ptr<nanodbc::transaction> m_transaction;
        const std::future<void> &m_future;

    public:

        AsyncTransactionImpl(const AsyncTransactionImpl &) = delete;

        /// <summary>
        /// Initializes a new instance of the <see cref="AsyncTransactionImpl" /> class.
        /// </summary>
        /// <param name="dbSession">The database session.</param>
        /// <param name="future">The future of the asynchronous operation.</param>
        AsyncTransactionImpl(DatabaseSession &dbSession, const std::future<void> &future);

        /// <summary>
        /// Finalizes an instance of the <see cref="AsyncTransactionImpl"/> class.
        /// </summary>
        virtual ~AsyncTransactionImpl();

        nanodbc::connection &GetConnection()
        {
            return m_dbSession.GetConnection();
        }

        /// <inheritdoc/>
        bool TryWait(uint16_t timeout) override;

        /// <inheritdoc/>
        bool Rollback(uint16_t timeout) override;

        /// <inheritdoc/>
        bool Commit(uint16_t timeout) override;
    };

}// end of namespace broker
}// end of namespace _3fd

#endif // end of header guard
