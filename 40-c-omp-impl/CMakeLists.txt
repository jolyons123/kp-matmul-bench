cmake_minimum_required(VERSION 3.5)

project(matrixmul)

############
# packages #
############
find_package(OpenMP REQUIRED)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

include_directories(${PROJECT_SOURCE_DIR})

###################
# Compile options #
###################
add_compile_options(-Wall -Wextra -Wpedantic -Wunknown-pragmas -Werror -fopenmp)

####################
# Build executable #
####################
add_library(matrix matrix/matrix.c format/format.c)
target_link_libraries(matrix PUBLIC OpenMP::OpenMP_CXX m)
set_property(TARGET matrix PROPERTY C_STANDARD 99)

add_executable(app main.c args/args.c)
target_link_libraries(app PUBLIC matrix)
set_property(TARGET app PROPERTY C_STANDARD 99)

###################
# Test executable #
###################
include_directories(${PROJECT_SOURCE_DIR}/test)
add_executable(tests test/test.cpp)
target_link_libraries(tests PUBLIC Catch2::Catch2WithMain matrix)

# modules for running tests and coverage
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras ${PROJECT_SOURCE_DIR}/cmake-modules)

include(CTest)
include(Catch)
catch_discover_tests(tests)

#################
# Code coverage #
#################
# Uncomment the following lines to enable code coverage with gcovr
# The find_program command has to be adjusted in order to find the gcovr location

#include(CodeCoverage)
#find_program( GCOVR_PATH gcovr PATHS "C:/ProgramData/Miniconda3/Scripts")

#append_coverage_compiler_flags()
#setup_target_for_coverage_gcovr_html(
#    NAME coverage
#    EXECUTABLE tests
#    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
#    EXCLUDE "build/*" "test/*" "args/*" "format/*" "main.c") 