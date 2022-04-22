#pragma once

struct PacketHeader
{
	int m_id;
	int m_size;
};

class Packet
{
public:
	Packet() = default;
	virtual ~Packet() = default;
private:
	PacketHeader m_header;
};

// ��뿹��
namespace C2S
{
	class TestPacket : public Packet
	{
	public:

	};
}

// ��뿹��
namespace S2C
{
	class TestPacket : public Packet
	{
	public:

	};
}