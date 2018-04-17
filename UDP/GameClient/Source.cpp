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


enum TipoProceso { RATON, GATO, PADRE };
char tablero[SIZE_TABLERO];

int posX, posY, posP2X, posP2Y, posP3X, posP3Y, posP4X, posP4Y;

/**
* Si vale true --> nos permite marcar casilla con el mouse
* Si vale false --> No podemos interactuar con el tablero y aparece un letrero de "esperando"
*/
bool startGame = false;

/**
* Ahora mismo no tiene efecto, pero luego lo necesitarás para validar los movimientos
* en función de si eres el gato o el ratón.
*/
TipoProceso quienSoy = TipoProceso::RATON;



/**
* Cuando el jugador clica en la pantalla, se nos da una coordenada del 0 al 512.
* Esta función la transforma a una posición entre el 0 y el 7
*/
sf::Vector2f TransformaCoordenadaACasilla(int _x, int _y)
{
	float xCasilla = _x / LADO_CASILLA;
	float yCasilla = _y / LADO_CASILLA;
	sf::Vector2f casilla(xCasilla, yCasilla);
	return casilla;
}

/**
* Si guardamos las posiciones de las piezas con valores del 0 al 7,
* esta función las transforma a posición de ventana (pixel), que va del 0 al 512
*/
sf::Vector2f BoardToWindows(sf::Vector2f _position)
{
	return sf::Vector2f(_position.x*LADO_CASILLA + OFFSET_AVATAR, _position.y*LADO_CASILLA + OFFSET_AVATAR);
}

/**
* Contiene el código SFML que captura el evento del clic del mouse y el código que pinta por pantalla
*/


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

bool playerOnline = false;
map<int, PlayerInfo> aPlayers;
int num;


struct Direction {
public:
	IpAddress ip;
	unsigned short port;
	string nickname;
	Direction(IpAddress _ip, unsigned short _port, string _nickname) : ip(_ip), port(_port), nickname(_nickname) {}
	Direction() {}
};

