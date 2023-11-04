#include "Json.hpp"

#include <iostream>

#include "fileutils.hpp"

using namespace std;
namespace wwc {
    JSONObject::JSONObject(cJSON* data) : data{data} {
        if (data == nullptr) {
            cerr << "data is null" << endl;
        }
    }

    string JSONObject::getString(const char* name) const {
        if (name == nullptr) {
            cerr << "name is empty " << endl;
            return "";
        }
        const auto string_data = cJSON_GetObjectItemCaseSensitive(data, name);
        const auto data_type = string_data->type;
        if (data_type != cJSON_String) {
            cerr << "name: " << name << " does not hold string value" << endl;
            return "";
        }
        return string_data->valuestring;
    }

    int JSONObject::getInt(const char* name) const {
        return getNumber<int, const char*>(data, name);
    }

    long JSONObject::getLong(const char* name) const {
        return getNumber<long, const char*>(data, name);
    }

    double JSONObject::getDouble(const char* name) const {
        return getNumber<double, const char*>(data, name);
    }

    bool JSONObject::getBool(const char* name) const {
        return getNumber<bool, const char*>(data, name);
    }

    bool JSONObject::toFile(const char* filename) const {
        return writeTextToFile(filename, toString());
    }

    shared_ptr<JSONObject> JSONObject::getJSONObject(const char* name) const {
        if (name == nullptr) {
            cerr << "name is null" << endl;
            return nullptr;
        }
        cJSON* d = cJSON_GetObjectItemCaseSensitive(data, name);
        return make_shared<JSONObject>(d);
    }

    shared_ptr<JSONArray> JSONObject::getJSONArray(const char* name) const {
        if (!contains(name)) {
            cerr << "Non-existing name: " << name << endl;
            return nullptr;
        }
        return make_shared<JSONArray>(
            cJSON_GetObjectItemCaseSensitive(data, name));
    }

    shared_ptr<JSONObject> JSONObject::parseFromFile(const char* filename) {
        string content = readTextFromFile(filename);
        cJSON* data = cJSON_Parse(content.c_str());
        auto jsonObject = make_shared<JSONObject>(data);
        return jsonObject;
    }

    // JSONArray
    int JSONArray::getInt(size_t i) const {
        return getNumber<int, size_t>(data, i);
    }

    long JSONArray::getLong(size_t i) const {
        return getNumber<long, size_t>(data, i);
    }

    double JSONArray::getDouble(size_t i) const {
        return getNumber<double, size_t>(data, i);
    }

    bool JSONArray::getBool(size_t i) const {
        return getNumber<bool, size_t>(data, i);
    }

    string JSONArray::getString(size_t i) const {
        if (!check_size(i)) {
            return "";
        }
        const cJSON* sub_data = cJSON_GetArrayItem(data, i);
        if (sub_data == nullptr) {
            cerr << "error get item: " << i << endl;
            return "";
        }
        const auto type = sub_data->type;
        if (type != cJSON_String) {
            cerr << "Item is not string" << endl;
            return "";
        }
        return sub_data->string;
    }

    shared_ptr<JSONObject> JSONArray::getJSONObject(size_t i) const {
        if (!check_size(i)) {
            return nullptr;
        }
        return make_shared<JSONObject>(cJSON_GetArrayItem(data, i));
    }

    shared_ptr<JSONArray> JSONArray::getJSONArray(size_t i) const {
        if (!check_size(i)) {
            return nullptr;
        }
        return make_shared<JSONArray>(cJSON_GetArrayItem(data, i));
    }

    const char* JSONArray::toString() const {
        if (!valid_data()) {
            cerr << "Data is null" << endl;
            return "";
        }
        return cJSON_Print(data);
    }

    bool JSONArray::toFile(const char* filename) const {
        return writeTextToFile(filename, toString());
    }
}  // namespace wwc