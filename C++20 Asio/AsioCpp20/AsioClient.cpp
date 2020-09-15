#include "stdafx.h"
#include "AsioClient.h"

AsioClient::AsioClient(asio::io_context& io)
	: m_socket{ io }
{
}
