#pragma once

#include "AsioServer.h"

// C++20 Coroutine
/// https://en.cppreference.com/w/cpp/coroutine/coroutine_handle
/// https://kukuta.tistory.com/222
/// https://openmynotepad.tistory.com/65

class Task
{
public:
	Task() {};
	~Task() {};

private:

};

// Not Safe in Multi Threads
class CoTaskHandler : public Singleton<CoTaskHandler>
{
public:
	CoTaskHandler() {};
	~CoTaskHandler() {};

	void Stop() { isRun = false; }
	void AddTask(const std::coroutine_handle<>& handle) { m_que.emplace(handle); }

	void Run()
	{
		do
		{
			if (false == isRun) { return; }
			if (m_que.empty()) { break; }

			auto& handle = m_que.front();
			m_que.pop();
			if (handle.done() == false) { handle.resume(); }

		} while (true);
	}

private:
	bool isRun{ true };
	// std::mutex m_lock;

	std::queue<std::coroutine_handle<>> m_que;
};

template <typename T>
class RTask : public Task
{
public:
	struct promise_type;
	using CoHandle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		RTask get_return_object() noexcept
		{
			return RTask{ CoHandle::from_promise(*this) };
		}
		auto initial_suspend() noexcept { return std::suspend_always{}; }
		auto final_suspend() noexcept { return std::suspend_always{}; }
		void unhandled_exception() { std::exit(1); }

		// return_void() or return_value( value ) => only select one
		// auto return_void() { return std::suspend_never{}; }
		// custom for co_return
		void return_value(T v) noexcept { this->value = v; }

		// custom for co_yield
		auto yield_value(T v) noexcept
		{
			this->value = v;
			return std::suspend_always{};
		}
		T value;
	};

	RTask(CoHandle&& handle) : coHandle(std::make_shared<CoHandle>(handle)) { Run(); }
	RTask(RTask&) = delete;
	RTask(RTask&& rhl) : coHandle(rhl.coHandle) {};
	~RTask() {}

	auto LastReturn() { return coHandle->promise().value; }
	void Run() { coHandle->resume(); }
	bool IsFinish() { return coHandle->done(); }
private:
	std::shared_ptr<CoHandle> coHandle;
};

class VTask : public Task
{
public:
	struct promise_type;
	using CoHandle = std::coroutine_handle<promise_type>;

	struct promise_type
	{
		VTask get_return_object() noexcept
		{
			return VTask{ CoHandle::from_promise(*this) };
		}
		auto initial_suspend() noexcept { return std::suspend_always{}; }
		auto final_suspend() noexcept { return std::suspend_always{}; }
		void unhandled_exception() { std::exit(1); }
		void return_void() { return; }
	};

	VTask(CoHandle&& handle) : coHandle(std::make_shared<CoHandle>(handle)) { Run(); }
	VTask(VTask&) = delete;
	VTask(VTask&& rhl) noexcept : coHandle(rhl.coHandle) {};
	~VTask() {}

	void Run() { coHandle->resume(); }
	bool IsFinish() { return coHandle->done(); }
private:
	std::shared_ptr<CoHandle> coHandle;
};

struct AwaitPost
{
	constexpr bool await_ready() const noexcept { return false; }
	void await_suspend(std::coroutine_handle<> handle) const noexcept
	{
		if (false == handle.done())
		{
			AsioContext::Inst().Post(
				[handle]() -> void
				{
					if (false == handle.done()) { handle.resume(); }
				});
		}
	}
	constexpr void await_resume() const noexcept {}
};
