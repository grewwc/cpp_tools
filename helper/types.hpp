#pragma once
#include <algorithm>
#include <memory>
#include <optional>
namespace __helper
{
    template <typename T>
    class _Heap
    {
    public:
        std::unique_ptr<T[]> data;
        _Heap(int sz = 16) : sz{0}, cap{sz} { data = std::make_unique<T[]>(cap); }
        bool insert(const T &val)
        {
            if (sz + 1 == cap)
            {
                cap *= 2;
                change_data(cap);
            }
            data[++sz] = val;
            swim(sz, 1);
            return true;
        }

        bool is_empty() const { return sz == 0; }

        size_t size() const { return sz; }

        const T &peek() const { return data[1]; }

        std::optional<T> remove()
        {
            if (is_empty())
            {
                return std::nullopt;
            }
            T result = data[1];
            std::swap(data[1], data[sz--]);
            sink(1, sz + 1);
            if (sz * 2 < cap)
            {
                cap /= 2;
                change_data(cap);
            }
            return {result};
        }

        virtual ~_Heap() = default;

    protected:
        virtual bool less(const T &val1, const T &val2) const = 0;

    private:
        size_t sz;
        void swim(size_t cur, size_t lo)
        {
            size_t p = cur / 2;
            // no parent
            if (p < lo)
            {
                return;
            }
            if (less(data[cur], data[p]))
            {
                std::swap(data[p], data[cur]);
            }
            swim(p, lo);
        }
        void sink(size_t cur, size_t hi)
        {
            size_t idx = cur * 2;
            // no child
            if (idx >= hi)
            {
                return;
            }

            // has right child && right child is smaller
            if (idx + 1 < hi && less(data[idx + 1], data[idx]))
            {
                idx++;
            }
            // cur is smaller than left child
            if (less(data[idx], data[cur]))
            {
                std::swap(data[cur], data[idx]);
            }
            sink(idx, hi);
        }
        void change_data(size_t new_cap)
        {
            if (new_cap <= 16) return;
            T *new_data = new T[new_cap];
            std::copy_n(data.get(), sz + 1, new_data);
            data.reset(new_data);
        }
        size_t cap;
    };

} // namespace __helper