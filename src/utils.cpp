#include "utils.h"

int GLUTwindow = 0;
int GLUTwindow_height = 512;
int GLUTwindow_width = 512;
double GLUTaspect = 1.;

double RandomNumber(void)
{
// Windows
#ifdef _WIN32

	// Seed random number generator
	static int first = 1;

	if (first) 
  {
		srand(GetTickCount());
		first = 0;
	}

	// Return random number
	int r1 = rand();
	double r2 = ((double) rand()) / ((double) RAND_MAX);
	return (r1 + r2) / ((double) RAND_MAX);

// Mac OS and Linux
#else 

	// Seed random number generator
	static int first = 1;

	if (first) 
  {
		struct timeval timevalue;
		gettimeofday(&timevalue, 0);
		srand48(timevalue.tv_usec);
		first = 0;
	}

	// Return random number
	return drand48();

#endif
}

double GetTime(void)
{
// Windows
#ifdef _WIN32

	// Return number of seconds since start of execution
	static int first = 1;
	static LARGE_INTEGER timefreq;
	static LARGE_INTEGER start_timevalue;

	// Check if this is the first time
	if (first) 
	{
		// Initialize first time
		QueryPerformanceFrequency(&timefreq);
		QueryPerformanceCounter(&start_timevalue);
		first = 0;
		return 0;
	}
	else 
	{
    // Return time since start
    LARGE_INTEGER current_timevalue;
    QueryPerformanceCounter(&current_timevalue);
    return ((double) current_timevalue.QuadPart - 
        (double) start_timevalue.QuadPart) / (double) timefreq.QuadPart;
  }

// Linux or Mac OS 
#else
	// Return number of seconds since start of execution
	static int first = 1;
	static struct timeval start_timevalue;

	// Check if this is the first time
	if (first) 
	{
		// Initialize first time
		gettimeofday(&start_timevalue, NULL);
		first = 0;
		return 0;
	}
	else 
	{
		// Return time since start
		struct timeval current_timevalue;
		gettimeofday(&current_timevalue, NULL);
		int secs = current_timevalue.tv_sec - start_timevalue.tv_sec;
		int usecs = current_timevalue.tv_usec - start_timevalue.tv_usec;
		return (double) (secs + 1.0E-6F * usecs);
	}

#endif
}

void LoadMatrix(R3Matrix *matrix)
{
	// Multiply matrix by top of stack
	// Take transpose of matrix because OpenGL represents vectors with 
	// column-vectors and R3 represents them with row-vectors
	R3Matrix m = matrix->Transpose();
	glMultMatrixd((double *) &m);
}

void GLUTDrawText(const R3Point& p, const char *s)
{
	// Draw text string s and position p
	glRasterPos3d(p[0], p[1], p[2]);
#ifndef __CYGWIN__
	while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *(s++));
#else
	while (*s) glutBitmapCharacter((void*)7, *(s++));
#endif
}

void GLUTDrawTitle(const R3Point& p, const char *s)
{
	// Draw text string s and position p
	glRasterPos3d(p[0], p[1], p[2]);
#ifndef __CYGWIN__
	while (*s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *(s++));
#else
	while (*s) glutBitmapCharacter((void*)7, *(s++));
#endif
}
