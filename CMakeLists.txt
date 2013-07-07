project(glwall)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
set(CMAKE_LINK_FLAGS "-s -Wl,-as-needed")
add_executable(glwall glwall.c)
target_link_libraries(glwall glfw GL X11)

