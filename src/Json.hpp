#pragma once
#include <cJSON.h>

#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_set>

namespace wwc {
    class JSONArray;

    template <typename T, typename name_type, typename _ = std::enable_if_t<std::is_default_constructible_v<T>, void>>
    std::optional<T> getNumber(const cJSON* data, name_type name) {
        if (data == nullptr) {
            return {};
        }
        cJSON* num_data;
        // const char*
        if constexpr (std::is_same_v<name_type, const char*>) {
            if (name == nullptr || data == nullptr) {
                return {};
            }

            if (!cJSON_HasObjectItem(data, name)) {
                return {};
            }
            num_data = cJSON_GetObjectItemCaseSensitive(data, name);
        } else if (std::is_integral_v<name_type>) {  // JSONArray
            if (name < 0 || data == nullptr) {
                return {};
            }
            const auto sz = cJSON_GetArraySize(data);
            if (name >= sz) {
                return {};
            }
            num_data = cJSON_GetArrayItem(data, name);
        }
        if (num_data == nullptr) {
            return {};
        }
        auto node_type = num_data->type;
        if (node_type != cJSON_Number && node_type != cJSON_True && node_type != cJSON_False) {
            return {};
        }
        using decay_type = std::decay_t<T>;
        if constexpr (std::is_integral_v<decay_type>) {
            return num_data->valueint;
        }
        return num_data->valuedouble;
    }

    class JSONObject {
    public:
        static std::shared_ptr<JSONObject> load(const char* filename);
        static std::shared_ptr<JSONObject> loads(const char* content);

    public:
        friend class JSONArray;
        explicit JSONObject() : data{cJSON_CreateObject()} {}
        explicit JSONObject(cJSON* data, bool is_root = false);
        ~JSONObject() {
            if (is_root && valid_data()) {
                cJSON_Delete(data);
            }
        }
        bool contains(const char* name) const {
            if (name == nullptr || data == nullptr) {
                return false;
            }
            return cJSON_HasObjectItem(data, name);
        }
        std::optional<std::string> getString(const char* name) const;
        std::optional<int> getInt(const char* name) const;
        std::optional<long> getLong(const char* name) const;
        std::optional<double> getDouble(const char* name) const;
        std::optional<bool> getBool(const char* name) const;
        std::shared_ptr<JSONObject> getJSONObject(const char* name) const;
        std::shared_ptr<JSONArray> getJSONArray(const char* name) const;
        std::unordered_set<std::string> keys() const;
        cJSON* mutableData() { return data; }
        const cJSON* constData() const { return data; }
        const char* toString() const {
            if (!valid_data()) {
                return "";
            }
            return cJSON_Print(data);
        }
        bool toFile(const char* filename) const;

        template <typename T>
        void put(const char* key, const T& value) {
            if (!valid_data() || key == nullptr) {
                std::cerr << "Invalid data " << std::endl;
                return;
            }
            using decay_type = std::decay_t<decltype(value)>;
            cJSON* d = cJSON_GetObjectItemCaseSensitive(data, key);
            // string
            if constexpr (std::is_same_v<decay_type, const char*> || std::is_same_v<decay_type, std::string>) {
                // exist
                if (d != nullptr) {
                    if (d->type == cJSON_String) {
                        cJSON_SetValuestring(d, value);
                        return;
                    }
                    // non-existing, delete first, then add
                    cJSON_DeleteItemFromObjectCaseSensitive(data, key);
                }
                cJSON* new_item = cJSON_CreateString(value);
                cJSON_AddItemToObject(data, key, new_item);
            } else if constexpr (std::is_same_v<bool, decay_type>) {
                if (d != nullptr) {
                    if (d->type == cJSON_True || d->type == cJSON_False) {
                        cJSON_SetBoolValue(d, value);
                        return;
                    }
                    cJSON_DeleteItemFromObjectCaseSensitive(data, key);
                }
                cJSON* new_item = cJSON_CreateBool(value);
                cJSON_AddItemToObject(data, key, new_item);
            } else if constexpr (std::is_integral_v<decay_type>) {
                // exist
                if (d != nullptr) {
                    if (d->type == cJSON_Number) {
                        cJSON_SetNumberValue(d, value);
                        return;
                    }
                    cJSON_DeleteItemFromObjectCaseSensitive(data, key);
                }
                cJSON* new_item = cJSON_CreateNumber(value);
                cJSON_AddItemToObject(data, key, new_item);
            } else if constexpr (std::is_same_v<std::decay_t<std::shared_ptr<JSONObject>>, decay_type> || std::is_same_v<std::decay_t<std::shared_ptr<JSONArray>>, decay_type>) {
                if (d != nullptr) {
                    if (d->type == cJSON_Object) {
                        cJSON_ReplaceItemInObjectCaseSensitive(data, key, value->mutableData());
                        return;
                    }
                }
                cJSON_AddItemToObject(data, key, value->mutableData());
            }
        }

