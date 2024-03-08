#include "files.hpp"

#include <iostream>

#include "fileutils.hpp"

namespace wwc {
    file::file(const char* name) : name_{name} {}

    bool file::is_dir() const noexcept {
        return wwc::is_dir(name_.c_str());
    }

    bool file::is_regular() const noexcept {
        return wwc::is_regular(name_.c_str());
    }

    bool file::exists() const noexcept {
        return wwc::exists(name_.c_str());
    }

    std::vector<file> file::ls(bool abs) const noexcept {
        if (!is_dir()) {
            return {};
        }
        std::vector<std::string> files = wwc::ls_dir(name_.c_str());
        std::vector<file> result;
        result.reserve(files.size());
        for (const std::string& f : files) {
            if (f == "." || f == "..") {
                continue;
            }
            if (abs) {
                result.emplace_back(utils::simple_join(name_, f.c_str()).c_str());
            } else {
                result.emplace_back(f.c_str());
            }
        }
        return result;
    }

    std::ostream& operator<<(std::ostream& os, const file& f) {
        return os << f.name_;
    }
}  // namespace wwc