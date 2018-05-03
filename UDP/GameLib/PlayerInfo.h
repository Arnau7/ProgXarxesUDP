#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

class PlayerInfo
{
	int id;
	sf::Vector2i position;
	int x, y;
	int lives;


public:
	int testPing;
	int coins;
	bool win, online;
	std::string nickname;
	PlayerInfo();
	PlayerInfo(int _id, std::string _nickname);
	void SetPosition(int _x, int _y);
	int GetId();
	int GetX();
	int GetY();
	~PlayerInfo();
};