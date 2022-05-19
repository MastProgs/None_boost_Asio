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

private:
};