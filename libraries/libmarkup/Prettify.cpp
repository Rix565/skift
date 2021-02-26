#include <libio/Format.h>
#include <libmarkup/Markup.h>

namespace markup
{

void prettify(IO::Prettifier &pretty, Node &node)
{
    IO::write_char(pretty, '<');

    pretty.color_depth();
    IO::write_string(pretty, node.type());
    pretty.color_clear();

    node.foreach_attributes([&](auto &key, auto &value) {
        IO::format(pretty, " {}", key);

        if (value != "")
        {
            IO::format(pretty, "=\"{}\"", value);
        }

        return Iteration::CONTINUE;
    });

    if (node.count_child() == 0)
    {
        IO::write_cstring(pretty, "/>");
        return;
    }
    else
    {
        IO::write_char(pretty, '>');
    }

    node.foreach_child([&](auto &child) {
        pretty.push_ident();
        pretty.ident();

        prettify(pretty, child);

        pretty.pop_ident();

        return Iteration::CONTINUE;
    });

    pretty.ident();

    IO::write_cstring(pretty, "</");

    pretty.color_depth();
    IO::write_string(pretty, node.type());
    pretty.color_clear();

    IO::write_char(pretty, '>');
}

} // namespace markup
