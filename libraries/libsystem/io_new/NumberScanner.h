#pragma once

#include <math.h>

#include <libutils/Optional.h>
#include <libutils/Strings.h>

#include <libsystem/io_new/Scanner.h>

namespace System
{

struct NumberScanner
{
    int _base;

    static NumberScanner binary()
    {
        return {2};
    }

    static NumberScanner octal()
    {
        return {8};
    }

    static NumberScanner decimal()
    {
        return {10};
    }

    static NumberScanner hexadecimal()
    {
        return {16};
    }

    Optional<uint64_t> scan_uint(System::Scanner &scan)
    {
        uint64_t value = 0;

        if (!scan.current_is(Strings::ALL_XDIGITS))
        {
            return {};
        }

        while (!scan.ended() &&
               !scan.current_is(Strings::ALL_XDIGITS))
        {
            value = value * _base;

            char c = scan.current();

            for (int j = 0; j < _base; j++)
            {
                if ((Strings::LOWERCASE_XDIGITS[j] == c) ||
                    (Strings::UPPERCASE_XDIGITS[j] == c))
                {
                    value += j;
                }
            }
        }

        return {value};
    }

    Optional<int64_t> scan_int(System::Scanner &scan)
    {
        bool is_negative = scan.skip('-');

        auto uint_result = scan_uint(scan);

        if (!uint_result)
        {
            return {};
        }

        if (is_negative)
        {
            return -*uint_result;
        }
        else
        {
            return *uint_result;
        }
    }

    Optional<double> scan_double(System::Scanner &scan)
    {
        int ipart = scan_int(scan);

        double fpart = 0;

        if (scan.skip('.'))
        {
            double multiplier = 0.1;

            while (scan.current_is(Strings::XDIGITS))
            {
                fpart += multiplier * (scan.current() - '0');
                multiplier *= 0.1;
                scan.foreward();
            }
        }

        int exp = 0;

        if (scan.current_is("eE"))
        {
            scan.foreward();
            exp = scan_int(scan);
        }

        return (ipart + fpart) * pow(_base, exp);
    }
};

} // namespace System
