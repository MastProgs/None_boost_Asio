#pragma once

class AsioClient;
class PacketHandler : public Singleton<PacketHandler>
{
public:
	PacketHandler() = default;
	virtual ~PacketHandler() = default;

	void ProcessPacket(std::string_view packet, std::shared_ptr<AsioClient> client);
private:
};

// 사용 예시
class GameClient;
class C2S_PacketHandler : public Singleton<C2S_PacketHandler>
{
public:
	C2S_PacketHandler() = default;
	virtual ~C2S_PacketHandler() = default;

	void ProcessPacket(std::string_view packet, std::shared_ptr<GameClient> client);
private:
};