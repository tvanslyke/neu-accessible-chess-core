cmake_minimum_required(VERSION 3.8)

find_package(Boost REQUIRED COMPONENTS system filesystem)
add_executable(test main.cpp)

set(CXX_STANDARD 17)
set_property(TARGET test PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=5 -ggdb -O0")
target_link_libraries(test ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} pthread)
