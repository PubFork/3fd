#include "stdafx.h"
#include "runtime.h"
#include "callstacktracer.h"
#include "rpc_helpers.h"
#include "rpc_test_shared.h"

#ifdef _WIN64
#   include "AcmeTesting_x64.h"
#else
#   include "AcmeTesting_w32.h"
#endif

namespace _3fd
{
    namespace integration_tests
    {
        using namespace core;
        using namespace rpc;

        void HandleException();

        RPC_STATUS OperateImpl(RPC_BINDING_HANDLE handle, double left, double right, double &result)
        {
        RpcTryExcept
            double result;
            Operate(handle, left, right, &result);
            return RPC_S_OK;
        RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
            return RpcExceptionCode();
        RpcEndExcept
        }

        RPC_STATUS ChangeCaseImpl(RPC_BINDING_HANDLE handle, const char *text, cstring &output)
        {
        RpcTryExcept
            cstring input = { strlen(text) + 1, (unsigned char *)text };
            ChangeCase(handle, &input, &output);
            return RPC_S_OK;
        RpcExcept(RpcExceptionFilter(RpcExceptionCode()))
            return RpcExceptionCode();
        RpcEndExcept
        }

        /// <summary>
        /// Proxy for AcmeTesting RPC server.
        /// </summary>
        class AcmeSvcProxy : public rpc::RpcClient
        {
        public:

            using RpcClient::RpcClient;

            double Operate(double left, double right)
            {
                double result;
                auto status = OperateImpl(GetBindingHandle(), left, right, result);
                ThrowIfError(status, "Failed to invoke RPC client stub routine \'Operate\'");
                return result;
            }

            string ChangeCase(const char *text)
            {
                cstring output;
                auto status = ChangeCaseImpl(GetBindingHandle(), text, output);
                ThrowIfError(status, "Failed to invoke RPC client stub routine \'ChangeCase\'");
                return string(output.data, output.data + output.size - 1);
            }

            void Shutdown()
            {
                ::Shutdown(GetBindingHandle());
            }
        };

        // The set of options for each test template instantiation
        struct TestOptions
        {
            ProtocolSequence protocolSequence;
            const char *objectUUID1;
            const char *objectUUID2;
            AuthenticationLevel authenticationLevel;
        };

        class Framework_RPC_TestCase :
            public ::testing::TestWithParam<TestOptions> {};

        /// <summary>
        /// 
        /// </summary>
        TEST(Framework_RPC_TestCase, ClientRun_LocalRequestTest)
        {
            // Ensures proper initialization/finalization of the framework
            FrameworkInstance _framework;

            CALL_STACK_TRACE;

            try
            {
                AcmeSvcProxy client1(
                    ProtocolSequence::TCP,
                    objectsUuidsImpl1[6],
                    "BR00200256.ericsson.se",
                    AuthenticationLevel::Integrity
                );

                EXPECT_EQ(696.0, client1.Operate(6.0, 116.0));
                EXPECT_EQ("SQUIRREL", client1.ChangeCase("squirrel"));

                AcmeSvcProxy client2(
                    ProtocolSequence::TCP,
                    objectsUuidsImpl2[6],
                    "BR00200256.ericsson.se",
                    AuthenticationLevel::Integrity
                );

                EXPECT_EQ(696.0, client2.Operate(606.0, 90.0));
                EXPECT_EQ("squirrel", client2.ChangeCase("SQUIRREL"));

                client2.Shutdown();
            }
            catch (...)
            {
                HandleException();
            }
        }

    }// end of namespace integration_tests
}// end of namespace _3fd
