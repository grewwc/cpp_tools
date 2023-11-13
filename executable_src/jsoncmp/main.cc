#include <gflags/gflags.h>

#include <iostream>
#include <unordered_set>

#include "Json.hpp"

using Ptr_JSONArray = std::shared_ptr<wwc::JSONArray>;
using Ptr_JSONObject = std::shared_ptr<wwc::JSONObject>;

struct config {
    bool only_compare_keys = false;
};

void add_key(std::unordered_set<std::string>& keyset, const std::string& key) {
    if (key != "") {
        keyset.emplace(key);
    }
}

void print_usage() {
    std::cout << "usage: jsoncmp ${file1} ${file2} -k true -array false" << std::endl;
}

template <typename T>
void compare_json(T data1, T data2, const std::string& prev_key, 
                    std::unordered_set<std::string>& keyset_1, 
                    std::unordered_set<std::string>& keyset_2,
                    std::unordered_set<std::string>& diff_keys, 
                    const config& options) {
    const bool only_compare_keys = options.only_compare_keys;
    if (only_compare_keys) {
        return;
    }
    if (data1 != data2) {
        add_key(diff_keys, prev_key);
        return;
    }
}

template <>
void compare_json<Ptr_JSONObject>(Ptr_JSONObject obj1, Ptr_JSONObject obj2, const std::string& prev_key, 
                        std::unordered_set<std::string>& keyset_1, 
                        std::unordered_set<std::string>& keyset_2, 
                        std::unordered_set<std::string>& diff_keys,
                        const config& options);

template <typename T>
void compare_json(std::optional<T> data1, std::optional<T> data2, const std::string& prev_key, 
                    std::unordered_set<std::string>& keyset_1, 
                    std::unordered_set<std::string>& keyset_2, 
                    std::unordered_set<std::string>& diff_keys,
                    const config& options) {
    if (!data1.has_value() && !data2.has_value()) {
        return;
    }
    if (data1.has_value() && !data2.has_value()) {
        add_key(keyset_1, prev_key);
        return;
    }
    if (data2.has_value() && !data1.has_value()) {
        add_key(keyset_2, prev_key);
        return;
    }
    return compare_json(data1.value(), data2.value(), prev_key, keyset_1, keyset_2, diff_keys, options);
}