/*void DibujaSFML()
{
	sf::Vector2f casillaOrigen, casillaDestino;
	bool casillaMarcada = false;

	sf::RenderWindow window(sf::VideoMode(512, 512), "Greedy Coins");
	while (window.isOpen())
	{
		sf::Event event;

		//Este primer WHILE es para controlar los eventos del mouse
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left && tienesTurno)
				{
					int x = event.mouseButton.x;
					int y = event.mouseButton.y;
					if (!casillaMarcada)
					{
						casillaOrigen = TransformaCoordenadaACasilla(x, y);
						casillaMarcada = true;
						//TODO: Comprobar que la casilla marcada coincide con las posición del raton (si le toca al ratón)
						//o con la posicion de alguna de las piezas del gato (si le toca al gato)

					}
					else
					{
						casillaDestino = TransformaCoordenadaACasilla(x, y);
						if (casillaOrigen.x == casillaDestino.x && casillaOrigen.y == casillaDestino.y)
						{
							casillaMarcada = false;
							//Si me vuelven a picar sobre la misma casilla, la desmarco
						}
						else
						{
							if (quienSoy == TipoProceso::RATON)
							{
								//TODO: Validar que el destino del ratón es correcto

								//TODO: Si es correcto, modificar la posición del ratón y enviar las posiciones al padre

							}
							else if (quienSoy == TipoProceso::GATO)
							{
								//TODO: Validar que el destino del gato es correcto

								//TODO: Si es correcto, modificar la posición de la pieza correspondiente del gato y enviar las posiciones al padre
							}
						}
					}
				}
				break;
			default:
				break;

			}
		}

		window.clear();

		//A partir de aquí es para pintar por pantalla
		//Este FOR es para el tablero
		for (int i = 0; i<11; i++)
		{
			for (int j = 0; j<11; j++)
			{
				sf::RectangleShape rectBlanco(sf::Vector2f(LADO_CASILLA, LADO_CASILLA));
				rectBlanco.setFillColor(sf::Color::White);
				if (i % 2 == 0)
				{
					//Empieza por el blanco
					if (j % 2 == 0)
					{
						rectBlanco.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
						window.draw(rectBlanco);
					}
				}
				else
				{
					//Empieza por el negro
					if (j % 2 == 1)
					{
						rectBlanco.setPosition(sf::Vector2f(i*LADO_CASILLA, j*LADO_CASILLA));
						window.draw(rectBlanco);
					}
				}
			}
		}

		//TODO: Para pintar el circulito del ratón
		sf::CircleShape shapeRaton(RADIO_AVATAR);
		shapeRaton.setFillColor(sf::Color::Blue);
		sf::Vector2f posicionRaton(4.f, 7.f);
		posicionRaton = BoardToWindows(posicionRaton);
		shapeRaton.setPosition(posicionRaton);
		window.draw(shapeRaton);

		//Pintamos los cuatro circulitos del gato
		sf::CircleShape shapeGato(RADIO_AVATAR);
		shapeGato.setFillColor(sf::Color::Red);

		sf::Vector2f positionGato1(1.f, 0.f);
		positionGato1 = BoardToWindows(positionGato1);
		shapeGato.setPosition(positionGato1);

		window.draw(shapeGato);

		sf::Vector2f positionGato2(3.f, 0.f);
		positionGato2 = BoardToWindows(positionGato2);
		shapeGato.setPosition(positionGato2);

		window.draw(shapeGato);

		sf::Vector2f positionGato3(5.f, 0.f);
		positionGato3 = BoardToWindows(positionGato3);
		shapeGato.setPosition(positionGato3);

		window.draw(shapeGato);

		sf::Vector2f positionGato4(7.f, 0.f);
		positionGato4 = BoardToWindows(positionGato4);
		shapeGato.setPosition(positionGato4);

		window.draw(shapeGato);


		if (!tienesTurno)
		{
			//Si no tengo el turno, pinto un letrerito de "Esperando..."
			sf::Font font;
			std::string pathFont = "arial.ttf";
			if (!font.loadFromFile(pathFont))
			{
				std::cout << "No se pudo cargar la fuente" << std::endl;
			}


			sf::Text textEsperando("Esperando...", font);
			textEsperando.setPosition(sf::Vector2f(180, 200));
			textEsperando.setCharacterSize(30);
			textEsperando.setStyle(sf::Text::Bold);
			textEsperando.setFillColor(sf::Color::Green);
			window.draw(textEsperando);
		}
		else
		{
			//Si tengo el turno y hay una casilla marcada, la marco con un recuadro amarillo.
			if (casillaMarcada)
			{
				sf::RectangleShape rect(sf::Vector2f(LADO_CASILLA, LADO_CASILLA));
				rect.setPosition(sf::Vector2f(casillaOrigen.x*LADO_CASILLA, casillaOrigen.y*LADO_CASILLA));
				rect.setFillColor(sf::Color::Transparent);
				rect.setOutlineThickness(5);
				rect.setOutlineColor(sf::Color::Yellow);
				window.draw(rect);
			}
		}

		window.display();
	}

}*/

void receieveMessage(UdpSocket* socket, string nickname) {
	IpAddress ip;
	unsigned short port;
	Packet newPack;

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
			}
			//Used to make the player use a new nick in case it is used
			else if (header == PT_USEDNICK) {
				// TODO the player can't ask for a new name, also he's still sending messages of hello
				IpAddress serverIp = SERVER_IP;
				unsigned short port = SERVER_PORT;
				cout << "Another name: ";
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
				newPack >> posX >> posY;
				cout << "Recibo la confirmacion: " << posX << " " << posY << std::endl;
			}
			else if (header == PT_START) 
			{
				for (int i = 0; i < 4; i++) {
					int posX;
					int posY;
					newPack >> posX >> posY;
					PlayerInfo player;
					aPlayers[i] = player;
					if (i != id) {
						player.SetPosition(posX, posY);
						cout << "Received player " << i << " at position: " << posX << ", " << posY << endl;
					}
					else if (i == id) {
						if (aPlayers[i].GetX() == posX && aPlayers[i].GetY() == posY) {
							cout << "My positions are correct" << endl;
						}
						else {
							player.SetPosition(posX, posY);
							cout << "Positions corrected" << endl;
						}
					}
				}
				startGame = true;
			}

		}
	}
}

