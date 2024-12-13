cmake_minimum_required(VERSION 3.16)

project(watertool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts LinguistTools)
qt_standard_project_setup()

qt_add_executable(watertool
    main.cpp
    window.cpp
    dataset.cpp
    location_dataset.cpp
    dashboard.cpp
    dashboard_item.cpp
    sample.cpp
    pollutant_overview_page.cpp
    persistent_organic_pollutants_page.cpp
    environmental_litter_page.cpp
    pollutant_card.cpp
    flowlayout.cpp
    fluorinated_compounds_page.cpp
    compliance_page.cpp
)

qt_add_translations(watertool TS_FILES i18n/watertool_fr.ts)

target_link_libraries(watertool PRIVATE Qt6::Widgets Qt6::Charts)

set_target_properties(watertool PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE OFF
)
