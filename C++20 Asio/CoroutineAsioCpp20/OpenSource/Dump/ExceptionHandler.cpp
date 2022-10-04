#include "pch.h"
#include "ExceptionHandler.h"

DWORD ExceptionHandler::initialize(
	__in LPCTSTR dump_file_name,
	__in const MINIDUMP_TYPE dump_type
) {

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

DWORD ExceptionHandler::run() {
	_prev_filter = ::SetUnhandledExceptionFilter(exception_callback);
	return ERROR_SUCCESS;
}

LONG ExceptionHandler::exception_callback(
	__in struct _EXCEPTION_POINTERS* exceptioninfo
) {

	do {

		if (nullptr == exceptioninfo) {
			break;
		}

		SYSTEMTIME st = { 0 };
		::GetSystemTime(&st);

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

	return (ExceptionHandler::Inst()._prev_filter) ? ExceptionHandler::Inst()._prev_filter(exceptioninfo) : EXCEPTION_EXECUTE_HANDLER;
}