#include "pch.h"
#include "Logger.h"
#include <ranges>

void Logger::Debug(std::string_view customLog)
{
#ifdef _DEBUG
	auto logStr = std::format("{} :: [ {} ] {}\n", GetUtcFormat(), LOG_LEVEL(LOG_LEVEL::DEBUG)._to_string(), customLog);
	Print(LOG_LEVEL::DEBUG, logStr);
#else

#endif // _DEBUG
}

void Logger::Log(const ERROR_LOG& err)
{
	MakeLog(err);
}

void Logger::Log(const ERROR_LOG& err, std::string_view additionalLog)
{
	MakeLog(err, "", -1, additionalLog);
}

void Logger::Log(const ERROR_LOG& err, std::string_view _file, int _line)
{
	MakeLog(err, _file, _line);
}

void Logger::Log(const ERROR_LOG& err, std::string_view additionalLog, std::string_view _file, int _line)
{
	MakeLog(err, _file, _line, additionalLog);
}



std::string Logger::GetUtcFormat()
{
	auto utcTime = std::chrono::system_clock::now();
	auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(utcTime);
	auto datetime = std::format("{0:%F} {0:%T}", timePoint);
	return datetime;
}

void Logger::MakeLog(const ERROR_LOG& err, std::string_view _file, int _line, std::string_view additionalLog)
{
	auto& log_detail = GetErrorInfo(err);
	auto logStr = MakeLogString(log_detail.m_logLevel._to_string(), log_detail.m_logName, log_detail.m_logDescription, _file, _line, additionalLog);
	Print(log_detail.m_logLevel, logStr);
}

std::string Logger::MakeLogString(std::string_view errLevelString, std::string_view errName, std::string_view errDesciption, std::string_view _file, int _line, std::string_view additionalLog)
{
	auto date = std::format("{} :: ", GetUtcFormat());
	auto log_level = std::format("[ {} ] {} - {}", errLevelString, errName, errDesciption);

	auto base_log = date + log_level;
	if (_file != "" && _line > 0)
	{
		std::vector<std::string_view> words;
		for (std::string_view word : std::ranges::split_view(_file, '\\')) { words.emplace_back(word); }
		base_log += std::format(". File ( {} ), Line No.{}", *words.rbegin(), _line);
	}
	if (additionalLog != "") { base_log += std::format(". Additional Log = {}", additionalLog); }

	return base_log + "\n";
}

ErrorLogDetail& Logger::GetErrorInfo(const ERROR_LOG& err)
{
	return LogManager::Inst().GetDetail(err);
}

void Logger::Print(const LOG_LEVEL& err_level, std::string_view log)
{
#ifdef _DEBUG
	std::cout << log;
#else
	switch (err_level)
	{
	case LOG_LEVEL::ALERT:
		std::cout << log;
	default:
		break;
	}
#endif // _DEBUG
}