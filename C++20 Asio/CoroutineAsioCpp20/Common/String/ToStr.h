#pragma once

#include <cstring>
#include <locale>
#include <codecvt>
#include <comdef.h>
#include <string>
#include <atlstr.h>


template <typename OutType>
class ToStr
{
public:
	ToStr() {};
	~ToStr() {};

	template <typename InType>
	OutType Convert(const InType& s);

private:

};

// -> wstring

//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const std::string_view& s)
//{
//	return std::wstring{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(s.data()) };
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const std::string& s)
//{
//	return std::wstring{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(s) };
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const CString& s)
//{
//	return std::wstring{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(s) };
//}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const PWCHAR& s)
{
	return std::wstring{ s };
}

//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const PCHAR& s)
//{
//	return std::wstring{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(s) };
//}


// -> string

template <> template <>
inline std::string ToStr<std::string>::Convert(const std::wstring_view& s)
{
	return std::string{ CW2A(s.data()) };
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const std::wstring& s)
{
	return std::string{ CW2A(s.c_str()) };
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const CString& s)
{
	return std::string{ CT2A(s) };
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const PWCHAR& s)
{
	return std::string{ CW2A(s) };
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const PCHAR& s)
{
	return std::string{ s };
}


// -> CString

template <> template <>
inline CString ToStr<CString>::Convert(const std::wstring_view& s)
{
	return CString{ CW2A(s.data()) };
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::wstring& s)
{
	return CString{ CW2A(s.c_str()) };
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::string_view& s)
{
	return CString{ s.data() };
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::string& s)
{
	return CString{ s.c_str() };
}

template <> template <>
inline CString ToStr<CString>::Convert(const PWCHAR& s)
{
	return CString{ CW2A(s) };
}

template <> template <>
inline CString ToStr<CString>::Convert(const PCHAR& s)
{
	return CString{ s };
}