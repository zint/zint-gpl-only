# (c) 2008 by BogDan Vatra < bogdan@licentia.eu >

cmake_minimum_required(VERSION 2.6)
project(zint-package)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#set(CMAKE_VERBOSE_MAKEFILE ON)
#comment or remove the above line before release

set (ZINT_VERSION_MAJOR 2)
set (ZINT_VERSION_MINOR 1)
set (ZINT_VERSION_RELEASE 2)
set (ZINT_VERSION "${ZINT_VERSION_MAJOR}.${ZINT_VERSION_MINOR}.${ZINT_VERSION_RELEASE}" )

add_definitions (-DZINT_VERSION=\"${ZINT_VERSION}\" -Wall)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

include (SetPaths.cmake)

###############################################################################
# Set defaults for Universal Binaries. We want 32-bit Intel/PPC on 10.4 
# and 32/64-bit Intel/PPC on >= 10.5. Anything <= 10.3 doesn't support.
IF(APPLE)
    # These are just defaults/recommendations, but how we want to build
    # out of the box. But the user needs to be able to change these options.
    # So we must only set the values the first time CMake is run, or we 
    # will overwrite any changes the user sets.
    # FORCE is used because the options are not reflected in the UI otherwise.
    IF(NOT "${OPENTHREADS_CONFIG_HAS_BEEN_RUN_BEFORE}")
        # This is really fragile, but CMake doesn't provide the OS system 
        # version information we need. (Darwin versions can be changed 
        # independently of OS X versions.)
        # It does look like CMake handles the CMAKE_OSX_SYSROOT automatically.
        IF(EXISTS /Developer/SDKs/10.5.sdk)
#Qt does not support ppc64 and x86_64
#            SET(CMAKE_OSX_ARCHITECTURES "ppc;i386;ppc64;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
            SET(CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
        ELSE(EXISTS /Developer/SDKs/10.5.sdk)
            IF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
#Qt does not support ppc64 and x86_64
#                SET(CMAKE_OSX_ARCHITECTURES "ppc;i386;ppc64;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
                SET(CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
                SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden" CACHE STRING "Flags used by the compiler during all build types." FORCE)
            ELSE(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
                # No Universal Binary support
            ENDIF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
        ENDIF(EXISTS /Developer/SDKs/10.5.sdk)
    ENDIF(NOT "${OPENTHREADS_CONFIG_HAS_BEEN_RUN_BEFORE}")
ENDIF(APPLE)


add_subdirectory(backend)
add_subdirectory(frontend)

find_package(Qt4)
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

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

install(FILES cmake/modules/FindZint.cmake DESTINATION ${CMAKE_ROOT}/Modules COMPONENT Devel)
