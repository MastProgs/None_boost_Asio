#include <include/asio.hpp>
#include <iostream>
#include <string>
#include <array>

const std::string SERVER_IP{ "127.0.0.1" };
constexpr int PORT_NUMBER{ 9000 };

class TCP_Client
{
public:
	TCP_Client(asio::io_service& io_service) : m_io_service{ io_service }, m_Socket{ io_service }, m_nSeqNumber{ 0 }{};
	~TCP_Client() {};

	void Connect(asio::ip::tcp::endpoint& endpoint)
	{
		m_Socket.async_connect(endpoint, std::bind(&TCP_Client::handle_connect, this, std::placeholders::_1));
	}

private:
	void PostWrite()
	{
		if (m_Socket.is_open() == false) { return; }

		if (m_nSeqNumber > 7)
		{
			m_Socket.close();
			return;
		}

		++m_nSeqNumber;

		char szMessage[128]{ 0 };
		sprintf_s(szMessage, 128 - 1, "%d - Send Message", m_nSeqNumber);

		m_writeMessage = szMessage;

		asio::async_write(m_Socket, 
			asio::buffer(m_writeMessage), 
			std::bind(&TCP_Client::handle_write, this, std::placeholders::_1, std::placeholders::_2));

		PostReceive();
	}

	void PostReceive()
	{
		memset(&m_ReceiveBuffer, '\0', sizeof(m_ReceiveBuffer));

		m_Socket.async_read_some(asio::buffer(m_ReceiveBuffer),
			std::bind(&TCP_Client::handle_receive, this, std::placeholders::_1, std::placeholders::_2));
	}

	void handle_connect(const asio::error_code& error)
	{
		if (error)
		{
			std::cout << "Connect Failed : " << error.message() << std::endl;
		}
		else
		{
			std::cout << "Connected" << std::endl;
			PostWrite();
		}
	}

	void handle_write(const asio::error_code& /*error*/, size_t /*bytes_transferred*/)
	{

	}

	void handle_receive(const asio::error_code& error, size_t bytes_transferred)
	{
		if (error)
		{
			if (error == asio::error::eof)
			{
				std::cout << "서버와 연결이 끊어졌습니다." << std::endl;
			}
			else
			{
				std::cout << "Error No." << error.value() << ", Error Message : " << error.message() << std::endl;
			}
		}
		else
		{
			const std::string strRecvMessage = m_ReceiveBuffer.data();
			std::cout << "서버에서 받은 메세지 : " << strRecvMessage << ", 받은 크기 : " << bytes_transferred << std::endl;

			PostWrite();
		}
	}

private:
	asio::io_service& m_io_service;
	asio::ip::tcp::socket m_Socket;
	int m_nSeqNumber;
	std::array<char, 128> m_ReceiveBuffer;
	std::string m_writeMessage;
};


int main()
{
    asio::io_service io_service;
	asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(SERVER_IP), PORT_NUMBER);

	TCP_Client client(io_service);
	client.Connect(endpoint);
	io_service.run();

	std::cout << "네트워크 접속 종료" << std::endl;

	return 0;
}
