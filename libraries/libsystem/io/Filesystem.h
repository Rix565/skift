#pragma once

#include <libutils/String.h>
#include <libsystem/Result.h>

namespace System
{

Result link(const String &old_path, const String &new_path);

Result unlink(const String &path);

Result mkdir(const String &path);

Result mkpipe(const String &path);

Result rename(const String &old_path, const String &new_path);

bool exist(const String &path, FileType type);

} // namespace System
