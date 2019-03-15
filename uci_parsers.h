#ifndef AC_UCI_PARSERS_H
#define AC_UCI_PARSERS_H

#include "UCI.h"
#include <boost/spirit/home/x3.hpp>

namespace ac::uci {

namespace x3 = boost::spirit::x3;

template <class P>
constexpr auto as_string(P&& p) {
	return x3::rule<struct as_string_tag, std::string>("as_string") = x3::as_parser(std::forward<P>(p));
}

inline constexpr auto strip_action = [](auto& ctx) {
	std::string s = std::move(x3::_attr(ctx));
	auto start = s.find_first_not_of(" ");
	auto stop = s.find_last_not_of(" ");
	assert(start != std::string::npos);
	x3::_val(ctx) = s.substr(start, (stop - start) + 1u);
};

template <auto MemPtr>
inline constexpr auto assign_member = [](auto& ctx) {
	x3::_val(ctx).*MemPtr = x3::_attr(ctx);
};

inline constexpr auto assign_attr = [](auto& ctx) {
	x3::_val(ctx) = x3::_attr(ctx);
};

inline constexpr auto insert_attr = [](auto& ctx) {
	x3::_val(ctx).insert(x3::_attr(ctx));
};

template <class P>
constexpr auto multiword_string_before(P&& parser) {
	return as_string(
		x3::lexeme[as_string(*(x3::char_ - std::forward<P>(parser)))][strip_action]
		>> std::forward<P>(parser)
	);
}

inline const auto option_header_parser
	= x3::rule<struct option_header_tag, std::string>()
	= (x3::lit("option name ") >> x3::omit[*x3::space]) >> multiword_string_before("type");

inline const auto string_option_parser
	= x3::rule<struct string_option_tag, StringOption>()
	= x3::lit("string") > "default" > x3::lexeme[*x3::char_][([](auto& ctx) {
		std::string s = std::move(x3::_attr(ctx));
		auto start = s.find_first_not_of(" ");
		if(start == std::string::npos) {
			x3::_val(ctx).value = std::move(s);
		} else {
			auto stop = s.find_last_not_of(" ");
			x3::_val(ctx).value = s.substr(start, (stop - start) + 1u);
		} 
	})];

inline const auto button_option_parser
	= x3::rule<struct button_option_tag, ButtonOption>()
	= x3::lit("button") > x3::eoi;

inline const auto check_option_parser
	= x3::rule<struct check_option_tag, CheckOption>()
	= x3::lit("check") > "default" > (
		(x3::lit("true") >> x3::attr(CheckOption{true}))
		| (x3::lit("false") >> x3::attr(CheckOption{false}))
	);

inline const auto mp_int_parser
	= x3::rule<struct mp_int_tag, mp_int>()
	= x3::lexeme[as_string(*(x3::char_ - ' '))][([](auto& ctx){
		x3::_val(ctx) = static_cast<mp_int>(x3::_attr(ctx));
	})];

inline const auto spin_option_parser
	= x3::rule<struct spin_option_tag, SpinOption>()
	= x3::eps >> "spin"
	> "default" > mp_int_parser[assign_member<&SpinOption::value>]
	> "min" > mp_int_parser[assign_member<&SpinOption::minimum>]
	> "max" > mp_int_parser[assign_member<&SpinOption::maximum>];

inline const auto combo_option_alternatives_parser
	= x3::rule<struct combo_option_tag, ComboOption::combo_list_type>()
	= (*x3::omit[multiword_string_before("var")[insert_attr]] > x3::omit[multiword_string_before(x3::eoi)[insert_attr]]);

inline const auto combo_option_parser
	= x3::rule<struct combo_option_tag, ComboOption>()
	= (x3::omit[x3::eps >> "combo" > "default"] > multiword_string_before("var")
	> x3::omit[combo_option_alternatives_parser[assign_member<&ComboOption::alternatives>]])[([](auto& ctx) {
		auto pos = x3::_val(ctx).alternatives.find(x3::_attr(ctx));
		assert(pos != x3::_val(ctx).alternatives.end());
		x3::_val(ctx).value = pos - x3::_val(ctx).alternatives.begin();
	})];

inline const auto uci_option_parser	
	= x3::rule<struct uci_option_tag, Option>()
	= spin_option_parser[assign_attr]
	| combo_option_parser[assign_attr]
	| string_option_parser[assign_attr]
	| button_option_parser[assign_attr]
	| check_option_parser[assign_attr];

inline const auto uci_id_name_parser = x3::lit("id") > "name" > multiword_string_before(x3::eoi);
inline const auto uci_id_author_parser = x3::lit("id") > "author" > multiword_string_before(x3::eoi);


} /* namespace ac::uci */

#endif /* AC_UCI_PARSERS_H */
