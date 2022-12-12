//
// Created by utusikov on 11.12.22.
//

#pragma once

#include <array>
#include <algorithm>
#include <string>
#include <cxxabi.h>

namespace utils {
    inline std::string demangle(const char *raw_name) {
        int status = 0;
        char *real_name = abi::__cxa_demangle(raw_name, nullptr, nullptr, &status);
        std::string res_name = (status == 0) ? real_name : raw_name;
        std::free(real_name);
        return res_name;
    }

    template<typename T>
    std::string demangle() {
        return demangle(typeid(T).name());
    }

    static inline uint64_t get_max_number_v(uint32_t len) {
        return (((1ULL * len * len) + len) * len) + len;
    }
    static inline uint32_t zip_point(std::tuple<uint32_t, uint32_t, uint32_t> point, uint32_t offset) {
        assert(1ULL * std::numeric_limits<uint32_t>::max() >= get_max_number_v(offset));
        assert(std::max(std::max(std::get<0>(point), std::get<1>(point)), std::get<2>(point)) < offset);
        return (((std::get<0>(point) * offset) + std::get<1>(point)) * offset) + std::get<2>(point);
    }

    static inline std::tuple<uint32_t, uint32_t, uint32_t> unzip_point(uint32_t v, uint32_t offset) {
        uint32_t z = v % offset;
        v /= offset;
        uint32_t y = v % offset;
        v /= offset;
        uint32_t x = v % offset;
        return std::make_tuple(x, y, z);
    }

    static inline std::string str_point(std::pair<std::pair<uint32_t, uint32_t>, uint32_t> point) {
        return "{" + std::to_string(point.first.first) + ", " + std::to_string(point.first.second) + ", " +
               std::to_string(point.second) + "}";
    }

    static inline std::vector<uint32_t> get_cube_nbs(uint32_t v, uint32_t len) {
        auto point = unzip_point(v, len);
        std::vector<uint32_t> res;
        res.reserve(6U);
        for (int dx = (std::get<0>(point) == 0 ? 0 : -1); dx <= (std::get<0>(point) + 1 == len ? 0 : 1); ++dx) {
            for (int dy = (std::get<1>(point) == 0 ? 0 : -1); dy <= (std::get<1>(point) + 1 == len ? 0 : 1); ++dy) {
                for (int dz = (std::get<2>(point) == 0 ? 0 : -1); dz <= (std::get<2>(point) + 1 == len ? 0 : 1); ++dz) {
                    if (dx * dx + dy * dy + dz * dz == 1u) {
                        res.emplace_back(utils::zip_point(
                                std::make_tuple(std::get<0>(point) + dx, std::get<1>(point) + dy,
                                                std::get<2>(point) + dz), len));
                    }
                }
            }
        }
        return res;
    }

}   // namespace utils
