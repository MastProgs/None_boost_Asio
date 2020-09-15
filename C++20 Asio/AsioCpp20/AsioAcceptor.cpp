#include "stdafx.h"
#include "AsioAcceptor.h"
#include "AsioClient.h"

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
		[c = client, this](const asio::error_code& error)
		{
			this->HandleAccept(c, error);
			this->Listen();
		}
	);
}

void AsioAcceptor::HandleAccept(std::shared_ptr<AsioClient> client, const asio::error_code& error)
{
	UNREFERENCED_PARAMETER(client);
	UNREFERENCED_PARAMETER(error);	
}
