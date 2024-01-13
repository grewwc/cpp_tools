#pragma once
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace wwc {
    template <typename T>
    class UF {
    public:
        UF(int sz, int (*id_func)(const T&) = nullptr, bool ordered = false) : id_func{id_func}, num_group{sz}, ordered{ordered} {
            ids.resize(sz);
            for (int i = 0; i < sz; i++) {
                ids[i] = i;
            }
            sizes_.resize(sz);
            std::fill_n(sizes_.begin(), sizes_.size(), 1);
            m.reserve(sz);
        }

        bool connect(const T& t1, const T& t2) noexcept {
            int r1 = find_root(t1);
            int r2 = find_root(t2);
            if (r1 == r2) {
                return false;
            }
            if (ordered) {
                if (r1 < r2) {
                    ids[r2] = r1;
                } else {
                    ids[r1] = r2;
                }
            } else {
                if (sizes_[r1] < sizes_[r2]) {
                    ids[r1] = r2;
                    sizes_[r2] += sizes_[r1];
                } else {
                    ids[r2] = r1;
                    sizes_[r1] += sizes_[r2];
                }
            }
            --num_group;
            return true;
        }

        const T& get_val_by_id(int id) noexcept { return m[id]; }

        int num_groups() const noexcept { return num_group; }

        void add_nodes(int num_node) {
            for (int i = 0; i < num_node; i++) {
                ids.push_back(ids.size() + i);
                sizes_.push_back(1);
            }
        }

        int find_root(const T& t) noexcept {
            int id = convert(t);
            int id1 = id;
            if (id < 0 || id >= ids.size()) {
                char buf[30];
                snprintf(buf, sizeof(buf)-1, "invalid id: %d", id);
                throw std::logic_error(buf);
            }
            while (ids[id] != id) {
                id = ids[id];
            }
            while (ids[id1] != id) {
                int next = ids[id1];
                ids[id1] = id;
                id1 = next;
            }
            return id;
        }

        bool is_connected(const T& t1, const T& t2) noexcept { return find_root(t1) == find_root(t2); }

    private:
        std::vector<int> ids;
        std::vector<int> sizes_;
        int num_group;
        std::unordered_map<int, T> m;
        bool ordered = false;
        int (*id_func)(const T&) = nullptr;

        int convert(const T& t) noexcept {
            int result = -1;
            if (id_func != nullptr) {
                result = id_func(t);
            } else {
                result = (int)t;
            }
            if (m.find(result) == m.cend()) {
                m[result] = t;
            }
            return result;
        }
    };
}  // namespace wwc