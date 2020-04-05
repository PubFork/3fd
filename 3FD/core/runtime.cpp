#include "pch.h"
#include "callstacktracer.h"
#include "exceptions.h"
#include "gc.h"
#include "logger.h"
#include "runtime.h"

#ifdef _WIN32
#   include <roapi.h>
#endif

#ifdef _3FD_PLATFORM_WINRT
#   include <cstring>
#   include <fcntl.h>
#   include <io.h>
#   include <sqlite3\sqlite3.h>
#   include <winrt\Windows.Storage.h>
#endif

#include <array>
#include <codecvt>
#include <sstream>

namespace _3fd
{
namespace core
{
#ifdef _3FD_PLATFORM_WIN32API

    /// <summary>
    /// Gets the name of the current component, even
    /// if this is running inside a dynamic library.
    /// </summary>
    /// <returns>The name of the current running module.</returns>
    static std::string GetCurrentComponentName()
    {
        HMODULE thisModule;

        auto rv = GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR> (&GetCurrentComponentName),
            &thisModule
        );

        if (rv == 0)
            return "UNKNOWN";

        std::array<wchar_t, 256> modFilePath;
        rv = GetModuleFileNameW(thisModule, modFilePath.data(), modFilePath.size());

        if (rv == 0)
            return "UNKNOWN";

        auto modNameStr = wcsrchr(modFilePath.data(), L'\\');
        if (modNameStr != nullptr)
            ++modNameStr;
        else
            modNameStr = modFilePath.data();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> transcoder;
        return transcoder.to_bytes(modNameStr);
    }
    

    /// <summary>
    /// Initializes a new instance of the <see cref="FrameworkInstance" /> class.
    /// </summary>
    /// <param name="threadModel">The COM thread model.</param>
    FrameworkInstance::FrameworkInstance(MsComThreadModel threadModel)
        : FrameworkInstance()
    {
        const char *libLabel;
        bool unsupported(false);
        HRESULT hr;

        libLabel = "Windows Runtime API";

        /* Initialize Windows Runtime API for COM library usage in classic
        desktop apps (for use of libraries like Microsoft Media Foundation) */
        switch (threadModel)
        {
        case ComSingleThreaded:
            hr = Windows::Foundation::Initialize(RO_INIT_SINGLETHREADED);
            break;
        case ComMultiThreaded:
            hr = Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
            break;
        default:
            _ASSERTE(false); // unknown or unsupported COM thread model
            unsupported = true;
            break;
        }

        if (FAILED(hr) || unsupported)
        {
            std::ostringstream oss;
            oss << "Failed to initialize" << libLabel << "! ";

            if (unsupported)
                oss << "Specified thread model is unknown or unsupported";
            else
                oss << core::WWAPI::GetDetailsFromHResult(hr);

            Logger::Write(oss.str(), Logger::PRIO_ERROR);

            oss.str("");
            oss << "3FD was shutdown in " << m_moduleName;
            Logger::Write(oss.str(), core::Logger::PRIO_DEBUG);
            Logger::Shutdown();

            exit(EXIT_FAILURE);
        }

        m_isComLibInitialized = true;
    }


    /// <summary>
    /// Initializes a new instance of the <see cref="FrameworkInstance" /> class.
    /// </summary>
    FrameworkInstance::FrameworkInstance()
        : m_moduleName(GetCurrentComponentName())
        , m_isComLibInitialized(false)
    {
        std::ostringstream oss;
        oss << "3FD has been initialized in " << m_moduleName;
        Logger::Write(oss.str(), core::Logger::PRIO_DEBUG);
    }

#elif defined _3FD_PLATFORM_WINRT

    /// <summary>
    /// Initializes a new instance of the <see cref="FrameworkInstance" /> class.
    /// </summary>
    /// <param name="thisComName">Name of the this WinRT component or app.</param>
    FrameworkInstance::FrameworkInstance(const char *thisComName)
        : m_moduleName(thisComName)
    {
        std::string tempFolderPath =
            winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().TemporaryFolder().Path());

        auto tempDirStrSize = tempFolderPath.length() + 1;
        sqlite3_temp_directory = (char *)sqlite3_malloc(tempDirStrSize);

        if (sqlite3_temp_directory == nullptr)
            throw std::bad_alloc();

        strncpy(sqlite3_temp_directory, tempFolderPath.data(), tempDirStrSize);

        std::ostringstream oss;
        oss << "3FD has been initialized in " << m_moduleName;
        Logger::Write(oss.str(), core::Logger::PRIO_DEBUG);
    }

#endif

    /// <summary>
    /// Finalizes an instance of the <see cref="FrameworkInstance"/> class.
    /// </summary>
    FrameworkInstance::~FrameworkInstance()
    {
        memory::GarbageCollector::Shutdown();

#ifdef _WIN32
        std::ostringstream oss;
        oss << "3FD was shutdown in " << m_moduleName;

        Logger::Write(oss.str(), core::Logger::PRIO_DEBUG);
#endif
        Logger::Shutdown();

#ifdef _3FD_PLATFORM_WINRT
        sqlite3_free(sqlite3_temp_directory);

#elif defined _3FD_PLATFORM_WIN32API
        if (m_isComLibInitialized)
            Windows::Foundation::Uninitialize();
#endif
    }

    /// <summary>
    /// This will set up detection of memory leaks (supported by CRT libraries).
    /// Side effect for UWP console apps: the output will be redirected to a file!
    /// </summary>
    void SetupMemoryLeakDetection()
    {
#ifdef _WIN32
#   ifdef _3FD_PLATFORM_WINRT

        const std::string reportFileName =
            winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path())
            + "\\crt_memory_check_report.txt";

        int reportFileDesc;
        errno_t rc = _sopen_s(&reportFileDesc,
                              reportFileName.c_str(),
                              _O_CREAT | _O_TRUNC,
                              _SH_DENYWR,
                              _S_IREAD | _S_IWRITE);
        if (rc != 0)
        {
            printf("*** Failed to set up CRT support for memory leak detection: "
                   "could not open file '%s' - %s ***\n", reportFileName.c_str(), strerror(rc));
            return;
        }

        const auto reportFileHandle = reinterpret_cast<HANDLE> (_get_osfhandle(reportFileDesc));
#   else
        const HANDLE reportFileHandle(_CRTDBG_FILE_STDERR);
#   endif
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);

        _CrtSetReportFile(_CRT_WARN, reportFileHandle);
        _CrtSetReportFile(_CRT_ERROR, reportFileHandle);
        _CrtSetReportFile(_CRT_ASSERT, reportFileHandle);

        if ((rc = errno) != 0)
        {
            printf("*** Failed to set up CRT support for memory leak detection: "
                   "could not set report file! %s ***\n", strerror(rc));
            return;
        }

#   ifdef _3FD_PLATFORM_WINRT
        printf("*** CRT memory leak detection has been set up to print report to file '%s' ***\n",
               reportFileName.c_str());
#   endif
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    }

}// end of namespace core
}// end of namespace _3fd
