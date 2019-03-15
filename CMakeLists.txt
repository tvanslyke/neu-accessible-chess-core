cmake_minimum_required(VERSION 3.8)

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(fmt)
find_package(Threads)
add_executable(test main.cpp ChessEngine.cpp)

set(CXX_STANDARD 17)
set_property(TARGET test PROPERTY CXX_STANDARD 17)
target_link_libraries(test ${CMAKE_THREAD_LIBS_INIT} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} fmt::fmt)
target_include_directories(test PRIVATE "ordered-map/include")

