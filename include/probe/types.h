#ifndef PROBE_TYPES_H
#define PROBE_TYPES_H

#include "probe/dllport.h"

#include <any>
#include <cstdint>
#include <functional>
#include <string>

namespace probe
{
    struct version_t
    {
        uint32_t major{};
        uint32_t minor{};
        uint32_t patch{};
        uint32_t build{};

        std::string codename{};
    };

    PROBE_API bool operator>(const version_t&, const version_t&);
    PROBE_API bool operator>=(const version_t&, const version_t&);

    PROBE_API bool operator<(const version_t&, const version_t&);
    PROBE_API bool operator<=(const version_t&, const version_t&);

    // not compare codename
    PROBE_API bool operator==(const version_t&, const version_t&);
    PROBE_API bool operator!=(const version_t&, const version_t&);

    // do  compare codename
    PROBE_API bool strict_equal(const version_t&, const version_t&);

    // version pattern
    // major.minor.path.build-codename or major.minor.path-build-codename
    inline constexpr char version_regex[] =
        R"((?:[^\.]*[^\d\.]{1})*(\d+)\.(\d+)(?:\.(\d+))?(?:[\.-]{1}(\d+))?(?:\-{1}(\w+))?(?:[^\d\.]{1}[^\.]*)*)";

    // use above version_regex to parse version string
    PROBE_API version_t to_version(const std::string&);

    enum class vendor_t
    {
        Unknown   = 0x0000,
        // just some commonly used vendor names
        NVIDIA    = 0x10de,
        Intel     = 0x8086,
        Microsoft = 0x1414,
        Qualcomm  = 0x17cb,
        AMD       = 0x1002,
        Apple     = 0x106b,
        // ... in pciids
    };

    // cast vendor_t to string
    PROBE_API std::string vendor_cast(vendor_t);

    // cast uint32_t to vendor_t
    PROBE_API inline vendor_t vendor_cast(uint32_t id) { return static_cast<vendor_t>(id); }

    // guess what vendor_t the string is
    PROBE_API vendor_t vendor_cast(const std::string&);

    PROBE_API std::string product_name(uint32_t, uint32_t);

    // TODO: bus / protocol ...
    enum class bus_type_t
    {
        // Windows: STORAGE_BUS_TYPE
        Unknown = 0x00,
        SCSI, // Small Computer System Interface
        ATAPI,
        ATA,  // Advanced Technology Attachment
        IEEE1394,
        SSA,
        Fibre,
        USB,
        RAID,
        iSCSI,
        SAS, // Serial Attached SCSI
        SATA,
        SDIO,
        MMC,
        Virtual,
        FileBackedVirtual,
        Spaces,
        NVMe,
        SCM,
        UFS,
        MAX,
        // other bus type
        AC97, // An audio codec standard developed by Intel Architecture Labs in 1997.
        ACPI, // Advanced Configuration and Power Interface
        Auxiliary,
        CPU,
        GPIO,
        HDAudio,
        HID,
        I2C,
        ISA, // Industry Standard Architecture
        PCI,
        PCIe,
        SPI,
        CAN,
        EISA,
        MDIO,
        IDE,
        Virtio,
        NVMEM,
        PnP,
        VME,
        Xen,
        CEC,
        MaxReserved = 0x7F
    };

    PROBE_API bus_type_t bus_cast(const std::string&);

    PROBE_API std::string bus_cast(bus_type_t);
} // namespace probe

namespace probe
{
    PROBE_API std::string to_string(const version_t&);

    PROBE_API inline std::string to_string(vendor_t id) { return vendor_cast(id); }

    PROBE_API inline std::string to_string(bus_type_t t) { return bus_cast(t); }
} // namespace probe

// listener interface
namespace probe
{
    class PROBE_API Listener
    {
    public:
        Listener()                           = default;
        Listener(const Listener&)            = delete;
        Listener& operator=(const Listener&) = delete;
        Listener(Listener&&)                 = delete;
        Listener& operator=(Listener&&)      = delete;
        virtual ~Listener()                  = default;

        virtual int listen(const std::any&, const std::function<void(const std::any&)>&) = 0;

        virtual bool running() = 0;

        virtual void stop() = 0;
    };
} // namespace probe

#ifdef _WIN32
// Windows versions
namespace probe
{
    // clang-format off
    // https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions, stable version
    // Windows 3.1, 1992-04-06
    inline const version_t WIN_3_1    {  3, 10,   102,     0,   "Sparta" };

    // Windows 95, 1995-08-24
    inline const version_t WIN_95     {  4,  0,   950,     0,   "Chicago" };

    // Windows 98, 1998-06-25
    inline const version_t WIN_98     {  4, 10,  1998,     0,   "Memphis" };

    // Windows 2000
    inline const version_t WIN_2000   {  5,  0,  2195,     0,   "Janus" };

    // Windows xp
    inline const version_t WIN_XP     {  5,  2,  2600,     0,   "Whistler" };

    // Windows vista
    inline const version_t WIN_VISTA  {  6,  0, 6000,     0,   "Longhorn" };

    // Windows 7
    inline const version_t WIN_7      {  6,  1,  7600,     0,   "7" };

    // Windows 8
    inline const version_t WIN_8_0    {  6,  2,  9200,     0,   "8" };
    inline const version_t WIN_8_1    {  6,  3,  9600,     0,   "Blue" };

    // Windows 10
    inline const version_t WIN_10_1507{ 10,  0, 10240, 16405,   "1507" };
    inline const version_t WIN_10_1511{ 10,  0, 10586,     3,   "1511" };
    inline const version_t WIN_10_1607{ 10,  0, 14393,    10,   "1607" };
    inline const version_t WIN_10_1703{ 10,  0, 15063,   138,   "1703" };
    inline const version_t WIN_10_1709{ 10,  0, 16299,    19,   "1709" };
    inline const version_t WIN_10_1803{ 10,  0, 17134,    48,   "1803" };
    inline const version_t WIN_10_1809{ 10,  0, 17763,     1,   "1809" };
    inline const version_t WIN_10_1903{ 10,  0, 18362,   116,   "1903" };
    inline const version_t WIN_10_1909{ 10,  0, 18363,   476,   "1909" };
    inline const version_t WIN_10_2004{ 10,  0, 19041,   264,   "2004" };
    inline const version_t WIN_10_20H2{ 10,  0, 19042,   572,   "20H2" };
    inline const version_t WIN_10_21H1{ 10,  0, 19043,   985,   "21H1" };
    inline const version_t WIN_10_21H2{ 10,  0, 19044,   288,   "21H2" };
    inline const version_t WIN_10_22H2{ 10,  0, 19045,  2130,   "22H2" };

    inline const version_t WIN_10 = WIN_10_1507;

    // Windows 11
    inline const version_t WIN_11_21H2{ 10,  0, 22000,   194,   "21H2" };
    inline const version_t WIN_11_22H2{ 10,  0, 22621,   382,   "22H2" };
    inline const version_t WIN_11_23H2{ 10,  0, 22631,  2428,   "23H2" };
    inline const version_t WIN_11_24H2{ 10,  0, 26100,   863,   "24H2" };

    inline const version_t WIN_11 = WIN_11_21H2;
    // clang-format on
} // namespace probe
#endif

#endif //! PROBE_TYPES_H