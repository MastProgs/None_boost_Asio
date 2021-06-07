#pragma once

template<typename ErrorType>
class LogType
{
public:
	explicit LogType(const std::string_view& typeName, int errCode, ErrorType errType)
		: m_typeName{ typeName }
		, m_errCode{ errCode }
		, m_errType{ errType }
	{
	}

	std::string_view GetTypeName() const { return m_typeName; }
	int GetErrorCode() const { return m_errCode; }
	ErrorType GetErrorType() const { return m_errType; }

private:
	std::string_view m_typeName;
	int m_errCode;
	ErrorType m_errType;
};

enum class LogErrorType
{
	NONE
	, DEBUG
	, WARNING
	, ERROR
};

namespace LOG
{
	int GetCode()
	{
		static int i{ 0 };
		++i;
		return i;
	}
}


class LogVirtual
{
public:
	LogVirtual() {};
	~LogVirtual() {};
};

template<typename ErrorType>
class Log : public LogVirtual
{
public:
	Log(const std::string_view& typeName, int errCode, ErrorType errType, const std::string_view& msg);
	Log(const LogType<ErrorType>& logType, const std::string_view& msg);

	std::string_view GetTypeName() const { return m_type.GetErrorType(); }
	int GetErrorCode() const { return m_type.GetErrorCode(); }
	ErrorType GetErrorType() const { return m_type.GetErrorType(); }
	std::string_view GetErrorMsg() const { return m_msg; }

private:
	LogType<ErrorType> m_type;
	std::string m_msg;
};


class LogManager : public Singleton<LogManager>
{
public:
	LogManager()
	{
		m_logMap.emplace_back(Log<LogErrorType>{"NoLog", 0, LogErrorType::NONE, "None Log"});
		m_logMap.emplace_back(Log<LogErrorType>{"LogTest", LOG::GetCode(), LogErrorType::DEBUG, "Log Test Successful."});
	}
	~LogManager() {};

private:
	std::vector<LogVirtual> m_logMap;
};
