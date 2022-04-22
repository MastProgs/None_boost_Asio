#include "pch.h"
#include "Logger.h"
#include "Coroutine.h"

CTask Logger::Debug(std::string_view customLog)
{
#ifdef _DEBUG
	auto utcTime = std::chrono::system_clock::now();
	auto timePoint = std::chrono::time_point_cast<std::chrono::milliseconds>(utcTime);
	auto datetime = std::format("{0:%F} {0:%T}", timePoint);

	auto logStr = std::format("{} :: [ {} ] {}\n", datetime, LOG_TYPE(LOG_TYPE::DEBUG)._to_string(), customLog);

	co_await AwaitPost{};
	std::cout << logStr;
#else

#endif // _DEBUG
}

CTask Logger::Log(ERROR_LOG err)
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
	co_await AwaitPost{};
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

CTask Logger::Log(ERROR_LOG err, std::string_view _file, int _line)
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
	co_await AwaitPost{};
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

CTask Logger::Log(ERROR_LOG err, std::string_view additionalLog, std::string_view _file, int _line)
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
	co_await AwaitPost{};
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