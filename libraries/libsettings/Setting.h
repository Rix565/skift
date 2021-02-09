#pragma once

#include <libsettings/Watcher.h>

namespace Settings
{

class Setting
{
private:
    __noncopyable(Setting);
    __nonmovable(Setting);

    OwnPtr<Watcher> _watcher;
    Settings::Path _path;
    WatcherCallback _callback;
    Optional<json::Value> _value;

public:
    Setting(const char *path, WatcherCallback callback)
    {
        _path = Path::parse(path);
        _callback = callback;

        _callback(read());

        _watcher = own<Watcher>(_path, [this](const auto &value) {
            _value = value;
            _callback(value);
        });
    }

    ~Setting()
    {
    }

    void write(const json::Value &value)
    {
        Settings::write(_path, value);
    }

    json::Value read()
    {
        if (!_value)
        {
            _value = Settings::read(_path);
        }

        return *_value;
    }
};

} // namespace Settings