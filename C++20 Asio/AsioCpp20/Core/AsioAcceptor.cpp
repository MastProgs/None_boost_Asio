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

	// �׽�Ʈ�� ����
	client->TEST_SampleResponse();
}

/// <summary>
/// 
/// GameClient �� ���� �������θ� Accept �� �����ϴ� Acceptor �� ������ �� ����
/// AsioServer::SetAcceptor() ���� ���ǰ� �ʿ��ϴ�.
/// 
/// </summary>
/// <param name="io"> AsioServer Ŭ������ ���� �Ǿ� �ִ� io_context �� �ʿ��� </param>
/// <param name="e"> ip4v �ɼǰ� port ������ ���� </param>

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

	// �׽�Ʈ�� ����
	client->TEST_SampleResponse();
}
