#include "argparser.hpp"
#include "String.hpp"
#include <iostream>

namespace wwc {

    void ArgumentParser::add_argument(const char *flag, const ArgType &type, const char *default_val,
                                      const char *help_msg) {
        String flag_str{flag};
        flag_str.ltrim('-');
        named_args_.emplace(std::piecewise_construct, std::forward_as_tuple(flag_str.c_str()),
                            std::forward_as_tuple(type, default_val, help_msg));
    }

    void ArgumentParser::print_help() const {
        std::cout << "Help Message: " << std::endl;
        for (const auto &[arg_name, info] : named_args_) {
            const char *type = info.arg_type == ArgType::BOOL ? "bool" : "string";
            auto msg = String::format("\t%s (%s) (default value: %s) (usage: %s)\n", arg_name.c_str(), type,
                                      info.val.c_str(), info.help_msg.c_str());
            std::cout << std::move(msg);
        }
    }

    ParsedResult ArgumentParser::parse() const {
        ParsedResult result;
        String data_str{data_};
        std::vector<String> data_vec = data_str.split(' ', true);
        bool is_flag = false;
        String prev_arg_name;
        int positional_args_index = 0;
        for (String &val : data_vec) {
            if (val.startsWith("-")) {
                val = val.ltrim('-');
                if (named_args_.find(val) == named_args_.cend()) {
                    continue;
                }
                const ParsedResult::ArgInfo &info = named_args_.at(val);
                if (is_flag) {
                    if (info.arg_type != ArgType::BOOL) {
                        throw std::logic_error(
                            String::format("parse failed: -%s -%s not valid", prev_arg_name.c_str(), val.c_str()));
                    } else {
                        // bool value
                        result.named_args_.emplace(std::piecewise_construct,
                                                   std::forward_as_tuple(val.c_str()),
                                                   std::forward_as_tuple(ArgType::BOOL, "true", info.help_msg.c_str()));
                        is_flag = false;
                    }
                } else if (info.arg_type == ArgType::BOOL) {
                    result.named_args_.emplace(std::piecewise_construct, std::forward_as_tuple(val.c_str()),
                                               std::forward_as_tuple(ArgType::BOOL, "true", info.help_msg.c_str()));
                }
                prev_arg_name = val;

            } else {
                if (prev_arg_name.empty()) {
                    result.positional_args_[positional_args_index++] = val;
                    prev_arg_name = val;
                    continue;
                }
                if (named_args_.find(prev_arg_name) == named_args_.cend()) {
                    continue;
                }
                const ParsedResult::ArgInfo &info = named_args_.at(prev_arg_name);
                if (!is_flag) {
                    result.positional_args_[positional_args_index++] = val;
                } else {
                    result.named_args_.emplace(
                        std::piecewise_construct, std::forward_as_tuple(prev_arg_name.c_str()),
                        std::forward_as_tuple(info.arg_type, val.c_str(), info.help_msg.c_str()));
                }

                is_flag = false;
            }
        }

        // set default value
        for (const auto &[key, val] : named_args_) {
            if (result.named_args_.find(key) == result.named_args_.cend()) {
                result.named_args_.emplace(std::piecewise_construct, std::forward_as_tuple(key.c_str()),
                                           std::forward_as_tuple(val.arg_type, val.val.c_str(), val.help_msg.c_str()));
            }
        }

        return result;
    }
    std::optional<std::variant<bool, std::string>> ParsedResult::get_val(const std::string &key) const {
        if (named_args_.find(key) == named_args_.cend()) {
            return {};
        }
        const ArgInfo &info = named_args_.at(key);
        switch (info.arg_type) {
        case ArgType::BOOL: return info.val == "true";
        default: return {};
        }
    }
    std::vector<std::string> ParsedResult::get_positional_args() const {
        std::vector<std::string> result;
        result.reserve(positional_args_.size());
        for (const auto &[k, v] : positional_args_) {
            result.emplace_back(v);
        }
        return result;
    }
    std::string ParsedResult::get_positional_arg(int index) const {
        if (positional_args_.find(index) == positional_args_.cend()) {
            throw std::logic_error("index out of range");
        }
        return positional_args_.at(index);
    }
} // namespace wwc