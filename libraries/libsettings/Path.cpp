#include <libio/Format.h>
#include <libsettings/Path.h>

namespace Settings
{

Path Path::parse(IO::Scanner &scan)
{
    Path path;

    auto parse_string = [&](char sep) {
        IO::MemoryWriter memory;
        while (scan.current() != sep && scan.do_continue())
        {
            IO::write_char(memory, scan.current());
            scan.forward();
        }

        return memory.string();
    };

    path.domain = parse_string(':');

    if (!scan.skip(':'))
    {
        return path;
    }

    path.bundle = parse_string('.');

    if (!scan.skip('.'))
    {
        return path;
    }

    path.key = parse_string('\0');

    return path;
}

Path Path::parse(const String &str)
{
    IO::MemoryReader reader{str.slice()};
    IO::Scanner scan{reader};
    return parse(scan);
};

Path Path::parse(const char *str, size_t size)
{
    IO::MemoryReader reader{str, size};
    IO::Scanner scan{reader};
    return parse(scan);
};

void Path::prettify(IO::Prettifier &pretty) const
{
    IO::format(pretty, "{}:{}.{}", domain, bundle, key);
}

bool Path::match(const Path &other) const
{
    return (other.domain == "*" || other.domain == domain) &&
           (other.bundle == "*" || other.bundle == bundle) &&
           (other.key == "*" || other.key == key);
}

bool Path::operator==(const Path &other)
{
    return domain == other.domain &&
           bundle == other.domain &&
           key == other.key;
}

} // namespace Settings