    private:
        bool valid_data() const {
            if (data == nullptr || !cJSON_IsObject(data)) {
                return false;
            }
            auto type = data->type;
            if (type != cJSON_Object) {
                return false;
            }
            return true;
        }

    private:
        cJSON* data = nullptr;
        bool is_root = false;
    };

    // JSONARray
    class JSONArray {
    public:
        static std::shared_ptr<JSONArray> load(const char* filename);
        static std::shared_ptr<JSONArray> loads(const char* content);

    public:
        explicit JSONArray() : data{cJSON_CreateArray()} {}
        explicit JSONArray(cJSON* data, bool is_root = false) : data{data}, is_root{is_root} {}
        ~JSONArray() {
            if (is_root && valid_data()) {
                cJSON_Delete(data);
            }
        }
        std::optional<int> getInt(std::size_t i) const;
        std::optional<long> getLong(std::size_t i) const;
        std::optional<double> getDouble(std::size_t i) const;
        std::optional<bool> getBool(std::size_t) const;
        std::optional<std::string> getString(std::size_t i) const;
        cJSON* mutableData() { return data; }
        const cJSON* constData() const { return data; }
        std::shared_ptr<JSONObject> getJSONObject(std::size_t i) const;
        std::shared_ptr<JSONArray> getJSONArray(std::size_t i) const;
        const char* toString() const;
        bool toFile(const char* filename) const;
        template <typename T>
        void put(std::size_t i, const char* key, const T& value) {
            if (!valid_data()) {
                return;
            }
            const auto sz = size();
            if (i < 0 || i > sz) {
                return;
            }
            cJSON* sub = cJSON_GetArrayItem(data, i);
            bool exist = sub == nullptr ? false : true;
            using decay_type = std::decay_t<decltype(value)>;
            cJSON* new_data = nullptr;
            if constexpr (std::is_same_v<bool, decay_type>) {
                new_data = cJSON_CreateBool(value);
            } else if constexpr (std::is_integral_v<decay_type>) {
                new_data = cJSON_CreateNumber(value);
            } else if constexpr (std::is_same_v<std::string, decay_type> || std::is_same_v<const char*, decay_type>) {
                new_data = cJSON_CreateString(value);
            } else if constexpr (std::is_same_v<std::decay_t<std::shared_ptr<JSONObject>>, decay_type> || std::is_same_v<std::decay_t<std::shared_ptr<JSONArray>>, decay_type>) {
                new_data = value->mutableData();
            }
            if (exist) {
                cJSON_ReplaceItemInArray(data, i, new_data);
            } else {
                cJSON_AddItemToArray(data, new_data);
            }
        }
        template <typename T>
        void add(const char* key, const T& value) {
            const auto sz = size();
            put(sz, key, value);
        }
        std::size_t size() const {
            if (data == nullptr) {
                return 0;
            }
            return cJSON_GetArraySize(data);
        }

    private:
        bool check_size(int i) const {
            const auto sz = cJSON_GetArraySize(data);
            if (i >= sz || i < 0) {
                return false;
            }
            return true;
        }
        bool valid_data() const {
            if (data == nullptr || !cJSON_IsArray(data)) {
                return false;
            }
            const auto type = data->type;
            if (type != cJSON_Array) {
                return false;
            }
            return true;
        }

    private:
        cJSON* data = nullptr;
        bool is_root = false;
        friend class JSONObject;
    };
}  // namespace wwc