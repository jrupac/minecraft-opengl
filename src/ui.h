#ifndef __UI_H__
#define __UI_H__

#include "minecraft.h"
#include "materials.h"
#include "R3/R3.h"
#include "utils.h"

void DrawHUD(R3Character *mainCharacter, bool dead, int FPS);

void DrawHUD_Hearts(R3Character *mainCharacter); 

void DrawHUD_Inventory(R3Character *mainCharacter); 

void DisplayStartMenu();

void DisplayDeathMenu(); 

void DisplayWonMenu(); 

void DisplayControls(); 

#endif
