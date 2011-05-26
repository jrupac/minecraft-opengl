#ifndef __DRAW_H__
#define __DRAW_H__

#include "minecraft.h"

void DrawSceneFullOptimization(R3Scene *scene);

void DrawSceneViewFrustrumOnly(R3Scene *scene); 

void DrawSceneOcclusionOnly(R3Scene *scene); 

void DrawSceneNone(R3Scene *scene); 

#endif
