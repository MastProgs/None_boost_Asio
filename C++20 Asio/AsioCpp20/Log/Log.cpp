#include "stdafx.h"
#include "Log.h"

#include "../ObjectBase/Singleton.h"

#include <vector>
#include <format>
#include <string>

template<typename ErrorType>
inline Log<ErrorType>::Log(const std::string_view& typeName, int errCode, ErrorType errType, const std::string_view& msg)
	: m_type(typeName, errCode, errType)
	, m_msg{ msg }
{

}

template<typename ErrorType>
Log<ErrorType>::Log(const LogType<ErrorType>& logType, const std::string_view& msg)
{

}