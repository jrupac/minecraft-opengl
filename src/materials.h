#ifndef __MATERIALS_H__
#define __MATERIALS_H__

typedef enum 
{
	DEFAULT,
	DIRT,
	ALLDIRT,
	GRASS,
	LEAF,
	STONE,
	WOOD,
	SAND,
	BEDROCK,
	OBSIDIAN,
	GOLD,
	COW,
	DEER,
	SUICIDE,
	DIRT_ICON,
	STONE_ICON,
	WOOD_ICON,
	SAND_ICON,
	OBSIDIAN_ICON,
	LOGO
} R3Texture;

#include "strings.h"
#include "R3Chunk.h"

extern R3Material **materials;

void MakeMaterials(R3Material **materials);

void LoadMaterial(R3Material *material);

#endif
