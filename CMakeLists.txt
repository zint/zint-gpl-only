# (c) 2008 by BogDan Vatra < bogdan@licentia.eu >

cmake_minimum_required(VERSION 2.6)
project(zint-package)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_VERBOSE_MAKEFILE ON)
#comment remove the above line before release

set (ZINT_VERSION_MAJOR 1)
set (ZINT_VERSION_MINOR 80)
set (ZINT_VERSION_RELEASE 0)
set (ZINT_VERSION "${ZINT_VERSION_MAJOR}.${ZINT_VERSION_MINOR}.${ZINT_VERSION_RELEASE}" )

add_definitions (-DZINT_VERSION=\"${ZINT_VERSION}\")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

find_package(Qt4)

add_subdirectory(backend)
add_subdirectory(frontend)

if (QT4_FOUND)
    set( QT_USE_QTGUI   TRUE )
    include( ${QT_USE_FILE} )
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR} 
        ${QT_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        )
    add_subdirectory(backend_qt4)
    add_subdirectory(frontend_qt4)
endif(QT4_FOUND)
