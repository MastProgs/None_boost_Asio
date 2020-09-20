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
		[client, self = shared_from_this()](const std::error_code& error)
	{
		self->HandleAccept(client, error);
		self->Listen();
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
/// GameClient 에 대해 한정으로만 Accept 를 진행하는 Acceptor 를 구현할 수 있음
/// AsioServer::SetAcceptor() 에서 정의가 필요하다.
/// 
/// </summary>
/// <param name="io"> AsioServer 클래스에 정의 되어 있는 io_context 가 필요함 </param>
/// <param name="e"> ip4v 옵션과 port 정보가 있음 </param>

GameClientAcceptor::GameClientAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e)
	: AsioAcceptor{ io, std::forward<asio::ip::tcp::endpoint>(e) }
{
}

void GameClientAcceptor::Listen()
{
	auto client = std::make_shared<GameClient>(m_io);
	m_acceptor.async_accept(client->GetSocket(),
		[client, self = shared_from_base<GameClientAcceptor>()](const std::error_code& error)
	{
		self->HandleAccept(client, error);
		self->Listen();
	}
	);
}

void GameClientAcceptor::HandleAccept(std::shared_ptr<GameClient> client, const std::error_code& error)
{
	if (error)
	{
		error.message();
		return;
	}

	// 테스트용 응답
	client->TEST_SampleResponse();
}
