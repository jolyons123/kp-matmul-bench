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

####################
# Build executable #
####################
#add_library(matrix matrix/matrix.c)
#target_link_libraries(matrix PUBLIC OpenMP::OpenMP_CXX)
add_executable(app main.c args/args.c matrix/matrix.c)
#target_link_libraries(app PRIVATE matrix)
target_link_libraries(app PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic -Wunknown-pragmas -Werror -fopenmp)
#set_property(TARGET app PROPERTY C_STANDARD 99)

###################
# Test executable #
###################
include_directories(${PROJECT_SOURCE_DIR}/test)
add_executable(tests test/test.cpp ${PROJECT_SOURCE_DIR}/matrix/matrix.c)
#target_link_libraries(tests PRIVATE Catch2::Catch2WithMain matrix)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain OpenMP::OpenMP_CXX)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Wunknown-pragmas -Werror -fopenmp)

# modules for running tests and coverage
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras ${PROJECT_SOURCE_DIR}/cmake-modules)

include(CTest)
include(Catch)
include(CodeCoverage)
# Override program path to gcovr
find_program( GCOVR_PATH gcovr PATHS "C:/ProgramData/Miniconda3/Scripts")

append_coverage_compiler_flags()
setup_target_for_coverage_gcovr_html(
    NAME coverage
    EXECUTABLE tests
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
    EXCLUDE "build/*" "test/*") 
catch_discover_tests(tests)