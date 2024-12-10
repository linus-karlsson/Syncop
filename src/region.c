#include "region.h"
#include "utils.h"
#include <string.h>

internal uint32_t get_alignment_offset(uint8_t* position, uint32_t alignment)
{
    const uintptr_t current_ptr = (uintptr_t)position;
    const uint32_t mask = alignment - 1;
    uint32_t result = current_ptr & mask;
    return ((alignment - result) * (result != 0));
}

internal void assert_region_size(const Region* region, uint32_t allocation)
{
    ASSERT((region->position + allocation) < region->capacity);
}

Region region_create(uint64_t capacity, RegionCreate create_function)
{
    Region region = { 0 };
    region.capacity = capacity;
    region.buffer = create_function(region.capacity);
    return region;
}

void* region_allocate_(Region* region, uint32_t size_in_bytes, uint32_t alignment)
{
    uint32_t alignment_offset =
        get_alignment_offset(region->buffer + region->position, alignment);

    assert_region_size(region, size_in_bytes + alignment_offset);

    region->position += alignment_offset;
    void* result = region->buffer + region->position;
    memset(result, 0, size_in_bytes);

    region->position += size_in_bytes;
    return result;
}

#if 0
void* region_array_(Region* region, uint32_t capacity, uint32_t stride,
                           uint32_t alignment)
{
    uint32_t alignment_offset =
        get_alignment_offset(region->buffer + region->position, alignment);

    uint32_t size_in_bytes = capacity * stride;
    assert_region_size(region,
                       size_in_bytes + alignment_offset + sizeof(RegionArrayHead));

    // TODO: RegionArrayHead could be having an effect on the alignment as well.
    region->position += alignment_offset;

    void* position = region->buffer + region->position;
    RegionArrayHead* head = position;
    head->size = 0;
    head->capacity = capacity;
    return ++head;
}
#endif
