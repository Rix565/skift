#pragma once

#include <libsystem/Handle.h>
#include <libsystem/Path.h>

namespace System
{

class Directory :
    public RawHandle
{
public:
    struct Entry
    {
        String name;
        FileState stat;
    };

private:
    RefPtr<System::Handle> _handle;
    Optional<Path> _path;
    Vector<Entry> _entries;

    void read_entries();

public:
    RefPtr<Handle> handle() override { return _handle; }
    const Optional<Path> &path() { return _path; }
    const Vector<Entry> &entries() { return _entries; }

    Directory(const char *path);
    Directory(String path);
    Directory(const Path &path);
    Directory(RefPtr<System::Handle> handle);

    bool exist();
};

} // namespace System
