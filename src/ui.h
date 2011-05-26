#ifndef __UI_H__
#define __UI_H__

#include "minecraft.h"
#include "materials.h"
#include "R3/R3.h"
#include "utils.h"

void DrawHUD(R3Character *Main_Character, bool dead, int FPS);

void DrawHUD_Hearts(R3Character *Main_Character); 

void DrawHUD_Inventory(R3Character *Main_Character); 

void DisplayStartMenu();

void DisplayDeathMenu(); 

void DisplayWonMenu(); 

void DisplayControls(); 

#endif
