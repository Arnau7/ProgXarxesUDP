#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <thread>
#include <PlayerInfo.h>
#include <AccumMove.h>
#include <list>
#include <CriticPack.h>
#include <mysql_connection.h>
#include <cppconn\driver.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <cppconn\exception.h>
#include "DBManager.h"

using namespace std;
using namespace sf;

#define SIZE_TABLERO 121
#define LADO_CASILLA 57
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

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

sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
	return sf::Vector2f(_position.x*LADO_CASILLA + OFFSET_AVATAR, _position.y*LADO_CASILLA + OFFSET_AVATAR);
}

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
list<AccumMove> aMoves;
map<int, CriticPack> criticPackets;
int idCriticPacket;
int playersOnline = 0;
int coinX = 0;
int coinY = 0;
bool checkInteract = false;

int ackReceived = 0;

void NewCoinPosition() {
	int randX = rand() % 8*57+5;
	int randY = rand() % 8*57+5;
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
							Vector2f vec(8 * 57 + 5, 0);
							//BoardToWindows(vec);
							cout << vec.x << vec.y << endl;
							player.SetPosition(vec.x, vec.y);
						}
						//Bot left
						else if (playersOnline == 2)
						{
							Vector2f vec(0, 8 * 57 + 5);
							//BoardToWindows(vec);
							player.SetPosition(vec.x, vec.y);
						}
						//Bot right
						else if (playersOnline == 3)
						{
							Vector2f vec(8 * 57 + 5, 8 * 57 + 5);
							//BoardToWindows(vec);
							player.SetPosition(vec.x, vec.y);
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
							pckStart << headerStart << idCriticPacket;
							for (map<int, PlayerInfo>::iterator it = aPlayers.begin(); it != aPlayers.end(); ++it) {
								cout << "ID: " << it->second.GetId() << endl;
								cout << "Packing position: " << it->second.GetX() << ", " << it->second.GetY() << endl;
								pckStart << it->second.GetX() << it->second.GetY();
							}
							/*for (int i = 0; i < 4; i++) {
								cout << "Packing position: " << aPlayers[i].GetX() << ", " << aPlayers[i].GetY() << endl;
								pckStart << aPlayers[i].GetX() << aPlayers[i].GetY();							
							}*/
							coinX = 4*57+5;
							coinY = 4*57+5;
							cout << "Coin position: " << coinX << " , " << coinY << endl;
							pckStart << coinX << coinY;

							for (int i = 0; i < playersOnline; i++) {
								CriticPack critic(idCriticPacket, pckStart, aClientsDir[i].ip, aClientsDir[i].port);
								criticPackets[critic.idPacket] = critic;
								idCriticPacket++;
							}

							cout << "Packets added as critic: " << criticPackets.size() << endl;

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
			else if (header == PacketType::PT_INTERACT) {
				checkInteract = true;
			}
			else if (header == PacketType::PT_MOVE) 
			{
				int idMove, playerNum;
				pack >> idMove >> playerNum;
				int posX, posY, deltaX, deltaY;
				pack >> deltaX  >> deltaY >> posX >> posY;
				Vector2f vector(posX, posY);
				BoardToWindows(vector);
				if (vector.x <= 0)		{vector.x = 0;}
				if (vector.y <= 0)		{vector.y = 0;}
				if (vector.x >= 461)		{vector.x = 461;}
				if (vector.y >= 461)		{vector.y = 461;}

				AccumMove acc(idMove, playerNum, deltaX, deltaY, vector.x, vector.y);
				int found = false;
				list<AccumMove>::iterator it;

				/*for (it = aMoves.begin(); it != aMoves.end(); ++it)
				{
					if (it->idPlayer == playerNum) {
						cout << "Values before merge: Pos: " << it->absolute_X << ", " << it->absolute_Y << " IdMove: " << it->idMove << " Delta: " << it->delta_Xs << ", " << it->delta_Ys << endl;
						it->absolute_X = posX;
						it->absolute_Y = posY;
						it->idMove = idMove;
						it->delta_Xs += deltaX;
						it->delta_Ys += deltaY;
						cout << "Values after merge: Pos: " << it->absolute_X << ", " << it->absolute_Y << " IdMove: " << it->idMove << " Delta: " << it->delta_Xs << ", " << it->delta_Ys << endl;
						found = true;
					}
					else {}
				}*/
				
				if (!found) {
					aMoves.push_back(acc);
				}

				//COMPROVAR POSICIÓ PLAYER == MONEDA

				//std::cout << "Se intenta la pos " << posX << " " << posY << std::endl;
				/*if ((posX >= 0 && posX <= 8) && (posY >= 0 && posY <= 8))
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
							cout << "Playernum: " << playerNum << ", i: " << i << endl;
							if (playerNum != i) {
								int8_t header2 = ((int8_t)PacketType::PT_MOVE);
								pckSend << headerPos << header2 << playerNum << posX << posY;
								socket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
							}
							else {
								cout << "Send ACK to " << i << endl;
								Packet ackPacket;
								int8_t headerACK = ((int8_t)PacketType::PT_ACKMOVE);
								ackPacket << headerPos << headerACK << idMove << posX << posY;
								socket->send(ackPacket, aClientsDir[i].ip, aClientsDir[i].port);
							}
						}
					}
				}*/
			}
			else if (header == PacketType::PT_PING) 
			{
				int id = 0;
				pack >> id;
				aPlayers[id].testPing = 0;
			}
			else if (header == PacketType::PT_ACK)
			{
				cout << "Received ACK" << endl;
				int idPacket;
				pack >> idPacket;
				ackReceived++;
				if (ackReceived == 4)
				{
					criticPackets.clear();
				}
				//for (auto it = criticPackets.begin(); it != criticPackets.end(); ++it) {
				//	if (it->first == idPacket) {
				//		criticPackets.erase(idPacket);
				//	}
				//}
			}
		}
	}
}

