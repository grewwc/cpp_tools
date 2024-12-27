#pragma once
#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>
namespace wwc {

    template <typename Key, typename Value>
    class LruCache {
    public:
        struct node;

    public:
        // constructors
        LruCache(std::size_t cap) : cap_{cap}, count_{0} {
        }

        void put(const Key& key, const Value& value) {
            const auto it = map_.find(key);
            if (it == map_.cend()) {
                auto new_node = std::make_shared<node>(key, value);
                map_.emplace(key, new_node);
                move_to_head(new_node);
                count_++;
                if (count_ > cap_) {
                    remove_tail();
                }
            } else {
                map_[key] = it->second;
                move_to_head(it->second);
            }
        }

        const std::shared_ptr<Value> get(const Key& key) {
            const auto it = map_.find(key);
            if (it == map_.cend()) {
                return {};
            }
            auto res = std::make_shared<Value>(it->second->value);
            // modify linked list
            move_to_head(it->second);
            return res;
        }

    public:
        std::size_t cap_ = 0;
        std::size_t count_ = 0;
        std::unordered_map<Key, std::shared_ptr<node>> map_;
        std::shared_ptr<node> head_ = nullptr;
        std::shared_ptr<node> tail_ = nullptr;

        struct node {
            explicit node(const Key& key, const Value& value) : key{key}, value{value} {
            }
            const Key& key;
            const Value& value;
            std::shared_ptr<node> prev = nullptr;
            std::shared_ptr<node> next = nullptr;
        };

        void move_to_head(std::shared_ptr<node> n) {
            if (n == nullptr || n == head_) {
                return;
            }
            if (head_ == nullptr) {
                head_ = n;
                tail_ = n;
                return;
            }
            auto prev = n->prev;
            auto next = n->next;
            if (prev != nullptr) {
                prev->next = next;
            }
            if (next != nullptr) {
                next->prev = prev;
            }
            n->next = head_;
            head_->prev = n;
            head_ = n;
            if (tail_ == n) {
                tail_ = n->prev;
            }
        }

        void remove_tail() {
            if (tail_ == nullptr || count_ == 0) {
                return;
            }
            map_.erase(tail_->key);
            auto prev = tail_->prev;
            if (prev != nullptr) {
                prev->next = nullptr;
            }
            tail_ = prev;
            count_--;
        }
    };
}  // namespace wwc