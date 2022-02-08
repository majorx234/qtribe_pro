cmake_minimum_required(VERSION 3.16)
project(qtribe_pro
  DESCRIPTION "a qt midi sequencer"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)

find_package(Jack REQUIRED)
find_package( Qt5Widgets REQUIRED)
find_package( Qt5Core REQUIRED)


include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)

set(SETSEQUENCE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stepSequence.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/stepSequence.h
  )


add_library(stepSquence
  ${SETSEQUENCE_SOURCES}
)

target_link_libraries(stepSquence PRIVATE ${JACK_LIBRARIES})