#include <assert.h>
#include <string.h>

#include <libutils/Strings.h>
#include <libutils/unicode/Codepoint.h>
#include <libio/Scanner.h>
#include <libio/Write.h>
#include <libsystem/Logger.h>
#include <libjson/EscapeSequence.h>

#include <libmarkup/Markup.h>

namespace markup
{

static void whitespace(IO::Scanner &scan)
{
    scan.eat(Strings::WHITESPACE);
}

static String identifier(IO::Scanner &scan)
{
    IO::MemoryWriter memory{};

    while (scan.current_is(Strings::ALL_ALPHA) &&
           scan.do_continue())
    {
        IO::write_char(memory, scan.current());
        scan.forward();
    }

    return memory.string();
}

static String string(IO::Scanner &scan)
{
    IO::MemoryWriter memory{};

    scan.skip('"');

    while (scan.do_continue() &&
           scan.current() != '"')
    {
        if (scan.current() == '\\')
        {
            IO::write_cstring(memory, Json::escape_sequence(scan));
        }
        else
        {
            IO::write_char(memory, scan.current());
            scan.forward();
        }
    }

    scan.skip('"');

    return memory.string();
}

static void attribute(IO::Scanner &scan, Attributes &attr)
{
    auto ident = identifier(scan);

    whitespace(scan);

    if (scan.skip('='))
    {
        whitespace(scan);
        attr[ident] = string(scan);
    }
    else
    {
        attr[ident] = "";
    }
}

static Node opening_tag(IO::Scanner &scan)
{
    if (!scan.skip('<'))
    {
        return {"error"};
    }

    whitespace(scan);

    auto type = identifier(scan);

    whitespace(scan);

    Attributes attr{};

    while (scan.current_is(Strings::ALL_ALPHA) &&
           scan.do_continue())
    {
        attribute(scan, attr);
        whitespace(scan);
    }

    auto flags = Node::NONE;

    if (scan.skip('/'))
    {
        scan.skip('>');

        flags = Node::SELF_CLOSING;
    }

    scan.skip('>');

    return {type, flags, move(attr)};
}

static void closing_tag(IO::Scanner &scan, const Node &node)
{
    scan.skip('<');
    scan.skip('/');

    whitespace(scan);

    auto type = identifier(scan);

    if (!node.is(type))
    {
        logger_warn(
            "Opening tag <%s> doesn't match closing tag </%s>",
            node.type().cstring(),
            type.cstring());
    }

    whitespace(scan);

    scan.skip('>');
}

static Node node(IO::Scanner &scan)
{
    whitespace(scan);

    auto n = opening_tag(scan);

    if (n.flags() & Node::SELF_CLOSING)
    {
        return n;
    }

    whitespace(scan);

    while (scan.peek(0) == '<' &&
           scan.peek(1) != '/' &&
           scan.do_continue())
    {
        n.add_child(node(scan));
        whitespace(scan);
    }

    whitespace(scan);

    closing_tag(scan, n);

    return n;
}

Node parse(IO::Scanner &scan)
{
    scan.skip_utf8bom();
    return node(scan);
}

} // namespace markup
