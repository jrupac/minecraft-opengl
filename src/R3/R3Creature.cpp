#include "R3Creature.h"
#include "minecraft.h"

R3Creature::
R3Creature(void)
{
	Health = 2;
	MaxHealth = 2;
	position = R3Point(2, .5, -3);
	box = R3Box((position-R3Point(.5, .5, .5)).Point(), position+R3Point(.5, .5, .5));
}

double RandomNumber(void)
{
#ifdef _WIN32
    // Seed random number generator
    static int first = 1;

    if (first) {
        srand(GetTickCount());
        first = 0;
    }

    // Return random number
    int r1 = rand();
    double r2 = ((double) rand()) / ((double) RAND_MAX);
    return (r1 + r2) / ((double) RAND_MAX);
#else 
    // Seed random number generator
    static int first = 1;
    if (first) {
        struct timeval timevalue;
        gettimeofday(&timevalue, 0);
        srand48(timevalue.tv_usec);
        first = 0;
    }

    // Return random number
    return drand48();
#endif
}

void R3Creature::
UpdateCreature(R3Character *character)
{
	double x = RandomNumber();
	double y = RandomNumber();
	double z = RandomNumber();

	x = (2 * x - 1) / 20;
	y = 0;
	z = (2 * z - 1) / 20;
	
	R3Vector translated;
	translated = character->position - position;
	translated.SetY(0);
	translated.Normalize();
	translated /= 20;
  
  // Give me some space!
  if (R3Distance(position + translated, character->position) >= 2.f)
  {
    position = position + translated;
    box.Translate(translated);
  }
}
