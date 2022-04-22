#pragma once

class CTask;

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

	CTask Debug(std::string_view customLog);
	CTask Log(ERROR_LOG err);
	CTask Log(ERROR_LOG err, std::string_view _file, int _line);
	CTask Log(ERROR_LOG err, std::string_view additionalLog, std::string_view _file, int _line);

private:

};
