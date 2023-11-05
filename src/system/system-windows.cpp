#ifdef _WIN32

#include "probe/system.h"
#include "probe/util.h"

#include <Windows.h>

namespace probe::system
{
    extern "C" NTSYSAPI NTSTATUS NTAPI RtlGetVersion(_Out_ PRTL_OSVERSIONINFOW lpVersionInformation);

    desktop_t desktop() { return desktop_t::Windows; }

    version_t desktop_version() { return os_version(); }

    theme_t theme()
    {
        if (os_version() >= probe::WIN_10_1ST) {
            if (probe::util::registry::read<DWORD>(
                    HKEY_CURRENT_USER, R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
                    "AppsUseLightTheme")
                    .value_or(1) == 0) {
                return theme_t::dark;
            }
        }

        return theme_t::light;
    }

    kernel_info_t kernel_info() { return { kernel_name(), kernel_version() }; }

    static unsigned int build_number()
    {
        auto ubr = probe::util::registry::read<DWORD>(
            HKEY_LOCAL_MACHINE, R"(Software\Microsoft\Windows NT\CurrentVersion)", "UBR");
        if (ubr.has_value()) {
            return ubr.value();
        }

        // Fall back to BuildLabEx in the early version of Windows 8.1 and less.
        auto buildlabex = probe::util::registry::read<std::string>(
            HKEY_LOCAL_MACHINE, R"(Software\Microsoft\Windows NT\CurrentVersion)", "BuildLabEx");
        if (!buildlabex.has_value()) {
            return 0;
        }

        char *ctx{};
        auto  token = strtok_s(&buildlabex.value()[0], ".", &ctx);
        token       = strtok_s(nullptr, ".", &ctx);
        return token ? std::strtoul(token, nullptr, 10) : 0;
    }

    version_t kernel_version()
    {
        // version
        RTL_OSVERSIONINFOW os_version_info  = { 0 };
        os_version_info.dwOSVersionInfoSize = sizeof(os_version_info);
        RtlGetVersion(&os_version_info);

        // https://learn.microsoft.com/en-us/windows/win32/sysinfo/operating-system-version
        // https://en.wikipedia.org/wiki/Comparison_of_Microsoft_Windows_versions
        // Windows              11 : 10.0.22000
        // Windows              10 : 10.0
        // Windows     Server 2022 : 10.0
        // Windows     Server 2019 : 10.0
        // Windows     Server 2016 : 10.0
        // Windows             8.1 :  6.3
        // Windows  Server 2012 R2 :  6.3
        // Windows             8.0 :  6.2
        // Windows     Server 2012 :  6.2
        // Windows               7 :  6.1
        // Windows  Server 2008 R2 :  6.1
        // Windows     Server 2008 :  6.0
        // Windows           Vista :  6.0
        // Windows  Server 2003 R2 :  5.2
        // Windows     Server 2003 :  5.2
        // Windows              XP :  5.1
        // Windows            2000 :  5.0
        return { os_version_info.dwMajorVersion, os_version_info.dwMinorVersion,
                 os_version_info.dwBuildNumber, build_number() };
    }

    version_t os_version()
    {
        auto os_ver = kernel_version();
        os_ver.codename =
            probe::util::registry::read<std::string>(
                HKEY_LOCAL_MACHINE, R"(Software\Microsoft\Windows NT\CurrentVersion)", "DisplayVersion")
                .value_or("");

        return os_ver;
    }

    std::string kernel_name() { return "Windows NT"; }

    std::string os_name()
    {
        auto name =
            probe::util::registry::read<std::string>(
                HKEY_LOCAL_MACHINE, R"(Software\Microsoft\Windows NT\CurrentVersion)", "ProductName")
                .value_or("Windows");

        // Windows 11
        if (os_version().patch >= 22'000 &&
            probe::util::registry::read<std::string>(
                HKEY_LOCAL_MACHINE, R"(Software\Microsoft\Windows NT\CurrentVersion)", "InstallationType")
                    .value_or("") == "Client") {

            auto pos = name.find("Windows 10");
            if (pos != std::string::npos) {
                name.replace(pos, pos + 10, "Windows 11");
            }
        }

        return name;
    }

    os_info_t os_info() { return { os_name(), theme(), os_version() }; }

    std::string hostname()
    {
        WCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1]{};
        DWORD size = sizeof(buffer);
        if (::GetComputerName(buffer, &size) != 0) {
            return probe::util::to_utf8(buffer);
        }
        return {};
    }

    std::string username()
    {
        WCHAR buffer[256]{};
        DWORD size = sizeof(buffer);
        if (::GetUserName(buffer, &size) != 0) {
            return probe::util::to_utf8(buffer);
        }
        return {};
    }

    window_system_t window_system() { return window_system_t::Windows; }
} // namespace probe::system

#endif // _WIN32