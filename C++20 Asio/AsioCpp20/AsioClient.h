#pragma once

#include <include/asio.hpp>

class AsioClient
{
public:
	explicit AsioClient(asio::io_context& io);

	asio::ip::tcp::socket& GetSocket() { return m_socket; }

private:

	asio::ip::tcp::socket m_socket;
};

