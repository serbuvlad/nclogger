cmake_minimum_required(VERSION 3.24)
project(nclogger VERSION "0.0.1")

find_package(Curses REQUIRED)

option(NCLOGGER_BUILD_EXAMPLES "Build examples" OFF)

set(
        NCLOGGER_SOURCES

        src/canvas.cpp
        src/line.cpp
        src/logger.cpp
        src/screen.cpp
        src/screen_manager.cpp
        src/watch.cpp
        src/watch_manager.cpp
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
)

add_library(
        nclogger

        SHARED

        ${NCLOGGER_SOURCES}

        ${NCLOGGER_HEADERS}
)

set_target_properties(
        nclogger PROPERTIES
        VERSION 0.0.1
        SOVERSION 0
)

target_include_directories(nclogger PUBLIC include PRIVATE ${CURSES_INCLUDE_DIR})

target_link_libraries(nclogger ${CURSES_LIBRARIES})
target_compile_features(nclogger PUBLIC cxx_std_11)

if (NCLOGGER_BUILD_EXAMPLES)
    add_subdirectory(examples/ex1)
endif ()

install(
        TARGETS nclogger
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/nclogger
)

install(
        DIRECTORY include/
        DESTINATION include
)
