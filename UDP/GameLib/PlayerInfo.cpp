#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{

}

PlayerInfo::PlayerInfo(int _id, std::string _nickname)
{
	id = _id;
	nickname = _nickname;
	testPing = 0;
	coins = 0;
	win = false;
	online = true;
}

void PlayerInfo::SetPosition(int _x, int _y) 
{
	x = _x;
	y = _y;
}

int PlayerInfo::GetId()
{
	return id;
}

int PlayerInfo::GetX() 
{
	return x;
}

int PlayerInfo::GetY() 
{
	return y;
}

PlayerInfo::~PlayerInfo()
{
	
}
