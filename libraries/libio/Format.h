#pragma once

#include <libio/Formatter.h>
#include <libio/MemoryReader.h>
#include <libio/NumberScanner.h>
#include <libio/Scanner.h>
#include <libio/Write.h>

class String;

namespace IO
{

struct Format
{
    virtual ResultOr<size_t> format(Writer &writer);
};

template <typename T>
concept Formatable = IsBaseOf<Format, T>::value || requires(Writer &writer, Formatter &fmt, const T &t)
{
    fmt.format(writer, t);
};

static inline ResultOr<size_t> format(Writer &writer, Scanner &scanner)
{
    size_t written = 0;

    while (!scanner.ended())
    {
        auto result = write_char(writer, scanner.current());

        if (result != SUCCESS)
        {
            return result;
        }

        written += 1;
        scanner.forward();
    }

    return written;
}

template <Formatable First, Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, Scanner &scanner, First first, Args... args)
{
    size_t written = 0;

    while (!(scanner.ended() || scanner.current() == '{'))
    {
        auto result = write_char(writer, scanner.current());

        if (result != SUCCESS)
        {
            return result;
        }

        written += 1;
        scanner.forward();
    }

    if (scanner.current() == '{')
    {
        auto formatter = Formatter::parse(scanner);

        auto format_proxy = [&]() {
            if constexpr (IsBaseOf<Format, First>::value)
            {
                return first.format(writer);
            }
            else if constexpr (requires(const First &t) {
                                   formatter.format(writer, t);
                               })
            {
                return formatter.format(writer, first);
            }
            else
            {
                ASSERT_NOT_REACHED();
            }
        };

        auto format_result = format_proxy();

        if (format_result)
        {
            written += *format_result;
        }
        else
        {
            return format_result.result();
        }
    }

    if (!scanner.ended())
    {
        auto format_result = format(writer, scanner, forward<Args>(args)...);

        if (format_result)
        {
            return written + *format_result;
        }
        else
        {
            return format_result.result();
        }
    }

    return written;
}

template <Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, const char *fmt, Args... args)
{
    Slice slice{fmt};
    MemoryReader reader{slice};
    Scanner scan{reader};

    return format(writer, scan, forward<Args>(args)...);
}

static inline ResultOr<size_t> format(Writer &writer, const char *fmt)
{
    return write_cstring(writer, fmt);
}

template <IO::Formatable... Args>
static inline String format(const char *fmt, Args... args)
{
    MemoryWriter memory{};
    format(memory, fmt, forward<Args>(args)...);

    return memory.string();
}

} // namespace IO
