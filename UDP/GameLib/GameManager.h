#pragma once
#include "Game.h"
#include <map>;
class GameManager
{
private:
	std::map<int, Game> aGames;
public:
	GameManager();
	~GameManager();
	void CreateGame();
	void JoinGame();
	void DeleteGame();
	void GetListGames();
};

