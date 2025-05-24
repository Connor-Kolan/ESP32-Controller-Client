cmake_minimum_required(VERSION 3.16)

if("${IDF_TARGET}" STREQUAL "linux")
    set(COMPONENTS main)
endif()

include(/home/connor/Documents/Projects/esp32/esp-idf/tools/cmake/project.cmake)
 
project(timed_lock)