cmake_minimum_required(VERSION 3.10)
project(CinemaServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

link_directories(/usr/local/lib)



file(GLOB CLASSES *.cpp)
set(SOURCES main.cpp ${CLASSES})
add_executable(program ${SOURCES})
target_link_libraries( program serial)
target_link_libraries( program httpserver)
target_link_libraries( program httpserver)
target_link_libraries( program boost_program_options)
target_link_libraries( program microhttpd)
target_link_libraries( program pthread)

