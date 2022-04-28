#include "pch.h"
#include "AsioServer.h"
#include "AsioAcceptor.h"

#include <thread>

AsioContext::AsioContext()
{
}

AsioContext::~AsioContext()
{
}

bool AsioContext::PrepareAsioServer(int threadCount)
{
	if (SetThreadCore(threadCount)
		&& SetServerIpList()
		&& SetAcceptor())
	{
		return false;
	}

	Logger::Inst().Debug("Server Prepare Finished");
	return true;
}

bool AsioContext::StartIOService()
{
	if (StartListen()
		&& StartThreads())
	{
		return true;
	}

	return false;
}

bool AsioContext::StartListen()
{
	return m_acceptor->StartListen();
}


bool AsioContext::StartThreads()
{
	m_threadList.reserve(m_cpuThreadCount);
	for (size_t i = 0; i < m_cpuThreadCount; i++)
	{
		Logger::Inst().Debug(std::format("Thread id no.{} is on", i));
		m_threadList.emplace_back(
			[&, i]() 
			{ 
				m_ioContext.run();
			});
	}

	Logger::Inst().Debug("Process On");

	for (auto& th : m_threadList)
	{
		th.join();
	}

	return true;
}

void AsioContext::Post(std::function<void()> callback)
{
	// dispatch 는 호출한 쓰레드에서만 관련 함수를 처리하지만 post 는 비어있는 쓰레드 어느곳에서나 함수 처리 가능함
	asio::post(m_ioContext, callback);

	/*
	* m_ioContext.post(callback);	// 이 함수는 이제 쓰면 안된다고 함
	* 한국어 설명 : https://zepeh.tistory.com/498
	* 예제 = https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/example/cpp03/services/logger_service.hpp
	* 스택오버플로우 = https://stackoverflow.com/questions/59753391/boost-asio-io-service-vs-io-context
	*/
}

void AsioContext::Dispatch(std::function<void()> callback)
{
	asio::dispatch(m_ioContext, callback);
}

void AsioContext::Stop()
{
	// io_context 와 관련한 run() 상태의 쓰레드들이 모두 return 을 하고, 관련 쓰레드들은 정상 종료를 하게 된다.
	m_ioContext.stop();
}

bool AsioContext::RestartThreads()
{
	if (m_threadList.size() == 0) { return false; }

	for (const auto& th : m_threadList)
	{
		if (th.joinable()) { return false; }
	}

	for (auto& th : m_threadList)
	{
		th.join();
	}

	return true;
}

bool AsioContext::SetThreadCore(int threadCount)
{
	m_cpuThreadCount = threadCount > 0 ? threadCount : std::thread::hardware_concurrency();
	Logger::Inst().Debug(std::format("Thread Count : {}", m_cpuThreadCount));

	return m_cpuThreadCount;
}

bool AsioContext::SetServerIpList()
{
	using asio::ip::tcp;
	tcp::resolver rs(m_ioContext);

	// asio::ip::tcp::resolver::query 는 삭제 대상
	for (const auto& iter : rs.resolve(asio::ip::host_name() , ""))
	{
		auto addr = iter.endpoint().address();
		auto ip = addr.to_string();
		if (addr.is_v4()) { m_ipv4List.emplace_back(ip); }
		else if (addr.is_v6()) { m_ipv6List.emplace_back(ip); }

		Logger::Inst().Debug(std::format("IP = {}", ip));
	}
	
	return (m_ipv4List.size() + m_ipv6List.size()) != 0;
}

bool AsioContext::SetAcceptor()
{
	// 여러 Acceptor 를 쓰는 경우, 포트 번호를 여기서 따로 따로 지정해주면 된다.

	constexpr int PORT_NUMBER = 9000;
	m_acceptor = std::make_shared<GameClientAcceptor>(m_ioContext, asio::ip::tcp::endpoint{ asio::ip::tcp::v4(), PORT_NUMBER });
	Logger::Inst().Debug(std::format("Port = {}", PORT_NUMBER));

	return m_acceptor != nullptr;
}
