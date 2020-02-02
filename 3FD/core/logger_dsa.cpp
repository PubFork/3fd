#include "pch.h"
#include "configuration.h"
#include "exceptions.h"
#include "logger.h"

#include "3fd/utils/utils_io.h"
#include <boost/filesystem.hpp>
#include <chrono>
#include <sstream>

namespace _3fd
{
namespace core
{
    /// <summary>
    /// Implements the contract of <see cref="ILogFileAccess"/> while hiding
    /// the particular implementation for IO access directly to the system.
    /// </summary>
    class DirectSystemFileAccess : public ILogFileAccess
    {
    private:

        const boost::filesystem::path m_filePath;

    public:

        DirectSystemFileAccess(const std::string &filePath)
            : m_filePath(filePath) {}

        void OpenStream(std::ofstream &ofs) override
        {
            ofs.open(m_filePath.string(), std::ios::app | std::ios::out);
            if (ofs.is_open() == false)
                throw AppException<std::runtime_error>("Could not open text log file", m_filePath.string());
        }

        void ShiftToNewLogFile(std::ofstream &ofs) override
        {
            ofs.close(); // first close the stream to the current log file

            using std::chrono::system_clock;
            auto now = system_clock::to_time_t(system_clock::now());

            using namespace boost::filesystem;

            std::array<char, 265> pathBuffer;
            utils::SerializeTo(pathBuffer,
                m_filePath.parent_path().string(),
                path::preferred_separator,
                m_filePath.stem().string(),
                '[', ctime(&now), "].log.txt");

            path archivedLogFilePath = pathBuffer.data();

            try
            {
                rename(m_filePath, archivedLogFilePath);
            }
            catch (filesystem_error &)
            {
                std::ostringstream oss;
                oss << m_filePath.string() << " -> " << archivedLogFilePath.string();
                throw AppException<std::runtime_error>("Failed to shift log file", oss.str());
            }

            OpenStream(ofs); // start new file
        }

        uint64_t GetFileSize() const override
        {
            try
            {
                return boost::filesystem::file_size(m_filePath);
            }
            catch (boost::filesystem::filesystem_error &ex)
            {
                throw AppException<std::runtime_error>("Failed to get size of log file",
                                                       StdLibExt::GetDetailsFromSystemError(ex.code()));
            }
        }
    };

    std::unique_ptr<ILogFileAccess> GetFileAccess(const string &loggerId)
    {
        return std::unique_ptr<ILogFileAccess>(new DirectSystemFileAccess(loggerId + ".log.txt"));
    }

}// end of namespace core
}// end of namespace _3fd
