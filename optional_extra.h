#ifndef AC_OPTIONAL_EXTRA_H
#define AC_OPTIONAL_EXTRA_H

#include <optional>
#include <type_traits>
#include <utility>

namespace ac {

template <class T>
constexpr auto some(T&& value)
	-> std::optional<std::remove_cv_t<std::remove_reference_t<T>>>
{
	return std::optional<T>(std::forward<T>(value));
}

} /* namespace ac */

#endif /* AC_OPTIONAL_EXTRA_H */(
