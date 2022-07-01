#pragma once

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
		auto idx = Dice(0, vec.size() - 1);
		return vec[idx];
	}

public:
	bool BDice(int percent, bool expect, int maxP)
	{
		percent = ValidCheck(percent, maxP);
		auto fixP = expect == false ? percent : (maxP - percent) * -1;
		auto res = Dice(maxP);
		return percent < res;
	}

	bool BDice100(int percent, bool expect = false) { return BDice(percent, expect, 100); }
	bool BDice10000(int percent, bool expect = false) { return BDice(percent, expect, 10000); }

private:
	int ValidCheck(int val, int max, int min = 0)
	{
		val = max < val ? max : val;
		val = val < min ? min : val;
		return val;
	}
};