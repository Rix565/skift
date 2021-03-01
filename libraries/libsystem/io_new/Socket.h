#pragma once

#include <libsystem/Handle.h>
#include <libsystem/io_new/Connection.h>

namespace System
{

class Socket :
    public RawHandle
{
private:
    RefPtr<Handle> _handle;

public:
    RefPtr<Handle> handle() { return _handle; }

    Socket(String &path, OpenFlag flags);

    static ResultOr<Connection> connect(String path);

    ResultOr<Connection> accept();
};

} // namespace System
