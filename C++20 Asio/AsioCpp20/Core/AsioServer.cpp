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

bool AsioServer::PrepareAsioServer(int threadCount)
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

void AsioServer::Post(std::function<void()> callback)
{
	// dispatch 는 호출한 쓰레드에서만 관련 함수를 처리하지만 post 는 비어있는 쓰레드 어느곳에서나 함수 처리 가능함
	//m_ioContext.get_executor().post(m_ioContext, callback);	// 빌드 에러 뜨는데 이유를 모르겠음
	// m_ioContext.post(callback);	// 이 함수는 이제 쓰면 안된다고 함

	/*
	* 예제 = https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio/example/cpp03/services/logger_service.hpp
	스택오버플로우 = https://stackoverflow.com/questions/59753391/boost-asio-io-service-vs-io-context
	io_service is deprecated.
	Yes, you should use io_context.Beware that the "old" API is deprecated as well(e.g.io_service.post(), you should use post(io_context, handler)).

		. . .

		io_service->io_context
		io_service.post()->io_context.get_executor().post()
		io_service.dispatch()->io_context.get_executor().dispatch()

		io_service::strand->strand<io_context::executor_type>

	there were also changes to composed operation customization hooks 
	- there are only 2 now - boost::asio::associated_allocator and boost::asio::associated_executor, 
	which default to looking for get_allocator(), get_executor(), T::allocator_type, T::executor_type members of the composed operation function object.

	This is not a complete list.
	*/
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
