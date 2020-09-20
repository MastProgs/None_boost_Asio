#pragma once

#include <include/asio.hpp>

class AsioClient : public Object, public std::enable_shared_from_this<AsioClient>
{
public:
	explicit AsioClient(asio::io_context& io);
	virtual ~AsioClient() = default;

	virtual bool Init() override;

	asio::ip::tcp::socket& GetSocket() { return m_socket; }

	void TEST_SampleResponse();

	// packet 클래스 구현 필요
	void SendPacket(const std::string_view& packet, std::function<void()> callback = nullptr);

protected:
	// packet 클래스 구현 필요
	virtual void PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback = nullptr);

private:

	asio::ip::tcp::socket m_socket;
};

// 사용 예시
class GameClient : public AsioClient
{
public:
	explicit GameClient(asio::io_context& io);
	virtual ~GameClient() = default;
	virtual bool Init() override final;

	// 기타 컨텐츠 적인거 넣고 초기화 할거 넣고 하면서 사용하면 될 듯
private:
	virtual void PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback = nullptr) override final;
};