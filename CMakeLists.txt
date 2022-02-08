cmake_minimum_required(VERSION 3.16)
project(qtribe_pro
  DESCRIPTION "a qt midi sequencer"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
find_package(Jack REQUIRED)
find_package( Qt5Widgets REQUIRED)
find_package( Qt5Core REQUIRED)


include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

set(JACK_IO_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/include/jackIO.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/jackIO.cpp)

set(STEP_SEQUENCE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/stepSequence.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/stepSequence.hpp
  )

set(STEP_SEQUENCER_CORE_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/include/sequencerCore.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sequencerCore.cpp
	)

set(STEP_SEQUENCER_WIDGET_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/include/stepSequencerWidget.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/stepSequencerWidget.cpp
	)

QT5_WRAP_UI(STEP_SEQUENCER_WIDGET_BASE_UISrcs 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/stepsequencerwidgetbase.ui)

QT5_WRAP_CPP(STEP_SEQUENCER_WIDGET_MOCrcs ${STEP_SEQUENCER_WIDGET_SOURCES})					

add_library(jackIO
	${JACK_IO_SOURCES})

add_library(stepSquence
  ${STEP_SEQUENCE_SOURCES}
)

add_library(stepSequencerWidget
	${STEP_SEQUENCER_WIDGET_SOURCES}
	${STEP_SEQUENCER_WIDGET_BASE_UISrcs}
	${STEP_SEQUENCER_WIDGET_MOCrcs}
	)
target_link_libraries(stepSquence PRIVATE ${JACK_LIBRARIES})

qt5_use_modules(stepSequencerWidget Core Gui Widgets)
