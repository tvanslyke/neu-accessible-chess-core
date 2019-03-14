#ifndef AC_CHESS_ENGINE_H
#define AC_CHESS_ENGINE_H

#include <boost/process/pipe.hpp>
#include <boost/process/child.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/io.hpp>
#include <boost/filesystem/path.hpp>
#include <utility>
#include <unordered_map>
#include "ChessPiece.h"

namespace ac {

namespace bp = boost::process;
namespace bfs = boost::filesystem;

struct ChessEngine {
	ChessEngine(bfs::path executable_path):
		engine_input_(),
		engine_output_(),
		engine_(
			bfs::path(std::move(executable_path)),
			bp::std_in < engine_input_,
			bp::std_out > engine_output_
		)
	{
		engine_input_.exceptions(std::ios_base::badbit);
		engine_output_.exceptions(std::ios_base::badbit);
	}
	
	void write(const char* text) {
		engine_input_ << text << '\n' << std::flush;
	}

	std::string read() {
		std::string str;
		std::getline(engine_output_, str);
		return str + "\n";
	}

private:
	struct ComboOption {
		std::
	};

	std::unordered_map<
		std::string,
		std::variant<
			std::monostate,
			
		>

	bp::opstream engine_input_;
	bp::ipstream engine_output_;
	bp::child engine_;
};

} /* namespace ac */

#endif /* AC_CHESS_ENGINE_H */
