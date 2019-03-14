#ifndef AC_CHESS_ENGINE_H
#define AC_CHESS_ENGINE_H

#include <boost/process/pipe.hpp>
#include <boost/process/child.hpp>
#include <boost/process/search_path.hpp>
#include <boost/process/io.hpp>
#include <boost/filesystem/path.hpp>
#include <utility>
#include <fmt/format.h>
#include "ChessPiece.h"
#include "UCI.h"
#include "tsl/ordered_map.h"

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

	ChessEngine(bfs::path executable_path):
		engine_input_(),
		engine_output_(),
		engine_(
			bfs::path(std::move(executable_path)),
			bp::std_out > engine_output_,
			bp::std_in < engine_input_
		),
		options_{}
	{
		engine_input_.exceptions(std::ios_base::badbit | std::ios_base::failbit);
		engine_output_.exceptions(std::ios_base::badbit);
		// ignore the first line of output from the engine.
		std::string line;
		std::getline(engine_output_, line);
		// engine_output_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << line << std::endl;
		send_command("uci");
		parse_uci_options();
	}
	
	void write(const char* text) {
		engine_input_ << text << std::endl;
	}

	std::string read() {
		std::string str;
		std::getline(engine_output_, str);
		return str + "\n";
	}

	const option_map_type& options() const {
		return options_;
	}

	void set_spin_option(std::string_view name, uci::mp_int value) {
		auto& opt = option_at(name).spin();
		if(value > opt.maximum or value < opt.minimum) {
			throw std::out_of_range(fmt::format(
				"Value '{}' is out of bounds for spin option '{}: {}'",
				[&]() { std::stringstream ss; ss << value; return ss.str(); }(),
				name,
				opt.repr()
			));
		}
		std::stringstream ss;
		ss << value;
		send_command(fmt::format("setoption name {} value {}", name, value.str()));
		opt.value = value;
	}

	void set_button_option(std::string_view name) {
		auto& opt = option_at(name).button();
		send_command(fmt::format("setoption name {}", name));
	}

	void set_check_option(std::string_view name, bool value) {
		auto& opt = option_at(name).check();
		send_command(fmt::format("setoption name {} value {}", name, value ? "true" : "false"));
	}

	void set_combo_option(std::string_view name, std::size_t index) {
		auto& opt = option_at(name).combo();
		if(index > opt.alternatives.size()) {
			throw std::out_of_range(fmt::format(
				"Attempt to set combo option {} with {} alternaves to alternative number {}.",
				name,
				opt.alternatives.size(),
				index + 1u
			));
		}
		send_command(fmt::format("setoption name {} value {}", name, *opt.alternatives.nth(index)));
		opt.value = index;
	}

	void set_combo_option(std::string_view name, std::string_view value) {
		auto& opt = option_at(name).combo();
		set_combo_option(name, opt.alternatives.find(value) - opt.alternatives.begin());
	}

	void set_string_option(std::string_view name, std::string value) {
		auto& opt = option_at(name).string();
		send_command(fmt::format("setoption name {} value {}", name, value));
		opt.value = std::move(value);
	}

	void set_option(std::string_view name, uci::mp_int value) {
		auto& opt = option_at(name);
		auto type = opt.type();
		if(type == uci::OptionType::Combo) {
			set_combo_option(name, static_cast<std::size_t>(value));
		} else if(type == uci::OptionType::Spin) {
			set_spin_option(name, std::move(value));
		} else {
			throw std::runtime_error(fmt::format(
				"'{}' (integer) is an invalid value for option '{}: {}'",
				[&]() { std::stringstream ss; ss << value; return ss.str(); }(),
				name,
				opt.repr()
			));
		}
	}

	void set_option(std::string_view name, bool value) {
		set_check_option(name, value);
	}

	void set_option(std::string_view name) {
		set_button_option(name);
	}

	void set_option(std::string_view name, std::string_view value) {
		auto& opt = option_at(name);
		auto type = opt.type();
		if(type == uci::OptionType::String) {
			set_string_option(name, static_cast<std::string>(value));
		} else if(type == uci::OptionType::Combo) {
			set_combo_option(name, value);
		} else {
			throw std::runtime_error(fmt::format(
				"'{}' (string) is an invalid value for option '{}: {}'",
				value,
				name,
				opt.repr()
			));
		}
	}

private:

	uci::Option& option_at(std::string_view name) {
		const auto& opt = std::as_const(*this).option_at(name);
		return const_cast<uci::Option&>(opt);
	}

	const uci::Option& option_at(std::string_view name) const {
		auto pos = options_.find(name);
		if(pos == options_.end()) {
			throw std::out_of_range(fmt::format("'{}' is not a valid option.", name));
		}
		return pos.value();
	}

	void send_command(std::string_view s) {
		engine_input_ << s << std::endl;
	}

	void parse_uci_options() {
		std::cout << "Parsing..." << std::endl;
		{
			std::string line;
			std::getline(engine_output_, line);
			auto first = line.begin();
			auto last = line.end();
			if(not uci::x3::phrase_parse(first, last, uci::x3::omit[uci::uci_id_name_parser], uci::x3::space)) {
				throw std::runtime_error(fmt::format("Bad UCI name string: '{}'", line));
			}
			std::getline(engine_output_, line);
			first = line.begin();
			last = line.end();
			if(not uci::x3::phrase_parse(first, last, uci::x3::omit[uci::uci_id_author_parser], uci::x3::space)) {
				throw std::runtime_error(fmt::format("Bad UCI author string: '{}'", line));
			}
			std::getline(engine_output_, line);
			assert(line.empty());
		}
		while(parse_next_uci_option()) {
			/* LOOP */
		}
	}

	bool parse_next_uci_option() {
		engine_output_.exceptions(std::ios_base::failbit | std::ios_base::badbit);
		try {
			std::string line_str;
			std::getline(engine_output_, line_str);
			std::string_view line(line_str);
			if(line.substr(0, 5) == "uciok") {
				return false;
			}
			auto first = line.begin();
			auto last = line.end();
			std::string name;
			if(not uci::x3::phrase_parse(first, last, uci::option_header_parser, uci::x3::space, name)) {
				throw std::runtime_error(fmt::format("Bad UCI option string: '{}'", line));
			}
			uci::Option option;
			if(not uci::x3::phrase_parse(first, last, uci::uci_option_parser, uci::x3::space, option)) {
				throw std::runtime_error(fmt::format("Bad UCI option string: '{}'", line));
			}
			options_.emplace(std::move(name), std::move(option));
		} catch(...) {
			engine_output_.exceptions(std::ios_base::badbit);
			throw;
		}
		engine_output_.exceptions(std::ios_base::badbit);
		return true;
	}

	bp::opstream engine_input_;
	bp::ipstream engine_output_;
	bp::child engine_;
	option_map_type options_;
};


} /* namespace ac */

#endif /* AC_CHESS_ENGINE_H */
