#pragma once

#include <include/asio.hpp>

class AsioAcceptor;

class AsioContext : public Singleton<AsioContext>
{
protected:
	AsioContext();
	friend AsioContext& Singleton<AsioContext>::Instance();

public:
	virtual ~AsioContext();

	bool PrepareAsioServer(int threadCount = 0);
	bool StartIOService();

	void Post(std::function<void()> callback);
	void Dispatch(std::function<void()> callback);
	void Stop();
	bool RestartThreads();

	void SetServerShutdown(bool b) { isServerShutdown = b; }
	bool GetServerShutdown() { return isServerShutdown; }
private:
	bool StartListen();
	bool StartThreads();

	bool SetThreadCore(int threadCount = 0);
	bool SetServerIpList();
	bool SetAcceptor();

private:
	asio::io_context m_ioContext;
	std::shared_ptr<AsioAcceptor> m_acceptor;

	std::vector<std::string> m_ipv4List;
	std::vector<std::string> m_ipv6List;

	unsigned int m_cpuThreadCount = 0;
	std::vector<std::thread> m_threadList;

	bool isServerShutdown{ false };
};