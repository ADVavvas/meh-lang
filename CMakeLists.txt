cmake_minimum_required(VERSION 3.5...3.21)
project(meh-lang VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

set (MEH_LANG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (MEH_LANG_SOURCE 
${MEH_LANG_SOURCE_DIR}/main.cpp
${MEH_LANG_SOURCE_DIR}/meh.cpp
${MEH_LANG_SOURCE_DIR}/meh.hpp
${MEH_LANG_SOURCE_DIR}/meh_scanner.cpp
${MEH_LANG_SOURCE_DIR}/meh_scanner.hpp
${MEH_LANG_SOURCE_DIR}/meh_error.hpp
${MEH_LANG_SOURCE_DIR}/meh_token_type.hpp
${MEH_LANG_SOURCE_DIR}/meh_token.hpp
)
add_executable(meh-lang ${MEH_LANG_SOURCE})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
