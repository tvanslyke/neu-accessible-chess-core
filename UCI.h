#ifndef AC_UCI_H
#define AC_UCI_H

#include <utility>
#include <string_view>
#include <variant>
#include <boost/multiprecision/cpp_int.hpp>

namespace ac::uci {

namespace bmp = boost::multiprecision;
using mp_int = bmp::cpp_int;

enum class OptionType {
	Button,
	Check,
	Spin,
	Combo,
	String
};

struct StringOption {
	static constexpr OptionType type = OptionType::String;
	
	constexpr std::string_view name() const {
		return name_;
	}

	constexpr std::string_view value() const {
		assert(value_);
		return *value_;
	}

private:
	friend class ::ac::ChessEngine;
	std::string_view name_;
	const std::string* value_;
};

struct ButtonOption {
	static constexpr OptionType type = OptionType::Button;

	constexpr std::string_view name() const {
		return name_;
	}

	constexpr bool value() const {
		assert(value_);
		return *value_;
	}

private:
	friend class ::ac::ChessEngine;
	std::string_view name_;
	const bool* value_;
};

struct CheckOption {
	static constexpr OptionType type = OptionType::Check;

	constexpr std::string_view name() const {
		return name_;
	}

private:
	friend class ::ac::ChessEngine;
	std::string_view name_;
	
};

struct SpinOption {

	static constexpr OptionType type = OptionType::Spin;

	constexpr std::string_view name() const {
		return name_;
	}

	mp_int value() const {
		assert(value_);
		return *value_;
	}

	mp_int minimum() const {
		assert(minm_);
		return *minm_;
	}

	mp_int maximum() const {
		assert(maxm_);
		return *maxm_;
	}

private:
	friend class ::ac::ChessEngine;
	std::string_view name_;
	const mp_int* value_;
	const mp_int* minm_;
	const mp_int* maxm_;
};

struct ComboOption {

	constexpr std::string_view name() const {
		return name_;
	}

	std::string_view value() const {
		assert(*value_);
		return *value_;
	}

	const std::vector<std::string>& options() const {
		assert(*options_);
		return *options_;
	}

private:
	friend class ::ac::ChessEngine;
	std::string_view name_;
	const std::string_view* value_;
	const std::vector<std::string>* options_;
};

} /* namespace ac */

#endif /* AC_UCI_H */
