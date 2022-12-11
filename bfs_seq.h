//
// Created by utusikov on 11.12.22.
//

#pragma once

#include <algorithm>
#include <queue>
#include <unordered_set>
#include <chrono>

#include <parlay/sequence.h>
#include "utils.h"

struct bfs_seq final {
    template<typename T>
    using result_t = std::vector<std::vector<T>>;

    bfs_seq() = delete;

    ~bfs_seq() = delete;

    template<typename V, typename G>
    static result_t<V> bfs(V start, uint32_t n, const G &graph) {
        std::unordered_set<V> visited;
        visited.reserve(n);
        std::vector<V> frontier(1, start);
        visited.insert(start);

        result_t<V> res;
        std::vector<V> new_frontier;
        while (!frontier.empty()) {
            new_frontier.clear();
            new_frontier.reserve(frontier.size());
            res.emplace_back(std::move(frontier));
            for (const V &from: res.back()) {
                auto edges = graph(from);
                if (!edges.empty()) {
                    for (V to: edges) {
                        if (visited.find(to) == visited.end()) {
                            visited.insert(to);
                            new_frontier.emplace_back(to);
                        }
                    }
                }
            }
            frontier = {};
            std::swap(frontier, new_frontier);
        }
        return res;
    }
};