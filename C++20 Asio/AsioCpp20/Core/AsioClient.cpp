#include "stdafx.h"
#include "AsioClient.h"
#include <Packet/PacketHandler.h>
#include "../../Protocol/Protocol.h"

namespace
{
	// https://dens.website/tutorials/cpp-asio/buffer-sequence
	// std::string first = "Hello Boost.Asio!";
	// char second[15] = { 0 };
	// char const* pointer = get_data();
	// std::size_t size = get_size();
	// 
	// std::array<asio::const_buffer, 3> sequence{ first, second, io::buffer(pointer, size) };
	// 
	// // Now we can pass this sequence into I/O function:
	// io::async_write(socket, sequence, handler);

	//template <typename... Args>
	//auto const_sequence(Args&&... args)
	//{
	//	return std::array<asio::const_buffer, sizeof...(Args)>{asio::buffer(args)...};
	//}
	//
	//template <typename... Args>
	//auto sequence(Args&&... args)
	//{
	//	return std::array<asio::mutable_buffer, sizeof...(Args)>{asio::buffer(args)...};
	//}

	// 사용 예시
	// io::async_write(socket, const_sequence(first, second, io::buffer(pointer, size)), handler);
}

AsioClient::AsioClient(asio::io_context& io)
	: m_socket{ io }
{
}

bool AsioClient::Init()
{
	// Socket option for disabling the Nagle algorithm.
	asio::ip::tcp::no_delay option{ true };
	m_socket.set_option(option);

	return true;
}

void AsioClient::TEST_SampleResponse()
{
	asio::async_write(m_socket, asio::buffer("Hello Asio C++ World"),
		[](const std::error_code& error, size_t bytes_transferred)
	{
		UNREFERENCED_PARAMETER(error);
		UNREFERENCED_PARAMETER(bytes_transferred);
	});
}

void AsioClient::SendPacket(const std::string_view& packet, std::function<void()> callback)
{
	asio::async_write(m_socket, asio::buffer(packet),
		[self = shared_from_this(), p = std::string{ packet }, func = callback](const std::error_code& error, size_t bytes_transferred)
	{
		self->PostSendPacket(error, bytes_transferred, p, func);
	});
}

void AsioClient::RecvPacket()
{
	asio::async_read_until(m_socket, asio::dynamic_buffer(m_recvBuffer, PACKET_MAX_SIZE), PACKET_DELIMITER, [self = shared_from_this(), sock = &m_socket, pBuf = &m_recvBuffer](const std::error_code& error, size_t bytes_transferred)
	{
		UNREFERENCED_PARAMETER(bytes_transferred);

		if (error)
		{
			if (error.value() == asio::error::operation_aborted)
			{
				return;
			}

			if (error.value() == asio::error::eof)
			{
				// client 가 연결 종료
				auto ep = sock->remote_endpoint();
				std::cout << std::format("Client Disonnected :: IP = {}, Port = {}\n", ep.address().to_string(), ep.port());

				sock->shutdown(sock->shutdown_both);
				sock->close();

				return;
			}

			return;
		}

		PacketHandler::Instance().ProcessPacket(*pBuf, self);
		pBuf->clear();

		self->RecvPacket();
	});
}

void AsioClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback)
{
	UNREFERENCED_PARAMETER(bytesTransferred);
	UNREFERENCED_PARAMETER(packet);

	if (error)
	{
		error.message();
		return;
	}

	if (nullptr != callback)
	{
		callback();
	}
}

/// <summary>
/// 
/// Acceptor 를 통해서 접속 받은 소켓에 대응되는 클라이언트 소켓 클래스를 정의내릴 수 있음
/// 게임 클라이언트가 될 수도 있고, 기타 서버 대 서버에 의한 클라이언트 소켓이 될 수도 있겠지?
/// 
/// </summary>
/// <param name="io"> AsioServer 의 io_context 가 들어와야, 관련 context 에서 처리가 진행된다 </param>

GameClient::GameClient(asio::io_context& io)
	: AsioClient{ io }
{
}

bool GameClient::Init()
{
	return __super::Init();
}

void GameClient::RecvPacket()
{
	asio::async_read_until(m_socket, asio::dynamic_buffer(m_recvBuffer, PACKET_MAX_SIZE), PACKET_DELIMITER, [self = shared_from_base<GameClient>(), sock = &m_socket, pBuf = &m_recvBuffer](const std::error_code& error, size_t bytes_transferred)
	{
		UNREFERENCED_PARAMETER(bytes_transferred);

		if (error)
		{
			if (error.value() == asio::error::operation_aborted)
			{
				return;
			}

			if (error.value() == asio::error::eof)
			{
				// client 가 연결 종료
				auto ep = sock->remote_endpoint();
				std::cout << std::format("Client Disonnected :: IP = {}, Port = {}\n", ep.address().to_string(), ep.port());

				sock->shutdown(sock->shutdown_both);
				sock->close();

				return;
			}

			return;
		}

		// process packet
		C2S_PacketHandler::Instance().ProcessPacket(*pBuf, self);
		pBuf->clear();

		self->RecvPacket();
	});
}

void GameClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback)
{
	// 일반적으로 send 를 하고 나서 후 처리를 할 일은 별로 없을것으로 보인다.
	/// 있다면 log 남기는거 정도가 있을듯
	__super::PostSendPacket(error, bytesTransferred, packet, callback);
}
