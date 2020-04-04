// UnitTests.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include <3fd/core/preprocessing.h>
#include <iostream>

#ifdef _MSC_VER
#   ifdef _3FD_PLATFORM_WIN32API // Win32 Console:
    int wmain(int argc, wchar_t *argv[])
    {
        std::cout << "Running wmain() from UnitTests.cpp\n";
        testing::InitGoogleTest(&argc, argv);
        int rc = RUN_ALL_TESTS();
        void *x = dbg_new int(696);

        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

        return rc;
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
