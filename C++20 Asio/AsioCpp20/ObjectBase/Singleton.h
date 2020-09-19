#pragma once

template <typename T>
class Singleton : public Object
{
protected:
	Singleton() = default;
public:
	virtual ~Singleton() = default;

	Singleton(const Singleton<T>&) noexcept = delete;
	Singleton<T>& operator=(const Singleton<T>&) noexcept = delete;
	Singleton(Singleton<T>&&) noexcept = delete;
	Singleton<T>& operator=(Singleton<T>&&) noexcept = delete;

	static T& Instance()
	{
		static T inst;
		return inst;
	}
};

