cmake_minimum_required(VERSION 3.29.4)

project(simple-c-compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_INIT "-fsanitize=address -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fsanitize=address -fsanitize=undefined")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fsanitize=address -fsanitize=undefined")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fsanitize=address -fsanitize=undefined")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

#[[
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
find_program(CLANG_TIDY_EXECUTABLE clang-tidy REQUIRED)
set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR})
set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR})
]]

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.6.0
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_library(compiler src/ast.cpp src/lexer.cpp src/tacky_ast.cpp src/tokens.cpp)
target_include_directories(compiler
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

add_executable(sc2 src/compiler.cpp)
target_include_directories(sc2
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_BINARY_DIR}/include>
)

target_link_libraries(sc2 PRIVATE compiler)

add_subdirectory(test)
