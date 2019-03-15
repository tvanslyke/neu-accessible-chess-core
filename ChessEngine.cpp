#include "ChessEngine.h"
#include "uci_parsers.h"

namespace ac {


ChessEngine::ChessEngine(const char* path):
	engine_output_(),
	engine_input_(),
	engine_(
		path,
		bp::std_out > engine_output_,
		bp::std_in < engine_input_
	),
	options_{}
{
	engine_input_.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	engine_output_.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	// engine_output_.exceptions(std::ios_base::badbit);
	// ignore the first line of output from the engine.
	std::string line;
	send_command("uci");
	std::getline(engine_output_, line);
	parse_uci_options();
}

const ChessEngine::option_map_type& ChessEngine::options() const {
	return options_;
}

void ChessEngine::set_spin_option(std::string_view name, uci::mp_int value) {
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

void ChessEngine::set_button_option(std::string_view name) {
	auto& opt = option_at(name).button();
	send_command(fmt::format("setoption name {}", name));
}

void ChessEngine::set_check_option(std::string_view name, bool value) {
	auto& opt = option_at(name).check();
	send_command(fmt::format("setoption name {} value {}", name, value ? "true" : "false"));
}

void ChessEngine::set_combo_option(std::string_view name, std::size_t index) {
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

void ChessEngine::set_combo_option(std::string_view name, std::string_view value) {
	auto& opt = option_at(name).combo();
	set_combo_option(name, opt.alternatives.find(value) - opt.alternatives.begin());
}

void ChessEngine::set_string_option(std::string_view name, std::string value) {
	auto& opt = option_at(name).string();
	send_command(fmt::format("setoption name {} value {}", name, value));
	opt.value = std::move(value);
}

void ChessEngine::set_option(std::string_view name, uci::mp_int value) {
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

void ChessEngine::set_option(std::string_view name, bool value) {
	set_check_option(name, value);
}

void ChessEngine::set_option(std::string_view name) {
	set_button_option(name);
}

void ChessEngine::set_option(std::string_view name, std::string_view value) {
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



uci::Option& ChessEngine::option_at(std::string_view name) {
	const auto& opt = std::as_const(*this).option_at(name);
	return const_cast<uci::Option&>(opt);
}

const uci::Option& ChessEngine::option_at(std::string_view name) const {
	auto pos = options_.find(name);
	if(pos == options_.end()) {
		throw std::out_of_range(fmt::format("'{}' is not a valid option.", name));
	}
	return pos.value();
}

void ChessEngine::send_command(std::string_view s) {
	assert(not s.empty());
	while(not s.empty() and s.back() == '\n') {
		s.remove_suffix(1);
	}
	assert(not s.empty());
	engine_input_ << s << std::endl;
}

void ChessEngine::parse_uci_options() {
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

bool ChessEngine::parse_next_uci_option() {
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
	return true;
}


void ChessEngine::set_position(const GameSnapshot& board) {
	send_command(fmt::format("position {}", board.fenstring()));
}



} /* namespace ac */
