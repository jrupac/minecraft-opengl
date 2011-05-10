/*
 *  materials.cpp 
 *
 */

#include "materials.h"

void MakeMaterials(R3Material **materials) 
{
	//printf("make materials\n");
	  //Load materials
	 R3Material *default_material = new R3Material();
	 default_material->ka = R3Rgb(0.2, 0.2, 0.2, 1);
	 default_material->kd = R3Rgb(0.5, 0.5, 0.5, 1);
	 default_material->ks = R3Rgb(0.5, 0.5, 0.5, 1);
	 default_material->kt = R3Rgb(0.0, 0.0, 0.0, 1);
	 default_material->emission = R3Rgb(0, 0, 0, 1);
	 default_material->shininess = 10;
	 default_material->indexofrefraction = 1;
	 default_material->texture = NULL;
	 default_material->id = 0;
	 
	 // Read texture image
	 default_material->texture = new R2Image();
	 if (!default_material->texture->Read(s_TexDefault)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	

	materials[DEFAULT] = default_material;
	 





	R3Material * alldirt_material = new R3Material();
	 alldirt_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	 alldirt_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	 alldirt_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	 alldirt_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	 alldirt_material->emission = R3Rgb(0, 0, 0, 0);
	 alldirt_material->shininess = 10;
	 alldirt_material->indexofrefraction = 1;
	 
	 // Read texture image
	 alldirt_material->texture = new R2Image();
	 if (!alldirt_material->texture->Read(s_TexAllDirt)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	

	 alldirt_material->id = 5;
     materials[ALLDIRT] = alldirt_material;
	 
	 R3Material *dirt_material = new R3Material();
	 dirt_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	 dirt_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	 dirt_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	 dirt_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	 dirt_material->emission = R3Rgb(0, 0, 0, 0);
	 dirt_material->shininess = 10;
	 dirt_material->indexofrefraction = 1;
	 
	 // Read texture image
	 dirt_material->texture = new R2Image();
	 if (!dirt_material->texture->Read(s_TexDirt)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	
	 dirt_material->id = 2;
     materials[DIRT] = dirt_material;
	 
	 R3Material *wood_material = new R3Material();
	 wood_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	 wood_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	 wood_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	 wood_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	 wood_material->emission = R3Rgb(0, 0, 0, 0);
	 wood_material->shininess = 10;
	 wood_material->indexofrefraction = 1;
	 
	 // Read texture image
	 wood_material->texture = new R2Image();
	 if (!wood_material->texture->Read(s_TexWood)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	

	 wood_material->id = 1;
     materials[WOOD] = wood_material; 
	
	 R3Material *grass_material = new R3Material();
	 grass_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	 grass_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	 grass_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	 grass_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	 grass_material->emission = R3Rgb(0, 0, 0, 0);
	 grass_material->shininess = 10;
	 grass_material->indexofrefraction = 1;
	 
	 // Read texture image
	 grass_material->texture = new R2Image();
	 if (!grass_material->texture->Read(s_TexGrass)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	
	 grass_material->id = 3;
     materials[GRASS] = grass_material; 
	
	 R3Material *leaf_material = new R3Material();
	 leaf_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	 leaf_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	 leaf_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	 leaf_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	 leaf_material->emission = R3Rgb(0, 0, 0, 0);
	 leaf_material->shininess = 10;
	 leaf_material->indexofrefraction = 1;
	 
	 // Read texture image
	 leaf_material->texture = new R2Image();
	 if (!leaf_material->texture->Read(s_TexLeaf)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	
	 leaf_material->id = 4;
     materials[LEAF] = leaf_material; 
	
	 R3Material *stone_material = new R3Material();
	 stone_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	 stone_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	 stone_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	 stone_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	 stone_material->emission = R3Rgb(0, 0, 0, 0);
	 stone_material->shininess = 10;
	 stone_material->indexofrefraction = 1;
	 
	 // Read texture image
	 stone_material->texture = new R2Image();
	 if (!stone_material->texture->Read(s_TexStone)) {
	 fprintf(stderr, "Unable to read texture from file");
	 //	return 0;
	 }	
	 stone_material->id = 5;
     materials[STONE] = stone_material;
	
	//call me mr.flintstone, cause i can make your bedrock
	R3Material *bedrock_material = new R3Material();
	bedrock_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	bedrock_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	bedrock_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	bedrock_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	bedrock_material->emission = R3Rgb(0, 0, 0, 0);
	bedrock_material->shininess = 10;
	bedrock_material->indexofrefraction = 1;
	
	// Read texture image
	bedrock_material->texture = new R2Image();
	if (!bedrock_material->texture->Read(s_TexBedrock)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	bedrock_material->id = 5;
	materials[BEDROCK] = bedrock_material;
	 //Icon materials

	R3Material *sand_material = new R3Material();
	sand_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	sand_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	sand_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	sand_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	sand_material->emission = R3Rgb(0, 0, 0, 0);
	sand_material->shininess = 10;
	sand_material->indexofrefraction = 1;
	
	// Read texture image
	sand_material->texture = new R2Image();
	if (!sand_material->texture->Read(s_TexSand)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	sand_material->id = 5;
	materials[SAND] = sand_material;
	
	R3Material *obdisidan_material = new R3Material();
	obdisidan_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	obdisidan_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	obdisidan_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	obdisidan_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	obdisidan_material->emission = R3Rgb(0, 0, 0, 0);
	obdisidan_material->shininess = 10;
	obdisidan_material->indexofrefraction = 1;
	
	// Read texture image
	obdisidan_material->texture = new R2Image();
	if (!obdisidan_material->texture->Read(s_TexObsidian)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	obdisidan_material->id = 5;
	materials[OBSIDIAN] = obdisidan_material;

	
	R3Material *gold_material = new R3Material();
	gold_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	gold_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	gold_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	gold_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	gold_material->emission = R3Rgb(0, 0, 0, 0);
	gold_material->shininess = 10;
	gold_material->indexofrefraction = 1;
	
	// Read texture image
	gold_material->texture = new R2Image();
	if (!gold_material->texture->Read(s_TexGold)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	gold_material->id = 5;
	materials[GOLD] = gold_material;

	R3Material *dirt_icon = new R3Material();
	dirt_icon->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	dirt_icon->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	dirt_icon->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	dirt_icon->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	dirt_icon->emission = R3Rgb(0, 0, 0, 0);
	dirt_icon->shininess = 10;
	dirt_icon->indexofrefraction = 1;
	
	// Read texture image
	dirt_icon->texture = new R2Image();
	if (!dirt_icon->texture->Read(s_TexDirtIcon)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	dirt_icon->id = 5;
	materials[DIRT_ICON] = dirt_icon;
	
	R3Material *stone_icon = new R3Material();
	stone_icon->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	stone_icon->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	stone_icon->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	stone_icon->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	stone_icon->emission = R3Rgb(0, 0, 0, 0);
	stone_icon->shininess = 10;
	stone_icon->indexofrefraction = 1;
	
	// Read texture image
	stone_icon->texture = new R2Image();
	if (!stone_icon->texture->Read(s_TexStoneIcon)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	stone_icon->id = 5;
	materials[STONE_ICON] = stone_icon;
	
	R3Material *wood_icon = new R3Material();
	wood_icon->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	wood_icon->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	wood_icon->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	wood_icon->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	wood_icon->emission = R3Rgb(0, 0, 0, 0);
	wood_icon->shininess = 10;
	wood_icon->indexofrefraction = 1;
	
	// Read texture image
	wood_icon->texture = new R2Image();
	if (!wood_icon->texture->Read(s_TexWoodIcon)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	wood_icon->id = 5;
	materials[WOOD_ICON] = wood_icon;

	R3Material *sand_icon = new R3Material();
	sand_icon->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	sand_icon->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	sand_icon->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	sand_icon->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	sand_icon->emission = R3Rgb(0, 0, 0, 0);
	sand_icon->shininess = 10;
	sand_icon->indexofrefraction = 1;
	
	// Read texture image
	sand_icon->texture = new R2Image();
	if (!sand_icon->texture->Read(s_TexSandIcon)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	sand_icon->id = 5;
	materials[SAND_ICON] = sand_icon;
	
	R3Material *obsidian_icon = new R3Material();
	obsidian_icon->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	obsidian_icon->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	obsidian_icon->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	obsidian_icon->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	obsidian_icon->emission = R3Rgb(0, 0, 0, 0);
	obsidian_icon->shininess = 10;
	obsidian_icon->indexofrefraction = 1;
	
	// Read texture image
	obsidian_icon->texture = new R2Image();
	if (!obsidian_icon->texture->Read(s_TexObsidianIcon)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	obsidian_icon->id = 5;
	materials[OBSIDIAN_ICON] = obsidian_icon;
	

	// Creature Materials

	R3Material *cow_material = new R3Material();
    cow_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    cow_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    cow_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    cow_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    cow_material->emission = R3Rgb(0, 0, 0, 0);
	cow_material->shininess = 10;
	cow_material->indexofrefraction = 1;
	
	// Read texture image
	cow_material->texture = new R2Image();
	if (!cow_material->texture->Read(s_TexCow)) {
		fprintf(stderr, "Unable to read texture from file");
	}	
	cow_material->id = 30;
	materials[COW] = cow_material;
	
	R3Material *deer_material = new R3Material();
	deer_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	deer_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	deer_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	deer_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	deer_material->emission = R3Rgb(0, 0, 0, 0);
	deer_material->shininess = 10;
	deer_material->indexofrefraction = 1;
	
	// Read texture image
	deer_material->texture = new R2Image();
	if (!deer_material->texture->Read(s_TexDeer)) {
		fprintf(stderr, "Unable to read texture from file");
	}	
	deer_material->id = 31;
	materials[DEER] = deer_material;

	R3Material *suicide_material = new R3Material();
	suicide_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	suicide_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	suicide_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	suicide_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	suicide_material->emission = R3Rgb(0, 0, 0, 0);
	suicide_material->shininess = 10;
	suicide_material->indexofrefraction = 1;
	
	// Read texture image
	suicide_material->texture = new R2Image();
	if (!suicide_material->texture->Read(s_TexSuicide)) {
		fprintf(stderr, "Unable to read texture from file");
	}	
	suicide_material->id = 31;
	materials[SUICIDE] = suicide_material;
	
	R3Material *logo_material = new R3Material();
	logo_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	logo_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	logo_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	logo_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	logo_material->emission = R3Rgb(0, 0, 0, 0);
	logo_material->shininess = 10;
	logo_material->indexofrefraction = 1;
	
	// Read texture image
	logo_material->texture = new R2Image();
	if (!logo_material->texture->Read(s_TexLogo)) {
		fprintf(stderr, "Unable to read texture from file");
	}	
	logo_material->id = 31;
	materials[LOGO] = logo_material;
}
