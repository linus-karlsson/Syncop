#ifndef SYNCOP_UNIT_BUILD
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#endif

FileData utils_read_file(const char* file_name)
{
    FILE* file = NULL;
    fopen_s(&file, file_name, "rb");

    FileData file_data = {0};

    fseek(file, 0, SEEK_END);
    file_data.size = (uint32_t)ftell(file);
    rewind(file);

    file_data.data = (uint8_t*)calloc(file_data.size, sizeof(uint8_t));

    if(fread(file_data.data, 1, file_data.size, file) != file_data.size)
    {
        // TODO: Logging
    }

    return file_data;
}
