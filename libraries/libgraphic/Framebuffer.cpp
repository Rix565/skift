
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/core/Plugs.h>
#include <libgraphic/Framebuffer.h>

ResultOr<OwnPtr<Framebuffer>> Framebuffer::open()
{
    System::Handle handle{FRAMEBUFFER_DEVICE_PATH, OPEN_READ | OPEN_WRITE};

    if (!handle.valid())
    {
        return handle.result();
    }

    IOCallDisplayModeArgs mode_info = {};
    auto result = handle.call(IOCALL_DISPLAY_GET_MODE, &mode_info);

    if (result != SUCCESS)
    {
        return result;
    }

    auto bitmap_or_error = Bitmap::create_shared(mode_info.width, mode_info.height);

    if (!bitmap_or_error.success())
    {
        return bitmap_or_error.result();
    }

    return own<Framebuffer>(move(handle), bitmap_or_error.take_value());
}

Framebuffer::Framebuffer(System::Handle &&handle, RefPtr<Bitmap> bitmap)
    : _handle{move(handle)},
      _bitmap{bitmap},
      _painter{bitmap}
{
}

Result Framebuffer::set_resolution(Vec2i size)
{
    auto bitmap_or_result = Bitmap::create_shared(size.x(), size.y());

    if (!bitmap_or_result.success())
    {
        return bitmap_or_result.result();
    }

    IOCallDisplayModeArgs mode_info = (IOCallDisplayModeArgs){size.x(), size.y()};

    auto result = _handle.call(IOCALL_DISPLAY_SET_MODE, &mode_info);

    if (result != SUCCESS)
    {
        return result;
    }

    _bitmap = bitmap_or_result.take_value();
    _painter = Painter(_bitmap);

    return SUCCESS;
}

void Framebuffer::mark_dirty(Recti new_bound)
{
    new_bound = _bitmap->bound().clipped_with(new_bound);

    if (new_bound.is_empty())
    {
        return;
    }

    bool merged = false;

    _dirty_bounds.foreach ([&](Recti &region) {
        int region_area = region.area();
        int merge_area = region.merged_with(new_bound).area();

        if (region.colide_with(new_bound) && (region_area + new_bound.area() > merge_area))
        {
            region = region.merged_with(new_bound);
            merged = true;

            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    if (!merged)
    {
        _dirty_bounds.push_back(new_bound);
    }
}

void Framebuffer::mark_dirty_all()
{
    _dirty_bounds.clear();
    mark_dirty(_bitmap->bound());
}

void Framebuffer::blit()
{
    if (_dirty_bounds.empty())
    {
        return;
    }
    _dirty_bounds.foreach ([&](auto &bound) {
        IOCallDisplayBlitArgs args;

        args.buffer = reinterpret_cast<uint32_t *>(_bitmap->pixels());
        args.buffer_width = _bitmap->width();
        args.buffer_height = _bitmap->height();

        args.blit_x = bound.x();
        args.blit_y = bound.y();
        args.blit_width = bound.width();
        args.blit_height = bound.height();

        auto result = _handle.call(IOCALL_DISPLAY_BLIT, &args);

        if (result != SUCCESS)
        {
            logger_error("Failed to iocall device " FRAMEBUFFER_DEVICE_PATH ": %s", get_result_description(result));
            return Iteration::STOP;
        }

        return Iteration::CONTINUE;
    });

    _dirty_bounds.clear();
}
