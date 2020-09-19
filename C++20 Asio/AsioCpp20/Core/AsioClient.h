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

private:
	// packet 클래스 구현 필요
	void PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback = nullptr);

private:

	asio::ip::tcp::socket m_socket;
};

