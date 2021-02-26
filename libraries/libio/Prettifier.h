#pragma once

#include <libio/Write.h>

namespace IO
{

class Prettifier :
    public Writer
{
private:
    Writer &_writer;
    int _flags;

    int _depth = 0;
    Optional<char> _buffer;

public:
    using Writer::write;

    static constexpr auto NONE = 0;
    static constexpr auto COLORS = 1 << 0;
    static constexpr auto INDENTS = 1 << 1;

    Prettifier(Writer &writer, int flags = NONE)
        : _writer(writer),
          _flags(flags)
    {
    }

    void ident()
    {
        if (_flags & INDENTS)
        {
            write_char(_writer, '\n');

            for (int i = 0; i < _depth; i++)
            {
                write_cstring(_writer, "    ");
            }
        }
    }

    void push_ident()
    {
        _depth++;
    }

    void pop_ident()
    {
        assert(_depth);
        _depth--;
    }

    void color_depth()
    {
        if (_flags & COLORS)
        {
            const char *depth_color[] = {
                "\e[91m",
                "\e[92m",
                "\e[93m",
                "\e[94m",
                "\e[95m",
                "\e[96m",
            };

            write_cstring(_writer, depth_color[_depth % 6]);
        }
    }

    void color_clear()
    {
        if (_flags & COLORS)
        {
            write_cstring(_writer, "\e[m");
        }
    }

    virtual ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        if (_buffer)
        {
            flush();
        }

        ResultOr<size_t> write_result = 0;

        if (size > 1)
        {
            write_result = _writer.write(buffer, size - 1);
        }

        if (!write_result)
        {
            return write_result;
        }

        _buffer = ((char *)buffer)[size - 1];

        return *write_result + 1;
    }

    virtual Result flush()
    {
        if (_buffer)
        {
            auto write_result = write_char(_writer, *_buffer);

            if (write_result)
            {
                _buffer.clear();
            }

            return write_result.result();
        }
        else
        {
            return SUCCESS;
        }
    }

    char rewind()
    {
        char chr = *_buffer;
        _buffer.clear();
        return chr;
    }
};

} // namespace IO