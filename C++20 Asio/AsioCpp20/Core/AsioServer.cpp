#include "stdafx.h"
#include "AsioServer.h"
#include "AsioAcceptor.h"

#include <thread>

AsioServer::AsioServer()
{
}

AsioServer::~AsioServer()
{
}

bool AsioServer::Init_AsioServer(int threadCount)
{
	if (SetThreadCore(threadCount)
		&& SetServerIpList()
		&& SetAcceptor())
	{
		return false;
	}

	return true;
}

bool AsioServer::StartListen()
{
	return m_acceptor->StartListen();
}

bool AsioServer::SetThreadCore(int threadCount)
{
	m_cpuThreadCount = threadCount > 0 ? threadCount : std::thread::hardware_concurrency();
	return m_cpuThreadCount;
}

bool AsioServer::SetServerIpList()
{
	using asio::ip::tcp;
	tcp::resolver rs(m_ioContext);
	for (const auto& iter : rs.resolve(tcp::resolver::query{ asio::ip::host_name(), "" }))
	{
		auto addr = iter.endpoint().address();
		if (addr.is_v4()) { m_ipv4List.emplace_back(addr.to_string()); }
		else if (addr.is_v6()) { m_ipv6List.emplace_back(addr.to_string()); }
	}
	
	return m_ipv4List.size() + m_ipv6List.size();
}

bool AsioServer::SetAcceptor()
{
	constexpr int PORT_NUMBER = 10001;
	m_acceptor = std::make_shared<AsioAcceptor>(m_ioContext, asio::ip::tcp::endpoint{ asio::ip::tcp::v4(), PORT_NUMBER });

	return m_acceptor != nullptr;
}
