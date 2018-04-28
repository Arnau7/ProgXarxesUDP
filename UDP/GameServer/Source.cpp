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
	PT_START = 10,
	PT_COIN = 11,
	PT_PING = 12,
	PT_WIN = 13,
	PT_PLAYING = 14,
	PT_ACKMOVE = 15
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
map<int, PlayerInfo> aPlayers;
int playersOnline = 0;
int coinX = 0;
int coinY = 0;

void NewCoinPosition() {
	int randX = rand() % 8;
	int randY = rand() % 8;
	coinX = randX;
	coinY = randY;
}

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
					//if (aPlayers.find(nick) == aPlayers.end()) {
					cout << "The client " << playersOnline << " has the nickname " << nick << endl;
						
						aClientsDir.push_back(Direction(ip, port, nick));
						PlayerInfo player(playersOnline, nick);
						
						//aPlayers[player.GetId()] = player;
						cout << "We have received the player: " << nick << " ID: " << player.GetId() << endl;

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
						}
						//Bot left
						else if (playersOnline == 2)
						{
							player.SetPosition(0, 8);
						}
						//Bot right
						else if (playersOnline == 3)
						{
							player.SetPosition(8, 8);
						}

						aPlayers.insert(pair<int, PlayerInfo>(playersOnline, player));

						cout << "Player " << player.GetId() << " at positions: " << player.GetX() << ", " << player.GetY() << endl;
						pck << welcome << player.GetId() << player.GetX() << player.GetY();
						socket->send(pck, aClientsDir[playersOnline].ip, aClientsDir[playersOnline].port);
						playersOnline++;
						if (playersOnline == 4) 
						{
							cout << "All players connected, start!" << endl;
							Packet pckStart;
							int8_t headerStart = (int8_t)PacketType::PT_START;
							pckStart << headerStart;
							for (map<int, PlayerInfo>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
								cout << "ID: " << it->second.GetId() << endl;
								cout << "Packing position: " << it->second.GetX() << ", " << it->second.GetY() << endl;
								pckStart << it->second.GetX() << it->second.GetY();
							}
							/*for (int i = 0; i < 4; i++) {
								cout << "Packing position: " << aPlayers[i].GetX() << ", " << aPlayers[i].GetY() << endl;
								pckStart << aPlayers[i].GetX() << aPlayers[i].GetY();							
							}*/
							coinX = 4;
							coinY = 4;
							cout << "Coin position: " << coinX << " , " << coinY << endl;
							pckStart << coinX << coinY;
							for (int i = 0; i < 4; i++) {
								socket->send(pckStart, aClientsDir[i].ip, aClientsDir[i].port);
							}
						}
					//}
					//else {
					//	cout << "The nickname is in use, please try another" << endl;
					//	//Packet pck;
					//	//int8_t header = ((int8_t)PacketType::PT_USEDNICK);
					//	//pack << header;
					//	//socket->send(pck, ip, port);
					//	//cout << "Sended warning" << endl;

					//	// TODO fix the problem
					//}

				}
				else
					cout << "4 players online, waiting on the queue" << endl;
				// TODO send warning, server full
			}
			else if (header == PacketType::PT_MOVE) 
			{
				int idMove, playerNum;
				pack >> idMove >> playerNum;
				int posX, posY, deltaX, deltaY;
				pack >> deltaX  >> deltaY >> posX >> posY;
				if (posX <= -1)		{posX = 0;}
				if (posY <= -1)		{posY = 0;}
				if (posX >= 9)		{posX = 8;}
				if (posY >= 9)		{posY = 8;}

				//COMPROVAR POSICIÓ PLAYER == MONEDA

				//std::cout << "Se intenta la pos " << posX << " " << posY << std::endl;
				if ((posX >= 0 && posX <= 8) && (posY >= 0 && posY <= 8))
				{
					int8_t headerPos = ((int8_t)PacketType::PT_POSITION);
					//std::cout << "Se confirma la pos " << posX << " " << posY << std::endl;
					sf::Packet pckSend;
					
					//socket->send(pckSend, ip, port);
					if (posX == coinX && posY == coinY) {
						int8_t header2 = (int8_t)PacketType::PT_COIN;
						pckSend << headerPos << header2 << playerNum << posX << posY;
						NewCoinPosition();
						pckSend << coinX << coinY;
						aPlayers[playerNum].coins++;

						//IF PLAYER WINS
						if (aPlayers[playerNum].coins >= 3) {
							int8_t header3 = (int8_t)PacketType::PT_WIN;
							pckSend << header3 << playerNum;
							for (int i = 0; i < 4; i++) {
								socket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
							}
						}
						//IF NO PLAYER WINS
						else {
							int8_t header3 = (int8_t)PacketType::PT_PLAYING;
							pckSend << header3;
							for (int i = 0; i < 4; i++) {
								socket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
							}
						}
					}
					else {
						for (int i = 0; i < 4; i++) {
							if (playerNum != i) {
								int8_t header2 = ((int8_t)PacketType::PT_MOVE);
								pckSend << headerPos << header2 << playerNum << posX << posY;
								socket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
							}
							else {
								cout << "Send ACK to " << i << endl;
								int8_t headerACK = ((int8_t)PacketType::PT_ACKMOVE);
								pckSend << headerPos << headerACK << idMove << posX << posY;
								socket->send(pckSend, aClientsDir[playerNum].ip, aClientsDir[playerNum].port);
							}
						}
					}
				}
			}
			else if (header == PacketType::PT_PING) 
			{
				int id = 0;
				pack >> id;
				aPlayers[id].testPing = 0;
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
	Clock clockPing;

	thread rM(receieveMessage, serverSocket);

	Packet packPing;
	int8_t headerPing = PacketType::PT_PING;
	packPing << headerPing;

		while (true) 
		{
			if (playersOnline > 0) {
				if ((clockPing.getElapsedTime().asMilliseconds() >= 5000))
				{
					for (int i = 0; i < playersOnline; i++)
					{
						serverSocket->send(packPing, aClientsDir[i].ip, aClientsDir[i].port);
						aPlayers[i].testPing++;
						if (aPlayers[i].testPing >= 3)
						{
							cout << "Player disconnected: " << i << endl;
						}
					}
					clockPing.restart();
				}
			}
		}
	return 0;
}