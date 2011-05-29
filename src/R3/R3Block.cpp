#include "R3Block.h"
#include "utils.h"

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
	  case GOLD_BLOCK:
		  health = GOLD_HEALTH;
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
  if (!transparent)
  {
    if (isSelected)
    {
      // Save all attributes
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      
      // Clear the stencil
      glClearStencil(0);
      glClear(GL_STENCIL_BUFFER_BIT);
      glEnable(GL_STENCIL_TEST);
      
      glStencilFunc(GL_ALWAYS, 1, 0xFFFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      
      // Render whole face in black first
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glColor3f(0.0f, 0.0f, 0.0f);
      box.DrawFace(face);

      glDisable(GL_LIGHTING);
      glStencilFunc(GL_NOTEQUAL, 1, 0xFFFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      
      // Now render just the outline
      glLineWidth(5);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      box.DrawFace(face);
     
      // Restore all attributes
      glPopAttrib();
    }
    else box.DrawFace(face);
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
	  case GOLD_BLOCK:
		  health = GOLD_HEALTH;
		  walkable = false;
		  transparent = false;
		  gravity = false;
		  break;
  }
}



