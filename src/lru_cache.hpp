#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <unordered_map>
#include <optional>

namespace wwc {

    template <typename K, typename V>
    class LruCache final {
    private:
        struct node;

    public:
        std::optional<std::reference_wrapper<const V>> get(const K& key);
        const V& get_or(const K& key, const V& default_val);
        void put(const K& key, const V& val);

        explicit LruCache(int cap) : cap_{cap} {
            head_ = std::make_shared<node>();
            tail_ = std::make_shared<node>();
            head_->next = tail_;
            tail_->prev = head_;
        }

    private:
        std::unordered_map<K, std::shared_ptr<node>> m_;
        struct node {
            K key;
            V val;
            std::weak_ptr<node> prev;
            std::shared_ptr<node> next;
        };
        std::shared_ptr<node> head_ = nullptr;
        std::shared_ptr<node> tail_ = nullptr;
        int cap_ = 0;

    private:
        void move_to_front(std::shared_ptr<node> n) {
            auto prev = n->prev.lock();
            auto next = n->next;
            prev->next = next;
            next->prev = prev;

            auto front = head_->next;

            head_->next = n;
            n->prev = head_;

            n->next = front;
            front->prev = prev;
        }
        void insert_to_front(std::shared_ptr<node> n) {
            auto front = head_->next;
            head_->next = n;
            n->prev = head_;

            front->prev = n;
            n->next = front;
        }
        void remove_tail() {
            auto tail = tail_->prev.lock();
            auto prev = tail->prev.lock();
            prev->next = tail_;
            tail_->prev = prev;
        }
    };

    template <typename K, typename V>
    std::optional<std::reference_wrapper<const V>> LruCache<K, V>::get(const K& key) {
        if (m_.find(key) == m_.cend()) {
            return std::nullopt;
        }
        auto n = m_.at(key);
        move_to_front(n);
        return {std::cref(n->val)};
    }

    template <typename K, typename V>
    void LruCache<K, V>::put(const K& key, const V& val) {
        if (m_.find(key) != m_.cend()) {
            auto node = m_[key];
            node->val = val;
            return;
        }
        auto new_node = std::make_shared<node>();
        m_[key] = new_node;
        new_node->val = val;
        new_node->key = key;
        insert_to_front(new_node);
        if (m_.size() > cap_) {
            auto tail = tail_->prev.lock();
            remove_tail();
            m_.erase(tail->key);
        }
    }

    template <typename K, typename V>
    const V& LruCache<K, V>::get_or(const K& key, const V& default_val) {
        return get(key).value_or(default_val);
    }

}


