#pragma once

using LogDescript = std::string;
using ErrorNumber = int;
using LogDetail = std::pair<ErrorNumber, LogDescript>;

class LogManager : public Singleton<LogManager>
{
protected:
	friend LogManager& Singleton<LogManager>::Inst();

	LogManager()
	{
		AddLogDetail(LOG_TYPE::NONE, ERROR_LOG::NO_LOG, "No Log");
		AddLogDetail(LOG_TYPE::DEBUG, ERROR_LOG::LOG_TEST, "Log Test Successful");
		AddLogDetail(LOG_TYPE::DEBUG, ERROR_LOG::DEBUG, "");
		AddLogDetail(LOG_TYPE::ALERT, ERROR_LOG::INVALID_CODE, "Invalid Error Code");
		AddLogDetail(LOG_TYPE::ALERT, ERROR_LOG::INVALID_NAME, "Invalid Error Name");
		AddLogDetail(LOG_TYPE::ALERT, ERROR_LOG::SAME_LOG_NAME_ERROR, "When you insert new error code, log name already exist in log manager");
		AddLogDetail(LOG_TYPE::ALERT, ERROR_LOG::INVALID_PACKET_DATA, "Packet PACKET_DELIMITER is not same");
	}

public:
	~LogManager() {};

	LogDetail GetDetail(ERROR_LOG err)
	{
		//std::scoped_lock<std::mutex> lock{ m_lock };

		auto log = std::make_pair(0, std::string{ "No Log" });
		auto iter = m_logDescriptionMap.find(err._to_integral());
		if (iter != m_logDescriptionMap.end()) { log = iter->second; }

		return log;
	}

private:
	std::mutex m_lock;
	std::unordered_map<int, LogDetail> m_logDescriptionMap;

	void AddLogDetail(LOG_TYPE errType, ERROR_LOG logType, std::string_view msg)
	{
		//std::scoped_lock lock{ m_lock };
		m_logDescriptionMap[errType._to_integral()] = std::make_pair(logType._to_integral(), std::string{msg});
	}
};


class Logger : public Singleton<Logger>
{
public:
	Logger() {};
	~Logger() {};

	void Debug(std::string_view customLog)
	{
#ifdef _DEBUG
		auto utcTime = std::chrono::system_clock::now();
		auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(utcTime);
		auto datetime = std::format("{0:%F} {0:%T}", timePoint);

		auto logStr = std::format("{} :: [ {} ] {}\n", datetime, LOG_TYPE(LOG_TYPE::DEBUG)._to_string(), customLog);
		std::cout << logStr;
#else

#endif // _DEBUG
	}

	void Log(ERROR_LOG err)
	{
		const auto& logDetail = LogManager::Inst().GetDetail(err);
		auto name = std::string_view{ err._to_string() };
		auto num = err._to_integral();
		auto logType = LOG_TYPE::_from_integral(logDetail.first);
		auto logTypeStr = std::string_view{ logType._to_string() };
		auto logDescription = std::string_view{ logDetail.second.data() };

		auto utcTime = std::chrono::system_clock::now();
		auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(utcTime);
		auto datetime = std::format("{0:%F} {0:%T}", timePoint);

		auto logStr = std::format("{} :: [ {} ] No.{}, {} - {}\n", datetime, logTypeStr, num, name, logDescription);
#ifdef _DEBUG
		std::cout << logStr;
#else
		switch (logType)
		{
		case LOG_TYPE::ALERT:
			std::cout << logStr;
		default:
			break;
		}
#endif // _DEBUG

	}

	void Log(ERROR_LOG err, std::string_view _file, int _line)
	{
		const auto& logDetail = LogManager::Inst().GetDetail(err);
		auto name = std::string_view{ err._to_string() };
		auto num = err._to_integral();
		auto logType = LOG_TYPE::_from_integral(logDetail.first);
		auto logTypeStr = std::string_view{ logType._to_string() };
		auto logDescription = std::string_view{ logDetail.second.data() };

		auto utcTime = std::chrono::system_clock::now();
		auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(utcTime);
		auto datetime = std::format("{0:%F} {0:%T}", timePoint);

		auto logStr = std::format("{} :: [ {} ] No.{}, {} - {}. Line No.{}, File ( {} )\n", datetime, logTypeStr, num, name, logDescription, _line, _file);
#ifdef _DEBUG
		std::cout << logStr;
#else
		switch (logType)
		{
		case LOG_TYPE::ALERT:
			std::cout << logStr;
		default:
			break;
		}
#endif // _DEBUG

	}

	void Log(ERROR_LOG err, std::string_view additionalLog, std::string_view _file, int _line)
	{
		const auto& logDetail = LogManager::Inst().GetDetail(err);
		auto name = std::string_view{ err._to_string() };
		auto num = err._to_integral();
		auto logType = LOG_TYPE::_from_integral(logDetail.first);
		auto logTypeStr = std::string_view{ logType._to_string() };
		auto logDescription = std::string_view{ logDetail.second.data() };

		auto utcTime = std::chrono::system_clock::now();
		auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(utcTime);
		auto datetime = std::format("{0:%F} {0:%T}", timePoint);

		auto logStr = std::format("{} :: [ {} ] No.{}, {} - {}. Line No.{}, File ( {} ). Additional Log = {}\n", datetime, logTypeStr, num, name, logDescription, _line, _file, additionalLog);
#ifdef _DEBUG
		std::cout << logStr;
#else
		switch (logType)
		{
		case LOG_TYPE::ALERT:
			std::cout << logStr;
		default:
			break;
		}

#endif // _DEBUG

	}

private:

};
