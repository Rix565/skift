#pragma once

#include <libutils/Callback.h>

#include <libsystem/Handle.h>
#include <libsystem/eventloop/EventLoop.h>

class Notifier
{
private:
    System::Handle &_handle;
    PollEvent _events;
    Callback<void()> _callback;

public:
    System::Handle &handle() { return _handle; }
    PollEvent events() { return _events; }

    Notifier(System::RawHandle &handle, PollEvent events, Callback<void()> callback)
        : _handle{handle.handle()},
          _events{events},
          _callback{callback}
    {
        EventLoop::register_notifier(this);
    }

    ~Notifier()
    {
        EventLoop::unregister_notifier(this);
    }

    void invoke() { _callback(); }
};
