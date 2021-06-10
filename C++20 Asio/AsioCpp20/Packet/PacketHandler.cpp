#include "stdafx.h"
#include "PacketHandler.h"

#include <Core/AsioClient.h>

void PacketHandler::ProcessPacket(std::string_view packet, std::shared_ptr<AsioClient> client)
{
	// packet 데이터 마지막엔 문자열이라서 '\0' 이 붙어있음, 문자열이 아닌 데이터는 '\0' 제외하고 써야함
	std::cout << std::format("C2S_PacketHandler : {}, LogMsg : {}\n", packet, LogManager::Instance().FindErrorNameToMsg("LogTest"));
	client->SendPacket(packet);
}

void C2S_PacketHandler::ProcessPacket(std::string_view packet, std::shared_ptr<GameClient> client)
{
	// packet 데이터 마지막엔 문자열이라서 '\0' 이 붙어있음, 문자열이 아닌 데이터는 '\0' 제외하고 써야함
	std::cout << std::format("C2S_PacketHandler : {}, LogMsg : {}\n", packet, LogManager::Instance().FindErrorNameToMsg("LogTest"));
	client->SendPacket(packet);
}
