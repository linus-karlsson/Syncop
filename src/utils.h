#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include <stdint.h>
#endif

#define array_create_d(array) array_create(array, 10)
#define array_create(array, array_capacity)                                         \
    do                                                                              \
    {                                                                               \
        (array)->size = 0;                                                          \
        (array)->capacity = (array_capacity);                                       \
        (array)->data = calloc((array_capacity), sizeof((*(array)->data)));         \
    } while (0)

#define array_append(array, value)                                                  \
    do                                                                              \
    {                                                                               \
        if ((array)->size >= (array)->capacity)                                     \
        {                                                                           \
            (array)->capacity *= 2;                                                 \
            (array)->data = realloc((array)->data,                                  \
                                    (array)->capacity * sizeof((*(array)->data)));  \
        }                                                                           \
        (array)->data[(array)->size++] = (value);                                   \
    } while (0)

#define array_pop(array, result)                                                    \
    do                                                                              \
    {                                                                               \
        if ((array)->size > 0)                                                      \
        {                                                                           \
            (array)->size--;                                                        \
            *(result) = true;                                                       \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            *(result) = false;                                                      \
        }                                                                           \
    } while (0)

typedef struct FileData
{
    uint32_t size;
    uint8_t* data;
} FileData;

FileData utils_read_file(const char* file_name);

