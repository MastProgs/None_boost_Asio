#pragma once

using ERROR_LOG_Name = std::string;
using ERROR_LOG_Descript = std::string;

struct ErrorLogDetail
{
	explicit ErrorLogDetail() {}
	explicit ErrorLogDetail(LOG_LEVEL logLv, std::string_view logName, std::string_view logDesc) : m_logLevel{ logLv }, m_logName{ logName }, m_logDescription{ logDesc }{}
	~ErrorLogDetail() {}

	LOG_LEVEL m_logLevel = LOG_LEVEL{ LOG_LEVEL::NONE };
	std::string m_logName = ERROR_LOG{ ERROR_LOG::NO_LOG }._to_string();
	std::string m_logDescription = "No Log";
};

class LogManager : public Singleton<LogManager>
{
protected:
	friend LogManager& Singleton<LogManager>::Inst();

	LogManager()
	{
		AddLogDetail(LOG_LEVEL::NONE, ERROR_LOG::NO_LOG, "No Log");
		AddLogDetail(LOG_LEVEL::DEBUG, ERROR_LOG::LOG_TEST, "Log Test Successful");
		AddLogDetail(LOG_LEVEL::DEBUG, ERROR_LOG::DEBUG, "");
		AddLogDetail(LOG_LEVEL::ALERT, ERROR_LOG::INVALID_CODE, "Invalid Error Code");
		AddLogDetail(LOG_LEVEL::ALERT, ERROR_LOG::INVALID_NAME, "Invalid Error Name");
		AddLogDetail(LOG_LEVEL::ALERT, ERROR_LOG::SAME_LOG_NAME_ERROR, "When you insert new error code, log name already exist in log manager");
		AddLogDetail(LOG_LEVEL::ALERT, ERROR_LOG::INVALID_PACKET_DATA, "Packet PACKET_DELIMITER is not same");
	}

public:
	~LogManager() {};

	ErrorLogDetail& GetDetail(const ERROR_LOG& err)
	{
		//std::scoped_lock<std::mutex> lock{ m_lock };
		auto err_no = ERROR_LOG(ERROR_LOG::NO_LOG)._to_integral();
		ErrorLogDetail& log = m_logDescriptionMap[err_no];

		err_no = err._to_integral();
		auto iter = m_logDescriptionMap.find(err_no);
		if (iter != m_logDescriptionMap.end()) { log = iter->second; }

		return log;
	}

private:
	std::mutex m_lock;
	std::unordered_map<int, ErrorLogDetail> m_logDescriptionMap;

	void AddLogDetail(LOG_LEVEL errLevel, ERROR_LOG logType, std::string_view msg)
	{
		//std::scoped_lock lock{ m_lock };
		m_logDescriptionMap[logType._to_integral()] = ErrorLogDetail{ errLevel, std::string{logType._to_string()}, std::string{msg} };
	}
};


class Logger : public Singleton<Logger>
{
public:
	Logger() {};
	~Logger() {};

	void Debug(std::string_view customLog);
	void Log(const ERROR_LOG& err);
	void Log(const ERROR_LOG& err, std::string_view additionalLog);
	void Log(const ERROR_LOG& err, std::string_view _file, int _line);
	void Log(const ERROR_LOG& err, std::string_view additionalLog, std::string_view _file, int _line);

private:
	std::string GetUtcFormat();
	void MakeLog(const ERROR_LOG& err, std::string_view _file = "", int _line = -1, std::string_view additionalLog = "");
	std::string MakeLogString(std::string_view errLevelString, std::string_view errName, std::string_view errDesciption, std::string_view _file = "", int _line = -1, std::string_view additionalLog = "");

	ErrorLogDetail& GetErrorInfo(const ERROR_LOG& err);

	void Print(const LOG_LEVEL& err_level, std::string_view log);
};
