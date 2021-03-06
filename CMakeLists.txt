cmake_minimum_required(VERSION 3.5)

project(fhticker LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}/inc
                    )


set(PROJECT_SOURCES
        main.cpp
        src/mainwindow.cpp
        inc/mainwindow.h
        src/mainwindow.ui
        src/connection.cpp
        inc/connection.h
        src/htmlparser.cpp
        inc/htmlparser.h
        src/checker.cpp
        inc/checker.h
        src/notification.cpp
        inc/notification.h
)

qt5_add_resources(PROJECT_SOURCES resources.qrc)

add_executable(fhticker
    ${PROJECT_SOURCES}
    )

target_link_libraries(fhticker PRIVATE Qt${QT_VERSION_MAJOR}::Widgets curl crypto Xss X11)
