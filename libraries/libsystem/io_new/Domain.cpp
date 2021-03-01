#include <libsystem/Handle.h>
#include <libsystem/io/Filesystem.h>
#include <abi/Syscalls.h>

namespace System
{

Result link(const String &old_path, const String &new_path)
{
    return hj_filesystem_link(
        old_path.cstring(),
        old_path.length(),
        new_path.cstring(),
        new_path.length());
}

Result unlink(const String &path)
{
    return hj_filesystem_unlink(path.cstring(), path.length());
}

Result mkdir(const String &path)
{
    return hj_filesystem_mkdir(path.cstring(), path.length());
}

Result mkpipe(const String &path)
{
    return hj_filesystem_mkpipe(path.cstring(), path.length());
}

Result rename(const String &old_path, const String &new_path)
{
    return hj_filesystem_rename(
        old_path.cstring(),
        old_path.length(),
        new_path.cstring(),
        new_path.length());
}

bool exist(const String &path, FileType type)
{
    Handle handle{path, 0};

    if (!handle.valid())
    {
        return false;
    }

    auto stat_result = handle.stat();

    if (!stat_result)
    {
        return false;
    }

    return ((*stat_result).type == type);
}

} // namespace System