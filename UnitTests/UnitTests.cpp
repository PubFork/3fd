// UnitTests.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include <3fd/core/runtime.h>
#include <iostream>

#ifdef _MSC_VER
#   ifdef _3FD_PLATFORM_WIN32API // Win32 Console:
    int wmain(int argc, wchar_t *argv[])
    {
        std::cout << "Running wmain() from " __FILE__ << std::endl;
        _3fd::core::SetupMemoryLeakDetection();
        testing::InitGoogleTest(&argc, argv);
        void *x = dbg_new int(696);
        return RUN_ALL_TESTS();
    }

#   else // UWP Console:
    int main()
    {
        std::cout << "Running main() from " __FILE__ << std::endl;
        testing::InitGoogleTest(&__argc, __argv);
        int rc = RUN_ALL_TESTS();
        getchar();
        return rc;
    }
#   endif

#else // POSIX:
int main(int argc, char *argv[])
{
    std::cout << "Running wmain() from " __FILE__ << std::endl;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif
