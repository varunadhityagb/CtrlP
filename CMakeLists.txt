cmake_minimum_required(VERSION 3.16)
project(CtrlP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(CtrlP
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

target_link_libraries(CtrlP Qt6::Widgets)
