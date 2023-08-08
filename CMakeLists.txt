cmake_minimum_required(VERSION 3.24)
project(
    nclogger

    VERSION 0.0.1
    DESCRIPTION "Ncurses logger"
    LANGUAGES CXX
)

# Command-line options

set(NCLOGGER_BUILD_SHARED ON CACHE BOOL "Build shared library")
set(NCLOGGER_BUILD_STATIC OFF CACHE BOOL "Build static library")
set(NCLOGGER_BUILD_EXAMPLES OFF CACHE BOOL "Build example applications")
set(NCLOGGER_SUPPORT_UNICODE ON CACHE BOOL "Support unicode")
set(NCLOGGER_INSTALL_DEV ON CACHE BOOL "Add development (header) files to install target")

# Includes and dependencies

include(CheckIPOSupported)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if(${NCLOGGER_SUPPORT_UNICODE})
    set(CURSES_NEED_WIDE TRUE)
endif()

find_package(Curses REQUIRED)

# Include the curses library -- prefer ncurses
if(DEFINED CURSES_HAVE_NCURSES_H)
    set(NCLOGGER_CURSES_INCLUDE "#include \"${CURSES_HAVE_NCURSES_H}\"")
elseif(DEFINED CURSES_HAVE_NCURSES_NCURSES_H)
    set(NCLOGGER_CURSES_INCLUDE "#include \"${CURSES_HAVE_NCURSES_NCURSES_H}\"")
elseif(DEFINED CURSES_HAVE_NCURSES_CURSES_H)
    set(NCLOGGER_CURSES_INCLUDE "#include \"${CURSES_HAVE_NCURSES_CURSES_H}\"")
elseif(DEFINED CURSES_HAVE_CURSES_H)
    set(NCLOGGER_CURSES_INCLUDE "#include \"${CURSES_HAVE_CURSES_H}\"")
else()
    message(FATAL_ERROR "Curses found but no include path found")
endif()

add_subdirectory(extern/utfcpp)

# Version header and file

configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/version.hpp.in
    ${PROJECT_BINARY_DIR}/include/nclogger/version.hpp
    @ONLY
)

# Files

set(
    NCLOGGER_SOURCES

    src/canvas.cpp
    src/help_manager.cpp
    src/line.cpp
    src/logger.cpp
    src/screen.cpp
    src/screen_manager.cpp
    src/watch.cpp
    src/watch_manager.cpp
    src/text_block.cpp
)

set(
    NCLOGGER_HEADERS

    include/nclogger/canvas.hpp
    include/nclogger/help_manager.hpp
    include/nclogger/line.hpp
    include/nclogger/line_db.hpp
    include/nclogger/knobs.hpp
    include/nclogger/logger.hpp
    include/nclogger/screen.hpp
    include/nclogger/screen_manager.hpp
    include/nclogger/text_block.hpp
    include/nclogger/to_string.hpp
    include/nclogger/watch.hpp
    include/nclogger/watch_manager.hpp

    ${PROJECT_BINARY_DIR}/include/nclogger/version.hpp
)

# Generate objects library

add_library(
    nclogger-objs OBJECT

    ${NCLOGGER_SOURCES}
    ${NCLOGGER_HEADERS}
)

target_include_directories(
        nclogger-objs
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${CURSES_INCLUDE_DIR}
)

target_link_libraries(nclogger-objs PUBLIC ${CURSES_LIBRARIES} PRIVATE utf8::cpp)
target_compile_features(nclogger-objs PUBLIC cxx_std_11)
target_compile_options(nclogger-objs PRIVATE ${CURSES_CFLAGS})

set_target_properties(
    nclogger-objs PROPERTIES

    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
)

# Function for generating the static/shared library

set(NCLOGGER_CREATED_SOMETHING OFF)

function(nclogger_create_library TARGET_NAME LIBRARY_NAME SHARED_OR_STATIC)
    if(${SHARED_OR_STATIC} STREQUAL SHARED)
        add_library(
            ${TARGET_NAME} SHARED

            $<TARGET_OBJECTS:nclogger-objs>
        )
    else()
        add_library(
            ${TARGET_NAME} STATIC

            $<TARGET_OBJECTS:nclogger-objs>
        )
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${CURSES_LIBRARIES})

    set_target_properties(
        ${TARGET_NAME} PROPERTIES

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
    nclogger_create_library(nclogger nclogger SHARED)
endif()

if(${NCLOGGER_BUILD_STATIC})
    if (UNIX)
        nclogger_create_library(nclogger-static nclogger STATIC)
    else()
        nclogger_create_library(nclogger-static nclogger-static STATIC)
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

if (${NCLOGGER_INSTALL_DEV})
    install(
        EXPORT ncloggerTargets
        FILE
            ncloggerTargets.cmake
        NAMESPACE
            nclogger::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/nclogger
    )

    write_basic_package_version_file(
            ncloggerConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion
    )

    install(
        FILES
            cmake/ncloggerConfig.cmake
            ${PROJECT_BINARY_DIR}/ncloggerConfigVersion.cmake
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/nclogger
    )

    install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        DIRECTORY ${PROJECT_BINARY_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

if(${NCLOGGER_BUILD_STATIC})
    set(NCLOGGER_LIB nclogger-static)
else()
    set(NCLOGGER_LIB nclogger)
endif()

if(${NCLOGGER_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()

# Add aliases if this project is included as subdirectory

if(NOT ${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})

    if(${NCLOGGER_BUILD_STATIC})
        add_library(nclogger::nclogger-static ALIAS nclogger-static)
    endif()

    if(${NCLOGGER_BUILD_SHARED})
        add_library(nclogger::nclogger ALIAS nclogger)
    endif()

endif()
