#pragma once
#include <algorithm>
#include <memory>
#include <optional>
namespace __helper {
    template <typename T>
    class _Heap {
    public:
        _Heap(std::size_t sz) : sz{0}, cap{sz} { data.resize(cap + 1); }

        bool insert(T val) {
            if (sz + 1 == cap) {
                cap *= 2;
                resize(cap);
            }
            data[++sz] = std::move(val);
            swim(sz, 1);
            return true;
        }

        bool is_empty() const { return sz == 0; }

        size_t size() const { return sz; }

        const T &peek() const { return data[1]; }

        std::optional<T> remove() {
            if (is_empty()) {
                return std::nullopt;
            }
            T result = data[1];
            std::swap(data[1], data[sz--]);
            sink(1, sz + 1);
            if (sz * 2 < cap) {
                cap /= 2;
                resize(cap, true);
            }
            return std::optional<T>(std::in_place, std::move(result));
        }

        virtual ~_Heap() = default;

    protected:
        virtual bool less(const T &val1, const T &val2) const = 0;

    private:
        void swim(size_t cur, size_t lo) {
            size_t p = cur / 2;
            // no parent
            if (p < lo) {
                return;
            }
            if (less(data[cur], data[p])) {
                std::swap(data[p], data[cur]);
            }
            swim(p, lo);
        }
        void sink(size_t cur, size_t hi) {
            size_t idx = cur * 2;
            // no child
            if (idx >= hi) {
                return;
            }

            // has right child && right child is smaller
            if (idx + 1 < hi && less(data[idx + 1], data[idx])) {
                idx++;
            }
            // cur is smaller than left child
            if (less(data[idx], data[cur])) {
                std::swap(data[cur], data[idx]);
            }
            sink(idx, hi);
        }
        void resize(size_t new_cap, bool shrink = false) {
            if (shrink && new_cap <= 1024) return;
            data.resize(new_cap);
        }

        // private members
        size_t cap;
        size_t sz;
        std::vector<T> data;
    };

}  // namespace __helper