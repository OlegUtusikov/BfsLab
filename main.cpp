//
// Created by utusikov on 11.12.22.
//

#include <vector>

#include <iostream>
#include <chrono>
#include <parlay/sequence.h>
#include <numeric>
#include <parlay/internal/get_time.h>
#include "bfs_seq.h"
#include "utils.h"
#include "bfs_par.h"

template<typename Impl, typename G>
double benchmark(uint32_t start,
                 const G &graph,
                 uint32_t n,
                 size_t tries,
                 bool need_check = false,
                 typename Impl::template result_t<uint32_t> correct_ans = {}) {
    std::cout << (need_check ? "Check: " : "Benchmark: ") << utils::demangle<Impl>() << ". Max number v: " << n << std::endl;

    const auto dump_seconds = [need_check](const std::string &prefix, double time) {
        if (!need_check) {
            std::cout << prefix << ": " << time << "s" << std::endl;
        }
    };

    std::vector<double> times;
    times.reserve(tries);

    typename Impl::template result_t<uint32_t> result;

    parlay::internal::timer t;
    for (int tr = 0; tr < tries; tr++) {
        t.start();
        result = Impl::bfs(start, n, graph);
        times.emplace_back(t.stop());
        dump_seconds("Time of try " + std::to_string(tr), times.back());
        if (need_check) {
            for (auto &fr: correct_ans) {
                std::sort(fr.begin(), fr.end());
            }
            for (auto &fr: result) {
                std::sort(fr.begin(), fr.end());
            }
            assert(correct_ans.size() == result.size());
            for (size_t i = 0; i < correct_ans.size(); ++i) {
                for (size_t j = 0; j < correct_ans[i].size(); ++j) {
                    assert(correct_ans[i][j] == result[i][j]);
                }
            }
        }
    }
    double avg_time = std::accumulate(times.begin(), times.end(), 0.0) / static_cast<double>(times.size());
    dump_seconds("Avg exec time", avg_time);
    return avg_time;
}


template<typename Impl1 = bfs_seq, typename Impl2 = bfs_par>
void test_equal(uint32_t len = 100) {
    const auto g = [len] (uint32_t v) { return utils::get_cube_nbs(v, len); };
    uint32_t n = utils::get_max_number_v(len);
    std::cout << "Start equal test. Size: " << n << std::endl;
    auto res1 = Impl1::bfs(0u, n, g);
    auto res2 = Impl2::bfs(0u, n, g);
    assert(res1.size() == res2.size());
    for (size_t i = 0; i < res1.size(); ++i) {
        std::sort(res1[i].begin(), res1[i].end());
        std::sort(res2[i].begin(), res2[i].end());
        for (size_t j = 0; j < res1[i].size(); ++j) {
            assert(res1[i][j] == res2[i][j]);
        }
    }
    std::cout << "Finish test!" << std::endl;
}

int main() {
    using namespace utils;

    benchmark<bfs_seq>(0ULL,
                       [](uint32_t v) { return utils::get_cube_nbs(v, 2u); },
                       get_max_number_v(2u),
                       1,
                       true,
                       {{zip_point({0u, 0u, 0u}, 2u)},
                        {zip_point({0u, 0u, 1u}, 2u), zip_point({0u, 1u, 0u}, 2u), zip_point({1u, 0u, 0u}, 2u)},
                        {zip_point({0U, 1u, 1u}, 2u), zip_point({1u, 0u, 1u}, 2u), zip_point({1u, 1u, 0u}, 2u)},
                        {zip_point({1u, 1u, 1u}, 2u)}});

    benchmark<bfs_par>(0ULL,
                       [](uint32_t v) { return utils::get_cube_nbs(v, 2u); },
                       get_max_number_v(2u),
                       1,
                       true,
                       {{zip_point({0u, 0u, 0u}, 2u)},
                        {zip_point({0u, 0u, 1u}, 2u), zip_point({0u, 1u, 0u}, 2u), zip_point({1u, 0u, 0u}, 2u)},
                        {zip_point({0U, 1u, 1u}, 2u), zip_point({1u, 0u, 1u}, 2u), zip_point({1u, 1u, 0u}, 2u)},
                        {zip_point({1u, 1u, 1u}, 2u)}});

    test_equal(4);
    test_equal(40);
    test_equal(100);

    int tries = 5;
    uint32_t start = 0;
    uint32_t len = 500;
    double avg_par = benchmark<bfs_par>(start,
                                        [len](uint32_t v) { return utils::get_cube_nbs(v, len); },
                                        get_max_number_v(len),
                                        tries);
    double avg_seq = benchmark<bfs_seq>(start,
                                        [len](uint32_t v) { return utils::get_cube_nbs(v, len); },
                                        get_max_number_v(len),
                                        tries);
    std::cout << "Speed up with std: " << avg_seq / avg_par << std::endl;
}
