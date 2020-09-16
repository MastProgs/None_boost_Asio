#pragma once
#include <include/asio.hpp>

class AsioClient;

class AsioAcceptor : public Object, public std::enable_shared_from_this<AsioAcceptor>
{
public:
	explicit AsioAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e);
	~AsioAcceptor();

	bool StartListen();

private:
	// 생성할 Client 클래스 재정의 필요
	virtual void Listen();

	void HandleAccept(std::shared_ptr<AsioClient> client, const std::error_code& error);

private:
	asio::ip::tcp::acceptor m_acceptor;
	asio::io_context& m_io;

	std::atomic<bool> m_isAlreadyListening{ false };
};

