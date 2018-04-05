#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

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


int main()
{
	cout << "Server online" << endl;
	UdpSocket serverSocket;
	vector<Direction> aClientsDir;
	serverSocket.bind(50000);
	Packet pack;

	map<string, Direction> aPlayers;

	for (int i = 0; i < 4; i++) {
		IpAddress ip;
		unsigned short port;
		int8_t header;

		if(serverSocket.receive(pack, ip, port) == Socket::Done) {
			string nick;
			pack >> header;
			if (header == PT_HELLO) {
				pack >> nick;
				cout << "We have recived the player: " << nick << endl;
				aClientsDir.push_back(Direction(ip, port, nick));
				
				if (aPlayers.find(nick) == aPlayers.end()) {
					aPlayers.insert(pair<string, Direction>(nick, aClientsDir[i]));
				}
				else {
					cout << "The nickname is in use" << endl;
				}
				
				Packet pck;
				int8_t welcome = ((int8_t)PacketType::PT_WELCOME);
				int plPosX = (0 + (rand() % static_cast<int>(10 - 0 + 1)));
				int plPosY = (0 + (rand() % static_cast<int>(10 - 0 + 1)));

				pck << welcome << i << plPosX << plPosY;
				serverSocket.send(pck, aClientsDir[i].ip, aClientsDir[i].port);
			}
		}
		
	}

	return 0;
}