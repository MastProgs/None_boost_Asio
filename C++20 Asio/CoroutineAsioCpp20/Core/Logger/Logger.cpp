#include "pch.h"
#include "Logger.h"
#include <ranges>

void Logger::Info(std::string_view customLog)
{
#ifdef _DEBUG
	auto logStr = Format("{} :: [ {} ] {}\n", GetUtcFormat(), LOG_LEVEL(LOG_LEVEL::INFO)._to_string(), customLog);
	Print(LOG_LEVEL::INFO, logStr);
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
	auto datetime = Format("{:%Y-%m-%d %H:%M:%OS}", timePoint);
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
	auto date = Format("{} :: ", GetUtcFormat());
	auto log_level = Format("[ {} ] {} - {}", errLevelString, errName, errDesciption);

	auto base_log = date + log_level;
	if (_file != "" && _line > 0)
	{
		std::vector<std::string_view> words;
		std::string_view delim{ "\\" };
		for (const auto& word : std::ranges::split_view(_file, delim))
		{
			words.emplace_back(std::string_view(word.begin(), word.end()));
		}
		base_log += Format(". File ( {} ), Line No.{}", *words.rbegin(), _line);
	}
	if (additionalLog != "") { base_log += Format(". Additional Log = {}", additionalLog); }

	return base_log;
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
