cmake_minimum_required(VERSION 3.3)
project(rApp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH "/Users/brycecater/Qt/5.5/clang_64")
# Find the QtWidgets library
find_package(Qt5Widgets)
set(CMAKE_VERBOSE_MAKEFILE OFF)

add_subdirectory("../../Firmware/rWifi" "rWifi")
add_subdirectory("../../Firmware/rCamera" "rCamera")
include_directories("../../Firmware/rWifi")

include_directories("../../Firmware/rCore")
include_directories("../../Firmware/rCamera")
include_directories("../../Firmware/rCamera/LinuxHeaders")


add_executable(rApp rApp.cpp rApp.h)
#add_executable(fakeRover fakeRover.cpp fakeRover.h)
add_executable(simpleTest simpleTest.cpp)

target_link_libraries(rApp Qt5::Widgets)
target_link_libraries(rApp rWifi)
target_link_libraries(rApp rCamera)
#target_link_libraries(rApp rCamera)
#target_link_libraries(fakeRover rWifi)
#target_link_libraries(fakeRover rCamera)

FIND_PACKAGE(libuvc REQUIRED)
INCLUDE_DIRECTORIES("${libuvc_INCLUDE_DIRS}")
#TARGET_LINK_LIBRARIES(fakeRover ${libuvc_LIBRARIES})


find_package(YCM REQUIRED)
FIND_PACKAGE(FFMPEG REQUIRED)
TARGET_LINK_LIBRARIES(rApp ${FFMPEG_LIBRARIES})

SET(noParenSourcePath "")
SET(noParenBinPath "")
STRING(REPLACE "(" "\\(adfasdf" noParenSourcePath ${CMAKE_CURRENT_SOURCE_DIR})
STRING(REPLACE ")" "\\)" noParenSourcePath ${CMAKE_CURRENT_SOURCE_DIR})
STRING(REPLACE "(" "\\(" noParenBinPath ${CMAKE_CURRENT_BINARY_DIR})
STRING(REPLACE ")" "\\)" noParenBinPath ${CMAKE_CURRENT_BINARY_DIR})

#if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
#
#    # list of files for which we add a copy rule
#    set(data_RAPP RoverLogo.jpg)
#
#    foreach(item IN LISTS data_RAPP)
#        message(STATUS ${item})
#        add_custom_command(
#                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
#                COMMAND ${CMAKE_COMMAND} -E copy "\"${noParenSourcePath}/${item}\"" "\"${noParenBinPath}/${item}\""
#                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
#        )
#    endforeach()
#endif()
#
## files are only copied if a target depends on them
#add_custom_target(data-target ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/RoverLogo.jpg")
#ADD_DEPENDENCIES(rApp data-target)