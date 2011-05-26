#ifndef __UTILS_H__
#define __UTILS_H__

#include "cos426_opengl.h"
#include "R3/R3.h"

extern int GLUTwindow;
extern int GLUTwindow_height;
extern int GLUTwindow_width;

double RandomNumber();

double GetTime();

void LoadMatrix(R3Matrix *matrix);

void GLUTDrawText(const R3Point& p, const char *s);

void GLUTDrawTitle(const R3Point& p, const char *s);

#endif
