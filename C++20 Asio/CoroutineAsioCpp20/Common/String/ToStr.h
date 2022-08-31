#pragma once

#include <cstring>
#include <locale>
#include <codecvt>
#include <comdef.h>
#include <string>
#include <atlstr.h>

namespace
{
	constexpr int MAX_CONVERT_STR_SIZE = 1000;
}

template <typename OutType>
class ToStr : public Singleton<ToStr<OutType>>
{
public:
	ToStr() {};
	~ToStr() {};

	template <typename InType>
	OutType Convert(const InType& s);

	template <typename InType>
	OutType UNICODE_to_UTF8(const InType& s);

	template <typename InType>
	OutType UTF8_to_UNICODE(const InType& s);

private:

	std::string Multibyte_to_UTF8(const std::string& str)
	{
		auto nLen = str.size();
		wchar_t warr[MAX_CONVERT_STR_SIZE] = { 0 };
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), -1, warr, nLen);
		char carr[MAX_CONVERT_STR_SIZE] = { 0 };
		WideCharToMultiByte(CP_UTF8, 0, warr, -1, carr, nLen, nullptr, nullptr);
		return carr;
	}

	std::string UTF8_to_Multibyte(const std::string& str)
	{
		auto nLen = str.size();
		wchar_t warr[MAX_CONVERT_STR_SIZE] = { 0 };
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, warr, nLen);
		char carr[MAX_CONVERT_STR_SIZE] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, warr, -1, carr, nLen, nullptr, nullptr);
		return carr;
	}

	DWORD convert_ansi_to_unicode_string(__out std::wstring& unicode, __in const char* ansi, __in const size_t ansi_size)
	{
		DWORD error = 0;
		do {
			if ((nullptr == ansi) || (0 == ansi_size))
			{
				error = ERROR_INVALID_PARAMETER;
				break;
			}
			unicode.clear();

			// getting required cch.
			int required_cch = ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int>(ansi_size), nullptr, 0);
			if (0 == required_cch)
			{
				error = ::GetLastError();
				break;
			}
			unicode.resize(required_cch);

			// convert.
			if (0 == ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int>(ansi_size), const_cast<wchar_t*>(unicode.c_str()), static_cast<int>(unicode.size())))
			{
				error = ::GetLastError();
				break;
			}
		} while (false);

		return error;
	}

	DWORD convert_unicode_to_ansi_string(__out std::string& ansi, __in const wchar_t* unicode, __in const size_t unicode_size)
	{
		DWORD error = 0;
		do {
			if ((nullptr == unicode) || (0 == unicode_size))
			{
				error = ERROR_INVALID_PARAMETER;
				break;
			}
			ansi.clear();

			// getting required cch.
			int required_cch = ::WideCharToMultiByte(CP_ACP, 0, unicode, static_cast<int>(unicode_size), nullptr, 0, nullptr, nullptr);
			if (0 == required_cch)
			{
				error = ::GetLastError();
				break;
			}

			// allocate.
			ansi.resize(required_cch);

			// convert.

			if (0 == ::WideCharToMultiByte(CP_ACP, 0, unicode, static_cast<int>(unicode_size), const_cast<char*>(ansi.c_str()), static_cast<int>(ansi.size()), nullptr, nullptr))
			{
				error = ::GetLastError();
				break;
			}
		} while (false);

		return error;
	}

	DWORD convert_unicode_to_utf8_string(__out std::string& utf8, __in const wchar_t* unicode, __in const size_t unicode_size)
	{
		DWORD error = 0;
		do {
			if ((nullptr == unicode) || (0 == unicode_size))
			{
				error = ERROR_INVALID_PARAMETER;
				break;
			}
			utf8.clear();

			// getting required cch.
			int required_cch = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, unicode, static_cast<int>(unicode_size), nullptr, 0, nullptr, nullptr);
			if (0 == required_cch)
			{
				error = ::GetLastError();
				break;
			}

			// allocate.
			utf8.resize(required_cch);

			// convert.
			if (0 == ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, unicode, static_cast<int>(unicode_size), const_cast<char*>(utf8.c_str()), static_cast<int>(utf8.size()), nullptr, nullptr))
			{
				error = ::GetLastError();
				break;
			}
		} while (false);

		return error;
	}

	DWORD convert_utf8_to_unicode_string(__out std::wstring& unicode, __in const char* utf8, __in const size_t utf8_size)
	{
		DWORD error = 0;
		do {
			if ((nullptr == utf8) || (0 == utf8_size))
			{
				error = ERROR_INVALID_PARAMETER;
				break;
			}
			unicode.clear();

			// getting required cch.
			int required_cch = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, static_cast<int>(utf8_size), nullptr, 0);
			if (0 == required_cch)
			{
				error = ::GetLastError();
				break;
			}

			// allocate.
			unicode.resize(required_cch);

			// convert.
			if (0 == ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, static_cast<int>(utf8_size), const_cast<wchar_t*>(unicode.c_str()), static_cast<int>(unicode.size())))
			{
				error = ::GetLastError();
				break;
			}
		} while (false);

		return error;
	}
};

// -> wstring

