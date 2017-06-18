cmake_minimum_required(VERSION 2.6)
project(rmg)

find_package (Threads)
add_executable(rmg src/constraints.cpp src/mgenerator.cpp src/chromosome.cpp src/main.cpp)
target_link_libraries (rmg pthread)
add_definitions(-std=c++17)
add_definitions(-lpthread)

install(TARGETS rmg RUNTIME DESTINATION bin)
