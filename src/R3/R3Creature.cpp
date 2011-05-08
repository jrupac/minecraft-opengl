#include "R3Creature.h"
#include "minecraft.h"



R3Creature::
R3Creature(void)
{
	Health = 2;
	MaxHealth = 2;
	position = R3Point(2, .5, -3);
	box = R3Box((position-R3Point(.5, .5, .5)).Point(), position+R3Point(.5, .5, .5));
	creaturetype = R3COW_CREATURE;
}

R3Creature::
	R3Creature(R3Point init, R3CreatureType type)
{
	Health = 2;
	MaxHealth = 2;
	position = init;
	box = R3Box((position-R3Point(.5, .5, .5)).Point(), position+R3Point(.5, .5, .5));
	creaturetype = type;
}


R3Vector R3Creature::
UpdateCreature(R3Character *character)
{
  double x, y, z;
  R3Vector translated;

  switch (creaturetype)
  {
    case R3COW_CREATURE:
      x = RandomNumber();
      y = RandomNumber();
      z = RandomNumber();

      x = (2 * x - 1) / 20;
      y = 0;
      z = (2 * z - 1) / 20;

      translated = character->position - position;
      translated.SetY(0);
      translated.Normalize();
      translated /= 100;

      // Give me some space!
      if (R3Distance(position + translated, character->position) >= 2.f)
        return translated;
      else 
        return R3zero_vector;
    case R3DEER_CREATURE:
      x = RandomNumber();
      y = RandomNumber();
      z = RandomNumber();

      x = (2 * x - 1) / 20;
      y = 0;
      z = (2 * z - 1) / 20;

      translated = position - character->position;
      translated.SetY(0);
      translated.Normalize();
      translated /= 100;

      // Give me some space!
      if (R3Distance(position + translated, character->position) >= 2.f)
        return translated;
      else 
        return R3zero_vector;
  }
}
