#pragma once
#include <string>
#include <vector>
#include <deque>
#include "String.hpp"
namespace wwc::utils {
    template <typename... Paths>
    std::string simple_join(Paths... paths) {
        std::vector<std::string> paths_vec{paths...};
        std::deque<std::string> dq;
        std::string result;
        constexpr char sep = '/';
        for (std::size_t i = 0; i < paths_vec.size(); i++) {
            String s{paths_vec[i]};
            s.strip_prefix("./");
            if ((s == "..") && dq.size() > 0) {
                dq.pop_back();
                continue;
            } 
            if (s == ".") {
                continue;
            }
            s.rstrip(sep);
            dq.push_back(s);
        }
        size_t cnt = 0;
        const auto sz = dq.size();
        while(!dq.empty()) {
            std::string s = dq.front();
            dq.pop_front();
            result += s;
            if (++cnt < sz) {
                result += sep;
            }
        }
        return result;
    }
}  // namespace wwc::utils

namespace wwc {
    std::string readTextFromFile(const char* filename);
    bool writeTextToFile(const char* filename, const char* content);
    std::vector<std::string> ls_dir(const char* dir_name);
    bool is_dir(const char* path);
    bool is_regular(const char* path);
    bool exists(const char* path);
    std::string expanduser(const char* path);
}  // namespace wwc
