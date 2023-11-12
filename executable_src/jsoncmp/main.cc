#include <gflags/gflags.h>

#include <iostream>
#include <unordered_set>

#include "Json.hpp"

DEFINE_bool(array, false, "If file contains JSONArray");
DEFINE_bool(h, false, "Help information");

using Ptr_JSONArray = std::shared_ptr<wwc::JSONArray>;
using Ptr_JSONObject = std::shared_ptr<wwc::JSONObject>;

void print_usage() {
    std::cout << "usage: jsoncmp ${file1} ${file2} --array false" << std::endl;
}

template <typename T>
void compare_json(T data1, T data2, const std::string& prev_key, std::unordered_set<std::string>& diff_keys) {
    if (data1 != data2) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
    }
}

template <>
void compare_json<Ptr_JSONObject>(Ptr_JSONObject obj1, Ptr_JSONObject obj2, const std::string& prev_key, std::unordered_set<std::string>& diff_keys);

template <typename T>
void compare_json(std::optional<T> data1, std::optional<T> data2, const std::string& prev_key, std::unordered_set<std::string>& diff_keys) {
    if (!data1.has_value() && !data2.has_value()) {
        return;
    }
    if ((data1.has_value() && !data2.has_value()) || (!data1.has_value() && data2.has_value())) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
        return;
    }
    return compare_json(data1.value(), data2.value(), prev_key, diff_keys);
}

template <>
void compare_json<Ptr_JSONArray>(Ptr_JSONArray arr1, Ptr_JSONArray arr2, const std::string& prev_key, std::unordered_set<std::string>& diff_keys) {
    if (arr1 == nullptr && arr2 == nullptr) {
        return;
    }
    if (arr1 == nullptr && arr2 != nullptr) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
        return;
    }
    if (arr1 == nullptr && arr2 != nullptr) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
        return;
    }
    if (arr1->size() != arr2->size()) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
        return;
    }
    for (size_t i = 0; i < arr1->size(); i++) {
        std::string temp_key = prev_key != "" ? prev_key + "." + std::to_string(i) : std::to_string(i);
        compare_json(arr1->getBool(i), arr2->getBool(i), temp_key, diff_keys);
        compare_json(arr1->getInt(i), arr2->getInt(i), temp_key, diff_keys);
        compare_json(arr1->getDouble(i), arr2->getDouble(i), temp_key, diff_keys);
        compare_json(arr1->getJSONArray(i), arr2->getJSONArray(i), temp_key, diff_keys);
        compare_json(arr1->getString(i), arr2->getString(i), temp_key, diff_keys);
        compare_json(arr1->getLong(i), arr2->getLong(i), temp_key, diff_keys);
        compare_json(arr1->getJSONObject(i), arr2->getJSONObject(i), temp_key, diff_keys);
    }
}

template <>
void compare_json<Ptr_JSONObject>(Ptr_JSONObject obj1, Ptr_JSONObject obj2, const std::string& prev_key, std::unordered_set<std::string>& diff_keys) {
    if (obj1 == nullptr && obj2 == nullptr) {
        return;
    }
    if (obj1 == nullptr && obj2 != nullptr) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
        return;
    }
    if (obj1 == nullptr && obj2 != nullptr) {
        diff_keys.emplace(std::move(const_cast<std::string&>(prev_key)));
        return;
    }
    const auto key_set1 = obj1->keys();
    const auto key_set2 = obj2->keys();
    for (const std::string& key : key_set1) {
        if (key_set2.find(key) == key_set1.cend()) {
            diff_keys.emplace(std::move(const_cast<std::string&>(key)));
            continue;
        }
        // found in key_set2
        std::string temp_key = prev_key != "" ? prev_key + "." + key : key;
        compare_json(obj1->getBool(key.c_str()), obj2->getBool(key.c_str()), temp_key, diff_keys);
        compare_json(obj1->getDouble(key.c_str()), obj2->getDouble(key.c_str()), temp_key, diff_keys);
        compare_json(obj1->getInt(key.c_str()), obj2->getInt(key.c_str()), temp_key, diff_keys);
        compare_json(obj1->getLong(key.c_str()), obj2->getLong(key.c_str()), temp_key, diff_keys);
        compare_json(obj1->getJSONArray(key.c_str()), obj2->getJSONArray(key.c_str()), temp_key, diff_keys);
        compare_json(obj1->getJSONObject(key.c_str()), obj2->getJSONObject(key.c_str()), temp_key, diff_keys);
        compare_json(obj1->getString(key.c_str()), obj2->getString(key.c_str()), temp_key, diff_keys);
    }
}

int compare_json_objects(const char* filename1, const char* filename2, std::unordered_set<std::string>& diff_keys) {
    auto obj1 = wwc::JSONObject::parseFromFile(filename1);
    if (obj1 == nullptr) {
        return 1;
    }
    auto obj2 = wwc::JSONObject::parseFromFile(filename2);
    if (obj2 == nullptr) {
        return 1;
    }
    compare_json(obj1, obj2, "", diff_keys);
    return 0;
}

int compare_json_array(const char* filename1, const char* filename2, std::unordered_set<std::string>& diff_keys) {
    auto arr1 = wwc::JSONArray::parseFromFile(filename1);
    if (arr1 == nullptr) {
        return 1;
    }
    auto arr2 = wwc::JSONArray::parseFromFile(filename2);
    if (arr2 == nullptr) {
        return 1;
    }
    compare_json(arr1, arr2, "", diff_keys);
    return 0;
}

int main(int argc, char* argv[]) {
    google::ParseCommandLineFlags(&argc, &argv, false);
    if (FLAGS_h || argc < 3) {
        print_usage();
        return 0;
    }
    const char* f1 = argv[1];
    const char* f2 = argv[2];
    std::unordered_set<std::string> diff_keys;
    if (FLAGS_array) {
        compare_json_array(f1, f2, diff_keys);
    } else {
        compare_json_objects(f1, f2, diff_keys);
    }
    std::cout << "..........Diff keys:" << std::endl;
    for (const auto& diff_key : diff_keys) {
        std::cout << diff_key << std::endl;
    }
}