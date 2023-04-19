#ifndef GABE_CPP_MAYBE_H
#define GABE_CPP_MAYBE_H
#include <type_traits>
#include <variant>

template<
	typename T, 
	typename E, 
	std::enable_if_t<std::is_convertible_v<E, int>, int> = 0>
struct g_Maybe
{
	g_Maybe(const T& value)
		: data(std::move(value)) {}

	g_Maybe(const E& error) 
		: data(error) {}

	std::variant<T, E> data;

	[[nodiscard]]
	constexpr inline bool hasValue() const { return data.index() == 0; }

	[[nodiscard]]
	constexpr inline const T& value() const { return std::get<T>(data); }

	[[nodiscard]]
	constexpr inline T& mut_value() { return std::get<T>(data); }

	[[nodiscard]]
	constexpr inline E error() const { return std::get<E>(data); }

	[[nodiscard]]
	constexpr inline T& operator*() { return std::get<T>(data); }

	[[nodiscard]]
	constexpr inline const T& operator*() const { return std::move(std::get<T>(data)); }
};

#endif 