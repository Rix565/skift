#pragma once

#include <libio/Reader.h>
#include <libio/Writer.h>
#include <libsystem/Handle.h>
#include <libsystem/Path.h>

namespace System
{

class File final :
    public IO::Reader,
    public IO::Writer,
    public IO::Seek,
    public RawHandle
{
private:
    RefPtr<System::Handle> _handle;
    Optional<Path> _path;

public:
    RefPtr<Handle> handle() override { return _handle; }
    const Optional<Path> &path() { return _path; }

    File();
    File(const char *path, OpenFlag flags);
    File(String path, OpenFlag flags);
    File(Path &path, OpenFlag flags);
    File(RefPtr<System::Handle> handle);

    ResultOr<size_t> read(void *buffer, size_t size) override;
    ResultOr<size_t> write(const void *buffer, size_t size) override;

    ResultOr<size_t> seek(IO::SeekFrom from) override;
    ResultOr<size_t> tell() override;

    ResultOr<size_t> length() override;

    bool exist();
};

} // namespace System
