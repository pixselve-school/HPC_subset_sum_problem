cmake_minimum_required(VERSION 3.27)
project(subset_sum_problem C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

add_executable(subset_sum_problem main.c utils.c)
