#ifndef RPC_H // header guard
#define RPC_H

#include <Poco\UUID.h>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <AuthZ.h>
#include <rpc.h>

namespace _3fd
{
    using std::string;

    namespace rpc
    {

        /// <summary>
        /// Enumerates the possible options for RPC transport.
        /// </summary>
        enum class ProtocolSequence { Local, TCP };

        /// <summary>
        /// Holds a definition for a particular RPC interface implementation.
        /// </summary>
        struct RpcSrvObject
        {
            /// <summary>
            /// The UUID of the object, which is an external identifier
            /// exposed to clients. (This is not the interface UUID.)
            /// </summary>
            Poco::UUID uuid;

            /// <summary>
            /// The interface handle defined in the stub generated by MIDL compiler from IDL file.
            /// This handle internally defines the default EPV (when MIDL has been run with '/use_epv').
            /// </summary>
            RPC_IF_HANDLE interfaceHandle;

            /// <summary>
            /// The entry point vector. If this is null, then the default EPV supplied
            /// by the inteface handle is used.
            /// </summary>
            RPC_MGR_EPV *epv;
        };

        class RpcServerImpl;

        /// <summary>
        /// Represents the RPC server that runs inside the application process.
        /// </summary>
        class RpcServer
        {
        private:

            static std::unique_ptr<RpcServerImpl> uniqueObject;

            static std::mutex singletonAccessMutex;

            RpcServer() {}

        public:

            ~RpcServer() {}

            static void Initialize(
                ProtocolSequence protSeq,
                const string &serviceClass,
                const std::function<bool (AUTHZ_CLIENT_CONTEXT_HANDLE)> &callbackAuthz,
                bool useActDirSec = true
            );

            static bool Start(const std::vector<RpcSrvObject> &objects);

            static bool Stop();

            static bool Resume();

            static bool Wait();

            static void Finalize() noexcept;
        };

        /// <summary>
        /// Implements an RPC client that provides an explicit binding handle
        /// to use as parameter for client stub code generated by the MIDL compiler.
        /// Client code is expected to derive from this class and call <see cref="RpcClient::GetBindingHandle"/>
        /// in order to obtain an explicit binding handle.
        /// </summary>
        class RpcClient
        {
        private:

            RPC_BINDING_HANDLE m_bindingHandle;

        protected:

            /// <summary>
            /// Gets the binding handle.
            /// </summary>
            /// <returns>The explicit binding handle expected as parameter for RPC.</returns>
            RPC_BINDING_HANDLE GetBindingHandle() const { return m_bindingHandle; }

        public:

            RpcClient(
                ProtocolSequence protSeq,
                const string &objUUID,
                const string &destination,
                const string &endpoint
            );

            ~RpcClient();

            void ResetBindings();
        };

    }// end of namespace rpc
}// end of namespace _3fd

#endif // end of header guard
