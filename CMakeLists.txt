cmake_minimum_required(VERSION 3.27)
project(Graph_Theory_LeetCode25)

set(CMAKE_CXX_STANDARD 23)

add_executable(project main.cpp
        classes/Graph.h
        classes/Parser.h
        functions/parserf.h
        functions/parserf.cpp
        libs.h
        classes/Node.h
)


