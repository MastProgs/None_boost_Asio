#pragma once

#include <format>

template<typename... TArgs>
std::string Format(const std::string_view& form, TArgs&&... args)
{
	return std::vformat(form, std::make_format_args(std::forward<TArgs>(args)...));
}

template<typename... TArgs>
std::wstring Format(const std::wstring_view& form, TArgs&&... args)
{
	return std::vformat(form, std::make_format_args(std::forward<TArgs>(args)...));
}