#pragma once

namespace
{
	using TP = std::chrono::system_clock::time_point;
}

class GTime : public Singleton<GTime>
{
public:
	GTime()
	{
		for (unsigned int i = 1; i <= 12; i++)
		{
			m_monthList.emplace_back(i);
		}

		for (unsigned int i = 1; i <= 31; i++)
		{
			m_dayList.emplace_back(i);
		}
	};
	virtual ~GTime() = default;

	auto UTC();
	auto LOCAL();
	auto UTC_String(std::string_view dateForm = "%Y-%m-%d %H:%M:%OS");
	auto LOCAL_String(std::string_view dateForm = "%Y-%m-%d %H:%M:%OS");
	TP StrToTP(const std::string& strDt, std::string_view dateForm = "%Y-%m-%d %H:%M:%S");
	std::string TpToStr(const TP& tp, std::string_view dateForm = "%Y-%m-%d %H:%M:%OS");

	// Sunday ~ Saturday, 0 ~ 6
	TP GetWeekTP(std::chrono::weekday findWeek = std::chrono::Sunday, const TP& periodTp = std::chrono::system_clock::now());

	// ex) 2022-10-01 10:15:31 -> 2022-10-01 00:00:00
	TP GetDateTP(const TP& periodTp = std::chrono::system_clock::now(), long long hour = 0, long long min = 0, long long sec = 0);

	auto IsInTime(const TP& periodTp, const TP& start, const TP& end);
	auto Count_Day(const TP& targetTp, const TP& periodTp);
	auto Count_Hour(const TP& targetTp, const TP& periodTp);
	auto Count_Minute(const TP& targetTp, const TP& periodTp);
	auto Count_Second(const TP& targetTp, const TP& periodTp);

	size_t MonthToInt(const std::chrono::month& m);
	size_t DayToInt(const std::chrono::day& d);

	int GetWeek(const TP& targetTp = std::chrono::system_clock::now());

	std::tuple<int, int, int> GetYMD(const TP& targetTp = std::chrono::system_clock::now());
	std::tuple<int, int, int> GetHMS(const TP& targetTp = std::chrono::system_clock::now());
	std::tuple<int, int, int, int, int, int> GetYMDHMS(const TP& targetTp = std::chrono::system_clock::now());
private:
	std::vector<std::chrono::month> m_monthList;
	std::vector<std::chrono::day> m_dayList;

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

	unsigned int Week() const { return week.c_encoding(); }
	int Hour() const { return hour.count(); }
	int Minute() const { return minute.count(); }
	long long Second() const { return second.count(); }
	long long ToDuration() const
	{
		long long w = Week() * 24 * 60 * 60;
		long long h = Hour() * 60 * 60;
		long long m = Minute() * 60;
		long long s = Second();
		return w + h + m + s;
	}

#pragma region OPERATORS
public:
	bool operator==(const week_hh_mm_ss& rhs) const { return ToDuration() == rhs.ToDuration(); }
	bool operator!=(const week_hh_mm_ss& rhs) const { return ToDuration() != rhs.ToDuration(); }
	bool operator<(const week_hh_mm_ss& rhs) const { return ToDuration() < rhs.ToDuration(); }
	bool operator<=(const week_hh_mm_ss& rhs) const { return ToDuration() <= rhs.ToDuration(); }
	bool operator>(const week_hh_mm_ss& rhs) const { return ToDuration() > rhs.ToDuration(); }
	bool operator>=(const week_hh_mm_ss& rhs) const { return ToDuration() >= rhs.ToDuration(); }

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

	week_hh_mm_ss& operator+=(const std::chrono::days add)
	{
		week += add;
		return *this;
	}
	week_hh_mm_ss& operator+=(const std::chrono::hours add)
	{
		week += std::chrono::floor<std::chrono::days>(add);
		hour += add % 24;
		return *this;
	}
	week_hh_mm_ss& operator+=(const std::chrono::minutes add)
	{
		week += std::chrono::floor<std::chrono::days>(add);
		hour += std::chrono::floor<std::chrono::hours>(add) % 24;
		minute += add % 60;
		return *this;
	}
	week_hh_mm_ss& operator+=(const std::chrono::seconds add)
	{
		week += std::chrono::floor<std::chrono::days>(add);
		hour += std::chrono::floor<std::chrono::hours>(add) % 24;
		minute += std::chrono::floor<std::chrono::minutes>(add) % 60;
		second += add % 60;
		return *this;
	}

	week_hh_mm_ss operator-(const std::chrono::days add)
	{
		auto tmp = *this;
		tmp.week -= add;
		return tmp;
	}
	week_hh_mm_ss operator-(const std::chrono::hours add)
	{
		auto tmp = *this;
		tmp.week -= std::chrono::floor<std::chrono::days>(add);
		tmp.hour -= add % 24;
		return tmp;
	}
	week_hh_mm_ss operator-(const std::chrono::minutes add)
	{
		auto tmp = *this;
		tmp.week -= std::chrono::floor<std::chrono::days>(add);
		tmp.hour -= std::chrono::floor<std::chrono::hours>(add) % 24;
		tmp.minute -= add % 60;
		return tmp;
	}
	week_hh_mm_ss operator-(const std::chrono::seconds add)
	{
		auto tmp = *this;
		tmp.week -= std::chrono::floor<std::chrono::days>(add);
		tmp.hour -= std::chrono::floor<std::chrono::hours>(add) % 24;
		tmp.minute -= std::chrono::floor<std::chrono::minutes>(add) % 60;
		tmp.second -= add % 60;
		return tmp;
	}

	week_hh_mm_ss& operator-=(const std::chrono::days add)
	{
		week -= add;
		return *this;
	}
	week_hh_mm_ss& operator-=(const std::chrono::hours add)
	{
		week -= std::chrono::floor<std::chrono::days>(add);
		hour -= add % 24;
		return *this;
	}
	week_hh_mm_ss& operator-=(const std::chrono::minutes add)
	{
		week -= std::chrono::floor<std::chrono::days>(add);
		hour -= std::chrono::floor<std::chrono::hours>(add) % 24;
		minute -= add % 60;
		return *this;
	}
	week_hh_mm_ss& operator-=(const std::chrono::seconds add)
	{
		week -= std::chrono::floor<std::chrono::days>(add);
		hour -= std::chrono::floor<std::chrono::hours>(add) % 24;
		minute -= std::chrono::floor<std::chrono::minutes>(add) % 60;
		second -= add % 60;
		return *this;
	}

	TP operator+(const TP& tp) const
	{
		TP realTp = GTime::Inst().GetWeekTP(week, tp);
		return GTime::Inst().GetDateTP(realTp, Hour(), Minute(), Second());
	}
#pragma endregion OPERATORS


private:

};