#pragma once

#include <functional>

#include <iostream>

template <typename T>
constexpr std::size_t TypeId() {
	return std::hash<std::string_view>()(std::string_view(__FUNCSIG__));
}
