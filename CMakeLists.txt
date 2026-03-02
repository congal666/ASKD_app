cmake_minimum_required(VERSION 3.19)
project(askd_clone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort)
qt_standard_project_setup()

qt_add_executable(askd_clone
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ConnectionCheckDialog.h
    ConnectionCheckDialog.cpp
    ConnectionCheckDialog.ui
)

target_link_libraries(askd_clone PRIVATE Qt6::Core Qt6::Widgets Qt6::SerialPort)
