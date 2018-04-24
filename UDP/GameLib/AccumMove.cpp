#include "AccumMove.h"



AccumMove::AccumMove()
{

}
AccumMove::AccumMove(int _idMove, int _delta_Xs, int _delta_Ys, int _absolute_X, int _absolute_Y)
{
	idMove = _idMove;
	delta_Xs = _delta_Xs;
	delta_Ys = _delta_Ys;
	absolute_X = _absolute_X;
	absolute_Y = _absolute_Y;
}

sf::Packet AccumMove::CreatePacket() 
{
	sf::Packet packet;
	int8_t header = (int8_t)PacketType::PT_MOVE;
	std::cout << "Packing the positions: " << absolute_X << ", " << absolute_Y << std::endl;
	packet << header << idMove << delta_Xs << delta_Ys << absolute_X << absolute_Y;
	return packet;
}


AccumMove::~AccumMove()
{
}