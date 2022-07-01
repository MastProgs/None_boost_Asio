#pragma once

class Rand : public Singleton<Rand>
{
public:
	Rand() {};
	~Rand() {};

	int Get(int end)
	{
		return Get(0, end);
	};

	int Get(int start, int end)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(start, end);

		return dis(gen);
	};

	template<typename T>
	T& Get(std::vector<T>& vec)
	{
		auto idx = Get(0, vec.size() - 1);
		return vec[idx];
	}

	bool BoolDice(int percent, bool expect, int maxP)
	{
		percent = ValidCheck(percent, maxP);
		auto fixP = expect == false ? percent : (maxP - percent) * -1;
		auto res = Get(maxP);
		return percent < res;
	}

	bool BoolDice100(int percent, bool expect = false, int maxP = 100)
	{
		return BoolDice(percent, expect, maxP);
	}

	bool BoolDice10000(int percent, bool expect = false, int maxP = 10000)
	{
		return BoolDice(percent, expect, maxP);
	}
private:
	int ValidCheck(int val, int max, int min = 0)
	{
		val = max < val ? max : val;
		val = val < min ? min : val;
		return val;
	}
};