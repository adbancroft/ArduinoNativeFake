#pragma once

#include <sstream>

static inline void reset(std::stringstream &stream, const char *content)
{
    stream.str(content);
    stream.clear();
    stream.seekg(0, std::ios::beg);
    stream.seekp(0, std::ios::beg);
}