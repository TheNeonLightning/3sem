cmake_minimum_required(VERSION 3.15)
project(desigion_tree)

set(CMAKE_CXX_STANDARD 14)

add_executable(desigion_tree main.cpp decision_tree.h decision_tree.cpp)