int main()
{
	string nickname = "No Nickname";
	cout << "Hello new player! Introduce your name please: ";
	cin >> nickname;

	IpAddress serverIp = SERVER_IP;
	unsigned short port = SERVER_PORT;
	UdpSocket* aSocket = new UdpSocket;
	Direction serverDir;

	Clock clockCounter;

	thread t(receieveMessage, aSocket, nickname);

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

	cout << "All OK boiii" << endl;
	/*while (true) {
		int temp = 0;
	}*/

	//-----START

	sf::Vector2f casillaOrigen, casillaDestino;
	bool casillaMarcada = false;

	sf::RenderWindow window(sf::VideoMode(512, 512), "Greedy Coins");
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
				if (event.key.code == sf::Keyboard::Left)
				{
					int8_t header = (int8_t)PacketType::PT_MOVE;
					sf::Packet pckLeft;
					posX = posX - 1;
					pckLeft << header << posX << posY;
					aSocket->send(pckLeft, SERVER_IP, SERVER_PORT);

				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					int8_t header = (int8_t)PacketType::PT_MOVE;
					sf::Packet pckRight;
					posX = posX + 1;
					pckRight << header << posX << posY;
					aSocket->send(pckRight, SERVER_IP, SERVER_PORT);
				}
				else if(event.key.code == sf::Keyboard::Up)
				{
					int8_t header = (int8_t)PacketType::PT_MOVE;
					sf::Packet pckUp;
					posY = posY - 1;
					pckUp << header << posX << posY;
					aSocket->send(pckUp, SERVER_IP, SERVER_PORT);
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					int8_t header = (int8_t)PacketType::PT_MOVE;
					sf::Packet pckDown;
					posY = posY + 1;
					pckDown << header << posX << posY;
					aSocket->send(pckDown, SERVER_IP, SERVER_PORT);
				}
				break;
				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left && startGame)
					{
						int x = event.mouseButton.x;
						int y = event.mouseButton.y;
						if (!casillaMarcada)
						{
							casillaOrigen = TransformaCoordenadaACasilla(x, y);
							casillaMarcada = true;

							//TODO: Comprobar que la casilla marcada coincide con las posición de la moneda (si le toca a la moneda)

						}
						else
						{
							casillaDestino = TransformaCoordenadaACasilla(x, y);
							if (casillaOrigen.x == casillaDestino.x && casillaOrigen.y == casillaDestino.y)
							{
								casillaMarcada = false;
								//Si me vuelven a picar sobre la misma casilla, la desmarco
							}
							else
							{
								if (quienSoy == TipoProceso::RATON)
								{
									//TODO: Validar que el destino del ratón es correcto

									//TODO: Si es correcto, modificar la posición del ratón y enviar las posiciones al padre

								}
								else if (quienSoy == TipoProceso::GATO)
								{
									//TODO: Validar que el destino del gato es correcto

									//TODO: Si es correcto, modificar la posición de la pieza correspondiente del gato y enviar las posiciones al padre
								}
							}
						}
					}
					break;
			default:
				break;

			}
		}

		window.clear();






			//-----MOVE

			if (!startGame)
			{

				//Player Draw
				sf::CircleShape shapePlayer(RADIO_AVATAR);
				shapePlayer.setFillColor(sf::Color::Green);

				sf::Vector2f posPlayer(posX, posY);
				posPlayer = BoardToWindows(posPlayer);
				shapePlayer.setPosition(posPlayer);

				window.draw(shapePlayer);
				//Si no tengo el turno, pinto un letrerito de "Esperando..."
				sf::Font font;
				std::string pathFont = "arial.ttf";
				if (!font.loadFromFile(pathFont))
				{
					std::cout << "No se pudo cargar la fuente" << std::endl;
				}


				sf::Text textEsperando("Esperando...", font);
				textEsperando.setPosition(sf::Vector2f(180, 200));
				textEsperando.setCharacterSize(30);
				textEsperando.setStyle(sf::Text::Bold);
				textEsperando.setFillColor(sf::Color::Green);
				window.draw(textEsperando);
			}
			else if(startGame)
			{
				//Coin Draw
				sf::CircleShape shapeCoin(RADIO_AVATAR);
				shapeCoin.setFillColor(sf::Color::Yellow);

				sf::Vector2f posCoin(4, 4);
				posCoin = BoardToWindows(posCoin);
				shapeCoin.setPosition(posCoin);

				window.draw(shapeCoin);

				for (int i = 0; i < 4; i++) 
				{
					if (i != num) {
						if (i == 0) {
							//Player Draw
							sf::CircleShape shapePlayer0(RADIO_AVATAR);
							shapePlayer0.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer0(aPlayers[i].GetX(), aPlayers[i].GetY());
							posPlayer0 = BoardToWindows(posPlayer0);
							shapePlayer0.setPosition(posPlayer0);

							window.draw(shapePlayer0);
						}
						if (i == 1) {
							//Player Draw
							sf::CircleShape shapePlayer1(RADIO_AVATAR);
							shapePlayer1.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer1(aPlayers[i].GetX(), aPlayers[i].GetY());
							posPlayer1 = BoardToWindows(posPlayer1);
							shapePlayer1.setPosition(posPlayer1);

							window.draw(shapePlayer1);
						}
						if (i == 2) {
							//Player Draw
							sf::CircleShape shapePlayer2(RADIO_AVATAR);
							shapePlayer2.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer2(aPlayers[i].GetX(), aPlayers[i].GetY());
							posPlayer2 = BoardToWindows(posPlayer2);
							shapePlayer2.setPosition(posPlayer2);

							window.draw(shapePlayer2);
						}
						if (i == 3) {
							//Player Draw
							sf::CircleShape shapePlayer3(RADIO_AVATAR);
							shapePlayer3.setFillColor(sf::Color::Red);

							sf::Vector2f posPlayer3(aPlayers[i].GetX(), aPlayers[i].GetY());
							posPlayer3 = BoardToWindows(posPlayer3);
							shapePlayer3.setPosition(posPlayer3);

							window.draw(shapePlayer3);
						}
					}
					
					else if(i == num){
						//Player Draw
						sf::CircleShape shapePlayer(RADIO_AVATAR);
						shapePlayer.setFillColor(sf::Color::Green);

						sf::Vector2f posPlayer(aPlayers[i].GetX(), aPlayers[i].GetY());
						posPlayer = BoardToWindows(posPlayer);
						shapePlayer.setPosition(posPlayer);

						window.draw(shapePlayer);
					}
				}

				//Si tengo el turno y hay una casilla marcada, la marco con un recuadro amarillo.
				/*if (casillaMarcada)
				{
					sf::RectangleShape rect(sf::Vector2f(LADO_CASILLA, LADO_CASILLA));
					rect.setPosition(sf::Vector2f(casillaOrigen.x*LADO_CASILLA, casillaOrigen.y*LADO_CASILLA));
					rect.setFillColor(sf::Color::Transparent);
					rect.setOutlineThickness(5);
					rect.setOutlineColor(sf::Color::Yellow);
					window.draw(rect);
				}*/
			}
			
			//-----MOVED

			window.display();
		}
	

	//getchar();
	//return 0;
}