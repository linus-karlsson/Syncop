#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include <stdint.h>
#endif

#define global static
#define presist static
#define internal static

#define KILOBYTE(n) ((n) * 1024ULL)
#define MEGABYTE(n) (KILOBYTE((n)) * 1024ULL)
#define GIGABYTE(n) (MEGABYTE((n)) * 1024ULL)

#define ASSERT(expression)                                                          \
    if (!(expression)) (*(uint32_t*)0 = 0)
#define VK_ASSERT(function)                                                         \
    {                                                                               \
        ASSERT((function) == VK_SUCCESS);                                           \
    }
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

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

#define array_create(region, array, array_capacity, type)                           \
    do                                                                              \
    {                                                                               \
        (array)->size = 0;                                                          \
        (array)->capacity = (array_capacity);                                       \
        (array)->data = region_allocate(region, (array_capacity), type);            \
    } while (0)

#define array_append(array, value)                                                  \
    do                                                                              \
    {                                                                               \
        ASSERT((array)->size < (array)->capacity);                                  \
        (array)->data[(array)->size++] = (value);                                   \
    } while (0)

typedef struct FileData
{
    uint32_t size;
    uint8_t* data;
} FileData;

FileData utils_read_file(const char* file_name);

