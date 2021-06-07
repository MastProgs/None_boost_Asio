#include "stdafx.h"
#include "PacketHandler.h"

#include <Core/AsioClient.h>

void PacketHandler::ProcessPacket(const std::string_view& packet, std::shared_ptr<AsioClient> client)
{
	std::cout << std::format("C2S_PacketHandler : {}, LogMsg : {}\n", packet, LogManager::Instance().FindErrorNameToMsg("LogTest"));
	client->SendPacket(packet);
}

void C2S_PacketHandler::ProcessPacket(const std::string_view& packet, std::shared_ptr<GameClient> client)
{
	std::cout << std::format("C2S_PacketHandler : {}\n", packet);
	client->SendPacket(packet);
}
