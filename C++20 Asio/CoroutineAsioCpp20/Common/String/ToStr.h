#pragma once

namespace
{
	constexpr int MAX_CONVERT_STR_SIZE = 1000;
	constexpr int DEFAULT_ENCODE = CP_ACP;
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
	DWORD convert_ansi_to_unicode_string(__out std::wstring& unicode, __in const char* ansi, __in const size_t ansi_size);
	DWORD convert_unicode_to_ansi_string(__out std::string& ansi, __in const wchar_t* unicode, __in const size_t unicode_size);
	DWORD convert_unicode_to_utf8_string(__out std::string& utf8, __in const wchar_t* unicode, __in const size_t unicode_size);
	DWORD convert_utf8_to_unicode_string(__out std::wstring& unicode, __in const char* utf8, __in const size_t utf8_size);

	DWORD convert_default_string(__out std::string& s, __in const wchar_t* pws, __in const size_t s_size);
	DWORD convert_default_wstring(__out std::wstring& ws, __in const char* ps, __in const size_t s_size);
private:
	std::locale loc{ "" };
};

#define To_WString(x) ToStr<std::wstring>::Inst().Convert(x)
#define To_String(x) ToStr<std::string>::Inst().Convert(x)
#define To_CString(x) ToStr<CString>::Inst().Convert(x)
#define To_CStringA(x) ToStr<CStringA>::Inst().Convert(x)

#define To_Utf8(t, x) ToStr<t>::Inst().UNICODE_to_UTF8(x)
#define To_Unicode(t, x) ToStr<t>::Inst().UTF8_to_UNICODE(x)
