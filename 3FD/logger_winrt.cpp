#include "stdafx.h"
#include "logger.h"
#include "configuration.h"
#include "callstacktracer.h"
#include "utils_winrt.h"

#include <array>
#include <codecvt>
#include <future>
#include <fstream>
#include <sstream>
#include <chrono>

namespace _3fd
{
namespace core
{
    /// <summary>
    /// Implements the contract of <see cref="ILogFileAccess"/> while
    /// hiding the C++/CX particular implementation for WinRT platform.
    /// </summary>
    class WinRtFileAccess : public ILogFileAccess
    {
    private:

        Windows::Storage::StorageFile ^m_logFile;

        static void OpenStreamImpl(Windows::Storage::StorageFile ^logFile, std::ofstream &ofs)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> transcoder;
            ofs.open(transcoder.to_bytes(logFile->Path->Data()), std::ios::app | std::ios::out);
            if (ofs.is_open() == false)
                throw AppException<std::runtime_error>("Could not open log file");
        }

    public:

        WinRtFileAccess(Windows::Storage::StorageFile ^logFile)
            : m_logFile(logFile) {}

        void OpenStream(std::ofstream &ofs) override
        {
            OpenStreamImpl(m_logFile, ofs);
        }

        void ShiftToNewLogFile(std::ofstream &ofs) override
        {
            using namespace std::chrono;
            using namespace Windows::Storage;
            using namespace Windows::Foundation;

            ofs.close(); // first close the stream to the current log file

            // Rename the current log file:
            std::wostringstream woss;
            woss << m_logFile->Path->Data() << L".old";
            auto currLogFileName = m_logFile->Name;
            auto tempLogFileName = ref new Platform::String(woss.str().c_str());
            utils::WinRTExt::WaitForAsync(m_logFile->RenameAsync(tempLogFileName));

            // Start reading log file to buffer:
            auto asyncOperReadLogToBuffer = FileIO::ReadBufferAsync(m_logFile);

            // Create a new log file:
            m_logFile = utils::WinRTExt::WaitForAsync(
                ApplicationData::Current->LocalFolder->CreateFileAsync(currLogFileName)
            );

            OpenStreamImpl(m_logFile, ofs);

            // Create the file which will contain the previous log (compressed):
            std::wstring_convert<std::codecvt_utf8<wchar_t>> transcoder;
            auto now = system_clock::to_time_t(system_clock::now());
            woss << m_logFile->DisplayName->Data() << L'[' << transcoder.from_bytes(ctime(&now)) << L"].log.dat";

            auto compressedLogFile = utils::WinRTExt::WaitForAsync(
                // moves it to the app temporary data store
                ApplicationData::Current->TemporaryFolder->CreateFileAsync(
                    ref new Platform::String(woss.str().c_str()),
                    CreationCollisionOption::ReplaceExisting
                )
            );

            // Create a writable stream for such file:
            auto outputStream = utils::WinRTExt::WaitForAsync(
                compressedLogFile->OpenAsync(FileAccessMode::ReadWrite)
            );

            // Await for completion of reading operation:
            auto readBuffer = utils::WinRTExt::WaitForAsync(asyncOperReadLogToBuffer);

            // Compress the text content of the previous log file:
            auto compressor = ref new Compression::Compressor(outputStream);
            utils::WinRTExt::WaitForAsync(compressor->WriteAsync(readBuffer));
            utils::WinRTExt::WaitForAsync(compressor->FinishAsync());
            utils::WinRTExt::WaitForAsync(compressor->FlushAsync());

            // Write log shift event in the new log:
            auto now = system_clock::to_time_t(system_clock::now());
            PrepareEventString(ofs, now, Logger::PRIO_NOTICE)
                << L"The log file has been shifted. The previous file has been compressed from "
                << readBuffer->Length / 1024 << L" to " << outputStream->Size / 1024
                << L" KB and moved to the app temporary data store." << std::endl << std::flush;
        }

        uint64_t GetFileSize() const override
        {
            return utils::WinRTExt::WaitForAsync(
                m_logFile->GetBasicPropertiesAsync()
            )->Size;
        }
    };

    std::unique_ptr<ILogFileAccess> GetFileAccess(const string &loggerId)
    {
        using namespace Windows::Storage;

        // Open or create the file:
        std::wstring_convert<std::codecvt_utf8<wchar_t>> transcoder;
        auto name = ref new Platform::String(transcoder.from_bytes(loggerId).c_str());
        auto logFile = utils::WinRTExt::WaitForAsync(
            ApplicationData::Current->LocalFolder->CreateFileAsync(name + L".log.txt", CreationCollisionOption::OpenIfExists)
        );

        return std::unique_ptr<ILogFileAccess>(new WinRtFileAccess(logFile));
    }

}// end of namespace core
}// end of namespace _3fd
