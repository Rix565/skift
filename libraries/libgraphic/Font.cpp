#include <assert.h>
#include <stdio.h>

#include <libutils/HashMap.h>
#include <libio/Format.h>
#include <libio/Read.h>
#include <libsystem/Logger.h>
#include <libsystem/Path.h>
#include <libsystem/Result.h>
#include <libsystem/io_new/File.h>
#include <libgraphic/Font.h>

static HashMap<String, RefPtr<Font>> _fonts;

static ResultOr<Vector<Glyph>> font_load_glyph(String name)
{
    auto glyph_path = IO::format("/Files/Fonts/{}.glyph", name);

    Glyph *glyph_buffer = nullptr;
    size_t glyph_size = 0;

    System::File glyph_file{glyph_path, OPEN_READ};
    auto slice_or_result = IO::read_slice(glyph_file);

    if (!slice_or_result)
    {
        return slice_or_result.result();
    }

    return Vector(ADOPT, glyph_buffer, glyph_size / sizeof(Glyph));
}

static ResultOr<RefPtr<Bitmap>> font_load_bitmap(String name)
{
    return Bitmap::load_from(IO::format("/Files/Fonts/{}.png", name));
}

ResultOr<RefPtr<Font>> Font::get(String name)
{
    if (!_fonts.has_key(name))
    {
        auto glyph_or_error = font_load_glyph(name);

        if (!glyph_or_error.success())
        {
            logger_error("Failed to load font %s: missing glyphs", name.cstring());
            return glyph_or_error.result();
        }

        auto bitmap_or_error = font_load_bitmap(name);

        if (!bitmap_or_error.success())
        {
            logger_error("Failed to load font %s: missing bitmap", name.cstring());
            return bitmap_or_error.result();
        }

        _fonts[name] = make<Font>(bitmap_or_error.take_value(), glyph_or_error.take_value());
    }

    return _fonts[name];
}

bool Font::has(Codepoint codepoint) const
{
    for (int i = 0; _glyphs[i].codepoint != 0; i++)
    {
        if (_glyphs[i].codepoint == codepoint)
        {
            return true;
        }
    }

    return false;
}

const Glyph &Font::glyph(Codepoint codepoint) const
{
    for (int i = 0; _glyphs[i].codepoint != 0; i++)
    {
        if (_glyphs[i].codepoint == codepoint)
        {
            return _glyphs[i];
        }
    }

    return _default;
}

Recti Font::mesure(Codepoint codepoint) const
{
    auto &g = glyph(codepoint);

    return {g.advance, metrics().lineheight()};
}

Recti Font::mesure(const char *string) const
{
    int width = 0;

    codepoint_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto codepoint) {
        auto &g = glyph(codepoint);
        width += g.advance;
    });

    return Recti(width, metrics().lineheight());
}

Recti Font::mesure_with_fulllineheight(const char *string)
{
    int width = 0;

    codepoint_foreach(reinterpret_cast<const uint8_t *>(string), [&](auto codepoint) {
        auto &g = glyph(codepoint);
        width += g.advance;
    });

    return Recti(width, metrics().fulllineheight());
}
