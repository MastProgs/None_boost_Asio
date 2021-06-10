#include "stdafx.h"
#include "PacketHandler.h"

#include <Core/AsioClient.h>

void PacketHandler::ProcessPacket(std::string_view packet, std::shared_ptr<AsioClient> client)
{
	// packet ������ �������� ���ڿ��̶� '\0' �� �پ�����, ���ڿ��� �ƴ� �����ʹ� '\0' �����ϰ� �����
	std::cout << std::format("C2S_PacketHandler : {}, LogMsg : {}\n", packet, LogManager::Instance().FindErrorNameToMsg("LogTest"));
	client->SendPacket(packet);
}

void C2S_PacketHandler::ProcessPacket(std::string_view packet, std::shared_ptr<GameClient> client)
{
	// packet ������ �������� ���ڿ��̶� '\0' �� �پ�����, ���ڿ��� �ƴ� �����ʹ� '\0' �����ϰ� �����
	std::cout << std::format("C2S_PacketHandler : {}, LogMsg : {}\n", packet, LogManager::Instance().FindErrorNameToMsg("LogTest"));
	client->SendPacket(packet);
}
