#include "stdafx.h"
#include "utils_io.h"
#include <cerrno>

namespace _3fd
{
namespace utils
{
    ///////////////////////////////////////
    // Specialization of serializations
    ///////////////////////////////////////

    size_t _estimate_string_size()
    {
        return 0;
    }

#ifdef _3FD_PLATFORM_WINRT
    
    SerializableValue<const wchar_t *> FormatArg(const winrt::hstring &value)
    {
        return SerializableValue<const wchar_t *>(value.c_str());
    }
#endif

}// end of namespace utils
}// end of namespace _3fd