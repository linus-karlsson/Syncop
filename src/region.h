#pragma once
#ifndef SYNCOP_UNIT_BUILD
#include <stdint.h>
#endif

typedef struct Region
{
    uint64_t position;
    uint64_t capacity;
    uint8_t* buffer;
} Region;

#define REGION_CREATE(name) uint8_t* name(uint64_t capacity)
typedef REGION_CREATE(RegionCreate);

#define region_allocate(region, capacity, type)                                       \
    (type*)region_allocate_(region, capacity * sizeof(type), _Alignof(type))

Region region_create(uint64_t capacity, RegionCreate create_function);
void* region_allocate_(Region* region, uint32_t size_in_bytes, uint32_t alignment);
