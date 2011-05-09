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

void R3Creature::
	UpdateCreatureFall(R3Character *character) {
		
	//printf("Enter Update Fall.\n");
		//printf("Animal Pos: (%f, %f, %f)\n", position.X(), position.Y(), position.Z());
		R3Index coords = getChunkCoordinates(position);

		int fallIndex = -1;
		for (int i = coords.y; i >= 0; i--)
		{
			R3Node *current = coords.current->chunk[coords.x][i][coords.z];
			//printf("Coords: (%d, %d, %d)\n", coords.x, i, coords.z);
			//if(current==NULL) printf("ITS NULL. \n");
			R3Block *curBlock = current->shape->block;
			//printf("I: %d\n", i);
			if (curBlock->walkable)
				fallIndex = i;
			else if (fallIndex != -1)
				break;
		}	

		// The art of falling
		if (fallIndex != -1) { 	  
			position -= (coords.y - fallIndex) * R3posy_vector;
		}
		
	//printf("Exit Update Fall..\n");

}
R3Vector R3Creature::
	UpdateCreature(R3Character *character)
{

	
	//printf("Enter Update Creature.\n");
	R3Vector box_translation( (position-R3Point(.5, .5, .5)).Point() - box.minpt);

	box.Translate(box_translation);

	











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
      translated /= 80;

      // Give me some space!
      if (R3Distance(position + translated, character->position) < 2.f) 
      {
        Creature_Attack(character, translated);
        translated = R3zero_vector;
      }
	  
	//printf("Exit Update Creature.\n");
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
