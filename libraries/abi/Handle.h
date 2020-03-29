#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>

#include <libsystem/Result.h>

#define SELECT_READ (1 << 0)
#define SELECT_WRITE (1 << 1)
#define SELECT_SEND (1 << 2)
#define SELECT_CONNECT (1 << 3)
#define SELECT_ACCEPT (1 << 4)

typedef unsigned int SelectEvent;

typedef struct
{
    int id;
    OpenFlag flags;
    Result result;
} Handle;

typedef struct
{
    int *handles;
    SelectEvent *events;
    size_t count;
} HandleSet;

#define HANDLE_INVALID_ID (-1)

#define HANDLE(__subclass) ((Handle *)(__subclass))

#define handle_has_error(__handle) (HANDLE(__handle)->result != SUCCESS)

#define handle_error_string(__handle) result_to_string(HANDLE(__handle)->result)

#define handle_get_error(__handle) (HANDLE(__handle)->result)

#define handle_clear_error(__handle) (HANDLE(__handle)->result = SUCCESS)

#define handle_has_flags(__handle, __flags) ((HANDLE(__handle)->flags & (__flags)) == (__flags))