#include "ChessEngine.h"
#include "Board.h"
#include <iostream>

static_assert(__cplusplus >= 201703L, "Compiler must support C++17.");

// Stockfish executable.
inline constexpr const char* stockfish_path = "./stockfish";

int main() {
	ac::ChessEngine engine(stockfish_path);
	
	// print all of the options provided by stockfish.
	for(const auto& [name, option]: engine.options()) {
		std::cout << '\t' << name << ": " << option.repr() << std::endl;
	}
}
