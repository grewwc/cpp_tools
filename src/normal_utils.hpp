#pragma once 
#include <string>
#include "String.hpp"
#include <optional>

namespace wwc 
{
    std::string run_cmd(const char *cmd);
    std::optional<std::string> get_md5(const char* real_path);
}