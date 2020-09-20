#pragma once
#include <include/asio.hpp>

class AsioClient;

class AsioAcceptor : public Object, public std::enable_shared_from_this<AsioAcceptor>
{
public:
	explicit AsioAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e);
	virtual ~AsioAcceptor();

	bool StartListen();

private:
	virtual void Listen();
	void HandleAccept(std::shared_ptr<AsioClient> client, const std::error_code& error);

protected:
	template <typename Derived>
	std::shared_ptr<Derived> shared_from_base()
	{
		return std::static_pointer_cast<Derived>(shared_from_this());
	}

protected:
	asio::ip::tcp::acceptor m_acceptor;
	asio::io_context& m_io;

	std::atomic<bool> m_isAlreadyListening{ false };
};

// 사용 예시
class GameClient;

class GameClientAcceptor : public AsioAcceptor
{
public:
	explicit GameClientAcceptor(asio::io_context& io, asio::ip::tcp::endpoint&& e);
	virtual ~GameClientAcceptor() = default;

	virtual void Listen();
	void HandleAccept(std::shared_ptr<GameClient> client, const std::error_code& error);
};