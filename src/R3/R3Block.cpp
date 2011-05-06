
#include "R3.h"

#define DIRT_HEALTH 10
#define AIR_HEALTH -1
#define LEAF_HEALTH 1
#define BRANCH_HEALTH 2

R3Block::
R3Block(const R3Box& box, int type)
  : box(box),
    blockType(type)/*,
    this.x(x),
    this.y(y),
    this.z(z)*/
{
  if (type == DIRT_BLOCK)
  {
    health = DIRT_HEALTH;
    walkable = true;
    transparent = false;
  }
  if (type == AIR_BLOCK)
  {
    health = AIR_HEALTH;
    walkable = false;
    transparent = true;
  }
	if (type == LEAF_BLOCK) {
		health = LEAF_HEALTH;
		walkable = false;
		transparent = false;
	}
	if (type == BRANCH_BLOCK) {
		health = BRANCH_HEALTH;
		walkable = false;
		transparent = false;
	}
}

void R3Block::
Draw(void) const
{
  //fprintf(stderr, "blockType is %d\n", blockType);
  //If its an air block, don't draw, otherwise draw
  if (!transparent)
    box.Draw();
  //else
  //{
  //  fprintf(stderr, "doing nothing\n");
  //}
}




