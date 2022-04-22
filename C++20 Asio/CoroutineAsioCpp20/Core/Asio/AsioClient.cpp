#include "pch.h"
#include "AsioClient.h"
#include "Packet/PacketHandler.h"
#include "../../../Protocol/Protocol.h"

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

std::string_view AsioClient::GetPacketData(std::string_view packetBuf)
{
	auto pos = packetBuf.find(PACKET_DELIMITER);
	if (pos != std::string::npos)
	{
		return packetBuf.substr(0, pos);
	}

	return "";
}

void AsioClient::RecvPacketErrorHandle(const std::error_code& err, asio::ip::tcp::socket& sock)
{
	if (err)
	{
		if (err.value() == asio::error::operation_aborted)
		{
			return;
		}

		if (err.value() == asio::error::eof)
		{
			// client 가 연결 종료
			auto ep = sock.remote_endpoint();
			Logger::Inst().Debug(std::format("Client Disonnected :: IP = {}, Port = {}", ep.address().to_string(), ep.port()));

			sock.shutdown(sock.shutdown_both);
			sock.close();

			return;
		}

		return;
	}
}

void AsioClient::SendPacket(std::string_view packet, std::function<void()> callback)
{
	std::string packetData = std::string{ packet };
	std::shared_ptr<std::string> sendBuffer{ new std::string{ packetData + std::string{ PACKET_DELIMITER } } };
	asio::async_write(m_socket, asio::buffer(*sendBuffer),
		[self = shared_from_this(), p = packetData, func = callback, pBuf = sendBuffer](const std::error_code& error, size_t bytes_transferred)
	{
		self->PostSendPacket(error, bytes_transferred, p, func);
		//std::cout << std::format("Sended Message : {}, 보낸 크기 : {}\n", p, bytes_transferred);
	});
}

void AsioClient::RecvPacket()
{
	std::shared_ptr<std::string> recvBuffer{ new std::string{ "" } };
	asio::async_read_until(m_socket, asio::dynamic_buffer(*recvBuffer, PACKET_MAX_SIZE), PACKET_DELIMITER, [self = shared_from_this(), sock = &m_socket, pBuf = recvBuffer](const std::error_code& error, size_t bytes_transferred)
	{
		UNREFERENCED_PARAMETER(bytes_transferred);

		if (error)
		{
			self->RecvPacketErrorHandle(error, *sock);
			return;
		}
				
		if (auto realData = self->GetPacketData(*pBuf); realData != "")
		{
			PacketHandler::Inst().ProcessPacket(realData, self);
		}
		else
		{
			Logger::Inst().Log(ERROR_LOG::INVALID_PACKET_DATA, std::format("AsioClient::RecvPacket() Packet Data : {}", *pBuf), __FILE__, __LINE__);
			_ASSERT(true);
		}

		self->RecvPacket();
	});
}

void AsioClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, std::string_view packet, std::function<void()> callback)
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
	std::shared_ptr<std::string> recvBuffer{ new std::string{ "" } };
	asio::async_read_until(m_socket, asio::dynamic_buffer(*recvBuffer, PACKET_MAX_SIZE), PACKET_DELIMITER, [self = shared_from_base<GameClient>(), sock = &m_socket, pBuf = recvBuffer](const std::error_code& error, size_t bytes_transferred)
	{
		UNREFERENCED_PARAMETER(bytes_transferred);

		if (error)
		{
			self->RecvPacketErrorHandle(error, *sock);
			return;
		}

		// process packet
		if (auto realData = self->GetPacketData(*pBuf); realData != "")
		{
			C2S_PacketHandler::Inst().ProcessPacket(self->GetPacketData(*pBuf), self);
		}
		else
		{
			Logger::Inst().Log(ERROR_LOG::INVALID_PACKET_DATA, std::format("GameClient::RecvPacket() Packet Data : {}", *pBuf), __FILE__, __LINE__);
			_ASSERT(true);
		}

		self->RecvPacket();
	});
}

void GameClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, std::string_view packet, std::function<void()> callback)
{
	// 일반적으로 send 를 하고 나서 후 처리를 할 일은 별로 없을것으로 보인다.
	/// 있다면 log 남기는거 정도가 있을듯
	__super::PostSendPacket(error, bytesTransferred, packet, callback);
}
