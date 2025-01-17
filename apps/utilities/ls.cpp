
#include <libio/Directory.h>
#include <libio/Streams.h>
#include <libsystem/Logger.h>
#include <libutils/ArgParse.h>

static bool option_all = false;
static bool option_list = false;
static bool option_color = false;

const char *file_type_name[] = {
    "-", // None
    "-", // File
    "c", // Char device
    "d", // Directory
    "p", // Pipe
};

void ls_print_entry(IO::Directory::Entry &entry)
{
    FileState stat = entry.stat;

    if (option_list)
    {
        IO::format(IO::out(), "{}rwxrwxrwx {5} ", file_type_name[stat.type], stat.size);
    }

    if (option_all || entry.name[0] != '.')
    {
        IO::format(IO::out(), (stat.type == FILE_TYPE_DIRECTORY && option_color) ? "\e[1;34m{}\e[0m/ " : "{}  ", entry.name);
    }

    if (option_list)
    {
        IO::write(IO::out(), "\n");
    }
}

Result ls(String target_path, bool with_prefix)
{
    IO::Directory directory{target_path};

    if (!directory.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (with_prefix)
    {
        IO::outln("{}:", target_path);
    }

    for (auto entry : directory.entries())
    {
        ls_print_entry(entry);
    }

    if (!option_list)
    {
        IO::out("\n");
    }

    return SUCCESS;
}

int main(int argc, const char *argv[])
{
    ArgParse args;

    args.usage("");
    args.usage("FILES...");
    args.usage("OPTIONS... FILES...");

    args.prologue("List files and directories in the current working directory by default.");

    args.option(option_all, 'a', "all", "Do not ignore entries starting with '.'.");
    args.option(option_list, 'l', "list", "Long listing mode.");
    args.option(option_color, 'c', "color", "Enable colored output.");

    args.prologue("Options can be combined.");

    if (args.eval(argc, argv) != PROCESS_SUCCESS)
    {
        return PROCESS_FAILURE;
    }

    if (args.argc() == 0)
    {
        return ls(".", false);
    }

    Result result;
    int exit_code = PROCESS_SUCCESS;

    for (auto file : args.argv())
    {
        result = ls(file, args.argc() > 2);

        if (result != SUCCESS)
        {
            exit_code = PROCESS_FAILURE;
        }
    }

    return exit_code;
}
