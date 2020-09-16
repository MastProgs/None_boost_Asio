#include "stdafx.h"
#include "AsioClient.h"

AsioClient::AsioClient(asio::io_context& io)
	: m_socket{ io }
{
}

void AsioClient::TEST_SampleResponse()
{
}

void AsioClient::SendPacket(const std::string_view& packet)
{
	asio::async_write(m_socket, asio::buffer(packet),
		[This = shared_from_this(), p = std::string{ packet }](const std::error_code& error, size_t bytes_transferred)
	{
		This->PostSendPacket(error, bytes_transferred, p);
	});
}

void AsioClient::PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet)
{
	UNREFERENCED_PARAMETER(bytesTransferred);
	UNREFERENCED_PARAMETER(packet);

	if (error)
	{
		error.message();
	}
}
