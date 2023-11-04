#pragma once
#include <string>

namespace wwc {
    std::string readTextFromFile(const char* filename);
    bool writeTextToFile(const char* filename, const char* content);
}
