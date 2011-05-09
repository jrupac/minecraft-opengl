#ifndef __MINECRAFT_H__
#define __MINECRAFT_H__

#include <sstream>

#include "R3/R3.h"
#include "R3Scene.h"
#include "raytrace.h"
#include "cos426_opengl.h"

#include <map>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

#define M_2PI (2. *M_PI)
#define RAD2DEG (180. / M_PI)
#define EPS 1e-4

// Amount of anti-aliasing
#define ACSIZE 0

// Return sign of x, with 0 as even
#define SIGN(x) ((x) >= 0 ? 1 : -1)
// Return the absolute value of the given number
#define ABS(x) ((x) * SIGN(x))
// Clamp x to [low, high]
#define CLAMP(x, low, high) MIN(MAX((x), (low)), (high))
// Wrap x around so it's between [low, high]
#define WRAP(x, low, high) (((x) > (high)) ? (x) - (high) : \
                            ((x) < (low)) ? (x) - (low) : (x))  
// Debugging function to print any vector
#define PRINT_VECTOR(x) printf("%lf %lf %lf\n", (x).X(), (x).Y(), (x).Z());
 
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
  GLfloat x, y;
} jitter_point;

const jitter_point j2[] =
{
  { 0.246490,  0.249999},
  {-0.246490, -0.249999}
};

const jitter_point j4[] =
{
  {-0.208147,  0.353730},
  { 0.203849, -0.353780},
  {-0.292626, -0.149945},
  { 0.296924,  0.149994}
};

////////////////////////////////////////////////////////////
// HELPER METHODS
////////////////////////////////////////////////////////////

double RandomNumber(void);
double GetTime(void);
R3Index getChunkCoordinates(R3Point p);
R3Vector InterpolateMotion(R3Point *start, R3Vector direction, bool isCharacter);
void InterpolateJump(R3Point *start, R3Vector direction);

////////////////////////////////////////////////////////////
// GAME LOGIC CODE
////////////////////////////////////////////////////////////

void EndGame();
void AlignReticle();
void AddBlock(int block);
void DrawHUD(); 
void DrawHUD_Hearts();
void DrawHUD_Inventory();
void LoadMaterial(R3Material *material);
void ChangeHealth(R3Character *character, int delta);
void ChangeHealth(R3Creature *creature, int delta);
void ChangeHealth(R3Block *block, int delta);

////////////////////////////////////////////////////////////
// SCENE DRAWING CODE
////////////////////////////////////////////////////////////

void DrawShape(R3Shape *shape);
void FindMaterial(R3Block *block, bool isTop);
void LoadMatrix(R3Matrix *matrix);
void LoadMaterial(R3Material *material);
void LoadCamera(R3Camera *camera);
void LoadLights(R3Scene *scene);
void DrawNode(R3Scene *scene, R3Node *node);
void DrawScene(R3Scene *scene);
void DrawCreatures();
void UpdateCharacter();

// Creature Functions

void RemoveCreature();
void MoveCharacter(R3Vector translated, double d);
void DrawCreatures();

////////////////////////////////////////////////////////////
// GLUT USER INTERFACE CODE
////////////////////////////////////////////////////////////

void GLUTMainLoop(void);
void GLUTIdleFunction(void);
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