template <>
void compare_json<Ptr_JSONArray>(Ptr_JSONArray arr1, Ptr_JSONArray arr2, const std::string& prev_key, 
                                    std::unordered_set<std::string>& keyset_1, 
                                    std::unordered_set<std::string>& keyset_2, 
                                    std::unordered_set<std::string>& diff_keys,
                                    const config& options) {
    const bool only_compare_keys = options.only_compare_keys;
    if (arr1 == nullptr && arr2 == nullptr) {
        return;
    }
    if (arr1 == nullptr && arr2 != nullptr) {
        add_key(keyset_2, prev_key);
        return;
    }
    if (arr2 == nullptr && arr1 != nullptr) {
        add_key(keyset_1, prev_key);
        return;
    }
    for (size_t i = 0; i < arr1->size(); i++) {
        std::string temp_key = prev_key != "" ? prev_key + "." + std::to_string(i) : std::to_string(i);
        compare_json(arr1->getBool(i), arr2->getBool(i), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(arr1->getInt(i), arr2->getInt(i), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(arr1->getDouble(i), arr2->getDouble(i), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(arr1->getJSONArray(i), arr2->getJSONArray(i), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(arr1->getString(i), arr2->getString(i), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(arr1->getLong(i), arr2->getLong(i), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(arr1->getJSONObject(i), arr2->getJSONObject(i), temp_key, keyset_1, keyset_2, diff_keys, options);
    }
    for (size_t i = arr1->size(); i<arr2->size(); i++) {
        std::string temp_key = prev_key != "" ? prev_key + "." + std::to_string(i) : std::to_string(i);
        add_key(keyset_2, temp_key);
    }
}

template <>
void compare_json<Ptr_JSONObject>(Ptr_JSONObject obj1, Ptr_JSONObject obj2, const std::string& prev_key, 
                                    std::unordered_set<std::string>& keyset_1, 
                                    std::unordered_set<std::string>& keyset_2,
                                    std::unordered_set<std::string>& diff_keys,
                                    const config& options) {
    if (obj1 == nullptr && obj2 == nullptr) {
        return;
    }
    if (obj1 == nullptr && obj2 != nullptr) {
        add_key(keyset_2, prev_key);
        return;
    }
    if (obj2 == nullptr && obj1 != nullptr) {
        add_key(keyset_1, prev_key);
        return;
    }
    const auto key_set1 = obj1->keys();
    const auto key_set2 = obj2->keys();
    for (const std::string& key : key_set1) {
        if (key_set2.find(key) == key_set2.cend()) {
            add_key(keyset_1, key);
            continue;
        }
        // found in key_set2
        std::string temp_key = prev_key != "" ? prev_key + "." + key : key;
        compare_json(obj1->getBool(key.c_str()), obj2->getBool(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(obj1->getDouble(key.c_str()), obj2->getDouble(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(obj1->getInt(key.c_str()), obj2->getInt(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(obj1->getLong(key.c_str()), obj2->getLong(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(obj1->getJSONArray(key.c_str()), obj2->getJSONArray(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(obj1->getJSONObject(key.c_str()), obj2->getJSONObject(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
        compare_json(obj1->getString(key.c_str()), obj2->getString(key.c_str()), temp_key, keyset_1, keyset_2, diff_keys, options);
    }
    
    for (const std::string& key : key_set2) {
        if (key_set1.find(key) == key_set1.cend()) {
            add_key(keyset_2, key);
        }
    }
}

int compare_json_objects(const char* filename1, const char* filename2, 
                            std::unordered_set<std::string>& keyset_1, 
                            std::unordered_set<std::string>& keyset_2,
                            std::unordered_set<std::string>& diff_keys, 
                            const config& options) {
    auto obj1 = wwc::JSONObject::load(filename1);
    if (obj1 == nullptr) {
        std::cerr << "[" << filename1 << "]" << " is not json object" << std::endl;
        return 1;
    }
    auto obj2 = wwc::JSONObject::load(filename2);
    if (obj2 == nullptr) {
        std::cerr << "[" << filename2 << "]" << " is not json object" << std::endl;
        return 1;
    }
    compare_json(obj1, obj2, "", keyset_1, keyset_2, diff_keys, options);
    return 0;
}

int compare_json_array(const char* filename1, const char* filename2, 
                        std::unordered_set<std::string>& keyset_1, 
                        std::unordered_set<std::string>& keyset_2, 
                        std::unordered_set<std::string>& diff_keys,
                        const config& options) {
    auto arr1 = wwc::JSONArray::load(filename1);
    if (arr1 == nullptr) {
        std::cerr << "[" << filename1 << "]" << " is not json array" << std::endl;
        return 1;
    }
    auto arr2 = wwc::JSONArray::load(filename2);
    if (arr2 == nullptr) {
        std::cerr << "[" << filename2 << "]" << " is not json array" << std::endl;
        return 1;
    }
    compare_json(arr1, arr2, "_root", keyset_1, keyset_2, diff_keys, options);
    return 0;
}

bool print_summary(const std::unordered_set<std::string>& data, const char* filename, const std::string& sign) {
    if (data.size() > 0) {
        if (filename != nullptr) {
            std::cout << sign << "[" << filename << "]" << std::endl;
        } else {
            std::cout << sign << std::endl;
        }
        for (const auto& key : data) {
            std::cout << '\t' << key << std::endl;
        }
        return true;
    }
    return false;
}

DEFINE_bool(array, false, "If file contains JSONArray.");
DEFINE_bool(h, false, "Help information.");
DEFINE_bool(k, false, "Only check if keys are the same, ignore values.");

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_h || argc < 3) {
        print_usage();
        return 0;
    }
    const char* f1 = argv[1];
    const char* f2 = argv[2];
    std::unordered_set<std::string> keyset_1;
    std::unordered_set<std::string> keyset_2;
    std::unordered_set<std::string> diff_keys;
    config options{.only_compare_keys = FLAGS_k};
    if (FLAGS_array) {
        const auto result = compare_json_array(f1, f2, keyset_1, keyset_2, diff_keys, options);
        if (result != 0) {
            return result;
        }
    } else {
        const auto result = compare_json_objects(f1, f2, keyset_1, keyset_2, diff_keys, options);
        if (result != 0) {
            return result;
        }
    }
    bool has_diff = false;
    has_diff = print_summary(keyset_1, f1, ">>>>>>") || has_diff;
    has_diff = print_summary(keyset_2, f2, "<<<<<<") || has_diff;
    has_diff = print_summary(diff_keys, nullptr, "Diff") || has_diff;
    if (!has_diff) {
        std::cout << "Same." << std::endl;
    }
    return 0;
}