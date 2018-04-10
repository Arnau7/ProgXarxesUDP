#include "PlayerInfo.h"
#include <random>


PlayerInfo::PlayerInfo()
{

}

PlayerInfo::PlayerInfo(std::string _nickname)
{
	nickname = _nickname;
}

void PlayerInfo::SetPosition(int _x, int _y) 
{
	x = _x;
	y = _y;
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
