#include "pch.h"
#include "Datetime.h"
#include <chrono>

auto GTime::UTC()
{
	return std::chrono::system_clock::now();
}

auto GTime::LOCAL()
{
	return std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}

std::string GTime::UTC_String(std::string_view dateForm)
{
	return Format("{:" + std::string{ dateForm } + "}", UTC());
}

std::string GTime::LOCAL_String(std::string_view dateForm)
{
	return Format("{:" + std::string{ dateForm } + "}", LOCAL());
}

TP GTime::StrToTP(const std::string& strDt, std::string_view dateForm)
{
	std::stringstream ss(strDt);
	TP tp;

	std::chrono::from_stream(ss, dateForm.data(), tp);
	return tp;
}

std::string GTime::TpToStr(const TP& tp, std::string_view dateForm)
{
	return Format("{:" + std::string{ dateForm } + "}", tp);
}

// Sunday ~ Saturday, 0 ~ 6
TP GTime::GetWeekTP(std::chrono::weekday findWeek, const TP& periodTp)
{
	auto one_day_hour = std::chrono::hours{ 24 };

	auto dt = std::chrono::floor<std::chrono::days>(periodTp);
	std::chrono::year_month_day ymd{ dt };
	std::chrono::weekday cur_week{ std::chrono::sys_days{ ymd } };

	auto day_gap = findWeek.c_encoding() - cur_week.c_encoding();
	auto fin = periodTp + (day_gap * one_day_hour);
	return fin;
}

// ex) 2022-10-01 10:15:31 -> 2022-10-01 00:00:00
TP GTime::GetDateTP(const TP& periodTp, long long hour, long long min, long long sec)
{
	auto dt = std::chrono::floor<std::chrono::days>(periodTp);
	std::chrono::year_month_day ymd{ dt };
	TP tp = std::chrono::sys_days{ ymd };
	tp += std::chrono::hours{ hour };
	tp += std::chrono::minutes{ min };
	tp += std::chrono::seconds{ sec };

	return tp;
}

auto GTime::IsInTime(const TP& periodTp, const TP& start, const TP& end)
{
	return start <= periodTp && periodTp < end;
}

auto GTime::Count_Day(const TP& targetTp, const TP& periodTp)
{
	auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
	std::chrono::hh_mm_ss hms{ ht };
	return (hms.hours() / 24).count();
}

auto GTime::Count_Hour(const TP& targetTp, const TP& periodTp)
{
	auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
	std::chrono::hh_mm_ss hms{ ht };
	return hms.hours().count();
}

auto GTime::Count_Minute(const TP& targetTp, const TP& periodTp)
{
	auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
	std::chrono::hh_mm_ss hms{ ht };
	return hms.minutes().count();
}

auto GTime::Count_Second(const TP& targetTp, const TP& periodTp)
{
	auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
	std::chrono::hh_mm_ss hms{ ht };
	return hms.seconds().count();
}

size_t GTime::MonthToInt(const std::chrono::month& m)
{
	for (size_t i = 0; i < m_monthList.size(); i++)
	{
		if (m_monthList[i] == m) { return i + 1; }
	}
	return 0;
}

size_t GTime::DayToInt(const std::chrono::day& d)
{
	for (size_t i = 0; i < m_dayList.size(); i++)
	{
		if (m_dayList[i] == d) { return i + 1; }
	}
	return 0;
}

int GTime::GetWeek(const TP& targetTp)
{
	std::chrono::year_month_weekday ymw{ std::chrono::floor<std::chrono::days>(targetTp) };
	return ymw.weekday().c_encoding();
}

std::tuple<int, int, int> GTime::GetYMD(const TP& targetTp)
{
	std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(targetTp) };
	return { int(ymd.year()), MonthToInt(ymd.month()), DayToInt(ymd.day()) };
}

std::tuple<int, int, int> GTime::GetHMS(const TP& targetTp)
{
	auto dp = std::chrono::floor<std::chrono::days>(targetTp);
	std::chrono::hh_mm_ss hms{ targetTp - dp };
	return { hms.hours().count(), hms.minutes().count(), hms.seconds().count() };
}

std::tuple<int, int, int, int, int, int> GTime::GetYMDHMS(const TP& targetTp)
{
	auto dp = std::chrono::floor<std::chrono::days>(targetTp);
	std::chrono::year_month_day ymd{ dp };
	std::chrono::hh_mm_ss hms{ targetTp - dp };
	return { int(ymd.year()), MonthToInt(ymd.month()), DayToInt(ymd.day()), hms.hours().count(), hms.minutes().count(), hms.seconds().count() };
}