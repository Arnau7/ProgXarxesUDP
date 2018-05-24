#pragma once
#include <SFML\Network.hpp>
#include <iostream>
#include <stdlib.h>
//#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
//#include <wait.h>
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <thread>
#include <PlayerInfo.h>
#include <AccumMove.h>
#include <list>

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

#define SERVER_IP "localhost"
#define SERVER_PORT 50000

#define MAX 100
#define SIZE_TABLERO 121
#define SIZE_FILA_TABLERO 9
#define LADO_CASILLA 121
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

#define SIZE_TABLERO 121
#define LADO_CASILLA 57
#define RADIO_AVATAR 25.f
#define OFFSET_AVATAR 5

char tablero[SIZE_TABLERO];

int posX, posY, posP2X, posP2Y, posP3X, posP3Y, posP4X, posP4Y;

bool startGame = false;
bool gameOver = false;
bool playerDisconnected = false;

/**
* Si guardamos las posiciones de las piezas con valores del 0 al 7,
* esta función las transforma a posición de ventana (pixel), que va del 0 al 512
*/
sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
	return sf::Vector2f(_position.x*LADO_CASILLA + OFFSET_AVATAR, _position.y*LADO_CASILLA + OFFSET_AVATAR);
}

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
	PT_ACKMOVE = 15,
	PT_INTERACT = 16,
	PT_ACK = 17
};

bool playerOnline = false;
map<int, PlayerInfo> aPlayers;
int num; //USED as client local global player ID
int coinX = 0;
int coinY = 0;
list<AccumMove>aMoves;
Clock clockMoves, clockDc;
int idMove = 0;
int playersOnline = 0;

struct Direction {
public:
	IpAddress ip;
	unsigned short port;
	string nickname;
	Direction(IpAddress _ip, unsigned short _port, string _nickname) : ip(_ip), port(_port), nickname(_nickname) {}
	Direction() {}
};

