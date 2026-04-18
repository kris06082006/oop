cmake_minimum_required(VERSION 3.10)

project(lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    main.cpp
    check.cpp
    cut.cpp
    io.cpp
    java_properties.cpp
)

add_executable(lab1 ${SOURCES})
