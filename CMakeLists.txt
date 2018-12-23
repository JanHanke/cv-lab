cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 17)
project(object_tracking)
find_package(OpenCV REQUIRED)
set(SOURCE_FILES
        src/MonoLoop.cpp
        src/LiveLoop.cpp
        )
set(HEADER_FILES
        includes/MonoLoop.h
        )
add_executable(object_tracking ${SOURCE_FILES} ${HEADER_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(object_tracking ${OpenCV_LIBS})
