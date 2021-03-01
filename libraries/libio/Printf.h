#pragma once

#include <libio/Scanner.h>
#include <libio/Write.h>

namespace IO
{

enum class PrintfAlign
{
    LEFT,
    RIGHT
};

struct PrintfFormat
{
    char padding;
    PrintfAlign align;
    size_t length;

    static PrintfFormat scan(Scanner &scan)
    {
        PrintfFormat fmt;
        scan.skip('%');

        return fmt;
    }
};

int printf(Writer &write, const char *fmt, size_t length, va_list va)
{
    MemoryReader memory{fmt, length};
    Scanner scan{memory};

    auto normal = [&]() {
        while (!scan.ended() && scan.current() != '%')
        {
            IO::write_char(write, scan.current());
            scan.forward();
        }
    };

    while (!scan.ended())
    {
        normal();

        if (scan.skip_word("%%"))
        {
            IO::write_char(write, '%');
        }

        if (scan.current() == '%')
        {
            auto format = PrintfFormat::scan(scan);
        }

        normal();
    }
}

} // namespace IO