void receieveMessage(UdpSocket* socket, string nickname) {
	IpAddress ip;
	//unsigned short port;
	Packet newPack;
	IpAddress serverIp = SERVER_IP;
	unsigned short port = SERVER_PORT;
	cout << "Waiting" << endl;

	while (true) {
		//ALL COMMANDS TO RECEIVE FROM THE SERVER
		if (socket->receive(newPack, ip, port) == Socket::Done) {
			int8_t header;
			newPack >> header;
			int id = 0;
			//Used to asign a initial position given by the server
			if (header == PT_WELCOME) {
				newPack >> id >> posX >> posY;
				PlayerInfo player(id, nickname);
				aPlayers[player.GetId()] = player;
				cout << "You are the player: " << id << " and you are in the position: " << posX << ", " << posY << endl;
				playerOnline = true;
				num = id;
				cout << endl << "Num: " << num << endl;
			}
			//Used to make the player use a new nick in case it is used
			else if (header == PT_USEDNICK) {
				// TODO the player can't ask for a new name, also he's still sending messages of hello
				
				cout << "Another name: " << endl;
				cin >> nickname;
				Packet pck;
				int8_t header = (int8_t)PacketType::PT_HELLO;
				pck << header << nickname;
				socket->send(pck, serverIp, port);
			}
			//Used to position a new player in the map
			else if (header == PT_NEWPLAYER)
			{

			}
			//Used to receive server position update
			else if (header == PT_POSITION)
			{
				int8_t header2;
				newPack >> header2;
				if (header2 == PT_COIN) 
				{
					int playerNum;
					int pX, pY;
					int cX, cY;
					newPack >> playerNum >> pX >> pY >> cX >> cY;
					aPlayers[playerNum].SetPosition(pX, pY);
					aPlayers[playerNum].coins++;	//Increase coins collected of this player
					coinX = cX;
					coinY = cY;
					//cout << "Recibo la confirmacion: " << pX << " " << pY << endl;
					cout << "New coin pos: " << cX << " " << cY << endl;
					cout << "Player " << playerNum << " " << "coins: " << aPlayers[playerNum].coins << endl;

					int8_t header3;
					newPack >> header3;
					if (header3 == PT_WIN)
					{
						int winner = 0;
						newPack >> winner;
						aPlayers[winner].win = true;
						gameOver = true;
					}
					else if (header3 == PT_PLAYING)
					{

					}
				}

				else if (header2 == PT_ACKMOVE) {
					//cout << "Received ack" <<  aMoves.size() << endl;
					int confirmedX, confirmedY, numMove;
					newPack >> numMove >> confirmedX >> confirmedY;
					aPlayers[num].SetPosition(confirmedX, confirmedY);

					list<AccumMove>::iterator it;

					bool eraseMoves = false;
					// 
					for (it = aMoves.begin(); it != aMoves.end(); ++it)
					{
						//Check if the positions are wrong
						if (it->idMove == numMove) {
							if (it->absolute_X != confirmedX || it->absolute_Y != confirmedY) {
								//cout << "Wrong position, setting to the correct one";
								eraseMoves = true;
							}
						}

						if (it->idMove <= numMove) {
							//cout << "Before erase: " << aMoves.size() << endl;
							aMoves.erase(it);
							//cout << "Found, erased: " << aMoves.size() << endl;
							break;
						}

						if (eraseMoves) {
							if (it->idMove >= numMove) {
								aMoves.erase(it);
							}
						}
					}
				}

				else if (header2 == PT_MOVE) {
					int playerNum;
					int pX, pY;
					newPack >> playerNum >> pX >> pY;
					cout << "Move the player: " << playerNum <<  " Size: " << aMoves.size() << endl;
					aPlayers[playerNum].SetPosition(pX, pY);
					//cout << "Recibo la confirmacion: " << pX << " " << pY << endl;
				}

			}
			else if (header == PT_START) 
			{
				int idPacket;
				newPack >> idPacket;
				for (int i = 0; i < 4; i++) {
					int pX = 0;
					int pY = 0;
					newPack >> pX >> pY;
					PlayerInfo player(i,nickname);
					aPlayers[i] = player;
					if (i != num) {
						aPlayers[i].SetPosition(pX, pY);
						cout << "Received player " << i << " at position: " << pX << ", " << pY << endl;
					}
					else if (i == num) {
						if (aPlayers[i].GetX() == pX && aPlayers[i].GetY() == pY) {
							cout << "My positions are correct" << endl;
						}
						else {
							aPlayers[i].SetPosition(pX, pY);
							cout << "Positions corrected" << endl;
						}
					}
				}
				playersOnline = 4;
				
				newPack >> coinX >> coinY;
				cout << "Coin position: " << coinX << " , " << coinY << endl;

				Packet ackPack;

				int8_t ackHeader = PacketType::PT_ACK;
				ackPack << ackHeader << idPacket;
				socket->send(ackPack, serverIp, port);

				startGame = true;
			}
			else if (header == PT_PING) {
				Packet pckPing;
				int8_t headerPing = PacketType::PT_PING;
				pckPing << headerPing << num;
				socket->send(pckPing, serverIp, port);
				//cout << "Ping sent back" << endl;
			}
			else if (header == PT_DISCONNECT)
			{
				int idDc = 0;
				newPack >> idDc;
				aPlayers[idDc].online = false;
				playerDisconnected = true;
				cout << "Player: " << idDc << " disconnected" << endl;
				clockDc.restart();
			}
		}
	}
}
/*
void SendMoves(UdpSocket* socket) {
	while (true)
	{
		if (aMoves.size() > 0) {
			//cout << "Found!" << endl;
			list<AccumMove>::iterator it;
			for (it = aMoves.begin(); it != aMoves.end(); ++it)
			{
				Packet pack = it->CreatePacket();
				int x, y, w, z, i;
				int8_t var;
				pack >> var >> x >> y >> w >> z >> i;
				//cout << "Send the packet with positions: " << z << ", " << i << endl;
				socket->send(pack, SERVER_IP, SERVER_PORT);
			}
			aMoves.pop_front();
		}
		else {

		}
	}
}
*/
int main()
{
	string nickname = "No Nickname";
	string pwd = "";
	int enterMode = 0;
	cout << "Use '1' to Log in OR use '2' to Register" << endl;
	cin >> enterMode;
	if (enterMode == 1) {
		cout << "Introduce your username: " << endl;
		cin >> nickname;
		cout << "Introduce your password: " << endl;
		cin >> pwd;
	}
	else if (enterMode == 2) {
		cout << "Pick your username: " << endl;
		cin >> nickname;
		cout << "Choose your password: " << endl;
		cin >> pwd;
	}
	else {
		cout << "Error" << endl;
	}

	//cout << "Hello new player! Introduce your name please: ";
	//cin >> nickname;

	IpAddress serverIp = SERVER_IP;
	unsigned short port = SERVER_PORT;
	UdpSocket* aSocket = new UdpSocket;
	aSocket->setBlocking(false);
	Direction serverDir;

	Clock clockCounter, clockMove;

	int deltaX = 0;
	int deltaY = 0;

	for (int i = 0; i<4; i++) {
		PlayerInfo player;
		aPlayers[i] = player;
	}

	thread t(receieveMessage, aSocket, nickname);
	//thread sM(SendMoves, aSocket);
		//thread r(rrr);
		do {
			if (clockCounter.getElapsedTime().asMilliseconds() >= 500) {
				Packet pack;
				int8_t header = (int8_t)PacketType::PT_HELLO;
				pack << header << nickname;
				aSocket->send(pack, serverIp, port);
				cout << "Send, time: " << clockCounter.getElapsedTime().asMilliseconds() << endl;
				clockCounter.restart();
			}
		} while (!playerOnline);

		cout << "All OK, start!" << endl;
		/*while (true) {
			int temp = 0;
		}*/

		//-----START

		sf::Vector2f casillaOrigen, casillaDestino;
		bool casillaMarcada = false;

		sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Greedy Coins");
		while (window.isOpen())
		{
			sf::Event event;

			while (window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape) {
						aSocket->unbind();
					}
					if (!gameOver) {
						if (event.key.code == sf::Keyboard::X) {
							Packet pckX;
							int8_t hInteract = (int8_t)PacketType::PT_INTERACT;
							pckX << hInteract << num;
							aSocket->send(pckX, SERVER_IP, SERVER_PORT);
						}

						if (event.key.code == sf::Keyboard::Left)
						{
							//int8_t header = (int8_t)PacketType::PT_MOVE;
							//sf::Packet pckLeft;
							deltaX -= 15;
							//cout << "Delta to send: " << deltaX << ", " << deltaY << endl;
							//AccumMove acc(posX, num, deltaX, deltaY, posX, posY);
							//aMoves.push_back(acc);
							//cout << "Added to aMoves: " << aMoves.size() << endl;
							//pckLeft << header << num << posX << posY;
							//aSocket->send(pckLeft, SERVER_IP, SERVER_PORT);

						}
						else if (event.key.code == sf::Keyboard::Right)
						{
							//int8_t header = (int8_t)PacketType::PT_MOVE;
							//sf::Packet pckRight;
							deltaX += 15;
							//cout << "Delta to send: " << deltaX << ", " << deltaY << endl;
							//AccumMove acc(posX, num, deltaX, deltaY, posX, posY);
							//aMoves.push_back(acc);
							//cout << "Added to aMoves: " << aMoves.size() << endl;
							//pckRight << header << num << posX << posY;
							//aSocket->send(pckRight, SERVER_IP, SERVER_PORT);
						}

						if (event.key.code == sf::Keyboard::Up)
						{
							//int8_t header = (int8_t)PacketType::PT_MOVE;
							//sf::Packet pckUp;
							deltaY -= 15;
							//cout << "Delta to send: " << deltaX << ", " << deltaY << endl;
							//AccumMove acc(posX, num, deltaX, deltaY, posX, posY);
							//aMoves.push_back(acc);
							//cout << "Added to aMoves: " << aMoves.size() << endl;
							//pckUp << header << num << posX << posY;
							//aSocket->send(pckUp, SERVER_IP, SERVER_PORT);
						}
						else if (event.key.code == sf::Keyboard::Down)
						{
							//int8_t header = (int8_t)PacketType::PT_MOVE;
							//sf::Packet pckDown;
							deltaY += 15;
							//cout << "Delta to send: " << deltaX << ", " << deltaY << endl;
							//AccumMove acc(posX, num, deltaX, deltaY, posX, posY);
							//aMoves.push_back(acc);
							//cout << "Added to aMoves: " << aMoves.size() << endl;
							//pckDown << header << num << posX << posY;
							//aSocket->send(pckDown, SERVER_IP, SERVER_PORT);
						}
					}
					break;
				default:
					break;

				}
			}
			window.clear();

			if (playerDisconnected) 
			{
				cout << "Disconnection text" << endl;
				sf::Font font;
				std::string pathFont = "arial.ttf";
				if (!font.loadFromFile(pathFont))
				{
					std::cout << "No se pudo cargar la fuente" << std::endl;
				}

				sf::Text textDisconnected("Player disconnected", font);
				textDisconnected.setPosition(sf::Vector2f(180, 50));
				textDisconnected.setCharacterSize(18);
				textDisconnected.setStyle(sf::Text::Bold);
				textDisconnected.setFillColor(sf::Color::Magenta);
				window.draw(textDisconnected);

				if (clockDc.getElapsedTime().asSeconds() >= 3)
				{
					cout << "Clock DC restart" << endl;
					playerDisconnected = false;
					clockDc.restart();
				}
			}

			if (!startGame)
			{

				//Player Draw
				sf::CircleShape shapePlayer(RADIO_AVATAR);
				shapePlayer.setFillColor(sf::Color::Green);

				sf::Vector2f posPlayer(posX, posY);
				//cout << posX << posY << endl;
				//posPlayer = BoardToWindows(posPlayer);
				shapePlayer.setPosition(posPlayer);

				window.draw(shapePlayer);
				//Si no tengo el turno, pinto un letrerito de "Esperando..."
				sf::Font font;
				std::string pathFont = "arial.ttf";
				if (!font.loadFromFile(pathFont))
				{
					std::cout << "No se pudo cargar la fuente" << std::endl;
				}

				sf::Text textEsperando("Waiting...\nMove with arrows\nUse 'X' to collect coins", font);
				textEsperando.setPosition(sf::Vector2f(180, 200));
				textEsperando.setCharacterSize(24);
				textEsperando.setStyle(sf::Text::Bold);
				textEsperando.setFillColor(sf::Color::Green);
				window.draw(textEsperando);
			}
			else if (startGame)
			{
				if (!gameOver) {
					//Coin Draw
					sf::CircleShape shapeCoin(RADIO_AVATAR);
					shapeCoin.setFillColor(sf::Color::Yellow);

					sf::Vector2f posCoin(coinX, coinY);
					//posCoin = BoardToWindows(posCoin);
					shapeCoin.setPosition(posCoin);

					window.draw(shapeCoin);
				}

				for (int i = 0; i < 4; i++)
				{
					if (i != num) {
						if (i == 0) {
							//Player Draw
							sf::CircleShape shapePlayer0(RADIO_AVATAR);
							shapePlayer0.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer0(aPlayers[i].GetX(), aPlayers[i].GetY());
							//posPlayer0 = BoardToWindows(posPlayer0);
							shapePlayer0.setPosition(posPlayer0);

							if(aPlayers[i].online)
								window.draw(shapePlayer0);
						}
						else if (i == 1) {
							//Player Draw
							sf::CircleShape shapePlayer1(RADIO_AVATAR);
							shapePlayer1.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer1(aPlayers[i].GetX(), aPlayers[i].GetY());
							//posPlayer1 = BoardToWindows(posPlayer1);
							shapePlayer1.setPosition(posPlayer1);

							if (aPlayers[i].online)
								window.draw(shapePlayer1);
						}
						else if (i == 2) {
							//Player Draw
							sf::CircleShape shapePlayer2(RADIO_AVATAR);
							shapePlayer2.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer2(aPlayers[i].GetX(), aPlayers[i].GetY());
							//posPlayer2 = BoardToWindows(posPlayer2);
							shapePlayer2.setPosition(posPlayer2);

							if (aPlayers[i].online)
								window.draw(shapePlayer2);
						}
						else if (i == 3) {
							//Player Draw
							sf::CircleShape shapePlayer3(RADIO_AVATAR);
							shapePlayer3.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer3(aPlayers[i].GetX(), aPlayers[i].GetY());
							//posPlayer3 = BoardToWindows(posPlayer3);
							shapePlayer3.setPosition(posPlayer3);

							if (aPlayers[i].online)
								window.draw(shapePlayer3);
						}
					}

					else if (i == num) {
						//Player Draw
						sf::CircleShape shapePlayer(RADIO_AVATAR);
						shapePlayer.setFillColor(sf::Color::Green);

						sf::Vector2f posPlayer(aPlayers[i].GetX(), aPlayers[i].GetY());
						//posPlayer = BoardToWindows(posPlayer);
						shapePlayer.setPosition(posPlayer);

						window.draw(shapePlayer);
					}
				}
			}
			if (gameOver) 
			{
				sf::Font font;
				std::string pathFont = "arial.ttf";
				if (!font.loadFromFile(pathFont))
				{
					std::cout << "No se pudo cargar la fuente" << std::endl;
				}

				if (aPlayers[num].win == true) 
				{

					sf::Text textWin("YOU WIN " + aPlayers[num].nickname, font);
					//center text
					sf::FloatRect textRect = textWin.getLocalBounds();
					textWin.setOrigin(textRect.width / 2, textRect.height / 2);
					textWin.setPosition(sf::Vector2f(SCREEN_WIDTH/2, SCREEN_HEIGHT/2));
					textWin.setCharacterSize(30);
					textWin.setStyle(sf::Text::Bold);
					textWin.setFillColor(sf::Color::Yellow);
					window.draw(textWin);
				}
				else
				{
					sf::Text textLose("YOU LOSE " + aPlayers[num].nickname, font);
					sf::FloatRect textRect = textLose.getLocalBounds();
					textLose.setOrigin(textRect.width / 2, textRect.height / 2);
					textLose.setPosition(sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
					textLose.setCharacterSize(30);
					textLose.setStyle(sf::Text::Bold);
					textLose.setFillColor(sf::Color::Red);
					window.draw(textLose);
				}
			}

			//-----MOVED
			if ((clockMove.getElapsedTime().asMilliseconds() >= 200 && (deltaX != 0 || deltaY != 0)) && startGame) {
				cout << "Move detected" << endl;
				idMove++;
				AccumMove acc(idMove, num, deltaX, deltaY, aPlayers[num].GetX(), aPlayers[num].GetY());
				deltaX = 0;
				deltaY = 0;
				aMoves.push_back(acc);
				Packet pack = acc.CreatePacket();
				aSocket->send(pack, SERVER_IP, SERVER_PORT);
				clockMove.restart();
			}
			else{}

			window.display();
		}

	
	//getchar();
	//return 0;
}