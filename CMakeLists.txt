cmake_minimum_required(VERSION 3.5...3.21)
project(meh-lang VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # 1.14
)
FetchContent_MakeAvailable(googletest)

set (MEH_LANG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (MEH_LANG_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/tests)

set (MEH_LANG_SOURCE 
${MEH_LANG_SOURCE_DIR}/main.cpp
${MEH_LANG_SOURCE_DIR}/meh.cpp
${MEH_LANG_SOURCE_DIR}/meh_scanner.cpp
${MEH_LANG_SOURCE_DIR}/meh_expr.cpp
${MEH_LANG_SOURCE_DIR}/meh_parser.cpp
${MEH_LANG_SOURCE_DIR}/meh_interpreter.cpp
)

set (MEH_LANG_TEST_SOURCE 
${MEH_LANG_TESTS_DIR}/pprinter_test.cpp
${MEH_LANG_SOURCE_DIR}/meh.cpp
${MEH_LANG_SOURCE_DIR}/meh_scanner.cpp
${MEH_LANG_SOURCE_DIR}/meh_expr.cpp
${MEH_LANG_SOURCE_DIR}/meh_parser.cpp
${MEH_LANG_SOURCE_DIR}/meh_interpreter.cpp
)


add_executable(meh-lang ${MEH_LANG_SOURCE})

add_executable(
 meh-lang_test 
  ${MEH_LANG_TEST_SOURCE}
)

target_include_directories(meh-lang_test PUBLIC ${MEH_LANG_SOURCE_DIR})
target_include_directories(meh-lang PUBLIC ${MEH_LANG_SOURCE_DIR})

target_link_libraries(
  meh-lang_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(meh-lang_test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
