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

	// ��� ����
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

void AsioClient::SendPacket(std::string_view packet, std::function<void()> callback)
{
	std::string packetData = std::string{ packet };
	std::shared_ptr<std::string> sendBuffer{ new std::string{ packetData + std::string{ PACKET_DELIMITER } } };
	asio::async_write(m_socket, asio::buffer(*sendBuffer),
		[self = shared_from_this(), p = packetData, func = callback, pBuf = sendBuffer](const std::error_code& error, size_t bytes_transferred)
	{
		self->PostSendPacket(error, bytes_transferred, p, func);
		//std::cout << std::format("Sended Message : {}, ���� ũ�� : {}\n", p, bytes_transferred);
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
			if (error.value() == asio::error::operation_aborted)
			{
				return;
			}

			if (error.value() == asio::error::eof)
			{
				// client �� ���� ����
				auto ep = sock->remote_endpoint();
				std::cout << std::format("Client Disonnected :: IP = {}, Port = {}\n", ep.address().to_string(), ep.port());

				sock->shutdown(sock->shutdown_both);
				sock->close();

				return;
			}

			return;
		}
				
		if (auto realData = self->GetPacketData(*pBuf); realData != "")
		{
			PacketHandler::Instance().ProcessPacket(realData, self);
		}
		else
		{
			std::cout << std::format("AsioClient::RecvPacket(), CodeLine : {}, LogMsg : {}, data : {}\n", __LINE__ , LogManager::Instance().FindErrorNameToMsg("InvalidPacketData"), *pBuf);
			_ASSERT(true);
		}

		pBuf->clear();

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
/// Acceptor �� ���ؼ� ���� ���� ���Ͽ� �����Ǵ� Ŭ���̾�Ʈ ���� Ŭ������ ���ǳ��� �� ����
/// ���� Ŭ���̾�Ʈ�� �� ���� �ְ�, ��Ÿ ���� �� ������ ���� Ŭ���̾�Ʈ ������ �� ���� �ְ���?
/// 
/// </summary>
/// <param name="io"> AsioServer �� io_context �� ���;�, ���� context ���� ó���� ����ȴ� </param>

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
			if (error.value() == asio::error::operation_aborted)
			{
				return;
			}

			if (error.value() == asio::error::eof)
			{
				// client �� ���� ����
				auto ep = sock->remote_endpoint();
				std::cout << std::format("Client Disonnected :: IP = {}, Port = {}\n", ep.address().to_string(), ep.port());

				sock->shutdown(sock->shutdown_both);
				sock->close();

				return;
			}

			return;
		}

		// process packet
		if (auto realData = self->GetPacketData(*pBuf); realData != "")
		{
			C2S_PacketHandler::Instance().ProcessPacket(self->GetPacketData(*pBuf), self);
		}
		else
		{
			std::cout << std::format("GameClient::RecvPacket() CodeLine : {}, LogMsg : {}, PacketData : {}\n", __LINE__, LogManager::Instance().FindErrorNameToMsg("InvalidPacketData"), *pBuf);
			_ASSERT(true);
		}

		pBuf->clear();

		self->RecvPacket();
	});
}

void GameClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, std::string_view packet, std::function<void()> callback)
{
	// �Ϲ������� send �� �ϰ� ���� �� ó���� �� ���� ���� ���������� ���δ�.
	/// �ִٸ� log ����°� ������ ������
	__super::PostSendPacket(error, bytesTransferred, packet, callback);
}
