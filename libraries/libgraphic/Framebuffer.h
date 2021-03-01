#pragma once

#include <libutils/OwnPtr.h>
#include <libsystem/Handle.h>
#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>

class Framebuffer
{
private:
    RefPtr<System::Handle> _handle;

    RefPtr<Bitmap> _bitmap;
    Painter _painter;

    Vector<Recti> _dirty_bounds{};

public:
    static ResultOr<OwnPtr<Framebuffer>> open();

    Painter &painter() { return _painter; }

    Recti resolution() { return _bitmap->bound(); }

    Framebuffer(RefPtr<System::Handle> _handle, RefPtr<Bitmap> bitmap);

    Result set_resolution(Vec2i size);

    void mark_dirty(Recti rectangle);

    void mark_dirty_all();

    void blit();
};
