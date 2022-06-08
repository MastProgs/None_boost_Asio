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

	TP StrToTP(const std::string& strDt, std::string_view dateForm = "%Y-%m-%d %H:%M:%S")
	{
		std::stringstream ss(strDt);
		TP tp;

		ss >> std::chrono::parse(dateForm.data(), tp);
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
		return start <= periodTp && periodTp < end;
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



class week_hh_mm_ss
{
public:
	week_hh_mm_ss(int week = 0, int hour = 0, int minute = 0, int second = 0)
		: week{ week }, hour{ hour }, minute{ minute }, second{ second }
	{
	};
	~week_hh_mm_ss() {};

	std::chrono::weekday week;
	std::chrono::hours hour;
	std::chrono::minutes minute;
	std::chrono::seconds second;

	int Week() const { return week.c_encoding(); }
	int Hour() const { return hour.count(); }
	int Minute() const { return minute.count(); }
	int Second() const { return second.count(); }

#pragma region OPERATORS
public:
	bool operator==(const week_hh_mm_ss& rhs) const
	{
		if (week.c_encoding() != rhs.week.c_encoding()) { return false; }
		if (hour != rhs.hour) { return false; }
		if (minute != rhs.minute) { return false; }
		if (second != rhs.second) { return false; }
		return true;
	}
	bool operator!=(const week_hh_mm_ss& rhs) const
	{
		if (week.c_encoding() != rhs.week.c_encoding()) { return true; }
		if (hour != rhs.hour) { return true; }
		if (minute != rhs.minute) { return true; }
		if (second != rhs.second) { return true; }
		return false;
	}
	bool operator<(const week_hh_mm_ss& rhs) const
	{
		if (week.c_encoding() < rhs.week.c_encoding()) { return true; }
		if (hour < rhs.hour) { return true; }
		if (minute < rhs.minute) { return true; }
		if (second < rhs.second) { return true; }
		return false;
	}
	bool operator<=(const week_hh_mm_ss& rhs) const
	{
		if (week.c_encoding() < rhs.week.c_encoding()) { return true; }
		if (hour < rhs.hour) { return true; }
		if (minute < rhs.minute) { return true; }
		if (second <= rhs.second) { return true; }
		return false;
	}
	bool operator>(const week_hh_mm_ss& rhs) const
	{
		if (week.c_encoding() > rhs.week.c_encoding()) { return true; }
		if (hour > rhs.hour) { return true; }
		if (minute > rhs.minute) { return true; }
		if (second > rhs.second) { return true; }
		return false;
	}
	bool operator>=(const week_hh_mm_ss& rhs) const
	{
		if (week.c_encoding() > rhs.week.c_encoding()) { return true; }
		if (hour > rhs.hour) { return true; }
		if (minute > rhs.minute) { return true; }
		if (second >= rhs.second) { return true; }
		return false;
	}

	week_hh_mm_ss operator+(const std::chrono::days add)
	{
		auto tmp = *this;
		tmp.week += add;
		return tmp;
	}
	week_hh_mm_ss operator+(const std::chrono::hours add)
	{
		auto tmp = *this;
		tmp.week += std::chrono::floor<std::chrono::days>(add);
		tmp.hour += add % 24;
		return tmp;
	}
	week_hh_mm_ss operator+(const std::chrono::minutes add)
	{
		auto tmp = *this;
		tmp.week += std::chrono::floor<std::chrono::days>(add);
		tmp.hour += std::chrono::floor<std::chrono::hours>(add) % 24;
		tmp.minute += add % 60;
		return tmp;
	}
	week_hh_mm_ss operator+(const std::chrono::seconds add)
	{
		auto tmp = *this;
		tmp.week += std::chrono::floor<std::chrono::days>(add);
		tmp.hour += std::chrono::floor<std::chrono::hours>(add) % 24;
		tmp.minute += std::chrono::floor<std::chrono::minutes>(add) % 60;
		tmp.second += add % 60;
		return tmp;
	}

	week_hh_mm_ss operator+=(const std::chrono::days add)
	{
		return operator+(add);
	}
	week_hh_mm_ss operator+=(const std::chrono::hours add)
	{
		return operator+(add);
	}
	week_hh_mm_ss operator+=(const std::chrono::minutes add)
	{
		return operator+(add);
	}
	week_hh_mm_ss operator+=(const std::chrono::seconds add)
	{
		return operator+(add);
	}

	week_hh_mm_ss& operator-(const std::chrono::days add)
	{
		auto tmp = *this;
		tmp.week -= add;
		return tmp;
	}
	week_hh_mm_ss& operator-(const std::chrono::hours add)
	{
		auto tmp = *this;
		tmp.week -= std::chrono::floor<std::chrono::days>(add);
		tmp.hour -= add % 24;
		return tmp;
	}
	week_hh_mm_ss& operator-(const std::chrono::minutes add)
	{
		auto tmp = *this;
		tmp.week -= std::chrono::floor<std::chrono::days>(add);
		tmp.hour -= std::chrono::floor<std::chrono::hours>(add) % 24;
		tmp.minute -= add % 60;
		return tmp;
	}
	week_hh_mm_ss& operator-(const std::chrono::seconds add)
	{
		auto tmp = *this;
		tmp.week -= std::chrono::floor<std::chrono::days>(add);
		tmp.hour -= std::chrono::floor<std::chrono::hours>(add) % 24;
		tmp.minute -= std::chrono::floor<std::chrono::minutes>(add) % 60;
		tmp.second -= add % 60;
		return tmp;
	}

	week_hh_mm_ss operator-=(const std::chrono::days add)
	{
		return operator-(add);
	}
	week_hh_mm_ss operator-=(const std::chrono::hours add)
	{
		return operator-(add);
	}
	week_hh_mm_ss operator-=(const std::chrono::minutes add)
	{
		return operator-(add);
	}
	week_hh_mm_ss operator-=(const std::chrono::seconds add)
	{
		return operator-(add);
	}


	using TP = std::chrono::system_clock::time_point;
	TP operator+(const TP& tp) const
	{
		auto realTp = GTime::Inst().GetThisWeekTP(tp, week);
		return GTime::Inst().GetDateTP(realTp, Hour(), Minute(), Second());
	}
#pragma endregion OPERATORS

private:

};