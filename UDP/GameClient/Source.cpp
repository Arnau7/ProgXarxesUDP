#pragma once
#include <SFML\Network.hpp>
#include <iostream>

using namespace std;
using namespace sf;

enum PacketType
{
	PT_EMPTY = 0,
	PT_HELLO = 1,
	PT_WELCOME = 2,
	PT_FULL = 3,
	PT_TRYPOSITION = 4,
	PT_POSITION = 5,
	PT_DISCONNECT = 6,
	PT_RESETPLAYER = 7
};

struct Direction {
public:
	IpAddress ip;
	unsigned short port;
	string nickname;
	Direction(IpAddress _ip, unsigned short _port, string _nickname) : ip(_ip), port(_port), nickname(_nickname) {}
	Direction() {}
};

void sendMessageThread(Packet pack) {

}

int main()
{
	string nickname = "No Nickname";
	cout << "Hello new player! Introduce your name please: ";
	cin >> nickname;

	int posX, posY;

	UdpSocket aSocket;
	Direction serverDir;
	IpAddress serverIp = "127.0.0.1";
	unsigned short port = 50000;
	Packet pack;
	int8_t header = (int8_t)PacketType::PT_HELLO;
	pack << header << nickname;
	aSocket.send(pack, serverIp, port);
	Packet newPack;
	if (aSocket.receive(newPack, serverIp, port) == Socket::Done) {
		string header;
		newPack >> header;
		if (header == "WELCOME") {
			int rol;
			newPack >> rol >> posX >> posY;
			cout << "Recived the rol: " << rol  << " and you are in the position: " << posX << ", " << posY << endl;
		}
	}
	getchar();
	return 0;
}