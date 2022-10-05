#include "pch.h"
#include "ExceptionHandler.h"
#include "StackWalker.h"

class StackWalkerToConsole : public StackWalker
{
protected:
	virtual void OnOutput(LPCSTR szText) { std::cout << szText; }
};


LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp) //, DWORD dwExpCode)
{
	//StackWalker sw;  // output to default (Debug-Window)
	StackWalkerToConsole sw; // output to the console
	sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
	return EXCEPTION_EXECUTE_HANDLER;
}


DWORD ExceptionHandler::initialize(
	__in LPCTSTR dump_file_name,
	__in const MINIDUMP_TYPE dump_type
)
{

	DWORD error = ERROR_SUCCESS;

	do {

		if (nullptr == dump_file_name) {
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		_dump_file_name.assign(dump_file_name);
		_dump_type = dump_type;

	} while (false);

	return error;
}

DWORD ExceptionHandler::run() 
{
	_prev_filter = ::SetUnhandledExceptionFilter(exception_callback);
	return ERROR_SUCCESS;
}

LONG ExceptionHandler::exception_callback(
	__in struct _EXCEPTION_POINTERS* exceptioninfo
) 
{
	std::cout << "\n\n <<< Server Crashed !!! >>> \n\n";
	do 
	{
		if (nullptr == exceptioninfo) { break; }

		ExpFilter(exceptioninfo); // , GetExceptionCode());

		SYSTEMTIME st = { 0 };
		::GetLocalTime(&st);

		std::wstring dump_file_name;
		dump_file_name.assign(ExceptionHandler::Inst()._dump_file_name + L".dmp");

		//
		// create dump file.
		//

		HANDLE dump_file_handle = ::CreateFile(
			dump_file_name.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		if (INVALID_HANDLE_VALUE == dump_file_handle) {
			break;
		}

		MINIDUMP_EXCEPTION_INFORMATION ex_info = { 0 };

		ex_info.ThreadId = ::GetCurrentThreadId(); // Threae ID 설정
		ex_info.ExceptionPointers = exceptioninfo; // Exception 정보 설정
		ex_info.ClientPointers = FALSE;

		//
		// write dump file.
		//

		if (FALSE == ::MiniDumpWriteDump(
			::GetCurrentProcess(),
			::GetCurrentProcessId(),
			dump_file_handle,
			ExceptionHandler::Inst()._dump_type,
			&ex_info,
			nullptr, nullptr
		)) {

			break;
		}

	} while (false);

	int i;
	std::cin >> i;
	return (ExceptionHandler::Inst()._prev_filter) ? ExceptionHandler::Inst()._prev_filter(exceptioninfo) : EXCEPTION_EXECUTE_HANDLER;
}