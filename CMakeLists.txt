cmake_minimum_required(VERSION 3.10)

project(games CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(test test.cpp)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(examples
    doublependulum
)

foreach(example ${examples})
    add_executable(${example} WIN32 example/${example}.cpp)
    target_include_directories(${example} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endforeach()
