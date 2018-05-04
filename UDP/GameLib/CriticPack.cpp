#include "CriticPack.h"



CriticPack::CriticPack()
{
}

CriticPack::CriticPack(int _idPacket, sf::Packet _packet, sf::IpAddress _ipReceiver, unsigned short _portReceiver)
{
	idPacket = _idPacket;
	packet = _packet;
	ipReceiver = _ipReceiver;
	portReceiver = _portReceiver;
}

void CriticPack::sendPacket(sf::UdpSocket* socket) {
	socket->send(packet, ipReceiver, portReceiver);
}

CriticPack::~CriticPack()
{
}
