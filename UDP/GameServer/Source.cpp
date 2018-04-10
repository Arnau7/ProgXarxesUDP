#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <thread>
#include <PlayerInfo.h>

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
	PT_RESETPLAYER = 7,
	PT_NEWPLAYER = 8,
	PT_MOVE = 9,
	PT_START = 10
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
map<string, PlayerInfo> aPlayers;
int playersOnline = 0;

void receieveMessage(UdpSocket* socket) {
	//while (true) { cout << ". "; }
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
						PlayerInfo player;
						aPlayers[nick] = player;
						cout << "We have received the player: " << nick << endl;

						Packet pck;
						int8_t welcome = ((int8_t)PacketType::PT_WELCOME);
						//Table size goes from 0 to 7
						//Top left
						if (playersOnline == 0)
						{
							player.SetPosition(0, 0);
						}
						//Top right
						else if (playersOnline == 1)
						{
							player.SetPosition(8, 0);

							//Send player 2 pos to player 1
							//Send player 1 pos to player 2
						}
						//Bot left
						else if (playersOnline == 2)
						{
							player.SetPosition(0, 8);

							//Send player 3 pos to player 1 and 2
							//Send player 1, 2 pos to player 3
						}
						//Bot right
						else if (playersOnline == 3)
						{
							player.SetPosition(8, 8);

							//Send player 4 pos to player 1,2,3
							//Send player 1,2,3 pos to player 4
						}

						pck << welcome << player.GetX() << player.GetY();
						socket->send(pck, aClientsDir[playersOnline].ip, aClientsDir[playersOnline].port);
						playersOnline++;
						if (playersOnline == 4) {

							cout << "All players connected, start!" << endl;
							Packet pckStart;
							int8_t headerStart = (int8_t)PacketType::PT_START;
							for (int i = 0; i < playersOnline; i++) {
								pckStart << headerStart;
									socket->send(pckStart, aClientsDir[i].ip, aClientsDir[i].port);
							
							}

						}
					}
					else {
						cout << "The nickname is in use, please try another" << endl;
						//Packet pck;
						//int8_t header = ((int8_t)PacketType::PT_USEDNICK);
						//pack << header;
						//socket->send(pck, ip, port);
						//cout << "Sended warning" << endl;

						// TODO fix the problem
					}

				}
				else
					cout << "4 players online, waiting on the queue" << endl;
				// TODO send warning, server full
			}
			else if (header == PacketType::PT_MOVE) 
			{
				int posX, posY;
				pack >> posX >> posY;
				if (posX <= -1)		{posX = 0;}
				if (posY <= -1)		{posY = 0;}
				if (posX >= 9)		{posX = 8;}
				if (posY >= 9)		{posY = 8;}

				std::cout << "Se intenta la pos " << posX << " " << posY << std::endl;
				if ((posX >= 0 && posX <= 8) && (posY >= 0 && posY <= 8))
				{
					int8_t headerPos = ((int8_t)PacketType::PT_POSITION);
					std::cout << "Se confirma la pos " << posX << " " << posY << std::endl;
					sf::Packet pckSend;
					pckSend << headerPos << posX << posY;
					socket->send(pckSend, ip, port);
				}
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
	while (true) {
		int i = 1;
	}
	return 0;
}