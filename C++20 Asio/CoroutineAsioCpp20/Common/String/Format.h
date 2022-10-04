#pragma once

#include <format>

template<typename... TArgs>
std::string Format(std::string_view form, TArgs&&... args)
{
	return std::vformat(form, std::make_format_args(std::forward<TArgs>(args)...));
}

template<typename... TArgs>
std::wstring Format(std::wstring_view form, TArgs&&... args)
{
	return std::vformat(form, std::make_wformat_args(std::forward<TArgs>(args)...));
}