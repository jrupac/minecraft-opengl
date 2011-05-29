#ifndef __R3CREATURE_H__
#define __R3CREATURE_H__

#include "R3.h"
#include "R3Character.h"

typedef enum {
  R3COW_CREATURE,
  R3DEER_CREATURE,
  R3SUICIDE_CREATURE
} R3CreatureType;

class R3Creature 
{
  public:
    R3Creature(void);
    R3Creature(R3Point init, R3CreatureType creaturetype);
    R3Vector UpdateCreature(R3Character *character);
    void UpdateCreatureFall(R3Character *character);
    void Creature_Attack(R3Character *character, R3Vector translated);

  public:
    int Health;
    int MaxHealth;
    R3Point position;
    R3Box box;
    R3CreatureType creaturetype;
};

#endif
