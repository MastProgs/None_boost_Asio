#pragma once

template<typename ErrorType>
class LogType
{
public:
	explicit LogType(std::string_view logName, int errCode, ErrorType errType)
		: m_logName{ logName }
		, m_errCode{ errCode }
		, m_errType{ errType }
	{
	}

	std::string_view GetLogName() const { return m_logName.c_str(); }
	int GetErrorCode() const { return m_errCode; }
	ErrorType GetErrorType() const { return m_errType; }

private:
	std::string m_logName;
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

class LogVirtual
{
public:
	LogVirtual() {};
	~LogVirtual() {};

	virtual std::string_view GetLogName() const = 0;
	virtual int GetErrorCode() const = 0;
	virtual std::string_view GetErrorMsg() const = 0;
};

template<typename ErrorType>
class Log : public LogVirtual
{
public:
	Log(std::string_view logName, int errCode, ErrorType errType, std::string_view msg)
		: m_type(logName, errCode, errType)
		, m_msg{ msg }
	{
	}
	Log(const LogType<ErrorType>& logType, std::string_view msg) {}

	virtual std::string_view GetLogName() const override final { return m_type.GetLogName(); }
	virtual int GetErrorCode() const override final { return m_type.GetErrorCode(); }
	virtual std::string_view GetErrorMsg() const override final { return m_msg.c_str(); }

	ErrorType GetErrorType() const { return m_type.GetErrorType(); }

private:
	LogType<ErrorType> m_type;
	std::string m_msg;

};


class LogManager : public Singleton<LogManager>
{
protected:
	friend LogManager& Singleton<LogManager>::Instance();

	LogManager()
	{
		InsertNewLog("NoLog", LogErrorType::NONE, "None Log");
		InsertNewLog("LogTest", LogErrorType::DEBUG, "Log Test Successful");
		InsertNewLog("InvalidCode", LogErrorType::ERROR, "Invalid Error Code");
		InsertNewLog("InvalidName", LogErrorType::ERROR, "Invalid Error Code");
		InsertNewLog("SameLogNameError", LogErrorType::ERROR, "When you insert new error code, log name already exist in log manager");
	}

public:
	~LogManager() {};

	std::string FindErrorCodeToMsg(int errCode)
	{
		std::scoped_lock<std::mutex> lock{ m_lock };

		auto iter = m_logMap.find(errCode);
		if (iter == m_logMap.end())
		{
			auto nIter = m_logNameMap.find("InvalidCode");
			if (nIter != m_logNameMap.end())
			{
				iter = m_logMap.find(nIter->second);
				if (iter != m_logMap.end())
				{
					return std::format("Log Name {} - Log Code {} : {}\n", nIter->first, nIter->second, iter->second->GetErrorMsg());
				}
			}

			return std::format("Invalid Error Code : {}\n", errCode);
		}

		return std::string{ iter->second->GetErrorMsg() };
	}

	std::string FindErrorNameToMsg(std::string_view logName)
	{
		std::scoped_lock<std::mutex> lock{ m_lock };

		auto iter = m_logNameMap.find(std::string{ logName });
		if (iter == m_logNameMap.end())
		{
			iter = m_logNameMap.find("InvalidName");
			if (iter != m_logNameMap.end())
			{
				auto nIter = m_logMap.find(iter->second);
				if (nIter != m_logMap.end())
				{
					return std::format("Log Name {} - Log Code {} : {}\n", iter->first, iter->second, nIter->second->GetErrorMsg());
				}
			}

			return std::format("Invalid Error Name : {}\n", logName);
		}

		auto nIter = m_logMap.find(iter->second);
		if (nIter == m_logMap.end())
		{
			return std::format("Invalid Error Code : {}\n", iter->second);
		}

		return std::string{ nIter->second->GetErrorMsg() };
	}

private:
	std::mutex m_lock;

	std::unordered_map<int, std::unique_ptr<LogVirtual>> m_logMap;
	std::unordered_map<std::string, int> m_logNameMap;

	int GetCode()
	{
		static std::atomic<int> i{ 0 };
		return i.fetch_add(1);
	}

	void InsertNewLog(std::string_view logName, LogErrorType errType, std::string_view msg)
	{
		std::scoped_lock<std::mutex> lock{ m_lock };

		auto iter = m_logNameMap.find(std::string{ logName });
		if (iter != m_logNameMap.end())
		{
			iter = m_logNameMap.find("SameLogNameError");
			auto logIter = m_logMap.find(iter->second);

			_ASSERT(true);
			return;
		}

		auto i = GetCode();
		m_logNameMap.insert({ std::string{ logName }, i });
		m_logMap.insert({ i, std::make_unique<Log<LogErrorType>>(logName, i, errType, msg) });
	}
};
