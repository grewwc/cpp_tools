#pragma once
#include <deque>
#include <unordered_set>
#include <utility>

#include "include/BS_thread_pool.hpp"
#include "src/files.hpp"
#include "src/hierachy_mutex.hpp"
#include "src/stopwatch.hpp"
#include "src/stream.hpp"
#include "src/types.hpp"
#include "src/uf.hpp"
namespace wwc {
    template <typename T>
    class MinHeap : public __helper::_Heap<T> {
    public:
        explicit MinHeap(std::size_t sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T& val1, const T& val2) const override { return val1 < val2; }
    };

    template <typename T>
    class MaxHeap : public __helper::_Heap<T> {
    public:
        explicit MaxHeap(std::size_t sz = 16) : __helper::_Heap<T>{sz} {}

    private:
        virtual bool less(const T& val1, const T& val2) const override { return val1 > val2; }
    };

    /** weighted dag
     */
    template <typename T>
    class WDag {
    public:
        using weight_t = long;
        struct Route;

        void add_edge(const T& from, const T& to, weight_t weight = 1) {
            if (add_edge_if_not_exists(from, to, weight)) {
                return;
            }
            adj_list_[from][to] = weight;
        }

        size_t get_node_num() const noexcept { return node_set_.size(); }

        size_t get_edge_num() const noexcept { return n_edge_; }

        bool add_edge_if_not_exists(const T& from, const T& to, weight_t weight = 1) {
            node_set_.emplace(from);
            node_set_.emplace(to);
            if (adj_list_.count(from) == 0) {
                adj_list_[from] = {};
            }
            if (adj_list_[from].count(to) == 0) {
                adj_list_[from][to] = weight;
                n_edge_++;
                return true;
            }
            return false;
        }

        bool is_connected(const T& from, const T& to) const noexcept {
            std::unordered_set<T> visited;
            std::deque<T> q;
            visited.reserve(get_node_num());
            q.push_back(from);
            while (!q.empty()) {
                const auto& curr = q.front();
                q.pop_front();
                visited.insert(curr);
                if (adj_list_.count(curr) == 0) {
                    return false;
                }
                for (const auto& [n, w] : adj_list_.at(curr)) {
                    if (visited.count(n) > 0) {
                        continue;
                    }
                    if (n == to) {
                        return true;
                    }
                    q.push_back(n);
                }
            }
            return false;
        }

        Route min_dist(const T& from, const T& to) const noexcept {
            Route result;
            parent_.clear();
            std::unordered_set<T> visited;
            visited.reserve(get_node_num());
            std::unordered_map<T, weight_t> dist;
            constexpr weight_t INF = std::numeric_limits<weight_t>::max();
            if (adj_list_.count(from) == 0) {
                result.weight = -1;
                result.path = {from};
                return result;
            }
            const std::unordered_map<T, weight_t> adj = adj_list_.at(from);
            for (const auto& n : node_set_) {
                if (n == from) {
                    continue;
                }
                if (adj.count(n) == 0) {
                    dist[n] = INF;
                } else {
                    dist[n] = adj.at(n);
                    parent_[n] = from;
                }
            }
            // for (const auto& [n, w] : dist) {
            //     std::cout << n << " " << w   << std::endl;
            // }
            // std::cout << "==\n";
            while (visited.size() + 1 < node_set_.size()) {
                const auto& next_pair = find_next_node(dist, visited);
                const T* next = next_pair.first;
                if (next == nullptr) {
                    break;
                }
                // std::cout << "next: " << *next<<std::endl;
                if (adj_list_.count(*next) == 0) {
                    visited.emplace(*next);
                    continue;
                }
                for (const auto& [n, w] : adj_list_.at(*next)) {
                    const weight_t new_dist = next_pair.second + w;
                    if (new_dist < dist[n]) {
                        dist[n] = new_dist;
                        parent_[n] = *next;
                    }
                }
                visited.emplace(*next);
            }
            const weight_t distance = dist[to];
            result.weight = distance;
            const T* curr = &to;
            result.path.push_back(*curr);
            while (*curr != from) {
                result.path.push_back(parent_[*curr]);
                curr = &parent_[*curr];
            }
            std::reverse(result.path.begin(), result.path.end());
            return result;
        }

    public:
        struct Route {
            weight_t weight;
            std::vector<T> path;
        };

    private:
        std::pair<const T*, weight_t> find_next_node(const std::unordered_map<T, weight_t>& dist,
                                                     const std::unordered_set<T>& visited) const noexcept {
            const T* result = nullptr;
            weight_t curr_dist = std::numeric_limits<weight_t>::max();
            for (const auto& [n, d] : dist) {
                if (visited.count(n) > 0) {
                    continue;
                }

                if (d < curr_dist) {
                    curr_dist = d;
                    result = &n;
                }
            }
            return {result, curr_dist};
        }

    private:
        std::unordered_map<T, std::unordered_map<T, weight_t>> adj_list_;
        std::unordered_set<T> node_set_;
        mutable std::unordered_map<T, T> parent_;
        size_t n_edge_ = 0;
    };
}  // namespace wwc
