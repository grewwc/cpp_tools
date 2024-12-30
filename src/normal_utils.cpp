#include "normal_utils.hpp"
#include "src/String.hpp"

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
    
    std::optional<std::string> get_md5(const char* real_path) {
        String cmd = wwc::String::format("md5sum \"%s\"", real_path);
        wwc::String md5 = run_cmd(cmd.c_str());

        auto vec = md5.split();
        if (vec.size() > 0) {
            return {vec[0]};
        }
        return {};
    }
}  // namespace wwc