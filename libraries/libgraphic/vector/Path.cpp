#include <libio/NumberScanner.h>
#include <libgraphic/vector/Path.h>

namespace Graphic
{
static constexpr auto WHITESPACE = "\n\r\t ";
static constexpr auto OPERATIONS = "MmZzLlHhVvCcSsQqTtAa";

static void whitespace(IO::Scanner &scan)
{
    scan.eat(WHITESPACE);
}

static void whitespace_or_comma(IO::Scanner &scan)
{
    whitespace(scan);

    if (scan.skip(','))
    {
        whitespace(scan);
    }
}

static float number(IO::Scanner &scan)
{
    return *IO::NumberScanner::decimal().scan_float(scan);
}

static Vec2f coordinate(IO::Scanner &scan)
{
    auto x = number(scan);
    whitespace_or_comma(scan);
    auto y = number(scan);
    whitespace_or_comma(scan);

    return Vec2f{(float)x, (float)y};
}

static int arcflags(IO::Scanner &scan)
{
    int flags = 0;

    if (scan.current_is("1"))
    {
        flags |= Arc::LARGE;
    }
    scan.forward();

    whitespace_or_comma(scan);

    if (scan.current_is("1"))
    {
        flags |= Arc::SWEEP;
    }

    scan.forward();

    whitespace_or_comma(scan);

    return flags;
}

static void operation(IO::Scanner &scan, Path &path, char operation)
{
    switch (operation)
    {
    case 'M':
        path.begin_subpath(coordinate(scan));

        whitespace(scan);

        // If a moveto is followed by multiple pairs of coordinates,
        // the subsequent pairs are treated as implicit lineto commands.
        while (scan.do_continue() &&
               !scan.current_is(OPERATIONS))
        {
            path.line_to(coordinate(scan));
        }
        break;

    case 'm':
        path.begin_subpath_relative(coordinate(scan));

        whitespace(scan);

        // If a moveto is followed by multiple pairs of coordinates,
        // the subsequent pairs are treated as implicit lineto commands.
        while (scan.do_continue() &&
               !scan.current_is(OPERATIONS))
        {
            path.line_to_relative(coordinate(scan));
        }
        break;

    case 'Z':
    case 'z':
        path.close_subpath();
        break;

    case 'L':
        path.line_to(coordinate(scan));
        break;

    case 'l':
        path.line_to_relative(coordinate(scan));
        break;

    case 'H':
        path.hline_to(number(scan));
        break;

    case 'h':
        path.hline_to_relative(number(scan));
        break;

    case 'V':
        path.vline_to(number(scan));
        break;

    case 'v':
        path.vline_to_relative(number(scan));
        break;

    case 'C':
    {
        auto cp1 = coordinate(scan);
        auto cp2 = coordinate(scan);
        auto point = coordinate(scan);

        path.cubic_bezier_to(cp1, cp2, point);

        break;
    }

    case 'c':
    {
        auto cp1 = coordinate(scan);
        auto cp2 = coordinate(scan);
        auto point = coordinate(scan);

        path.cubic_bezier_to_relative(cp1, cp2, point);

        break;
    }

    case 'S':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.smooth_cubic_bezier_to(cp, point);

        break;
    }

    case 's':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.smooth_cubic_bezier_to_relative(cp, point);

        break;
    }

    case 'Q':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.quad_bezier_to(cp, point);

        break;
    }

    case 'q':
    {
        auto cp = coordinate(scan);
        auto point = coordinate(scan);
        path.quad_bezier_to_relative(cp, point);

        break;
    }

    case 'T':
    {
        path.smooth_quad_bezier_to(coordinate(scan));
        break;
    }

    case 't':
    {
        path.smooth_quad_bezier_to_relative(coordinate(scan));
        break;
    }

    case 'A':
    {
        auto rx = number(scan);
        whitespace_or_comma(scan);

        auto ry = number(scan);
        whitespace_or_comma(scan);

        auto a = number(scan);
        whitespace_or_comma(scan);

        auto flags = arcflags(scan);

        auto point = coordinate(scan);

        path.arc_to(rx, ry, a, flags, point);
        break;
    }

    case 'a':
    {
        auto rx = number(scan);
        whitespace_or_comma(scan);

        auto ry = number(scan);
        whitespace_or_comma(scan);

        auto a = number(scan);
        whitespace_or_comma(scan);

        auto flags = arcflags(scan);

        auto point = coordinate(scan);
        path.arc_to_relative(rx, ry, a, flags, point);

        break;
    }

    default:
        break;
    }
}

Path Path::parse(IO::Scanner &scan)
{
    Path path;

    whitespace(scan);

    if (scan.skip("none"))
    {
        // None indicates that there is no path data for the element
        return path;
    }

    while (scan.do_continue() &&
           scan.current_is(OPERATIONS))
    {
        char op = scan.current();
        scan.forward();

        do
        {
            whitespace(scan);
            operation(scan, path, op);
            whitespace_or_comma(scan);
        } while (scan.do_continue() && !scan.current_is(OPERATIONS));
    }

    return path;
}

} // namespace Graphic
