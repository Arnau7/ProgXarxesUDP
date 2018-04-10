#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

class PlayerInfo
{
	std::string nickname;
	sf::Vector2i position;
	int x, y;
	int lives, coins;

public:
	PlayerInfo();
	PlayerInfo(std::string _nickname);
	void SetPosition(int _x, int _y);
	int GetX();
	int GetY();
	~PlayerInfo();
};