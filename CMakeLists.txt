cmake_minimum_required(VERSION 3.20)
project(searcher C)
project(collector C)

set(CMAKE_C_STANDARD 11)

add_executable(searcher number_parser.c number_parser.h set.c set.h searcher.c number_record.c number_record.h)
add_executable(collector number_parser.c number_parser.h set.c set.h collector.c number_record.c number_record.h)
