#ifndef AC_CHESS_ENGINE_H
#define AC_CHESS_ENGINE_H

#include <boost/process/pipe.hpp>
#include <boost/process/child.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/io.hpp>
#include <boost/filesystem/path.hpp>
#include <utility>
#include <fmt/format.h>
#include "Board.h"
#include "ChessPiece.h"
#include "UCI.h"
#include "tsl/ordered_map.h"
#include "GameSnapshot.h"

namespace ac {

namespace bp = boost::process;
namespace bfs = boost::filesystem;

struct ChessEngine {
	struct KeyEqual: std::equal_to<std::string_view> {
		using is_transparent = void;
	};
	using option_map_type = tsl::ordered_map<
		std::string,
		uci::Option,
		std::hash<std::string_view>,
		KeyEqual,
		std::allocator<std::pair<std::string, uci::Option>>,
		std::vector<std::pair<std::string, uci::Option>>,
		std::size_t
	>;

	ChessEngine(const char* executable_path);
	
	const option_map_type& options() const;

	void set_spin_option(std::string_view name, uci::mp_int value);
	void set_button_option(std::string_view name);
	void set_check_option(std::string_view name, bool value);
	void set_combo_option(std::string_view name, std::size_t index);
	void set_combo_option(std::string_view name, std::string_view value);
	void set_string_option(std::string_view name, std::string value);

	void set_option(std::string_view name, uci::mp_int value);
	void set_option(std::string_view name, bool value);
	void set_option(std::string_view name);
	void set_option(std::string_view name, std::string_view value);

	void send_command(std::string_view s);

	void set_position(const GameSnapshot& board);

	// void go();
private:

	uci::Option& option_at(std::string_view name);
	const uci::Option& option_at(std::string_view name) const;

	void parse_uci_options();
	bool parse_next_uci_option();


	bp::ipstream engine_output_;
	bp::opstream engine_input_;
	bp::child engine_;
	option_map_type options_;
};


} /* namespace ac */

#endif /* AC_CHESS_ENGINE_H */