//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const std::wstring& s)
//{
//	return s;
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const std::string_view& s)
//{
//	static std::locale loc("");
//	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(s.data());
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const std::string& s)
//{
//	static std::locale loc("");
//	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(s.data());
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const CString& s)
//{
//	return std::wstring{ s };
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const CStringA& s)
//{
//	return std::wstring{ std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(s) };
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const PWCHAR& s)
//{
//	return std::wstring{ s };
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::Convert(const PCHAR& s)
//{
//	static std::locale loc("");
//	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(s);
//}
//
//#define TO_WSTRING(x) ToStr<std::wstring>::Inst().Convert(x)
//
//
//// -> string
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const std::string& s)
//{
//	return s;
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const std::wstring_view& s)
//{
//	static std::locale loc("");
//	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(s.data());
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const std::wstring& s)
//{
//	static std::locale loc("");
//	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(s.data());
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const CString& s)
//{
//	return std::string{ CW2A(s) };
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const CStringA& s)
//{
//	return std::string{ s };
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const PWCHAR& s)
//{
//	static std::locale loc("");
//	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
//	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(s);
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::Convert(const PCHAR& s)
//{
//	return std::string{ s };
//}
//
//#define TO_STRING(x) ToStr<std::string>::Inst().Convert(x)
//
//
//// -> CString
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const CString& s)
//{
//	return s;
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const CStringA& s)
//{
//	return CString(s);
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const std::wstring_view& s)
//{
//	return CString{ s.data() };
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const std::wstring& s)
//{
//	return CString{ s.c_str() };
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const std::string_view& s)
//{
//	return CString{ s.data() };
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const std::string& s)
//{
//	return CString{ s.c_str() };
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const PWCHAR& s)
//{
//	return CString{ s };
//}
//
//template <> template <>
//inline CString ToStr<CString>::Convert(const PCHAR& s)
//{
//	return CString{ s };
//}
//
//#define TO_CSTRING(x) ToStr<CString>::Inst().Convert(x)
//
//
//
//// -> CStringA
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const CStringA& s)
//{
//	return s;
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const CString& s)
//{
//	return CStringA(s);
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const std::wstring_view& s)
//{
//	return CStringA{ s.data() };
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const std::wstring& s)
//{
//	return CStringA{ s.c_str() };
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const std::string_view& s)
//{
//	return CStringA{ s.data() };
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const std::string& s)
//{
//	return CStringA{ s.c_str() };
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const PWCHAR& s)
//{
//	return CStringA{ s };
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::Convert(const PCHAR& s)
//{
//	return CStringA{ s };
//}
//
//#define TO_CSTRINGA(x) ToStr<CStringA>::Inst().Convert(x)
//
//
//
//
///// UTF8 <-> UNICODE
//
//template <> template <>
//inline std::string ToStr<std::string>::UNICODE_to_UTF8(const CString& s)
//{
//	std::string utf8 = "";
//	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
//	return ToStr<std::string>::Inst().Convert(utf8);
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::UNICODE_to_UTF8(const std::wstring_view& s)
//{
//	std::string utf8 = "";
//	convert_unicode_to_utf8_string(utf8, s.data(), s.size());
//	return ToStr<std::string>::Inst().Convert(utf8);
//}
//
//template <> template <>
//inline std::string ToStr<std::string>::UNICODE_to_UTF8(const PWCHAR& s)
//{
//	std::string utf8 = "";
//	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
//	return ToStr<std::string>::Inst().Convert(utf8);
//}
//
//
//template <> template <>
//inline CStringA ToStr<CStringA>::UNICODE_to_UTF8(const CString& s)
//{
//	std::string utf8 = "";
//	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
//	return ToStr<CStringA>::Inst().Convert(utf8);
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::UNICODE_to_UTF8(const std::wstring_view& s)
//{
//	std::string utf8 = "";
//	convert_unicode_to_utf8_string(utf8, s.data(), s.size());
//	return ToStr<CStringA>::Inst().Convert(utf8);
//}
//
//template <> template <>
//inline CStringA ToStr<CStringA>::UNICODE_to_UTF8(const PWCHAR& s)
//{
//	std::string utf8 = "";
//	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
//	return ToStr<CStringA>::Inst().Convert(utf8);
//}
//
//////
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::UTF8_to_UNICODE(const CStringA& s)
//{
//	std::wstring unicode = L"";
//	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
//	return ToStr<std::wstring>::Inst().Convert(unicode);
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::UTF8_to_UNICODE(const std::string_view& s)
//{
//	std::wstring unicode = L"";
//	convert_utf8_to_unicode_string(unicode, s.data(), s.size());
//	return ToStr<std::wstring>::Inst().Convert(unicode);
//}
//
//template <> template <>
//inline std::wstring ToStr<std::wstring>::UTF8_to_UNICODE(const PCHAR& s)
//{
//	std::wstring unicode = L"";
//	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
//	return ToStr<std::wstring>::Inst().Convert(unicode);
//}
//
//
//template <> template <>
//inline CString ToStr<CString>::UTF8_to_UNICODE(const CStringA& s)
//{
//	std::wstring unicode = L"";
//	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
//	return ToStr<CString>::Inst().Convert(unicode);
//}
//
//template <> template <>
//inline CString ToStr<CString>::UTF8_to_UNICODE(const std::string_view& s)
//{
//	std::wstring unicode = L"";
//	convert_utf8_to_unicode_string(unicode, s.data(), s.size());
//	return ToStr<CString>::Inst().Convert(unicode);
//}
//
//template <> template <>
//inline CString ToStr<CString>::UTF8_to_UNICODE(const PCHAR& s)
//{
//	std::wstring unicode = L"";
//	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
//	return ToStr<CString>::Inst().Convert(unicode);
//}
//
//#define TO_UTF8(t, x) ToStr<t>::Inst().UNICODE_to_UTF8(x);
//#define TO_UNICODE(t, x) ToStr<t>::Inst().UTF8_to_UNICODE(x);