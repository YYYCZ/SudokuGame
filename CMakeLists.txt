cmake_minimum_required(VERSION 3.0.0)
project(Sudoku VERSION 1.0.0)

include(CTest)
enable_testing()

# add_library(Sudoku Sudoku.cpp)
aux_source_directory(./src/ DIR_SRCS)
add_executable(Sudoku ${DIR_SRCS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
