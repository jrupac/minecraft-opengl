#ifndef __R3CREATURE_H__
#define __R3CREATURE_H__
#include "R3.h"
#include "R3Character.h"
class R3Creature {
public:
	// Constructor functions
	R3Creature(void);




	int Health;
	int MaxHealth;
	R3Point position;
	R3Box box;

	void UpdateCreature(R3Character *character);
};

#endif