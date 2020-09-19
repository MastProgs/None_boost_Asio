#pragma once

#include <include/asio.hpp>

class AsioAcceptor;

class AsioServer : public Singleton<AsioServer>
{
protected:
	AsioServer();
	friend AsioServer& Singleton<AsioServer>::Instance();

public:
	virtual ~AsioServer();

	bool PrepareAsioServer(int threadCount = 0);
	bool StartListen();

	void Post(std::function<void()> callback);

private:
	bool SetThreadCore(int threadCount = 0);
	bool SetServerIpList();

	// 생성할 Accept 클래스 재정의 필요
	virtual bool SetAcceptor();

private:
	asio::io_context m_ioContext;
	std::shared_ptr<AsioAcceptor> m_acceptor;

	unsigned int m_cpuThreadCount = 0;
	std::vector<std::string> m_ipv4List;
	std::vector<std::string> m_ipv6List;
};