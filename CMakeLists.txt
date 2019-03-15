cmake_minimum_required(VERSION 3.8)

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(fmt)
find_package(Threads)
add_executable(test main.cpp ChessEngine.cpp)

set(CXX_STANDARD 17)
set_property(TARGET test PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

target_link_libraries(test ${CMAKE_THREAD_LIBS_INIT} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} fmt::fmt)
target_include_directories(test PRIVATE "ordered-map/include")

add_executable(
	stockfish
	Stockfish/src/benchmark.cpp
	Stockfish/src/bitbase.cpp
	Stockfish/src/bitboard.cpp
	Stockfish/src/endgame.cpp
	Stockfish/src/evaluate.cpp
	Stockfish/src/main.cpp
	Stockfish/src/material.cpp
	Stockfish/src/misc.cpp
	Stockfish/src/movegen.cpp
	Stockfish/src/movepick.cpp
	Stockfish/src/pawns.cpp
	Stockfish/src/position.cpp
	Stockfish/src/psqt.cpp
	Stockfish/src/search.cpp
	Stockfish/src/thread.cpp
	Stockfish/src/timeman.cpp
	Stockfish/src/tt.cpp
	Stockfish/src/uci.cpp
	Stockfish/src/ucioption.cpp
	Stockfish/src/syzygy/tbprobe.cpp	
)

target_link_libraries(stockfish ${CMAKE_THREAD_LIBS_INIT})
