
#include "R3.h"


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
	case SAND_BLOCK:
		  health = SAND_HEALTH;
		  walkable = false;
		  transparent = false;
		  gravity = true;
		  break;
	  case OBSIDIAN_BLOCK:
		  health = OBSIDIAN_HEALTH;
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
	//printf("Why is this called?!!?\n");
  // Don't draw transparent air blocks
	
  if (!transparent) {
	  
    box.DrawFace(face);
  }

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
	
	glDisable(GL_COLOR_MATERIAL);
}


void R3Block::
Draw(int face, bool isSelected, double ratio) const
{
	/*
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glColor3f(1, ratio, ratio);*/
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
	
	glDisable(GL_COLOR_MATERIAL);
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
	  case SAND_BLOCK:
		  health = SAND_HEALTH;
		  walkable = false;
		  transparent = false;
		  gravity = true;
		  break;
	  case OBSIDIAN_BLOCK:
		  health = OBSIDIAN_HEALTH;
		  walkable = false;
		  transparent = false;
		  gravity = false;
		  break;
		  
  }
}



