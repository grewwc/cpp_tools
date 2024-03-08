#pragma once

#include <limits.h>
#include <unistd.h>

#include <ostream>

#include "fileutils.hpp"
namespace wwc {
    class file {
    public:
        explicit file(const char* name);
        file(const file& other) = default;
        file& operator=(const file& other) = default;

        bool exists() const noexcept;
        bool is_dir() const noexcept;
        bool is_regular() const noexcept;
        std::vector<file> ls(bool abs=false) const noexcept;

        const char* name() const noexcept { return name_.c_str(); }


    public:
        friend std::ostream& operator<<(std::ostream& os, const file& f);

    private:
        std::string name_;
        mutable std::string abs_name_;
    };
}  // namespace wwc