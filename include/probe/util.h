#ifndef PROBE_UTIL_H
#define PROBE_UTIL_H

#include <optional>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "probe/dllport.h"

namespace probe::util
{
    PROBE_API std::string to_utf8(const wchar_t *, size_t = 0);
    PROBE_API std::string to_utf8(const std::wstring&);

    // must has a null-terminating-character if the size_t == 0
    PROBE_API std::string to_utf8(const char *, size_t = 0);
    PROBE_API std::string to_utf8(const std::string&);

    PROBE_API std::wstring to_utf16(const std::string&);
    PROBE_API std::wstring to_utf16(const char *, size_t = 0);

    // linux: The thread name is a meaningful C language string, whose length is
    // restricted to 16 characters, including the terminating null byte ('\0')
    PROBE_API int thread_set_name(const std::string&);
    PROBE_API std::string thread_get_name();
} // namespace probe::util

// windows registry
#ifdef _WIN32
namespace probe::util::registry
{
    template<typename T> std::optional<T> read(HKEY key, const std::string&, const std::string&);

    template<>
    PROBE_API std::optional<std::string> read<std::string>(HKEY, const std::string&, const std::string&);

    template<> PROBE_API std::optional<DWORD> read<DWORD>(HKEY, const std::string&, const std::string&);
} // namespace probe::util::registry
#endif

// linux
#ifdef __linux__
namespace probe::util
{
    PROBE_API std::optional<std::string> exec(const char *);
} // namespace probe::util
#endif
#endif //! PROBE_UTIL_H