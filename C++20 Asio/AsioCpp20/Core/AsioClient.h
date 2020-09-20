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

	// packet Ŭ���� ���� �ʿ�
	void SendPacket(const std::string_view& packet, std::function<void()> callback = nullptr);

protected:
	// packet Ŭ���� ���� �ʿ�
	virtual void PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback = nullptr);

private:

	asio::ip::tcp::socket m_socket;
};

// ��� ����
class GameClient : public AsioClient
{
public:
	explicit GameClient(asio::io_context& io);
	virtual ~GameClient() = default;
	virtual bool Init() override final;

	// ��Ÿ ������ ���ΰ� �ְ� �ʱ�ȭ �Ұ� �ְ� �ϸ鼭 ����ϸ� �� ��
private:
	virtual void PostSendPacket(const std::error_code& error, size_t bytesTransferred, const std::string_view& packet, std::function<void()> callback = nullptr) override final;
};