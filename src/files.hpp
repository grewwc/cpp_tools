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
        std::vector<file> ls() const noexcept;

        const char* name() const noexcept { return name_.c_str(); }

        std::string absolute_path() const noexcept {
            if (abs_name_.size() > 0) {
                return abs_name_;
            }
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) == nullptr) {
                return "";
            }
            abs_name_ = utils::simple_join(cwd, name_.c_str());
            return abs_name_;
        }

    public:
        friend std::ostream& operator<<(std::ostream& os, const file& f);

    private:
        std::string name_;
        mutable std::string abs_name_;
    };
}  // namespace wwc