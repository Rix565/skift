#include <abi/Syscalls.h>
#include <skift/Environment.h>

#include <libsystem/Path.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/process/Process.h>

Result __plug_process_get_directory(char *buffer, size_t size)
{
    auto &pwd = environment().get("POSIX").get("PWD");
    strncpy(buffer, pwd.as_string().cstring(), size);
    return SUCCESS;
}

Result __plug_process_set_directory(const char *path)
{
    auto new_path = process_resolve(path);

    System::Directory directory(new_path);

    if (!directory.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    environment().get("POSIX").put("PWD", new_path);

    return SUCCESS;
}

String __plug_process_resolve(String raw_path)
{
    if (!environment().has("POSIX") ||
        !environment().get("POSIX").has("PWD") ||
        !environment().get("POSIX").get("PWD").is(Json::STRING))
    {
        return raw_path;
    }

    auto &raw_pwd = environment().get("POSIX").get("PWD");

    auto path = System::Path::parse(raw_path);

    if (!path.absolute())
    {
        auto pwd = System::Path::parse(raw_pwd.as_string());

        path = System::Path::join(pwd, path);
    }

    return path.normalized().string();
}
