#pragma once

namespace
{
	constexpr std::chrono::weekday Sunday{ 0 };
	constexpr std::chrono::weekday Monday{ 1 };
	constexpr std::chrono::weekday Tuesday{ 2 };
	constexpr std::chrono::weekday Wednesday{ 3 };
	constexpr std::chrono::weekday Thursday{ 4 };
	constexpr std::chrono::weekday Friday{ 5 };
	constexpr std::chrono::weekday Saturday{ 6 };
}

class GTime : public Singleton<GTime>
{
public:
	GTime() = default;
	virtual ~GTime() = default;

	using TP = std::chrono::system_clock::time_point;

	auto NowUtc()
	{
		return std::chrono::system_clock::now();
	}

	auto NowLocal()
	{
		return std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	}

	auto NowUtc_String(std::string_view dateForm = "%Y-%m-%d %H:%M:%OS")
	{
		return std::format("{:" + std::string{ dateForm } + "}", NowUtc());
	}

	auto NowLocal_String(std::string_view dateForm = "%Y-%m-%d %H:%M:%OS")
	{
		return std::format("{:" + std::string{ dateForm } + "}", NowLocal());
	}

	std::optional<TP> StrToTP(const std::string& strDt, std::string_view dateForm = "%Y-%m-%d %H:%M:%S")
	{
		std::stringstream ss(strDt);
		TP tp;

		ss >> std::chrono::parse(dateForm.data(), tp);
		if (false == ss.fail()) { return std::nullopt; }

		return tp;
	}

	// Sunday ~ Saturday, 0 ~ 6
	auto GetThisWeekTP(const TP& periodTp, std::chrono::weekday findWeek = Sunday)
	{
		auto one_day_hour = std::chrono::hours{ 24 };

		auto dt = std::chrono::floor<std::chrono::days>(periodTp);
		std::chrono::year_month_day ymd{ dt };
		std::chrono::weekday cur_week{ std::chrono::sys_days{ ymd } };

		auto day_gap = int(findWeek.c_encoding()) - int(cur_week.c_encoding());
		auto fin = periodTp + (day_gap * one_day_hour);
		return fin;
	}

	// ex) 2022-10-01 10:15:31 -> 2022-10-01 00:00:00
	auto GetDateTP(const TP& periodTp, int hour = 0, int min = 0, int sec = 0)
	{
		auto dt = std::chrono::floor<std::chrono::days>(periodTp);
		std::chrono::year_month_day ymd{ dt };
		TP tp = std::chrono::sys_days{ ymd };
		tp += std::chrono::hours{ hour };
		tp += std::chrono::minutes{ min };
		tp += std::chrono::seconds{ sec };

		return tp;
	}

	auto IsInTime(const TP& periodTp, const TP& start, const TP& end)
	{
		return start <= periodTp && periodTp <= end;
	}

	auto PeriodDay(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return (hms.hours() / 24).count();
	}

	auto PeriodHour(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return hms.hours().count();
	}

	auto PeriodMinute(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return hms.minutes().count();
	}

	auto PeriodSecond(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return hms.seconds().count();
	}

private:
};