#include <iostream>
#include <string>
#include <include/asio.hpp>

class TcpConnection
	: public std::enable_shared_from_this<TcpConnection>
{
public:
	static std::shared_ptr<TcpConnection> Create(asio::io_context& ioContext)
	{
		return std::shared_ptr<TcpConnection>(new TcpConnection(ioContext));
	}

	asio::ip::tcp::socket& socket()
	{
		return m_socket;
	}

	void Start()
	{
		auto MakeDaytimeString = []() 
		{
			time_t now = time(0);
			static char buf[100];
			auto errorCode = ctime_s(buf, 100, &now);
			return std::string{ buf };
		};

		m_message = MakeDaytimeString();

		asio::async_write(m_socket, asio::buffer(m_message),
			std::bind(&TcpConnection::HandleWrite, shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2));
	}

private:
	TcpConnection(asio::io_context& io_context)
		: m_socket(io_context)
	{
	}

	void HandleWrite(const asio::error_code& /*error*/,	size_t /*bytes_transferred*/)
	{
	}

	asio::ip::tcp::socket m_socket;
	std::string m_message;
};

class TcpServer
{
public:
	TcpServer(asio::io_context& ioContext)
		: m_ioContext(ioContext),
		m_acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13))
	{
		StartAccept();
	}

private:
	void StartAccept()
	{
		std::shared_ptr<TcpConnection> newConnection = TcpConnection::Create(m_ioContext);

		m_acceptor.async_accept(newConnection->socket(),
			std::bind(&TcpServer::HandleAccept,
				this,
				newConnection,
				std::placeholders::_1));
	}

	void HandleAccept(std::shared_ptr<TcpConnection> newConnection, const asio::error_code& error)
	{
		if (!error)
		{
			newConnection->Start();
		}

		StartAccept();
	}

	asio::io_context& m_ioContext;
	asio::ip::tcp::acceptor m_acceptor;
};

int main()
{
	try
	{
		asio::io_context io_context;
		TcpServer server(io_context);
		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}