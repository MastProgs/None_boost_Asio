#include "pch.h"
#include "ToStr.h"

#include <cstring>
#include <locale>
#include <codecvt>
#include <comdef.h>
#include <string>
#include <atlstr.h>


template<typename OutType>
inline DWORD ToStr<OutType>::convert_ansi_to_unicode_string(__out std::wstring& unicode, __in const char* ansi, __in const size_t ansi_size)
{
	DWORD error = 0;
	unicode.clear();

	do {
		if ((nullptr == ansi) || (0 == ansi_size))
		{
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		// getting required cch.
		int required_cch = ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int>(ansi_size), nullptr, 0);
		if (0 == required_cch)
		{
			error = ::GetLastError();
			break;
		}
		unicode.resize(required_cch);

		// convert.
		if (0 == ::MultiByteToWideChar(CP_ACP, 0, ansi, static_cast<int>(ansi_size), const_cast<wchar_t*>(unicode.data()), static_cast<int>(unicode.size())))
		{
			error = ::GetLastError();
			break;
		}
	} while (false);

	return error;
}

template<typename OutType>
inline DWORD ToStr<OutType>::convert_unicode_to_ansi_string(__out std::string& ansi, __in const wchar_t* unicode, __in const size_t unicode_size)
{
	DWORD error = 0;
	ansi.clear();

	do {
		if ((nullptr == unicode) || (0 == unicode_size))
		{
			error = ERROR_INVALID_PARAMETER;
			break;
		}

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

		if (0 == ::WideCharToMultiByte(CP_ACP, 0, unicode, static_cast<int>(unicode_size), const_cast<char*>(ansi.data()), static_cast<int>(ansi.size()), nullptr, nullptr))
		{
			error = ::GetLastError();
			break;
		}
	} while (false);

	return error;
}

template<typename OutType>
inline DWORD ToStr<OutType>::convert_unicode_to_utf8_string(__out std::string& utf8, __in const wchar_t* unicode, __in const size_t unicode_size)
{
	DWORD error = 0;
	utf8.clear();

	do {
		if ((nullptr == unicode) || (0 == unicode_size))
		{
			error = ERROR_INVALID_PARAMETER;
			break;
		}

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
		if (0 == ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, unicode, static_cast<int>(unicode_size), const_cast<char*>(utf8.data()), static_cast<int>(utf8.size()), nullptr, nullptr))
		{
			error = ::GetLastError();
			break;
		}
	} while (false);

	return error;
}

template<typename OutType>
inline DWORD ToStr<OutType>::convert_utf8_to_unicode_string(__out std::wstring& unicode, __in const char* utf8, __in const size_t utf8_size)
{
	DWORD error = 0;
	unicode.clear();

	do {
		if ((nullptr == utf8) || (0 == utf8_size))
		{
			error = ERROR_INVALID_PARAMETER;
			break;
		}

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
		if (0 == ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8, static_cast<int>(utf8_size), const_cast<wchar_t*>(unicode.data()), static_cast<int>(unicode.size())))
		{
			error = ::GetLastError();
			break;
		}
	} while (false);

	return error;
}


template<typename OutType>
inline DWORD ToStr<OutType>::convert_default_string(__out std::string& s, __in const wchar_t* pws, __in const size_t s_size)
{
	DWORD error = 0;
	s.clear();

	do {
		if ((nullptr == pws) || (0 == s_size))
		{
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		// getting required cch.
		int required_cch = ::WideCharToMultiByte(DEFAULT_ENCODE, WC_ERR_INVALID_CHARS, pws, static_cast<int>(s_size), nullptr, 0, nullptr, nullptr);
		if (0 == required_cch)
		{
			error = ::GetLastError();
			break;
		}

		// allocate.
		s.resize(required_cch);

		// convert.
		if (0 == ::WideCharToMultiByte(DEFAULT_ENCODE, WC_ERR_INVALID_CHARS, pws, static_cast<int>(s_size), const_cast<char*>(s.data()), static_cast<int>(s.size()), nullptr, nullptr))
		{
			error = ::GetLastError();
			break;
		}
	} while (false);

	return error;
}


template<typename OutType>
inline DWORD ToStr<OutType>::convert_default_wstring(__out std::wstring& ws, __in const char* ps, __in const size_t s_size)
{
	DWORD error = 0;
	ws.clear();

	do {
		if ((nullptr == ps) || (0 == s_size))
		{
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		// getting required cch.
		int required_cch = ::MultiByteToWideChar(DEFAULT_ENCODE, MB_ERR_INVALID_CHARS, ps, static_cast<int>(s_size), nullptr, 0);
		if (0 == required_cch)
		{
			error = ::GetLastError();
			break;
		}

		// allocate.
		ws.resize(required_cch);

		// convert.
		if (0 == ::MultiByteToWideChar(DEFAULT_ENCODE, MB_ERR_INVALID_CHARS, ps, static_cast<int>(s_size), const_cast<wchar_t*>(ws.data()), static_cast<int>(ws.size())))
		{
			error = ::GetLastError();
			break;
		}
	} while (false);

	return error;
}



// -> wstring

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const std::wstring& s)
{
	return s;
}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const std::string_view& s)
{
	std::wstring ws;
	convert_default_wstring(ws, s.data(), s.size());
	return ws;
}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const std::string& s)
{
	std::wstring ws;
	convert_default_wstring(ws, s.data(), s.size());
	return ws;
}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const CString& s)
{
	return std::wstring{ s };
}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const CStringA& s)
{
	std::wstring ws;
	convert_default_wstring(ws, s, CStringA::StringLength(s));
	return ws;
}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const PWCHAR& s)
{
	return std::wstring{ s };
}

