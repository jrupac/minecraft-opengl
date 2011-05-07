#ifndef __MINECRAFT_H__
#define __MINECRAFT_H__

#include "R3/R3.h"
#include "R3Scene.h"
#include "raytrace.h"
#include "cos426_opengl.h"
#include <sstream>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

#define M_2PI (2. *M_PI)
#define RAD2DEG (180. / M_PI)

// Return sign of x, with 0 as even
#define SIGN(x) ((x) >= 0 ? 1 : -1)
// Clamp x to [low, high]
#define CLAMP(x, low, high) MIN(MAX((x), (low)), (high))
// Wrap x around so it's between [low, high]
#define WRAP(x, low, high) (((x) > (high)) ? (x) - (high) : \
                            ((x) < (low)) ? (x) - (low) : (x))  
 
// GLUT command list

enum {
  DISPLAY_FACE_TOGGLE_COMMAND,
  DISPLAY_EDGE_TOGGLE_COMMAND,
  DISPLAY_BBOXES_TOGGLE_COMMAND,
  DISPLAY_LIGHTS_TOGGLE_COMMAND,
  DISPLAY_CAMERA_TOGGLE_COMMAND,
  SAVE_IMAGE_COMMAND,
  QUIT_COMMAND,
};

typedef struct 
{
  int x;
  int y;
  int z;
} R3Index;

////////////////////////////////////////////////////////////
// HELPER METHODS
////////////////////////////////////////////////////////////

double GetTime(void);
bool LegalBlock(R3Index index);
R3Index getChunkCoordinates(R3Point p);
void InterpolateMotion(R3Point *start, R3Vector direction);

////////////////////////////////////////////////////////////
// GAME LOGIC CODE
////////////////////////////////////////////////////////////

void AlignReticle();
void AddBlock();
void DrawHUD(); 
void DrawHUD_Hearts();
void DrawHUD_Inventory();
void LoadMaterial(R3Material *material);
void MakeMaterials(void);

////////////////////////////////////////////////////////////
// SCENE DRAWING CODE
////////////////////////////////////////////////////////////

void DrawShape(R3Shape *shape);
void DrawShape(R3Node *node);
void LoadMatrix(R3Matrix *matrix);
void LoadMaterial(R3Material *material);
void LoadCamera(R3Camera *camera);
void LoadLights(R3Scene *scene);
void DrawNode(R3Scene *scene, R3Node *node);
void DrawScene(R3Scene *scene);

////////////////////////////////////////////////////////////
// GLUT USER INTERFACE CODE
////////////////////////////////////////////////////////////

void GLUTMainLoop(void);
void GLUTDrawText(const R3Point& p, const char *s);
void GLUTSaveImage(const char *filename);
void GLUTStop(void);
void GLUTResize(int w, int h);
void GLUTRedraw(void);
void GLUTPassiveMotion(int x, int y);
void GLUTMouse(int button, int state, int x, int y);
void GLUTMouseEntry(int state);
void GLUTSpecial(int key, int x, int y);
void GLUTKeyboard(unsigned char key, int x, int y);
void GLUTCommand(int cmd);
void GLUTCreateMenu(void);
void GLUTInit(int *argc, char **argv);

////////////////////////////////////////////////////////////
// SCENE READING
////////////////////////////////////////////////////////////

R3Scene *ReadScene(const char *filename);

////////////////////////////////////////////////////////////
// PROGRAM ARGUMENT PARSING
////////////////////////////////////////////////////////////

int ParseArgs(int argc, char **argv);

#endif
