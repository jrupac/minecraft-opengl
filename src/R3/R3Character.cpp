#include "R3.h"

R3Character::
R3Character(void)
{
	health = 7;
	maxhealth = 8;
	velocity = R3Vector(0, 0, 0);
	position = R3Point(0, 0, 0);
	item = R3Item(R3BLOCK_AIR);
	belt[0] = R3BLOCK_AIR;
	belt[1] = R3BLOCK_DIRT;
	belt[2] = R3BLOCK_BEDROCK;
	belt[3] = R3BLOCK_STONE;
	belt[4] = R3BLOCK_SAND;
	belt[5] = R3STICK;
	belt[6] = R3HATCHET;
	belt[7] = R3TORCH;
	for(int i = 0; i < 8; i++) {
		number_items[i] = 0;
	}

}