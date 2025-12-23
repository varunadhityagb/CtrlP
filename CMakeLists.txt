cmake_minimum_required(VERSION 3.16)
project(CtrlP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)

# Use poppler-qt6 via pkg-config (Arch-correct)
pkg_check_modules(POPPLER REQUIRED poppler-qt6)

add_executable(CtrlP
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/Document.cpp
    src/Document.h
)

target_include_directories(CtrlP PRIVATE
    ${POPPLER_INCLUDE_DIRS}
)

target_link_libraries(CtrlP PRIVATE
    Qt6::Widgets
    ${POPPLER_LIBRARIES}
)

target_compile_options(CtrlP PRIVATE
    ${POPPLER_CFLAGS_OTHER}
)
