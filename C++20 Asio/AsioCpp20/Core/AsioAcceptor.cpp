#include "stdafx.h"
#include "AsioAcceptor.h"
#include "AsioClient.h"

#include <atomic>

AsioAcceptor::AsioAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e)
	: m_io{ io }
	, m_acceptor{ io, e }
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

AsioAcceptor::~AsioAcceptor()
{
}

void AsioAcceptor::Listen()
{
	auto client = std::make_shared<AsioClient>(m_io);
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
	if (error)
	{
		error.message();
		return;
	}

	// 테스트용 응답
	client->TEST_SampleResponse();
}

/// <summary>
/// 
/// </summary>
/// <param name="io"></param>
/// <param name="e"></param>

GameClientAcceptor::GameClientAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e)
	: AsioAcceptor{ io, std::forward<asio::ip::tcp::endpoint>(e) }
{
}

void GameClientAcceptor::Listen()
{
	auto client = std::make_shared<GameClient>(m_io);
	m_acceptor.async_accept(client->GetSocket(),
		[client, This = shared_from_base<GameClientAcceptor>()](const std::error_code& error)
	{
		This->HandleAccept(client, error);
		This->Listen();
	}
	);
}

void GameClientAcceptor::HandleAccept(std::shared_ptr<GameClient> client, const std::error_code& error)
{
}
