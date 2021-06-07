#include "stdafx.h"
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
		m_threadList.emplace_back([&, i]() { m_ioContext.run(); });
	}

	for (auto& th : m_threadList)
	{
		th.join();
	}

	return true;
}

void AsioContext::Post(std::function<void()> callback)
{
	// dispatch �� ȣ���� �����忡���� ���� �Լ��� ó�������� post �� ����ִ� ������ ����������� �Լ� ó�� ������
	asio::post(m_ioContext, callback);

	/*
	* m_ioContext.post(callback);	// �� �Լ��� ���� ���� �ȵȴٰ� ��
	* �ѱ��� ���� : https://zepeh.tistory.com/498
	* ���� = https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/example/cpp03/services/logger_service.hpp
	* ���ÿ����÷ο� = https://stackoverflow.com/questions/59753391/boost-asio-io-service-vs-io-context
	*/
}

void AsioContext::Dispatch(std::function<void()> callback)
{
	asio::dispatch(m_ioContext, callback);
}

void AsioContext::Stop()
{
	// io_context �� ������ run() ������ ��������� ��� return �� �ϰ�, ���� ��������� ���� ���Ḧ �ϰ� �ȴ�.
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
	return m_cpuThreadCount;
}

bool AsioContext::SetServerIpList()
{
	using asio::ip::tcp;
	tcp::resolver rs(m_ioContext);

	// asio::ip::tcp::resolver::query �� ���� ���
	for (const auto& iter : rs.resolve(asio::ip::host_name() , ""))
	{
		auto addr = iter.endpoint().address();
		if (addr.is_v4()) { m_ipv4List.emplace_back(addr.to_string()); }
		else if (addr.is_v6()) { m_ipv6List.emplace_back(addr.to_string()); }
	}
	
	return (m_ipv4List.size() + m_ipv6List.size()) != 0;
}

bool AsioContext::SetAcceptor()
{
	// ���� Acceptor �� ���� ���, ��Ʈ ��ȣ�� ���⼭ ���� ���� �������ָ� �ȴ�.

	constexpr int PORT_NUMBER = 9000;
	m_acceptor = std::make_shared<GameClientAcceptor>(m_ioContext, asio::ip::tcp::endpoint{ asio::ip::tcp::v4(), PORT_NUMBER });

	return m_acceptor != nullptr;
}
