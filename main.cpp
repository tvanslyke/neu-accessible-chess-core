#include "ChessEngine.h"
#include <iostream>
#include <thread>
#include <mutex>

static_assert(__cplusplus >= 201703L);

std::mutex cout_mutex;

int main() {
	std::cout << "Creating engine" << std::endl;
	auto engine = ac::ChessEngine("/home/tim/neu/software-engineering/project/Stockfish/src/stockfish");
	std::cout << "Done" << std::endl;
	// std::cout << "Spawning output thread" << std::endl;
	// std::thread output_thread([&]() {
	// 	std::cout << "Output thread spawned" << std::endl;
	// 	for(;;) {
	// 		auto out = engine.read();
	// 		if(not out.empty()) {
	// 			auto g = std::lock_guard(cout_mutex);
	// 			std::cout.write(out.data(), out.size());
	// 			std::cout << std::flush;
	// 		}
	// 	}
	// });
	// output_thread.detach();
	
	for(const auto& [name, option]: engine.options()) {
		std::cout << '\t' << name << ": " << option.repr() << std::endl;
	}
	{
		auto g = std::lock_guard(cout_mutex);
		std::cout << "Reading from stdin." << std::endl;
	}
	for(;;) {
		std::string line;
		std::getline(std::cin, line);
		{
			auto g = std::lock_guard(cout_mutex);
			std::cout << "Got line: '" << line  << "'" << std::endl;
		}
		engine.write(line.c_str());
	}
}
