#include "stdafx.h"
#include "AsioClient.h"
#include <Packet/PacketHandler.h>

#include <functional>

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
	asio::async_write(m_socket, asio::buffer("HelloWorld\n"),
		[](const std::error_code& error, size_t bytes_transferred)
	{
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
	asio::async_read(m_socket, asio::buffer(m_recvBuffer), [self = shared_from_this(), sock = &m_socket, buf = &m_recvBuffer[0]](const std::error_code& error, size_t bytes_transferred)
	{
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
				std::cout << "Client Disonnected :: IP = " << ep.address().to_string() << ", Port = " << ep.port() << "\n";

				sock->shutdown(sock->shutdown_both);
				sock->close();

				return;
			}

			return;
		}

		PacketHandler::Instance().ProcessPacket(buf, self);

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
	asio::async_read(m_socket, asio::buffer(m_recvBuffer), [self = shared_from_base<GameClient>(), sock = &m_socket, buf = &m_recvBuffer[0]](const std::error_code& error, size_t bytes_transferred)
	{
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
				std::cout << "Client Disonnected :: IP = " << ep.address().to_string() << ", Port = " << ep.port() << "\n";

				sock->shutdown(sock->shutdown_both);
				sock->close();

				return;
			}

			return;
		}

		// process packet
		C2S_PacketHandler::Instance().ProcessPacket(buf, self);

		self->RecvPacket();
	});
}

void GameClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback)
{
	// �Ϲ������� send �� �ϰ� ���� �� ó���� �� ���� ���� ���������� ���δ�.
	/// �ִٸ� log ����°� ������ ������
	__super::PostSendPacket(error, bytesTransferred, packet, callback);
}
