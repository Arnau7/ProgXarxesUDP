#pragma once
#include <iostream>
#include "PlayerInfo.h"
#include <map>
class Game
{
private:
	std::string name;
	int maxPlayers;
	int playersOnline;
	std::map<int, PlayerInfo> aPlayers;
	float coinX, coinY;
public:
	Game();
	~Game();

};

