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

void R3Creature::
Creature_Attack(R3Character *character, R3Vector translated) 
{
  switch (creaturetype)
  {
    case R3DEER_CREATURE:
      character->Health--;
      MoveCharacter(translated, 3);
      return;
    case R3SUICIDE_CREATURE:
      character->Health--;
      character->Health--;
      character->Health--;
      character->Health--;
      MoveCharacter(translated, 4);
      //RemoveCreature(this);
    default:
      break;
  }
}

R3Vector R3Creature::
	UpdateCreature(R3Character *character)
{
	double x, y, z;
	R3Vector translated;

  switch (creaturetype)
  {
    case R3DEER_CREATURE:
      /*    x = RandomNumber();
            y = RandomNumber();
            z = RandomNumber();

            x = (2 * x - 1) * 5;
            y = 0;
            z = (2 * z - 1) * 5;*/

      translated = character->position - position;
      translated.SetY(0);
      //translated.SetX(translated.X() + x);
      //translated.SetZ(translated.Z() + z);
      translated.Normalize();
      translated /= 20;

      // Give me some space!
      if (R3Distance(position + translated, character->position) < 2.f) 
      {
        Creature_Attack(character, translated);
        translated = R3zero_vector;
      }
      return translated;

    case R3SUICIDE_CREATURE:
      translated = character->position - position;
      translated.SetY(0);
      //translated.SetX(translated.X() + x);
      //translated.SetZ(translated.Z() + z);
      translated.Normalize();
      translated /= 20;

      // Give me some space!
      if (R3Distance(position + translated, character->position) < 2.f) 
      {
        Creature_Attack(character, translated);
        translated = R3zero_vector;
      }
      return translated;

    case R3COW_CREATURE:
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
    default:
      return R3zero_vector;
  }
}
