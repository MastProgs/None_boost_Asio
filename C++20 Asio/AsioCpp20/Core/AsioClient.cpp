#include "stdafx.h"
#include "AsioClient.h"
#include "../ObjectBase/Object.h"
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

void GameClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback)
{
	// �Ϲ������� send �� �ϰ� ���� �� ó���� �� ���� ���� ���������� ���δ�.
	/// �ִٸ� log ����°� ������ ������
	__super::PostSendPacket(error, bytesTransferred, packet, callback);
}
