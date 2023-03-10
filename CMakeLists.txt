cmake_minimum_required(VERSION 3.5)

project(GeotiffConverter_2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(interface
    tiff.h
    tiffio.h
    CImg.h
    tifffunctions.h
    qtfunctions.h
    commonfunctions.h
    pngfunctions.h
    configurergbform.h
    conversionparameters.h
    displaytask.h
)
set(src
    tifffunctions.cpp
    qtfunctions.cpp
    commonfunctions.cpp
    pngfunctions.cpp
    configurergbform.cpp
    conversionparameters.cpp
    displaytask.cpp
)
set(uis
    configurergbform.ui
)

set(all_src
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${interface}
        ${src}
        ${uis}
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GeotiffConverter_2
        MANUAL_FINALIZATION
        ${all_src}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GeotiffConverter_2 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GeotiffConverter_2 SHARED
            ${all_src}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GeotiffConverter_2
            ${all_src}
        )
    endif()
endif()

include(FindZLIB)
include(FindPNG)

target_link_libraries(GeotiffConverter_2 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ZLIB::ZLIB PNG::PNG -lX11 -ltiff)

set_target_properties(GeotiffConverter_2 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS GeotiffConverter_2
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GeotiffConverter_2)
endif()
