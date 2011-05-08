
#include "R3.h"

#define DIRT_HEALTH 3
#define AIR_HEALTH -1
#define LEAF_HEALTH 1
#define WOOD_HEALTH 2
#define STONE_HEALTH 5

R3Block::
R3Block(const R3Box& box, int type)
  : box(box),
    blockType(type)
{
  switch (type)
  {
    case DIRT_BLOCK:
      health = DIRT_HEALTH;
      walkable = false;
      transparent = false;
      gravity = true;
      break;
    case AIR_BLOCK:
      health = AIR_HEALTH;
      walkable = true;
      transparent = true;
      gravity = false;
      break;
    case LEAF_BLOCK:
      health = LEAF_HEALTH;
      walkable = false;
      transparent = false;
      gravity = false;
      break;
    case WOOD_BLOCK:
      health = WOOD_HEALTH;
      walkable = false;
      transparent = false;
      gravity = false;
      break;
    case STONE_BLOCK:
      health = STONE_HEALTH;
      walkable = false;
      transparent = false;
      gravity = false;
      break;
  }
}

void R3Block::
Draw(void) const
{
  // Don't draw transparent air blocks
  if (!transparent)
    box.Draw();
}

void R3Block::
Draw(int face, bool isSelected) const
{

  // Don't draw transparent air blocks
  if (!transparent)
    box.DrawFace(face);

  if (!transparent && isSelected)
  {
      glDisable(GL_LIGHTING);
      glColor3d(0., 0., 0.);
      glLineWidth(15);
      glPolygonMode(GL_FRONT, GL_LINE);
      box.DrawFace(face);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1);
      glEnable(GL_LIGHTING);
  }
}

void R3Block::
changeBlock(int newType)
{
  blockType = newType;
  switch (newType)
  {
    case DIRT_BLOCK:
      health = DIRT_HEALTH;
      walkable = false;
      transparent = false;
      gravity = true;
      break;
    case AIR_BLOCK:
      health = AIR_HEALTH;
      walkable = true;
      transparent = true;
      gravity = false;
      break;
    case LEAF_BLOCK:
      health = LEAF_HEALTH;
      walkable = false;
      transparent = false;
      gravity = false;
      break;
    case WOOD_BLOCK:
      health = WOOD_HEALTH;
      walkable = false;
      transparent = false;
      gravity = false;
      break;
    case STONE_BLOCK:
      health = STONE_HEALTH;
      walkable = false;
      transparent = false;
      gravity = false;
      break;
  }
}



