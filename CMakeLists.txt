cmake_minimum_required(VERSION 3.14)
project(rpp)

add_executable(rpp rpp.cpp)
target_compile_features(rpp PRIVATE cxx_std_17)
