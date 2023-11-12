#include "Json.hpp"

#include <iostream>

#include "fileutils.hpp"

using namespace std;

namespace wwc {
    JSONObject::JSONObject(cJSON* data, bool is_root) : data{data}, is_root{is_root} {}

    optional<string> JSONObject::getString(const char* name) const {
        if (name == nullptr) {
            return {};
        }
        const auto string_data = cJSON_GetObjectItemCaseSensitive(data, name);
        const auto data_type = string_data->type;
        if (data_type != cJSON_String) {
            return {};
        }
        return string_data->valuestring;
    }

    optional<int> JSONObject::getInt(const char* name) const {
        return getNumber<int, const char*>(data, name);
    }

    optional<long> JSONObject::getLong(const char* name) const {
        return getNumber<long, const char*>(data, name);
    }

    optional<double> JSONObject::getDouble(const char* name) const {
        return getNumber<double, const char*>(data, name);
    }

    optional<bool> JSONObject::getBool(const char* name) const {
        return getNumber<bool, const char*>(data, name);
    }

    unordered_set<string> JSONObject::keys() const {
        if (!valid_data()) {
            return {};
        }
        unordered_set<string> result;
        cJSON* child = data->child;
        while (child != nullptr) {
            result.emplace(child->string);
            child = child->next;
        }
        return result;
    }

    bool JSONObject::toFile(const char* filename) const {
        return writeTextToFile(filename, toString());
    }

    shared_ptr<JSONObject> JSONObject::getJSONObject(const char* name) const {
        if (name == nullptr) {
            return nullptr;
        }
        cJSON* d = cJSON_GetObjectItemCaseSensitive(data, name);
        if (d == nullptr) {
            return nullptr;
        }
        return make_shared<JSONObject>(d);
    }

    shared_ptr<JSONArray> JSONObject::getJSONArray(const char* name) const {
        if (!contains(name)) {
            return nullptr;
        }
        const auto result = make_shared<JSONArray>(cJSON_GetObjectItemCaseSensitive(data, name));
        if (!result->valid_data()) {
            return nullptr;
        }
        return result;
    }

    shared_ptr<JSONObject> JSONObject::parseFromFile(const char* filename) {
        string content = readTextFromFile(filename);
        if (content == "") {
            return nullptr;
        }
        cJSON* data = cJSON_Parse(content.c_str());
        const auto result = make_shared<JSONObject>(data, true);
        if (!result->valid_data()) {
            return nullptr;
        }
        return result;
    }

    // JSONArray
    shared_ptr<JSONArray> JSONArray::parseFromFile(const char* filename) {
        string content = readTextFromFile(filename);
        if (content == "") {
            return nullptr;
        }
        cJSON* data = cJSON_Parse(content.c_str());
        const auto result = make_shared<JSONArray>(data, true);
        if (!result->valid_data()) {
            return nullptr;
        }
        return result;
    }
    optional<int> JSONArray::getInt(size_t i) const {
        return getNumber<int, size_t>(data, i);
    }

    optional<long> JSONArray::getLong(size_t i) const {
        return getNumber<long, size_t>(data, i);
    }

    optional<double> JSONArray::getDouble(size_t i) const {
        return getNumber<double, size_t>(data, i);
    }

    optional<bool> JSONArray::getBool(size_t i) const {
        return getNumber<bool, size_t>(data, i);
    }

    optional<string> JSONArray::getString(size_t i) const {
        if (!check_size(i) || !valid_data()) {
            return {};
        }
        const cJSON* sub_data = cJSON_GetArrayItem(data, i);
        if (sub_data == nullptr) {
            return {};
        }
        const auto type = sub_data->type;
        if (type != cJSON_String) {
            return {};
        }
        return sub_data->valuestring;
    }

    shared_ptr<JSONObject> JSONArray::getJSONObject(size_t i) const {
        if (!check_size(i)) {
            return nullptr;
        }
        const auto result = make_shared<JSONObject>(cJSON_GetArrayItem(data, i));
        if (!result->valid_data()) {
            return nullptr;
        }
        return result;
    }

    shared_ptr<JSONArray> JSONArray::getJSONArray(size_t i) const {
        if (!check_size(i)) {
            return nullptr;
        }
        const auto result = make_shared<JSONArray>(cJSON_GetArrayItem(data, i));
        if (!result->valid_data()) {
            return nullptr;
        }
        return result;
    }

    const char* JSONArray::toString() const {
        if (!valid_data()) {
            return "";
        }
        return cJSON_Print(data);
    }

    bool JSONArray::toFile(const char* filename) const {
        return writeTextToFile(filename, toString());
    }
}  // namespace wwc