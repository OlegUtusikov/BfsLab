//
// Created by utusikov on 11.12.22.
//

#pragma once

#include <algorithm>
#include <chrono>

#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/sequence.h>
#include "utils.h"

struct bfs_par final {
    template<typename T>
    using result_t = parlay::sequence<parlay::sequence<T>>;

    bfs_par() = delete;

    ~bfs_par() = delete;

    template<typename V, typename G>
    static result_t<V> bfs(V start, uint32_t n, const G &graph) {
        auto visited = parlay::tabulate<std::atomic<bool>>(n, [&](uint32_t i) {
            return i == start;
        });

        parlay::sequence<V> frontier(1, start);
        result_t<V> frontiers;
        while (!frontier.empty()) {
//            auto [ranges, total_nbs] = parlay::scan(parlay::map(frontier, [&](V from) {
//                return graph(from).size();
//            }));
//            parlay::sequence<V> out(total_nbs);
//            parlay::parallel_for(0, frontier.size(), [&](size_t i) {
//                auto edges = graph(frontier[i]);
//                size_t offset = ranges[i];
//                parlay::copy(edges, out.cut(offset, offset + edges.size()));
//            });
            auto out = parlay::flatten(parlay::map(frontier, [&] (V from) {
                return graph(from);
            }));
            frontiers.emplace_back(std::move(frontier));
            frontier = filter(out, [&](auto &&v) {
                bool expected = false;
                return (!visited[v]) && visited[v].compare_exchange_strong(expected, true, std::memory_order::acq_rel);
            });
        }
        return frontiers;
    }
};