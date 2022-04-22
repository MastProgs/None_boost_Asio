#include "pch.h"
#include "PacketHandler.h"

#include "AsioClient.h"

void PacketHandler::ProcessPacket(std::string_view packet, std::shared_ptr<AsioClient> client)
{
	// packet ������ �������� ���ڿ��̶� '\0' �� �پ�����, ���ڿ��� �ƴ� �����ʹ� '\0' �����ϰ� �����
	Logger::Inst().Debug(std::format("C2S_PacketHandler : {}", packet));
	client->SendPacket(packet);
}

void C2S_PacketHandler::ProcessPacket(std::string_view packet, std::shared_ptr<GameClient> client)
{
	// packet ������ �������� ���ڿ��̶� '\0' �� �پ�����, ���ڿ��� �ƴ� �����ʹ� '\0' �����ϰ� �����
	Logger::Inst().Debug(std::format("C2S_PacketHandler : {}", packet));
	client->SendPacket(packet);
}
