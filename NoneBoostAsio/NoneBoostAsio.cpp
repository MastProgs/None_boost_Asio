#include <iostream>
#include <string>
#include <include/asio.hpp>

class TcpConnection
	: public std::enable_shared_from_this<TcpConnection>
{
public:
	asio::ip::tcp::socket& GetSocket()
	{
		return m_socket;
	}

	void Start()
	{
		// 접속하면 그냥 날짜 문자열을 버퍼에 담아서 송신해주는 일만 함
		auto MakeDaytimeString = []() 
		{
			time_t now = time(0);
			static char buf[100];
			auto errorCode = ctime_s(buf, 100, &now);
			return std::string{ buf };
		};

		m_message = MakeDaytimeString();

		asio::async_write(m_socket, asio::buffer(m_message),
			[me = shared_from_this()](const asio::error_code& error, size_t bytes_transferred) 
		{
			me->HandleWrite(error, bytes_transferred);
		});

			//std::bind(&TcpConnection::HandleWrite, shared_from_this(),
			//	std::placeholders::_1,
			//	std::placeholders::_2));
	}

	TcpConnection(asio::io_context& ioContext)
		: m_socket(ioContext)
	{
	}

private:
	void HandleWrite(const asio::error_code& /*error*/,	size_t /*bytes_transferred*/)
	{
		// 클라로 송신한 이후 처리할 게 있는 경우
	}

	asio::ip::tcp::socket m_socket;
	std::string m_message;
};

// 사실상 Acceptor 임
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
		// 글로벌로 newConnection 소켓만 고유의 id 값을 줘서 잘 관리해주면 클라이언트와의 소통은 처리됨
		std::shared_ptr<TcpConnection> newConnection = std::make_shared<TcpConnection>(m_ioContext);

		m_acceptor.async_accept(newConnection->GetSocket(),
			[newConnect = newConnection, me = this](const asio::error_code& error)
			{
				me->HandleAccept(newConnect, error);
			});

			//std::bind(&TcpServer::HandleAccept,
			//	this,
			//	newConnection,
			//	std::placeholders::_1));
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
		asio::io_context ioContext;

		{
			// Check My Ip
			asio::ip::tcp::resolver resolver(ioContext);
			asio::ip::tcp::resolver::query query(asio::ip::host_name(), "");
			asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);

			while (iter != asio::ip::tcp::resolver::iterator())
			{
				asio::ip::address addr = (iter++)->endpoint().address();
				if (!addr.is_v6()) { std::cout << "IPv4 address : " << addr.to_string() << "\n"; }
				else if (addr.is_v6()) { std::cout << "IPv6 address : " << addr.to_string() << "\n"; }
			}
		}
		{
			// Check CPU Core
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			int cpuCore = static_cast<int>(si.dwNumberOfProcessors) * 2;
			printf("CPU Core Count = %d, threads = %d\n", cpuCore / 2, cpuCore);
		}

		// 하나의 ioContext에서 쓰레드 별로 run 을 실행시켜주면, 여러 쓰레드에서 읽고 쓰기가 가능해진다
		TcpServer server(ioContext);
		ioContext.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}