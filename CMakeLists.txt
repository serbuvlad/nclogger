cmake_minimum_required(VERSION 3.24)
project(
    nclogger

    VERSION 0.0.1
    DESCRIPTION "Ncurses logger"
    LANGUAGES CXX
)

# Includes and dependencies

include(CheckIPOSupported)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(Curses REQUIRED)

# Command-line options

set(NCLOGGER_BUILD_SHARED ON CACHE BOOL "Build shared library")
set(NCLOGGER_BUILD_STATIC OFF CACHE BOOL "Build static library")
set(NCLOGGER_BUILD_EXAMPLES OFF CACHE BOOL "Build example applications")

# Constants

set(NCLOGGER_SHARED_LIB nclogger)
set(NCLOGGER_STATIC_LIB nclogger-static)

# Version header and file

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/version.hpp.in
    ${PROJECT_BINARY_DIR}/include/nclogger/version.hpp
    @ONLY
)

write_basic_package_version_file(
    ncloggerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Files

set(
    NCLOGGER_SOURCES

    src/canvas.cpp
    src/line.cpp
    src/logger.cpp
    src/screen.cpp
    src/screen_manager.cpp
    src/watch.cpp
    src/watch_manager.cpp
    src/logger_help.cpp
)

set(
    NCLOGGER_HEADERS

    include/nclogger/canvas.hpp
    include/nclogger/line.hpp
    include/nclogger/knobs.hpp
    include/nclogger/logger.hpp
    include/nclogger/screen.hpp
    include/nclogger/screen_manager.hpp
    include/nclogger/to_string.hpp
    include/nclogger/watch.hpp
    include/nclogger/watch_manager.hpp

    ${PROJECT_BINARY_DIR}/include/nclogger/version.hpp
)

# Function for generating the static/shared library

set(NCLOGGER_CREATED_SOMETHING OFF)

function(nclogger_create_library TARGET_NAME LIBRARY_NAME SHARED_OR_STATIC)
    if(${SHARED_OR_STATIC} STREQUAL SHARED)
        add_library(
            ${TARGET_NAME} SHARED

            ${NCLOGGER_SOURCES}
            ${NCLOGGER_HEADERS}
        )
    else()
        add_library(
            ${TARGET_NAME} STATIC

            ${NCLOGGER_SOURCES}
            ${NCLOGGER_HEADERS}
        )
    endif()

    target_include_directories(
            ${TARGET_NAME}
            PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                $<INSTALL_INTERFACE:include>
            PRIVATE
                ${CURSES_INCLUDE_DIR}
    )

    target_link_libraries(${TARGET_NAME} ${CURSES_LIBRARIES})
    target_compile_features(${TARGET_NAME} PUBLIC cxx_std_11)

    set_target_properties(
            ${TARGET_NAME} PROPERTIES

            CXX_EXTENSIONS OFF
            VERSION ${PROJECT_VERSION}
            OUTPUT_NAME ${LIBRARY_NAME}
    )

    if(${SHARED_OR_STATIC} STREQUAL SHARED)
        set_target_properties(${TARGET_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
    endif()

    check_ipo_supported(RESULT result)
    if(result)
        set_target_properties(${TARGET_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()

    set(NCLOGGER_CREATED_SOMETHING ON PARENT_SCOPE)

    set(NCLOGGER_CREATED_TARGETS "${NCLOGGER_CREATED_TARGETS};${TARGET_NAME}" PARENT_SCOPE)
endfunction()

# Generate the static/shared library

if(${NCLOGGER_BUILD_SHARED})
    nclogger_create_library(${NCLOGGER_SHARED_LIB} ${NCLOGGER_SHARED_LIB} SHARED)
endif()

if(${NCLOGGER_BUILD_STATIC})
    if (UNIX)
        nclogger_create_library(${NCLOGGER_STATIC_LIB} ${NCLOGGER_SHARED_LIB} STATIC)
    else()
        nclogger_create_library(${NCLOGGER_STATIC_LIB} ${NCLOGGER_STATIC_LIB} STATIC)
    endif()
endif()

if(NOT ${NCLOGGER_CREATED_SOMETHING})
    message(FATAL_ERROR "Either static or shared must be selected")
endif()

# Installation

install(
    TARGETS ${NCLOGGER_CREATED_TARGETS}
    EXPORT ncloggerTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT ncloggerTargets
    FILE
        ncloggerTargets.cmake
    NAMESPACE
        nclogger::
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/nclogger
)

install(
    FILES
        cmake/ncloggerConfig.cmake
        ${PROJECT_BINARY_DIR}/ncloggerConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/nclogger
)

install(
    DIRECTORY include
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${PROJECT_BINARY_DIR}/include
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(${NCLOGGER_BUILD_STATIC})
    set(NCLOGGER_LIB ${NCLOGGER_STATIC_LIB})
else()
    set(NCLOGGER_LIB ${NCLOGGER_SHARED_LIB})
endif()

if(${NCLOGGER_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()
