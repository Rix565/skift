#include <libsystem/io_new/File.h>

namespace System
{

File::File()
{
}

File::File(const char *path, OpenFlag flags)
    : _handle{make<Handle>(path, flags | OPEN_STREAM)},
      _path{Path::parse(path)}
{
}

File::File(String path, OpenFlag flags)
    : _handle{make<Handle>(path, flags | OPEN_STREAM)},
      _path{Path::parse(path)}
{
}

File::File(Path &path, OpenFlag flags)
    : _handle{make<Handle>(path.string(), flags | OPEN_STREAM)},
      _path{path}
{
}

ResultOr<size_t> File::read(void *buffer, size_t size)
{
    if (!_handle)
        return ERR_STREAM_CLOSED;

    return _handle->read(buffer, size);
}

ResultOr<size_t> File::write(const void *buffer, size_t size)
{
    if (!_handle)
        return ERR_STREAM_CLOSED;

    return _handle->write(buffer, size);
}

ResultOr<size_t> File::seek(IO::SeekFrom from)
{
    if (!_handle)
        return ERR_STREAM_CLOSED;

    // FIXME: hj_handle_seek should return the current position.
    _handle->seek(from);
    return (size_t)_handle->tell().value();
}

ResultOr<size_t> File::tell()
{
    if (!_handle)
        return ERR_STREAM_CLOSED;

    // FIXME: sketchy cast
    return (size_t)_handle->tell().value();
}

ResultOr<size_t> File::length()
{
    if (!_handle)
        return ERR_STREAM_CLOSED;

    auto result_or_stat = _handle->stat();

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
    if (!_handle)
        return ERR_STREAM_CLOSED;

    return _handle->valid();
}

} // namespace System