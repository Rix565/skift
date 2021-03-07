#include <libutils/String.h>

#include <libio/Format.h>
#include <libio/NumberFormat.h>
#include <libio/Write.h>

namespace IO
{

ResultOr<size_t> Format::format(Writer &writer)
{
    return IO::format(writer, "Object({#x})", reinterpret_cast<uintptr_t>(this));
}



} // namespace IO
