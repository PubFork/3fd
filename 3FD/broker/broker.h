//
// Copyright (c) 2020 Part of 3FD project (https://github.com/faburaya/3fd)
// It is FREELY distributed by the author under the Microsoft Public License
// and the observance that it should only be used for the benefit of mankind.
//
#ifndef BROKER_H // header guard
#define BROKER_H

#include <3fd/core/preprocessing.h>
#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

namespace _3fd {
namespace broker {

    using std::string;

    class DatabaseSession;

    /// <summary>
    /// Common interface for asynchronous database operation
    /// that takes place within a transaction.
    /// </summary>
    class INTFOPT IAsyncTransaction
    {
    public:

        virtual ~IAsyncTransaction() = default;

        /// <summary>
        /// Waits for the asynchronous operation (write/read step) to finish.
        /// </summary>
        /// <param name="timeout">The timeout in milliseconds.</param>
        /// <returns>
        ///   <c>true</c> if the operation is already finished or
        ///   finishes before timeout, otherwise, <c>false</c>.
        /// </returns>
        virtual bool TryWait(uint16_t timeout) = 0;

        /// <summary>
        /// Rolls back the changes accumulated in the current transaction.
        /// All messages read since last commit are put back into the queue.
        /// All messages written since last commit are removed from the queue.
        /// </summary>
        /// <param name="timeout">The timeout (in milliseconds)
        /// when waiting for the lastest read/write operation to finish.</param>
        /// <returns>
        ///   <c>true</c> if current transaction was successfully rolled back, otherwise, <c>false</c>.
        /// </returns>
        virtual bool Rollback(uint16_t timeout) = 0;


        /// <summary>
        /// Commits in persistent storage all the changes accumulated
        /// in the current transaction.
        /// All messages read so far are permanently removed from the queue.
        /// All messages written become available for readers to consume.
        /// </summary>
        /// <param name="timeout">The timeout (in milliseconds)
        /// when waiting for the lastest read/write operation to finish.</param>
        /// <returns>
        ///   <c>true</c> if current transaction was successfully committed, otherwise, <c>false</c>.
        /// </returns>
        virtual bool Commit(uint16_t timeout) = 0;
    };

    /// <summary>
    /// Enumerates options for service broker backend.
    /// </summary>
    enum class Backend : short
    {
        MsSqlServer
        //, OracleDatabase
    };

    /// <summary>
    /// Enumerates options for backend validation of message content.
    /// </summary>
    enum class MessageContentValidation : short
    {
        None,
        WellFormedXml
    };

    /// <summary>
    /// Packs the necessary info to specify a message type for creation or use.
    /// </summary>
    struct MessageTypeSpec
    {
        uint32_t nBytes;
        MessageContentValidation contentValidation;
    };

    /// <summary>
    /// Controls retrieval of results from asynchronous read of queue.
    /// </summary>
    class INTFOPT IAsyncRead : public IAsyncTransaction
    {
    public:

        virtual ~IAsyncRead() = default;

        /// <summary>
        /// Start the asynchronous execution of the next step.
        /// </summary>
        virtual void StepNext() = 0;

        /// <summary>
        /// Gets the result from the last asynchronous step execution.
        /// </summary>
        /// <param name="timeout">The timeout in milliseconds.</param>
        /// <returns>A vector of read messages after waiting for completion of
        /// asynchronous read operation. The retrieved messages are not guaranteed
        /// to appear in the same order they had when inserted.</returns>
        virtual std::vector<string> GetStepResult(uint16_t timeout) = 0;
    };

    /// <summary>
    /// Represents a queue in the broker, from which a service can read
    /// its incoming messages. This implementation is NOT THREAD SAFE!
    /// </summary>
    /// <seealso cref="notcopiable" />
    class QueueReader
    {
    private:

        DatabaseSession *m_dbSession;
        
        string m_serviceURL;

    public:

        QueueReader(Backend svcBrokerBackend,
                    const string &connString,
                    const string &serviceURL,
                    const MessageTypeSpec &msgTypeSpec);

        QueueReader(const QueueReader &) = delete;

        std::unique_ptr<IAsyncRead> ReadMessages(uint16_t msgCountStepLimit, uint16_t msgRecvTimeout);
    };

    /// <summary>
    /// Helps synchronizing with an asynchronous write to a broker queue.
    /// </summary>
    class INTFOPT IAsyncWrite : public IAsyncTransaction
    {
    public:

        virtual ~IAsyncWrite() = default;

        /// <summary>
        /// Rethrows any eventual exception captured in the worker thread.
        /// May only be called ONCE!
        /// </summary>
        virtual void Rethrow() = 0;
    };

    /// <summary>
    /// Represents a queue in the broker, into which
    /// a service can write messages to another.
    /// The underlying implementation is NOT THREAD SAFE!
    /// </summary>
    /// <seealso cref="notcopiable" />
    class QueueWriter
    {
    private:

        DatabaseSession *m_dbSession;

        string m_serviceURL;

    public:

        QueueWriter(Backend svcBrokerBackend,
                    const string &connString,
                    const string &serviceURL,
                    const MessageTypeSpec &msgTypeSpec);

        QueueWriter(const QueueWriter &) = delete;

        std::unique_ptr<IAsyncWrite> WriteMessages(const std::vector<string> &messages);
    };

}// end of namespace broker
}// end of namespace _3fd

#endif // end of header guard
