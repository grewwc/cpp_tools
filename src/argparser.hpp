#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace wwc {

    enum class ArgType {
        STRING,
        BOOL,
    };

    class ParsedResult {
    public:
        std::optional<std::variant<bool, std::string>> get_val(const std::string &key) const;

        std::variant<bool, std::string> operator[](const std::string &key) const {
            const auto result = get_val(key);
            return result.value_or(false);
        }

        std::vector<std::string> get_positional_args() const;
        std::string get_positional_arg(int index) const;

    private:
        friend class ArgumentParser;
        struct ArgInfo {
            const ArgType arg_type;
            const std::string val;
            const std::string help_msg;
            ArgInfo(const ArgType &type, const char *default_val, const char *help_msg)
                : arg_type{type}, val{default_val}, help_msg{help_msg} {}
        };
        std::unordered_map<int, std::string> positional_args_;
        std::unordered_map<std::string, ArgInfo> named_args_;
    };

    class ArgumentParser {
    public:
        explicit ArgumentParser(const char *args) : data_(args) {}
        void add_argument(const char *flag, const ArgType &type, const char *default_val = nullptr,
                          const char *help_msg = nullptr);
        void print_help() const;
        ParsedResult parse() const;

    private:
        std::unordered_map<std::string, ParsedResult::ArgInfo> named_args_;
        const std::string data_;
    };
} // namespace wwc