int main()
{
	DBManager* dbm = new DBManager();
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;

	cout << "Server online" << endl;
	UdpSocket* serverSocket = new UdpSocket;
	serverSocket->bind(50000);
	Packet pack;
	Clock clockPing;
	Clock clockMove;
	Clock clockCritics;

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
						if (aPlayers[i].online) { // si aquest player estava online
							if (aPlayers[i].testPing >= 3) //té 3 avisos acumulats (fins a 15 segons) informem als clients
							{
								Packet pckDc;
								int8_t headerDc = (int8_t)PacketType::PT_DISCONNECT;
								pckDc << headerDc << aPlayers[i].GetId();
								cout << "Player disconnected" << endl;
								for (int i = 0; i < 4; i++) {
									serverSocket->send(pckDc, aClientsDir[i].ip, aClientsDir[i].port);
								}
								aPlayers[i].online = false;
							}
						}
					}
					clockPing.restart();
				}

				if (clockMove.getElapsedTime().asMilliseconds() >= 100) {
					list<AccumMove>::iterator it;
					//cout << "Check" << endl;
					if (aMoves.size() > 0) {
						cout << "Found moves" << endl;
						for (it = aMoves.begin(); it != aMoves.end(); ++it)
						{
							if ((it->absolute_X >= 0 && it->absolute_X <= 8*57+5) && (it->absolute_Y >= 0 && it->absolute_Y <= 8*57+5)) {
								int8_t headerPos = ((int8_t)PacketType::PT_POSITION);	
								sf::Packet pckSend;
								if (checkInteract && ((it->absolute_X >= coinX && it->absolute_X <= coinX+RADIO_AVATAR*2) 
									&& (it->absolute_Y >= coinY && it->absolute_Y <= coinY+RADIO_AVATAR*2))) {
									int8_t header2 = (int8_t)PacketType::PT_COIN;
									pckSend << headerPos << header2 << it->idPlayer << it->absolute_X << it->absolute_Y;

									NewCoinPosition();
									pckSend << coinX << coinY;
									aPlayers[it->idPlayer].coins++;
									//IF PLAYER WINS
									if (aPlayers[it->idPlayer].coins >= 3) {
										int8_t header3 = (int8_t)PacketType::PT_WIN;
										pckSend << header3 << it->idPlayer;
										for (int i = 0; i < 4; i++) {
											serverSocket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
										}
									}
									//IF NO PLAYER WINS
									else {
										int8_t header3 = (int8_t)PacketType::PT_PLAYING;
										pckSend << header3;
										for (int i = 0; i < 4; i++) {
											serverSocket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
										}
									}
									checkInteract = false;
								}

								else {
									for (int i = 0; i < 4; i++) {
										//cout << "Playernum: " << it->idPlayer << ", i: " << i << endl;
										if (it->idPlayer != i) {
											int8_t header2 = ((int8_t)PacketType::PT_MOVE);
											pckSend << headerPos << header2 << it->idPlayer << it->absolute_X << it->absolute_Y;
											serverSocket->send(pckSend, aClientsDir[i].ip, aClientsDir[i].port);
										}
										else {
											cout << "Send ACK to " << i << endl;
											Packet ackPacket;
											int8_t headerACK = ((int8_t)PacketType::PT_ACKMOVE);
											ackPacket << headerPos << headerACK << it->idMove << it->absolute_X << it->absolute_Y;
											serverSocket->send(ackPacket, aClientsDir[i].ip, aClientsDir[i].port);
										}
									}
								}
							}
						}
						aMoves.clear();
					}
					clockMove.restart();
				}

				if ((clockCritics.getElapsedTime().asMilliseconds() >= 1000))
				{
					if (criticPackets.size() > 0) {
						for (int i = 0; i < criticPackets.size(); i++) {
							criticPackets[i].sendPacket(serverSocket);
						}
						clockCritics.restart();
					}
				}

			}
		}
	return 0;
}