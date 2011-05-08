// Include file for the R3 scene stuff

#define R3Rgb R2Pixel
#define MIDCHUNK = (CHUNKS-1)/2
#define CHUNKS 3

#include "R3/R3.h"
#include "R3Chunk.h"
#include <set>

// Constant definitions

//moved all this stuff to R3Chunk.h

//moved node to R3Chunk.h

// Scene graph definition

typedef struct 
{
  int x;
  int y;
  int z;
  R3Chunk* current;
} R3Index;

struct R3Scene {
 public:
  // Constructor functions
  R3Scene(void);

  // Access functions
  R3Node *Root(void) const;
  int NLights(void) const;
  R3Light *Light(int k) const;
  R3Camera& Camera(void);
  R3Box& BBox(void);
  
  R3Index getIndex(R3Point p);
  int UpdateScene(R3Point loc);
  R3Node* getBlock(R3Point loc);
  R3Chunk* LoadChunk(int x_chunk, int z_chunk);

  // I/O functions
  int Read(const char *filename, R3Node *root = NULL);
  //int WriteChunk(const char *filename); //remove me later

 public:
  R3Node *root;
  R3Chunk *terrain[CHUNKS][CHUNKS];
  R3Point start;
  vector<R3Light *> lights;
  R3Camera camera;
  R3Box bbox;
  R3Rgb background;
  R3Rgb ambient;
  //R3Character* main_character;
  set< pair<int, int> > generatedChunks;
};



// Inline functions 

inline R3Node *R3Scene::
Root(void) const
{
  // Return root node
  return root;
}



inline int R3Scene::
NLights(void) const
{
  // Return number of lights
  return lights.size();
}



inline R3Light *R3Scene::
Light(int k) const
{
  // Return kth light
  return lights[k];
}



inline R3Camera& R3Scene::
Camera(void) 
{
  // Return camera
  return camera;
}



inline R3Box& R3Scene::
BBox(void) 
{
  // Return bounding box 
  return bbox;
}



