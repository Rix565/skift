#pragma once

#include <abi/Syscalls.h>

#include <libutils/Optional.h>
#include <libutils/ResultOr.h>
#include <libutils/String.h>
#include <libio/Seek.h>
#include <libsystem/process/Process.h>

namespace System
{

class Handle : public RefCounted<Handle>
{
private:
    int _handle = HANDLE_INVALID_ID;
    Result _result = ERR_BAD_HANDLE;

    __noncopyable(Handle);
    __nonmovable(Handle);

public:
    Handle(int handle) : _handle(handle), _result(handle != HANDLE_INVALID_ID ? SUCCESS : ERR_BAD_HANDLE)
    {
    }

    Handle(const String path, OpenFlag flags)
    {
        auto resolved_path = process_resolve(path);
        _result = hj_handle_open(&_handle, resolved_path.cstring(), resolved_path.length(), flags);
    }

    ~Handle()
    {
        close();
    }

    Result close()
    {
        if (_handle == HANDLE_INVALID_ID)
        {
            return ERR_STREAM_CLOSED;
        }

        auto result = hj_handle_close(_handle);
        _handle = HANDLE_INVALID_ID;
        return result;
    }

    ResultOr<size_t> read(void *buffer, size_t size)
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        size_t data_read = 0;
        _result = hj_handle_read(_handle, buffer, size, &data_read);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return data_read;
        }
    }

    ResultOr<size_t> write(const void *buffer, size_t size)
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        size_t data_written = 0;
        _result = hj_handle_write(_handle, buffer, size, &data_written);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return data_written;
        }
    }

    Result call(IOCall request, void *args)
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        _result = hj_handle_call(_handle, request, args);
        return _result;
    }

    Result seek(IO::SeekFrom from)
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        _result = hj_handle_seek(_handle, from.position, (HjWhence)from.whence);
        return _result;
    }

    ResultOr<int> tell()
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        int offset = 0;
        _result = hj_handle_tell(_handle, HJ_WHENCE_START, &offset);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return offset;
        }
    }

    ResultOr<FileState> stat()
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        FileState stat{};
        _result = hj_handle_stat(_handle, &stat);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return stat;
        }
    }

    ResultOr<RefPtr<Handle>> accept()
    {
        if (!valid())
        {
            return ERR_STREAM_CLOSED;
        }

        int connection_handle;
        _result = hj_handle_accept(_handle, &connection_handle);

        if (_result != SUCCESS)
        {
            return _result;
        }
        else
        {
            return make<Handle>(connection_handle);
        }
    }

    bool valid()
    {
        return _handle != HANDLE_INVALID_ID;
    }

    Result result()
    {
        return _result;
    }
};

struct RawHandle
{
    virtual RefPtr<Handle> handle() = 0;
};

bool valid(RawHandle &raw)
{
    return raw.handle() != nullptr && raw.handle()->valid();
}

bool close(RawHandle &raw)
{
    return raw.handle() != nullptr && raw.handle()->close() == SUCCESS;
}

} // namespace System
