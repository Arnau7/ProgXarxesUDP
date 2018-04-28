#pragma once
#include <SFML\Network.hpp>
#include <iostream>

class AccumMove
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
		PT_ACKMOVE = 15
	};
public:

	int idMove;
	int idPlayer;
	int delta_Xs, delta_Ys;
	int absolute_X, absolute_Y;
	AccumMove();
	AccumMove(int _idMove, int _idPlayer, int _delta_Xs, int _delta_Ys, int _absolute_X, int _absolute_Y);
	sf::Packet CreatePacket();
	~AccumMove();
};

