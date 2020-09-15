#pragma once
#include <include/asio.hpp>

class AsioClient;

class AsioAcceptor
{
public:
	explicit AsioAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e);
	~AsioAcceptor();

	bool StartListen();

private:
	void Listen();
	void HandleAccept(std::shared_ptr<AsioClient> client, const asio::error_code& error);

private:
	asio::ip::tcp::acceptor m_acceptor;
	asio::io_context& m_io;

	std::atomic<bool> m_isAlreadyListening{ false };
};

