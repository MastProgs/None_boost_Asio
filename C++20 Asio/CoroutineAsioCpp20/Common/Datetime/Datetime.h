#pragma once

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

	using TP = std::chrono::system_clock::time_point;

	auto UTC()
	{
		return std::chrono::system_clock::now();
	}

	auto LOCAL()
	{
		return std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
	}

	auto UTC_String(std::string_view dateForm = "%Y-%m-%d %H:%M:%OS")
	{
		return std::format("{:" + std::string{ dateForm } + "}", UTC());
	}

	auto LOCAL_String(std::string_view dateForm = "%Y-%m-%d %H:%M:%OS")
	{
		return std::format("{:" + std::string{ dateForm } + "}", LOCAL());
	}

	TP StrToTP(const std::string& strDt, std::string_view dateForm = "%Y-%m-%d %H:%M:%S")
	{
		std::stringstream ss(strDt);
		TP tp;

		std::chrono::from_stream(ss, dateForm.data(), tp);
		return tp;
	}

	std::string TpToStr(const TP& tp, std::string_view dateForm = "%Y-%m-%d %H:%M:%OS")
	{
		return std::format("{:" + std::string{ dateForm } + "}", tp);
	}

	// Sunday ~ Saturday, 0 ~ 6
	auto GetWeekTP(std::chrono::weekday findWeek = std::chrono::Sunday, const TP& periodTp = std::chrono::system_clock::now())
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
	auto GetDateTP(const TP& periodTp = std::chrono::system_clock::now(), int hour = 0, int min = 0, int sec = 0)
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

	auto Count_Day(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return (hms.hours() / 24).count();
	}

	auto Count_Hour(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return hms.hours().count();
	}

	auto Count_Minute(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return hms.minutes().count();
	}

	auto Count_Second(const TP& targetTp, const TP& periodTp)
	{
		auto ht = std::chrono::floor<std::chrono::milliseconds>(periodTp - targetTp);
		std::chrono::hh_mm_ss hms{ ht };
		return hms.seconds().count();
	}

	int MonthToInt(const std::chrono::month& m)
	{
		for (size_t i = 0; i < m_monthList.size(); i++)
		{
			if (m_monthList[i] == m) { return i + 1; }
		}
		return 0;
	}

	int DayToInt(const std::chrono::day& d)
	{
		for (size_t i = 0; i < m_dayList.size(); i++)
		{
			if (m_dayList[i] == d) { return i + 1; }
		}
		return 0;
	}

	int GetWeek(const TP& targetTp = std::chrono::system_clock::now())
	{
		std::chrono::year_month_weekday ymw{ std::chrono::floor<std::chrono::days>(targetTp) };
		return ymw.weekday().c_encoding();
	}

	std::tuple<int, int, int> GetYMD(const TP& targetTp = std::chrono::system_clock::now())
	{
		std::chrono::year_month_day ymd{ std::chrono::floor<std::chrono::days>(targetTp) };
		return { int(ymd.year()), MonthToInt(ymd.month()), DayToInt(ymd.day()) };
	}

	std::tuple<int, int, int> GetHMS(const TP& targetTp = std::chrono::system_clock::now())
	{
		auto dp = std::chrono::floor<std::chrono::days>(targetTp);
		std::chrono::hh_mm_ss hms{ targetTp - dp };
		return { hms.hours().count(), hms.minutes().count(), hms.seconds().count() };
	}

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

	int Week() const { return week.c_encoding(); }
	int Hour() const { return hour.count(); }
	int Minute() const { return minute.count(); }
	int Second() const { return second.count(); }
	int ToDuration() const
	{
		int w = Week() * 24 * 60 * 60;
		int h = Hour() * 60 * 60;
		int m = Minute() * 60;
		int s = Second();
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


	using TP = std::chrono::system_clock::time_point;
	TP operator+(const TP& tp) const
	{
		auto realTp = GTime::Inst().GetWeekTP(week, tp);
		return GTime::Inst().GetDateTP(realTp, Hour(), Minute(), Second());
	}
#pragma endregion OPERATORS


private:

};