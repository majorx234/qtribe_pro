cmake_minimum_required(VERSION 3.16)
project(qtribe_pro
  DESCRIPTION "a qt midi sequencer"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
find_package(Jack REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Threads REQUIRED)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

set(JACK_IO_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jackIO.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/jackIO.cpp
)

set(STEP_SEQUENCE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/serializer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stepSequence.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/stepSequence.hpp
)

set(NOTICATOR_WIDGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/noticator_widget.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/noticator_widget.cpp
)

set(STEP_SEQUENCER_CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/sequencerCore.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sequencerCore.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/basic_serializer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/basic_serializer.cpp
)

set(STEP_SEQUENCER_WIDGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/stepSequencerWidget.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stepSequencerWidget.cpp
)

set(QTRIBE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/qtribe.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/qtribe.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(NOTICATOR_WIDGET_TEST_SOURCES
  tests/main_window_test.hpp
  tests/main_window_test.cpp
  tests/noticator_widget_test.cpp
)

QT5_WRAP_UI(STEP_SEQUENCER_WIDGET_BASE_UISrcs
            ${CMAKE_CURRENT_SOURCE_DIR}/src/stepsequencerwidgetbase.ui)

QT5_WRAP_CPP(STEP_SEQUENCER_WIDGET_MOCrcs ${STEP_SEQUENCER_WIDGET_SOURCES})
QT5_WRAP_CPP(NOTICATOR_WIDGET_MOCrcs ${NOTICATOR_WIDGET_SOURCES})
QT5_WRAP_CPP(QTRIBE_MOCrcs ${QTRIBE_SOURCES})

add_library(jackIO
  ${JACK_IO_SOURCES}
)

add_library(stepSequence
  ${STEP_SEQUENCE_SOURCES}
)

target_link_libraries(stepSequence PRIVATE ${JACK_LIBRARIES})

add_library(stepSequencerCore
  ${STEP_SEQUENCER_CORE_SOURCES}
)

target_link_libraries(stepSequencerCore
  jackIO
  stepSequence
)

add_library(noticator_widget
  ${NOTICATOR_WIDGET_SOURCES}
  ${NOTIFICATOR_SOURCES_MOCrcs}
)
target_include_directories(noticator_widget PRIVATE include)
target_include_directories(noticator_widget PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(noticator_widget SYSTEM
 PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
)
target_link_libraries(noticator_widget
 PRIVATE
  Qt5::Core
  Qt5::Widgets
)

add_library(stepSequencerWidget
  ${STEP_SEQUENCER_WIDGET_SOURCES}
  ${STEP_SEQUENCER_WIDGET_BASE_UISrcs}
  ${STEP_SEQUENCER_WIDGET_MOCrcs}
)
target_link_libraries(stepSequencerWidget
  Threads::Threads
  stepSequencerCore
)

add_executable(qtribe
  ${QTRIBE_SOURCES}
  ${QTRIBE_MOCrcs}
)
target_link_libraries(qtribe
  stepSequencerWidget
)

add_executable(noticator_widget_test)
target_sources(noticator_widget_test
 PRIVATE
  ${NOTICATOR_WIDGET_TEST_SOURCES}
  ${NOTICATOR_WIDGET_TEST_MOCrcs}
)
target_include_directories(noticator_widget_test PRIVATE include)
target_link_libraries(noticator_widget_test
  noticator_widget
  Qt5::Core
  Qt5::Widgets
)

qt5_use_modules(stepSequencerWidget Core Gui Widgets)
qt5_use_modules(qtribe Core Gui Widgets)
