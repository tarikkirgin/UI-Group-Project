cmake_minimum_required(VERSION 3.16)

project(cwk2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(watertool
    main.cpp
    window.cpp
    dataset.cpp
    dashboard.cpp
    dashboard_item.cpp
    sample.cpp
    page.cpp
    pollutant_overview_page.cpp
)

target_link_libraries(watertool PRIVATE Qt6::Widgets)

set_target_properties(watertool PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE OFF
)
