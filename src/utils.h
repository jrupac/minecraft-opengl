#ifndef __UTILS_H__
#define __UTILS_H__

#include "cos426_opengl.h"
#include "minecraft.h"

#define M_2PI (2. * M_PI)
#define RAD2DEG (180. / M_PI)

// Fudge factor
#define EPS 1e-4

// Return square of x
#define SQ(x) ((x) * (x))
// Return sign of x, with 0 as even
#define SIGN(x) ((x) >= 0 ? 1 : -1)
// Return the absolute value of the given number
#define ABS(x) ((x) * SIGN(x))
// Clamp x to [low, high]
#define CLAMP(x, low, high) MIN(MAX((x), (low)), (high))
// Wrap x around so it's between [low, high]
#define WRAP(x, low, high) (((x) > (high)) ? (x) - (high) : \
                            ((x) < (low)) ? (x) - (low) : (x))  
// Proper rounding of a number
#define ROUND(x) (((x) < 0) ? (int)((x) - .5) : (int)((x) + .5))
// Debugging function to print any vector
#define PRINT_VECTOR(x) printf("%lf %lf %lf\n", (x).X(), (x).Y(), (x).Z());
 
extern int GLUTwindow;
extern int GLUTwindow_height;
extern int GLUTwindow_width;
extern double GLUTaspect;

double RandomNumber();

double GetTime();

void LoadMatrix(R3Matrix *matrix);

void GLUTDrawText(const R3Point& p, const char *s);

void GLUTDrawTitle(const R3Point& p, const char *s);

#endif
