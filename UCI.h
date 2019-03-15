#ifndef AC_UCI_H
#define AC_UCI_H

#include <utility>
#include <string_view>
#include <variant>
#include <boost/multiprecision/cpp_int.hpp>
#include "tsl/ordered_set.h"

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
	std::string value;


	std::string repr() const {
		return "StringOption{\"" + value + "\"}";
	}
};

struct ButtonOption {
	static constexpr OptionType type = OptionType::Button;
	std::string repr() const {
		return "ButtonOption{}";
	}
};

struct CheckOption {
	static constexpr OptionType type = OptionType::Check;
	bool value;

	std::string repr() const {
		return std::string("ButtonOption{") + (value ? "true}" : "false}");
	}
};

struct SpinOption {
	static constexpr OptionType type = OptionType::Spin;
	mp_int value;
	mp_int minimum;
	mp_int maximum;

	std::string repr() const {
		std::stringstream ss;
		ss << "SpinOption{";
		ss << "value=";
		ss << value;
		ss << ", minimum=";
		ss << minimum;
		ss << ", maximum=";
		ss << maximum;
		ss << "}";
		return ss.str();
	}
};

struct ComboOption {
	static constexpr OptionType type = OptionType::Combo;
	struct KeyEqual: std::equal_to<std::string_view> {
		using is_transparent = void;
	};
	using combo_list_type = tsl::ordered_set<
		std::string,
		std::hash<std::string_view>,
		KeyEqual,
		std::allocator<std::string>,
		std::vector<std::string>,
		std::size_t
	>;
	std::size_t value;
	combo_list_type alternatives;

	std::string repr() const {
		std::stringstream ss;
		ss << "ComboOption{";
		ss << "value=" << '"' << *alternatives.nth(value) << "\", ";
		ss << "alternatives={";
		for(const auto& alt: alternatives) {
			ss << '"' << alt << '"' << ", ";
		}
		ss << "}}";
		return ss.str();
	}

};

struct Option;

template <class Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, const Option& opt);

template <class Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, Option& opt);

struct Option:
	std::variant<
		ButtonOption,
		CheckOption,
		SpinOption,
		ComboOption,
		StringOption
	>
{
	using base_type = std::variant<
		ButtonOption,
		CheckOption,
		SpinOption,
		ComboOption,
		StringOption
	>;

	using base_type::base_type;
	using base_type::operator=;

	Option() = default;
	Option(const Option&) = default;
	Option(Option&&) = default;

	Option& operator=(const Option&) = default;
	Option& operator=(Option&&) = default;

	constexpr OptionType type() const noexcept {
		return std::visit(
			[](const auto& value){ return value.type; },
			static_cast<const base_type&>(*this)
		);
	}

	std::string repr() const noexcept {
		return std::visit(
			[](const auto& opt) -> std::string { return opt.repr(); },
			static_cast<const base_type&>(*this)
		);
	}

	const base_type& as_variant() const { return *this; }
	base_type& as_variant() { return *this; }

	const SpinOption& spin() const {
		return std::get<SpinOption>(as_variant());
	}

	SpinOption& spin() {
		return std::get<SpinOption>(as_variant());
	}

	const ComboOption& combo() const {
		return std::get<ComboOption>(as_variant());
	}

	ComboOption& combo() {
		return std::get<ComboOption>(as_variant());
	}

	const CheckOption& check() const {
		return std::get<CheckOption>(as_variant());
	}

	CheckOption& check() {
		return std::get<CheckOption>(as_variant());
	}

	const StringOption& string() const {
		return std::get<StringOption>(as_variant());
	}

	StringOption& string() {
		return std::get<StringOption>(as_variant());
	}

	const ButtonOption& button() const {
		return std::get<ButtonOption>(as_variant());
	}

	ButtonOption& button() {
		return std::get<ButtonOption>(as_variant());
	}

};

template <class Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, const Option& opt) {
	return std::visit(std::forward<Visitor>(visitor), static_cast<const Option::base_type&>(opt));
}

template <class Visitor>
constexpr decltype(auto) visit(Visitor&& visitor, Option& opt) {
	return std::visit(std::forward<Visitor>(visitor), static_cast<Option::base_type&>(opt));
}

} /* namespace ac::uci */

#endif /* AC_UCI_H */
