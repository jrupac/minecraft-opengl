// Include the appropriate OpenGL and GLUT headers

/* Windows */
#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef USE_OPENGL32
#  define USE_OPENGL32
#endif

#ifndef NOMINMAX
#  define NOMINMAX
# endif

#include <windows.h>
#include <GL/glut.h>

/* Mac OS */
#elif defined(__APPLE__) 
#include <GLUT/glut.h>
#include <sys/time.h>

/* Linux */
#else 
#include <sys/time.h>
#include <GL/glut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#endif

