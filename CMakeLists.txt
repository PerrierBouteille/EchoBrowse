cmake_minimum_required(VERSION 3.29)
project(EchoBrowse)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0 webkit2gtk-4.0)

add_executable(EchoBrowse
    src/main.cpp
        src/BrowserWindow.cpp
        src/Navigation.h
        src/History.cpp
        src/History.h
        src/Navigation.cpp)

target_include_directories(EchoBrowse PRIVATE ${GTK_INCLUDE_DIRS})
target_link_libraries(EchoBrowse ${GTK_LIBRARIES})