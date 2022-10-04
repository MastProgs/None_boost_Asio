#pragma once

#pragma warning(push)
#pragma warning(disable: 4091)  // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)
#pragma comment(lib, "Dbghelp.lib")

//
// exception handler.
//

class ExceptionHandler : public Singleton<ExceptionHandler>
{
public:
	~ExceptionHandler() {}

	DWORD initialize(
		__in LPCTSTR dump_file_name,
		__in const MINIDUMP_TYPE dump_type = MINIDUMP_TYPE::MiniDumpNormal
	);

	DWORD run();

	static LONG WINAPI exception_callback(
		__in struct _EXCEPTION_POINTERS* exceptioninfo
	);

	std::wstring _dump_file_name;
	MINIDUMP_TYPE _dump_type;
	LPTOP_LEVEL_EXCEPTION_FILTER _prev_filter;
};
