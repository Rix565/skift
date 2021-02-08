#pragma once

#include <libutils/String.h>

#include <libsystem/io_new/Format.h>
#include <libsystem/io_new/MemoryWriter.h>
#include <libsystem/io_new/Reader.h>
#include <libsystem/io_new/Scanner.h>
#include <libsystem/io_new/Writer.h>

namespace System
{

Reader &in();

Writer &out();

Writer &err();

Writer &log();

static inline ResultOr<String> inln()
{
    Scanner<Reader &> scan{in()};
    MemoryWriter writer{};

    while (!(scan.ended() || scan.current() == '\n'))
    {
        writer.write(scan.current());
        out().write(scan.current());
        scan.foreward();
    }

    out().write('\n');
    scan.skip('\n');

    return String{writer.string()};
}

template <Formatable... Args>
static ResultOr<size_t> print(Writer &writer, const char *fmt, Args... args)
{
    StringScanner scan{fmt, strlen(fmt)};
    auto format_result = format(writer, scan, forward<Args>(args)...);

    if (!format_result)
    {
        return format_result.result();
    }

    return *format_result;
}

template <Formatable... Args>
static ResultOr<size_t> println(Writer &writer, const char *fmt, Args... args)
{
    StringScanner scan{fmt, strlen(fmt)};
    auto format_result = format(writer, scan, forward<Args>(args)...);

    if (!format_result)
    {
        return format_result.result();
    }

    auto endline_result = writer.write('\n');

    if (endline_result != SUCCESS)
    {
        return endline_result;
    }

    return *format_result + 1;
}

template <Formatable... Args>
static ResultOr<size_t> out(const char *fmt, Args... args)
{
    return print(out(), fmt, forward<Args>(args)...);
}

template <Formatable... Args>
static ResultOr<size_t> outln(const char *fmt, Args... args)
{
    return println(out(), fmt, forward<Args>(args)...);
}

template <Formatable... Args>
static ResultOr<size_t> err(const char *fmt, Args... args)
{
    return print(err(), fmt, forward<Args>(args)...);
}

template <Formatable... Args>
static ResultOr<size_t> errln(const char *fmt, Args... args)
{
    return println(err(), fmt, forward<Args>(args)...);
}

template <Formatable... Args>
static ResultOr<size_t> log(const char *fmt, Args... args)
{
    return print(log(), fmt, forward<Args>(args)...);
}

template <Formatable... Args>
static ResultOr<size_t> logln(const char *fmt, Args... args)
{
    return println(log(), fmt, forward<Args>(args)...);
}

} // namespace System
