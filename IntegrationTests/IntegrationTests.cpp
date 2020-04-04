#include "pch.h"
#include <3fd/core/exceptions.h>
#include <3fd/core/logger.h>
#include <exception>
#include <iostream>

#ifdef _MSC_VER
#   ifdef _3FD_PLATFORM_WIN32API // Win32 Console:
#       include <vld.h>

// Win32 Console:
int wmain(int argc, wchar_t *argv[])
{
    std::cout << "Running wmain() from IntegrationTests.cpp\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#   else // UWP Console:
int main()
{
    std::cout << "Running main() from IntegrationTests.cpp\n";
    testing::InitGoogleTest(&__argc, __argv);
    int rc = RUN_ALL_TESTS();
    getchar();
    return rc;
}
#   endif

#else // POSIX:
int main(int argc, char *argv[])
{
    std::cout << "Running main() from IntegrationTests.cpp\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif

namespace _3fd
{
namespace integration_tests
{
    using namespace _3fd::core;

    /// <summary>
    /// Handles several types of exception.
    /// </summary>
    void HandleException()
    {
        try
        {
            throw;
        }
        catch(IAppException &appEx)
        {
            //std::cerr << appEx.ToString() << std::endl;
            core::Logger::Write(appEx, core::Logger::PRIO_ERROR);
        }
        catch(std::exception &stdEx)
        {
            std::cerr << stdEx.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "An unexpected exception has been caught." << std::endl;
        }

        FAIL();
    }

}// end of namespace integration_tests
}// end of namespace _3fd
