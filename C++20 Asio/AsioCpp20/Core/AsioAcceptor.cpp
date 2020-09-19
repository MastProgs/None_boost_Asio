#include "stdafx.h"
#include "AsioAcceptor.h"
#include "AsioClient.h"

#include <atomic>

AsioAcceptor::AsioAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e)
	: m_io{ io }
	, m_acceptor { io, e }
{
}

AsioAcceptor::~AsioAcceptor()
{
}

bool AsioAcceptor::StartListen()
{
	bool bFalse = false;
	if (m_isAlreadyListening.compare_exchange_strong(bFalse, true))
	{
		Listen();
		return true;
	}

	return false;
}

void AsioAcceptor::Listen()
{
	auto client = std::make_shared<AsioClient>( m_io );
	m_acceptor.async_accept(client->GetSocket(),
		[client, This = shared_from_this()](const std::error_code& error)
		{
			This->HandleAccept(client, error);
			This->Listen();
		}
	);
}

void AsioAcceptor::HandleAccept(std::shared_ptr<AsioClient> client, const std::error_code& error)
{
	UNREFERENCED_PARAMETER(client);

	if (error)
	{
		error.message();
	}
}
