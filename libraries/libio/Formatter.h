#pragma once

#include <ctype.h>

#include <libutils/Strings.h>
#include <libio/Scanner.h>

namespace IO
{

class Formatter
{
public:
    enum Align
    {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum Type
    {
        DEFAULT,
        STRING,
        CHARACTER,
        NUMBER,
    };

private:
    Align _align;
    Type _type = DEFAULT;
    bool _capitalized = false;
    bool _group_thousands = false;

    int _width = 0;
    int _precision = 5;
    int _base = 10;

    bool _prepend_prefix = false; // like 0x 0b etc
    bool _prepend_plus = false;
    bool _prepend_space = false;
    bool _prepend_zero = false;

    ResultOr<size_t> format_unsigned(Writer &, uint64_t) const;
    ResultOr<size_t> format_signed(Writer &, int64_t) const;
    ResultOr<size_t> format_float(Writer &, double) const;
    ResultOr<size_t> format_character(Writer &, Codepoint) const;
    ResultOr<size_t> format_string(Writer &, const char *, size_t) const;

public:
    static Formatter parse_printf(Scanner &scanner);

    static Formatter parse(Scanner &scanner);

    Formatter binary() const
    {
        auto copy = *this;
        copy._type = NUMBER;
        copy._base = 2;
        return copy;
    }

    Formatter octal() const
    {
        auto copy = *this;
        copy._type = NUMBER;
        copy._base = 8;
        return copy;
    }

    Formatter decimal() const
    {
        auto copy = *this;
        copy._type = NUMBER;
        copy._base = 10;
        return copy;
    }

    Formatter hexadecimal() const
    {
        auto copy = *this;
        copy._type = NUMBER;
        copy._base = 16;
        return copy;
    }

    Formatter capitalized() const
    {
        auto copy = *this;
        copy._capitalized = true;
        return copy;
    }

    ResultOr<size_t> format(Writer &, char) const;
    ResultOr<size_t> format(Writer &, unsigned char) const;
    ResultOr<size_t> format(Writer &, short int) const;
    ResultOr<size_t> format(Writer &, unsigned short int) const;
    ResultOr<size_t> format(Writer &, int) const;
    ResultOr<size_t> format(Writer &, unsigned int) const;
    ResultOr<size_t> format(Writer &, long int) const;
    ResultOr<size_t> format(Writer &, unsigned long int) const;
    ResultOr<size_t> format(Writer &, float) const;
    ResultOr<size_t> format(Writer &, double) const;
    ResultOr<size_t> format(Writer &, const char *) const;
    ResultOr<size_t> format(Writer &, const String) const;
};

} // namespace IO
