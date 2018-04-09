#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <thread>

using namespace std;
using namespace sf;

enum PacketType
{
	PT_EMPTY = 0,
	PT_HELLO = 1,
	PT_WELCOME = 2,
	PT_FULL = 3,
	PT_USEDNICK = 4,
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

vector<Direction> aClientsDir;
map<string, Direction> aPlayers;
int playersOnline = 0;

void receieveMessage(UdpSocket* socket) {

	Packet pack;
	IpAddress ip;
	unsigned short port;

	while (true) {
		if (socket->receive(pack, ip, port) == Socket::Done) {
			int8_t header;
			pack >> header;

			if (header == PacketType::PT_HELLO) {
				if (playersOnline < 4) {
					string nick;
					pack >> nick;
					cout << "A client says Hello!" << endl;
					if (aPlayers.find(nick) == aPlayers.end()) {
						aClientsDir.push_back(Direction(ip, port, nick));
						aPlayers.insert(pair<string, Direction>(nick, aClientsDir[playersOnline]));
						cout << "We have received the player: " << nick << endl;

						Packet pck;
						int8_t welcome = ((int8_t)PacketType::PT_WELCOME);
						//Table size goes from 0 to 7
						int plPosX, plPosY;
						//Top left
						if (playersOnline == 0)
						{
							/*int plPosX = (0 + (rand() % static_cast<int>(7 - 0 + 1)));
							int plPosY = (0 + (rand() % static_cast<int>(7 - 0 + 1)));*/
							plPosX = 0;
							plPosY = 0;
						}
						//Top right
						else if (playersOnline == 1)
						{
							plPosX = 11;
							plPosY = 0;
						}
						//Bot left
						else if (playersOnline == 2)
						{
							plPosX = 0;
							plPosY = 11;
						}
						//Bot right
						else if (playersOnline == 3)
						{
							plPosX = 11;
							plPosY = 11;
						}

						pck << welcome << plPosX << plPosY;
						socket->send(pck, aClientsDir[playersOnline].ip, aClientsDir[playersOnline].port);
						playersOnline++;
						if (playersOnline == 4) {
							cout << "All players connected, start!" << endl;
							// TODO send start to all players
						}
					}
					else {
						cout << "The nickname is in use, please try another" << endl;
						/*Packet pck;
						int8_t header = ((int8_t)PacketType::PT_USEDNICK);
						pack << header;
						socket->send(pck, ip, port);
						cout << "Sended warning" << endl;*/

						// TODO fix the problem
					}

				}
				else
					cout << "4 players online, waiting on the queue" << endl;
				// TODO send warning, server full
			}
		}
	}
}

int main()
{
	cout << "Server online" << endl;
	UdpSocket* serverSocket = new UdpSocket;
	serverSocket->bind(50000);
	Packet pack;

	thread rM(receieveMessage, serverSocket);

	return 0;
}