#pragma once
#include <SFML\Network.hpp>
#include <iostream>

class CriticPack
{
	enum PacketType
	{
		PT_EMPTY = 0,
		PT_HELLO = 1,
		PT_WELCOME = 2,
		PT_FULL = 3,
		PT_USEDNICK = 4,
		PT_POSITION = 5,
		PT_DISCONNECT = 6,
		PT_RESETPLAYER = 7,
		PT_NEWPLAYER = 8,
		PT_MOVE = 9,
		PT_START = 10,
		PT_COIN = 11,
		PT_PING = 12,
		PT_WIN = 13,
		PT_PLAYING = 14,
		PT_ACKMOVE = 15,
		PT_INTERACT = 16,
		PT_ACK = 17
	};
public:

	int idPacket;
	sf::Packet packet;
	sf::IpAddress ipReceiver;
	unsigned short portReceiver;

	CriticPack();
	CriticPack(int _idPacket, sf::Packet _packet, sf::IpAddress _ipReceiver, unsigned short _portReceiver);
	sf::Packet CreatePacket();
	void sendPacket(sf::UdpSocket* socket);
	~CriticPack();
};

