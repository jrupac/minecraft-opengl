#include "R3.h"

R3Character::
R3Character(void)
{
	Health = 7;
	MaxHealth = 8;
	velocity = R3Vector(0, 0, 0);
	position = R3Point(0, 0, 0);
	item = R3BLOCK_AIR;

	for (int i =0; i < 8; i++) {
		number_items[i] = 0;

	}
/*	for (int i = 0; i < 8; i++) {
		belt[i] = R3BLOCK_AIR;
	}*/
/*	
	//NOTE: NOT UPDATED!
	belt[0] = R3BLOCK_AIR;
	belt[1] = R3BLOCK_DIRT;
	belt[2] = R3BLOCK_BEDROCK;
	belt[3] = R3BLOCK_STONE;
	belt[4] = R3BLOCK_SAND;
	belt[5] = R3STICK;
	belt[6] = R3HATCHET;
	belt[7] = R3TORCH;
  */
  // Clear all items
  memset(number_items, 0x0, 8);
}


