#include "normal_utils.hpp"

namespace wwc {
    std::string run_cmd(const char* cmd) {
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe{popen(cmd, "r"), pclose};
        if (pipe.get() == nullptr) {
            const auto temp = wwc::String::format("failed to execute command: %s", cmd);
            perror(temp.c_str());
            return result;
        }
        char buf[512];
        while (fgets(buf, sizeof(buf), pipe.get()) != nullptr) {
            result += buf;
        }
        return result;
    }
}  // namespace wwc