#pragma once

template <typename Data>
class RandItem
{
public:
	RandItem(int per, const Data& d) : percent{ per }, data{ d } {};

	int percent;
	Data data;
};

class Rand : public Singleton<Rand>
{
public:
	Rand() {};
	~Rand() {};

	int Dice(int end) { return Dice(0, end); };
	int Dice(int start, int end)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(start, end);
		return dis(gen);
	};

	template<typename T>
	T& Dice(std::vector<T>& vec)
	{
		auto idx = Dice(vec.size() - 1);
		return vec[idx];
	}
public:
	template<typename T>
	T* ItemDice(std::vector<RandItem<T>>& vec, int maxPercent)
	{
		if (vec.size() < 1) { return nullptr; }

		int end = 0;
		for (const auto& e : vec) { end += e.percent; }
		if (end != maxPercent) { return nullptr; }

		end = 0;
		int start = 0;
		auto randVal = Dice(maxPercent - 1);
		for (auto& e : vec)
		{
			end += e.percent;
			if (start <= randVal && randVal < end) { return &e.data; }
			start = end;
		}

		return nullptr;
	}


public:
	bool BDice(int percent, bool expect, int maxPer)
	{
		percent = ValidCheck(percent, maxPer);
		auto fixP = expect == false ? percent : (maxPer - percent) * -1;
		auto res = Dice(maxPer);
		return percent < res;
	}

	bool BDice100(int percent, bool expect = true) { return BDice(percent, expect, 100); }
	bool BDice10000(int percent, bool expect = true) { return BDice(percent, expect, 10000); }

private:
	int ValidCheck(int val, int max, int min = 0)
	{
		val = max < val ? max : val;
		val = val < min ? min : val;
		return val;
	}
};