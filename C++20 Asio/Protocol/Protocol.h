#pragma once
#include <string>

constexpr int PACKET_MAX_SIZE = 65535;
constexpr std::string_view PACKET_DELIMITER = "\r\t\n\0";