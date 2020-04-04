// UnitTests.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include <3fd/core/preprocessing.h>
#include <iostream>

#ifdef _MSC_VER
#   ifdef _3FD_PLATFORM_WIN32API // Win32 Console:
#       include <vld.h>

    // Win32 Console:
    int wmain(int argc, wchar_t *argv[])
    {
        std::cout << "Running wmain() from UnitTests.cpp\n";
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

#   else // UWP Console:
    int main()
    {
        std::cout << "Running main() from UnitTests.cpp\n";
        testing::InitGoogleTest(&__argc, __argv);
        int rc = RUN_ALL_TESTS();
        getchar();
        return rc;
    }
#   endif

#else // POSIX:
int main(int argc, char *argv[])
{
    std::cout << "Running main() from UnitTests.cpp\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#endif
