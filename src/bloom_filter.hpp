#include <functional>

namespace wwc {
    template <typename T>
    class BloomFilter {
    public:
        BloomFilter() = default;

        bool possible_exist(const T& val) const noexcept {
            std::size_t hash_val = hasher_(val);
            while (hash_val > 0) {
                const size_t idx = hash_val % size_;
                if (record_[idx] == 0) {
                    return false;
                }
                calc_next_hash_val(hash_val);
            }
            return true;
        }

        bool non_exist(const T& val) const noexcept { return !possible_exist(val); }

        void add(const T& val) noexcept {
            std::size_t hash_val = hasher_(val);
            while (hash_val > 0) {
                const size_t idx = hash_val % size_;
                ++record_[idx];
                calc_next_hash_val(hash_val);
            }
        }
        void remove(const T& val) noexcept {
            std::size_t hash_val = hasher_(val);
            while (hash_val > 0) {
                const size_t idx = hash_val % size_;
                auto& cnt = record_[idx];
                if (cnt > 0) {
                    --cnt;
                }
                calc_next_hash_val(hash_val);
            }
        }

    private:
        static void calc_next_hash_val(std::size_t& hash_val) noexcept {
            if (hash_val == 0) {
                return 0;
            }
            hash_val = (hash_val - 1) / 3;
        }

    private:
        std::hash<T> hasher_{};
        constexpr static inline int size_ = 1024 * 1024;
        int record_[size_] = {0};
    };
}  // namespace wwc