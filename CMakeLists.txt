cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 17)
project(object_tracking)
find_package(OpenCV REQUIRED)
find_package(Boost 1.68.0 REQUIRED system)
find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(SOURCE_FILES
        src/MonoLoop.cpp
        src/LiveLoop.cpp
        )
set(HEADER_FILES
        includes/MonoLoop.h
        )

add_executable(server src/Server.cpp)
target_link_libraries(server Threads::Threads ${Boost_LIBRARIES})

add_executable(object_tracking ${SOURCE_FILES} ${HEADER_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(object_tracking ${OpenCV_LIBS})
