#pragma once
#include <algorithm>
#include <vector>

#include "RandomUtils.hpp"
#include "Types.hpp"
#include "src/algorithm.hpp"
#include "src/String.hpp"
#include "src/barrier.hpp"
#include "src/countdown_latch.hpp"
#include "src/download_utils.hpp"
#include "src/fileutils.hpp"
#include "src/jthread.hpp"
#include "src/mdc.hpp"
#include "src/num_utils.hpp"
#include "src/argparser.hpp"

namespace wwc {
    template <typename T, template <typename, typename...> class Container, typename = std::enable_if_t<!std::is_pointer_v<std::decay_t<T>>>>
    Container<const T *> to_pointer_container(const Container<T> &container) {
        Container<const T *> result;
        result.reserve(container.size());
        std::transform(std::cbegin(container), std::cend(container), std::back_inserter<Container<const T *>>(result),
                       [](const T &val) { return &val; });
        return result;
    }

    template <typename T, template <typename, typename...> class Container, typename = std::enable_if_t<!std::is_pointer_v<std::decay_t<T>>>>
    Container<T *> to_pointer_container(Container<T> &container) {
        Container<T *> result;
        result.reserve(container.size());
        std::transform(std::begin(container), std::end(container), std::back_inserter<Container<T *>>(result), [](T &val) { return &val; });
        return result;
    }

    template <typename FirstArg, typename... Args>
    void print(FirstArg &&first, Args &&...args) {
        std::cout << "[" << std::forward<FirstArg>(first) << "]";
        ((std::cout << ", "
                    << "[" << std::forward<Args>(args) << "]"),
         ...);
        std::cout << std::endl;
    }

    std::string run_cmd(const char *cmd) {
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe{popen(cmd, "r"), pclose};
        if (pipe.get() == nullptr) {
            const auto temp = String::format("failed to execute command: %s", cmd);
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