template <> template <>
inline std::wstring ToStr<std::wstring>::Convert(const PCHAR& s)
{
	std::wstring ws;
	convert_default_wstring(ws, s, CStringA::StringLength(s));
	return ws;
}



// -> string

template <> template <>
inline std::string ToStr<std::string>::Convert(const std::string& s)
{
	return s;
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const std::wstring_view& s)
{
	std::string ss;
	convert_default_string(ss, s.data(), s.size());
	return ss;
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const std::wstring& s)
{
	std::string ss;
	convert_default_string(ss, s.data(), s.size());
	return ss;
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const CString& s)
{
	return std::string{ CW2A(s) };
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const CStringA& s)
{
	return std::string{ s };
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const PWCHAR& s)
{
	std::string ss;
	convert_default_string(ss, s, CString::StringLength(s));
	return ss;
}

template <> template <>
inline std::string ToStr<std::string>::Convert(const PCHAR& s)
{
	return std::string{ s };
}



// -> CString

template <> template <>
inline CString ToStr<CString>::Convert(const CString& s)
{
	return s;
}

template <> template <>
inline CString ToStr<CString>::Convert(const CStringA& s)
{
	return CString(s);
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::wstring_view& s)
{
	return CString{ s.data() };
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::wstring& s)
{
	return CString{ s.data() };
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::string_view& s)
{
	return CString{ s.data() };
}

template <> template <>
inline CString ToStr<CString>::Convert(const std::string& s)
{
	return CString{ s.data() };
}

template <> template <>
inline CString ToStr<CString>::Convert(const PWCHAR& s)
{
	return CString{ s };
}

template <> template <>
inline CString ToStr<CString>::Convert(const PCHAR& s)
{
	return CString{ s };
}


// -> CStringA

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const CStringA& s)
{
	return s;
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const CString& s)
{
	return CStringA(s);
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const std::wstring_view& s)
{
	return CStringA{ s.data() };
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const std::wstring& s)
{
	return CStringA{ s.data() };
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const std::string_view& s)
{
	return CStringA{ s.data() };
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const std::string& s)
{
	return CStringA{ s.data() };
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const PWCHAR& s)
{
	return CStringA{ s };
}

template <> template <>
inline CStringA ToStr<CStringA>::Convert(const PCHAR& s)
{
	return CStringA{ s };
}



/// UTF8 <-> UNICODE

template <> template <>
inline std::string ToStr<std::string>::UNICODE_to_UTF8(const CString& s)
{
	std::string utf8;
	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
	return utf8;
}

template <> template <>
inline std::string ToStr<std::string>::UNICODE_to_UTF8(const std::wstring_view& s)
{
	std::string utf8;
	convert_unicode_to_utf8_string(utf8, s.data(), s.size());
	return utf8;
}

template <> template <>
inline std::string ToStr<std::string>::UNICODE_to_UTF8(const PWCHAR& s)
{
	std::string utf8;
	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
	return utf8;
}


template <> template <>
inline CStringA ToStr<CStringA>::UNICODE_to_UTF8(const CString& s)
{
	std::string utf8;
	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
	return ToStr<CStringA>::Inst().Convert(utf8);
}

template <> template <>
inline CStringA ToStr<CStringA>::UNICODE_to_UTF8(const std::wstring_view& s)
{
	std::string utf8;
	convert_unicode_to_utf8_string(utf8, s.data(), s.size());
	return ToStr<CStringA>::Inst().Convert(utf8);
}

template <> template <>
inline CStringA ToStr<CStringA>::UNICODE_to_UTF8(const PWCHAR& s)
{
	std::string utf8;
	convert_unicode_to_utf8_string(utf8, s, CString::StringLength(s));
	return ToStr<CStringA>::Inst().Convert(utf8);
}

////

template <> template <>
inline std::wstring ToStr<std::wstring>::UTF8_to_UNICODE(const CStringA& s)
{
	std::wstring unicode;
	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
	return ToStr<std::wstring>::Inst().Convert(unicode);
}

template <> template <>
inline std::wstring ToStr<std::wstring>::UTF8_to_UNICODE(const std::string_view& s)
{
	std::wstring unicode;
	convert_utf8_to_unicode_string(unicode, s.data(), s.size());
	return ToStr<std::wstring>::Inst().Convert(unicode);
}

template <> template <>
inline std::wstring ToStr<std::wstring>::UTF8_to_UNICODE(const PCHAR& s)
{
	std::wstring unicode;
	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
	return ToStr<std::wstring>::Inst().Convert(unicode);
}


template <> template <>
inline CString ToStr<CString>::UTF8_to_UNICODE(const CStringA& s)
{
	std::wstring unicode;
	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
	return ToStr<CString>::Inst().Convert(unicode);
}

template <> template <>
inline CString ToStr<CString>::UTF8_to_UNICODE(const std::string_view& s)
{
	std::wstring unicode;
	convert_utf8_to_unicode_string(unicode, s.data(), s.size());
	return ToStr<CString>::Inst().Convert(unicode);
}

template <> template <>
inline CString ToStr<CString>::UTF8_to_UNICODE(const PCHAR& s)
{
	std::wstring unicode;
	convert_utf8_to_unicode_string(unicode, s, CStringA::StringLength(s));
	return ToStr<CString>::Inst().Convert(unicode);
}