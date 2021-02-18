#include <libio/File.h>

namespace System
{

File::File(const char *path, OpenFlag flags)
    : _handle(path, flags | OPEN_STREAM),
      _path{Path::parse(path)}
{
}

File::File(String path, OpenFlag flags)
    : _handle(path, flags | OPEN_STREAM),
      _path{Path::parse(path)}
{
}

File::File(Path &path, OpenFlag flags)
    : _handle{path.string(), flags | OPEN_STREAM},
      _path{path}
{
}

File::File(System::Handle &&handle)
    : _handle{move(handle)}
{
}

ResultOr<size_t> File::read(void *buffer, size_t size)
{
    return _handle.read(buffer, size);
}

ResultOr<size_t> File::write(const void *buffer, size_t size)
{
    return _handle.write(buffer, size);
}

ResultOr<size_t> File::seek(SeekFrom from)
{
    // FIXME: hj_handle_seek should return the current position.
    _handle.seek(pos, whence);
    return pos;
}

ResultOr<size_t> File::tell()
{
    // FIXME: sketchy cast
    return (size_t)_handle.tell(HJ_WHENCE_START).value();
}

ResultOr<size_t> File::length()
{
    auto result_or_stat = _handle.stat();

    if (result_or_stat)
    {
        return result_or_stat->size;
    }
    else
    {
        return 0;
    }
}

bool File::exist()
{
    return _handle.valid();
}

} // namespace System