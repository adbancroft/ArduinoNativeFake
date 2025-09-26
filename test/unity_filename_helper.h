#pragma once

#include <stdint.h>
#include <unity.h>

struct unity_filename_helper_t
{
    unity_filename_helper_t(const char* newName) : savedName(Unity.TestFile)
    {
        Unity.TestFile = newName;
    }
    ~unity_filename_helper_t()
    {
        Unity.TestFile = savedName;
    }
    const char* savedName;
};
