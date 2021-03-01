#pragma once

#include <libio/Reader.h>
#include <libio/Writer.h>
#include <libsystem/Handle.h>
#include <libsystem/Path.h>

namespace System
{

class Connection final :
    public IO::Reader,
    public IO::Writer,
    public RawHandle
{
private:
    RefPtr<System::Handle> _handle;

public:
    RefPtr<Handle> handle() override { return _handle; }

    Connection(RefPtr<System::Handle> handle)
        : _handle{handle} {}

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        return _handle->read(buffer, size);
    }

    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        return _handle->write(buffer, size);
    }
};

} // namespace System
