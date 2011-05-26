#ifndef __UI_H__
#define __UI_H__

#include "materials.h"
#include "R3/R3.h"
#include "utils.h"

void DrawHUD(R3Character *Main_Character, bool dead, int FPS);

void DrawHUD_Hearts(R3Character *Main_Character); 

void DrawHUD_Inventory(R3Character *Main_Character); 

#endif
