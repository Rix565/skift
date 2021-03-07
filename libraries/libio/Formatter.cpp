#include <libio/Formatter.h>
#include <libio/NumberScanner.h>
#include <libio/Write.h>

namespace IO
{

Formatter Formatter::parse_printf(Scanner &scanner)
{
    Formatter format;

    auto parse_flags = [&]() {
        switch (scanner.current())
        {
        case '-':
            format._align = LEFT;
            break;

        case '+':
            format._prepend_plus = true;
            break;

        case '<':
            format._align = RIGHT;
            break;

        case '>':
            format._align = LEFT;
            break;

        case '^':
            format._align = CENTER;
            break;

        case ' ':
            format._prepend_space = true;
            break;

        case '0':
            format._prepend_zero = true;
            break;

        case '\'':
            format._group_thousands = true;
            break;

        case '#':
            format._prepend_prefix = true;
            break;

        default:
            ASSERT_NOT_REACHED();
        }
    };

    auto parse_width = [&]() {
        format._width = NumberScanner::decimal().scan_int(scanner);
    };

    auto parse_precison = [&]() {
        format._precision = NumberScanner::decimal().scan_int(scanner);
    };

    auto parse_type = [&]() {
        if (isupper(scanner.current()))
        {
            format._capitalized = true;
        }

        switch (tolower(scanner.current()))
        {
        case 's':
            format._type = STRING;
            break;

        case 'c':
            format._type = CHARACTER;
            break;

        case 'b':
            format = format.binary();
            break;

        case 'd':
            format = format.decimal();
            break;

        case 'o':
            format._type = NUMBER;
            format._base = 8;
            break;

        case 'p':
            format._type = NUMBER;
            format._base = 16;
            format._prepend_prefix = true;
            format._prepend_zero = true;
            break;

        case 'x':
            format._type = NUMBER;
            format._base = 16;
            break;

        default:
            ASSERT_NOT_REACHED();
        }
    };

    while (scanner.current_is("-+<>^ 0'#"))
    {
        parse_flags();
        scanner.forward();
    }

    if (scanner.current_is(Strings::DIGITS))
    {
        parse_width();
    }

    if (scanner.skip('.'))
    {
        parse_precison();
    }

    if (scanner.current_is("SsCcBbDdOoPpXx"))
    {
        parse_type();
        scanner.forward();
    }

    return format;
}

Formatter Formatter::parse(Scanner &scanner)
{
    scanner.skip('{');
    Formatter format = parse_printf(scanner);

    // eat the rest
    while (!scanner.ended() && scanner.current() != '}')
    {
        scanner.forward();
    }
    scanner.skip('}');

    return format;
}

ResultOr<size_t> Formatter::format_unsigned(Writer &, uint64_t) const
{
}

ResultOr<size_t> Formatter::format_signed(Writer &, int64_t) const
{
}

ResultOr<size_t> Formatter::format_float(Writer &, double) const
{
}

ResultOr<size_t> Formatter::format_character(Writer &, Codepoint) const
{
}

ResultOr<size_t> Formatter::format_string(Writer &, const char *, size_t) const
{
}

ResultOr<size_t> Formatter::format(Writer &writer, char value) const
{
    if (_type == CHARACTER)
    {
        if (value < 0)
        {
            return format_character(writer, U'�');
        }
        else
        {
            return format_character(writer, value);
        }
    }
    else
    {
        return format_signed(writer, value);
    }
}

ResultOr<size_t> Formatter::format(Writer &writer, unsigned char value) const
{
    if (_type == CHARACTER)
    {
        return format_character(writer, value);
    }
    else
    {
        return format_unsigned(writer, value);
    }
}

ResultOr<size_t> Formatter::format(Writer &writer, short int value) const
{
    if (_type == CHARACTER)
    {
        if (value < 0)
        {
            return format_character(writer, U'�');
        }
        else
        {
            return format_character(writer, value);
        }
    }
    else
    {
        return format_signed(writer, value);
    }
}

ResultOr<size_t> Formatter::format(Writer &writer, unsigned short int value) const
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> format(Writer &writer, int value)
{
    return NumberFormat::decimal().format(writer, (int64_t)value);
}

ResultOr<size_t> Formatter::format(Writer &writer, unsigned int value) const
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> Formatter::format(Writer &writer, long int value) const
{
    return NumberFormat::decimal().format(writer, (int64_t)value);
}

ResultOr<size_t> Formatter::format(Writer &writer, unsigned long int value) const
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> Formatter::format(Writer &writer, float value) const
{
    return NumberFormat::decimal().format(writer, value);
}

ResultOr<size_t> Formatter::format(Writer &writer, double value) const
{
    return NumberFormat::decimal().format(writer, value);
}

ResultOr<size_t> Formatter::format(Writer &writer, const char *cstring) const
{
    return writer.write(cstring, strlen(cstring));
}

ResultOr<size_t> Formatter::format(Writer &writer, const String string) const
{
    return write_string(writer, string);
}

} // namespace IO
