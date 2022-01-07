cmake_minimum_required(VERSION 2.8.12)

project(matrixmul)

find_package(OpenMP REQUIRED)
include_directories(${PROJECT_SOURCE_DIR})
add_executable(app main.c args/args.c matrix/matrix.c)  # be sure there's exactly one main() function in the source files
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(app PUBLIC OpenMP::OpenMP_CXX)
endif()