#pragma once

#include <libio/Seek.h>
#include <libutils/ResultOr.h>

namespace IO
{

struct Reader
{
    virtual ResultOr<size_t> read(void *buffer, size_t size) = 0;
};

template <typename T>
concept SeekableReader = IsBaseOf<Reader, T>::value &&IsBaseOf<Seek, T>::value;

} // namespace IO