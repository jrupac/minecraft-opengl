#include "R3.h"

R3Character::
R3Character(void)
{
  //TODO: FIX BACK
	Health = 1;
	MaxHealth = 8;
	velocity = R3Vector(0, 0, 0);
	position = R3Point(0, 0, 0);
	item = R3BLOCK_AIR;
	number_gold = 0;

  // Clear all items
	for (int i = 0; i < 8; i++) 
    number_items[i] = 0;
}


