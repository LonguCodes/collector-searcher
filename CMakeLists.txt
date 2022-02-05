cmake_minimum_required(VERSION 3.20)
project(Oleks C)

set(CMAKE_C_STANDARD 11)

add_executable(Oleks main.c searcher.c number_parser.c number_parser.h set.c set.h collector.c)
