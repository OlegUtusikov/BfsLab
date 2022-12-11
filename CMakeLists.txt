cmake_minimum_required(VERSION 3.18)
project(BfsLab)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Ofast -march=native -mtune=native")

find_package(Threads REQUIRED)
find_package(Parlay REQUIRED)

add_executable(BfsLab main.cpp bfs_par.h bfs_seq.h utils.h)

target_link_libraries(BfsLab PUBLIC Parlay::parlay)
