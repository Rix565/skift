#pragma once

#include <libsystem/io_new/Socket.h>

namespace System
{

Socket::Socket(String &path, OpenFlag flags)
    : _handle{make<Handle>(path, flags)}
{
}

Socket::~Socket()
{
}

ResultOr<Connection> Socket::connect(String path)
{
    int hnd;
    TRY(hj_handle_connect(&hnd, path.cstring(), path.length()));

    auto connection_handle = make<Handle>(hnd);
    return Connection{connection_handle};
}

ResultOr<Connection> Socket::accept()
{
    auto accept_result = _handle->accept();
    TRY(accept_result);
    return Connection{*accept_result};
}

} // namespace System
