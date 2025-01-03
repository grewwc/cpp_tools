#include "download_utils.hpp"
#include "normal_utils.hpp"


namespace wwc {
    bool download_url(const char* url, std::string& chunk) {
        try {
            const auto cmd = wwc::String::format("curl -L -s %s ", url);
            chunk = wwc::run_cmd(cmd.c_str());
            return true;
        } catch(std::exception& ex) {
            return false;
        }
    }
}  // namespace wwc