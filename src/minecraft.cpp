////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////

#include "minecraft.h"
#include "float.h"

////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

// Program arguments

static char *input_scene_name = NULL;
static char *output_image_name = NULL;

// Display variables

static R3Scene *scene = NULL;
static R3Camera camera;
static int show_faces = 1;
static int show_edges = 0;
static int show_bboxes = 0;
static int show_lights = 0;
static int show_camera = 0;
static int save_image = 0;
static int quit = 0;
static int INTERPOLATION = 1;
static R3Node *currentSelection = NULL;
static R3Vector currentNormal;
static R3Rgb background = R3Rgb(0.529, 0.807, 0.980, 1.);
static float picker_height = 10;
static float picker_width = 10;
static bool CAPTURE_MOUSE = false;
static R3Vector rot;	
static R3Character Main_Character;
static R3Vector towards;
double previous_time = 0;
double current_time = 0;
int FPS = 0;

R3Intersection closestintersect;

// Materials

R3Material *default_material;
R3Material *branch_material;
R3Material *dirt_material;
R3Material *grass_material;
R3Material *leaf_material;
R3Material *alldirt_material;
R3Material *stone_material;
R3Material *heart_material;
R3Material *empty_heart_material;

// GLUT variables 

static int GLUTwindow = 0;
static int GLUTwindow_height = 512;
static int GLUTwindow_width = 512;
static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTmodifiers = 0;

// OpenAL nonsense

#ifdef  __linux__
// Buffers to hold sound data
ALuint Buffer;
// Point source of sound
ALuint Source;
// Position of the source sound
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
// Velocity of the source sound
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
// Position of the listener
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  
                          0.0, 1.0, 0.0 };
#endif

////////////////////////////////////////////////////////////
// HELPER METHODS
////////////////////////////////////////////////////////////

static double GetTime(void)
{
#ifdef _WIN32
  // Return number of seconds since start of execution
  static int first = 1;
  static LARGE_INTEGER timefreq;
  static LARGE_INTEGER start_timevalue;

  // Check if this is the first time
  if (first) {
    // Initialize first time
    QueryPerformanceFrequency(&timefreq);
    QueryPerformanceCounter(&start_timevalue);
    first = 0;
    return 0;
  }
  else {
    // Return time since start
    LARGE_INTEGER current_timevalue;
    QueryPerformanceCounter(&current_timevalue);
    return ((double) current_timevalue.QuadPart - 
            (double) start_timevalue.QuadPart) / 
            (double) timefreq.QuadPart;
  }
#else
  // Return number of seconds since start of execution
  static int first = 1;
  static struct timeval start_timevalue;

  // Check if this is the first time
  if (first) {
    // Initialize first time
    gettimeofday(&start_timevalue, NULL);
    first = 0;
    return 0;
  }
  else {
    // Return time since start
    struct timeval current_timevalue;
    gettimeofday(&current_timevalue, NULL);
    int secs = current_timevalue.tv_sec - start_timevalue.tv_sec;
    int usecs = current_timevalue.tv_usec - start_timevalue.tv_usec;
    return (double) (secs + 1.0E-6F * usecs);
  }
#endif
}

bool LegalBlock(R3Index index)
{
  return (index.x >= 0 && index.x < CHUNK_X &&
          index.y >= 0 && index.y < CHUNK_Y &&
          index.z >= 0 && index.z < CHUNK_Z);
}

R3Index getChunkCoordinates(R3Point p)
{
  R3Vector diff = p - scene->start;  
  R3Index ret;

  ret.x = (int) diff[0];
  ret.y = (int) diff[1];
  ret.z = (int) diff[2];

  return ret;
}

void InterpolateMotion(R3Point *start, R3Vector direction)
{
  R3Index coords = getChunkCoordinates((*start) + (direction / INTERPOLATION));
  int fallIndex = -1;
  
  // Check if next potential location is legal
  if (!(scene->chunk[coords.x][coords.y-1][coords.z]->shape->block->walkable &&
      LegalBlock(coords)))
    return;

  *start += direction / INTERPOLATION;
  
  // Starting from below character, keep going down until you find something
  // you can't walk through 
  for (int i = coords.y - 2; i >= 0; i--)
  {
    R3Node *current = scene->chunk[coords.x][i][coords.z];
    R3Block *curBlock = current->shape->block;
    
    if (curBlock->walkable)
      fallIndex = i;
    else if (fallIndex != -1)
        break;
  }
 
  // The art of falling
  if (fallIndex != -1)
    (*start) -= (coords.y - fallIndex - 1) * R3posy_vector;
}

////////////////////////////////////////////////////////////
// GAME LOGIC CODE
////////////////////////////////////////////////////////////

void AlignReticle()
{
	currentSelection = NULL;
	R3Ray ray = R3Ray(camera.eye, towards);
	R3Intersection intersect;
	intersect.hit = false;
	R3Intersection closestIntersect;
	closestIntersect.hit = false;
	double smallest = DBL_MAX;

	for (int dz = 0; dz < CHUNK_Z; dz++) 
  {
		for (int dy = 0; dy < CHUNK_Y; dy++) 
    {
			for (int dx = 0; dx < CHUNK_X; dx++) 
      {
        R3Node *currentNode = scene->chunk[dx][dy][dz];

				if (currentNode->shape->block->getBlockType() != AIR_BLOCK) {
					intersect = IntersectBox(ray, currentNode->shape->block->getBox());

					if (intersect.hit && intersect.t < smallest) {
            smallest = intersect.t;
						closestIntersect.t = intersect.t;
						closestIntersect.hit = intersect.hit;
						closestIntersect.position = intersect.position;
						closestIntersect.normal = intersect.normal;
						closestIntersect.node = currentNode;
						//printf("find intersection");
					}
				}
			}
		}
	}

	if (closestIntersect.hit) {
		currentSelection = closestIntersect.node;
		currentNormal = closestIntersect.normal;
	//	printf("%f, %f, %f\n", currentNormal.X(), currentNormal.Y(), currentNormal.Z());
	//if (currentSelection->shape->block->blockType == AIR_BLOCK) {
	//	printf("notair\n");
	//}
	}
}	

void AddBlock()
{
    // Force-update detection of current selection
    AlignReticle();

    // If no selection, don't add a block
    if (!currentSelection)
        return;

	//If selection is a block that cannot be built on, return (ie. leaf)
	if (currentSelection->shape->block->blockType == LEAF_BLOCK) {
		return;
	}
	
	
	R3Node *added = NULL;
	if (currentNormal.Y() == 1.0) {
    // Create new block and add it to the scene graph
    added = scene->
	chunk[currentSelection->shape->block->dx][currentSelection->shape->block->dy + 1][currentSelection->shape->block->dz];
	}
	else if (currentNormal.X() == 1.0) {
		added = scene->
		chunk[currentSelection->shape->block->dx+1][currentSelection->shape->block->dy][currentSelection->shape->block->dz];
	}
	else if (currentNormal.X() == -1.0) {
		added = scene->
		chunk[currentSelection->shape->block->dx-1][currentSelection->shape->block->dy][currentSelection->shape->block->dz];
	}
	else if (currentNormal.Z() == 1.0) {
		added = scene->
		chunk[currentSelection->shape->block->dx][currentSelection->shape->block->dy][currentSelection->shape->block->dz+1];
	}
	else if (currentNormal.Z() == -1.0) {
		added = scene->
		chunk[currentSelection->shape->block->dx][currentSelection->shape->block->dy + 1][currentSelection->shape->block->dz-1];
	}
	
	//R3Block *newBlock;
		
	if (added != NULL) {
	//add new block only if upper block is an air block
	if (added->shape->block->blockType == AIR_BLOCK) {
		//newBlock = new R3Block(upper->shape->block->getBox(), currentSelection->shape->block->getBlockType());
		added->shape->block->blockType = currentSelection->shape->block->blockType;
		//upper->shape->block = newBlock;
		added->shape->block->health = currentSelection->shape->block->health;
		added->shape->block->walkable = currentSelection->shape->block->walkable;
		added->shape->block->transparent = currentSelection->shape->block->transparent;
		added->shape->block->gravity = currentSelection->shape->block->gravity;
	}
	}
	else {
		return;
	}
}

void RemoveBlock() {
	R3Node *lower = currentSelection;
	R3Node *upper = scene->
	chunk[currentSelection->shape->block->dx][currentSelection->shape->block->dy + 1][currentSelection->shape->block->dz];
	if (upper->shape->block->gravity == false) {
		//printf("remove block");
		currentSelection->shape->block->blockType = AIR_BLOCK;
		currentSelection->shape->block->health = -1;
		currentSelection->shape->block->walkable = true;
		currentSelection->shape->block->transparent = true;
		currentSelection->shape->block->gravity = false;
		return;
	}
	else {
	while (upper->shape->block->gravity == true) {
		lower->shape->block->blockType = upper->shape->block->blockType;
		lower->shape->block->health = upper->shape->block->health;
		lower->shape->block->walkable = upper->shape->block->walkable;
		lower->shape->block->transparent = upper->shape->block->transparent;
		lower->shape->block->gravity = upper->shape->block->gravity;
		/*scene->chunk[lower->shape->block->dx][lower->shape->block->dy][lower->shape->block->dz] =
			scene->chunk[upper->shape->block->dx][upper->shape->block->dy][upper->shape->block->dz];*/
		
		lower = upper;
		upper = scene->chunk[upper->shape->block->dx][upper->shape->block->dy + 1][upper->shape->block->dz];
	}
		lower->shape->block->blockType = AIR_BLOCK;
		lower->shape->block->health = -1;
		lower->shape->block->walkable = true;
		lower->shape->block->transparent = true;
		lower->shape->block->gravity = false;
	}

}

void DrawHUD() 
{  
    glLineWidth(3);
    glColor3d(.1, .1, .1);

    // Make "+" ticker on the middle of the screen 
    glBegin(GL_LINES);
    glVertex2f((GLUTwindow_width / 2) - picker_width, GLUTwindow_height / 2); 
    glVertex2f((GLUTwindow_width / 2) + picker_width, GLUTwindow_height / 2); 
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(GLUTwindow_width / 2, (GLUTwindow_height / 2) - picker_height); 
    glVertex2f(GLUTwindow_width / 2, (GLUTwindow_height / 2) + picker_height); 
    glEnd();

    glLineWidth(1);

    // Draw text
    GLUTDrawText(R3Point(5, 13, 0), "Minecraft v0.0.0.0.0.1/2");
    GLUTDrawText(R3Point(400, 13, 0), "FPS: " );
    stringstream ss;
    ss << FPS;
    GLUTDrawText(R3Point(450, 13, 0), ss.str().c_str()); 

    // Draw bottom pane
    glColor3d(.7, .7, .7);

    // Load default material
    LoadMaterial(default_material);

    //Draw Hearts
    DrawHUD_Hearts();

    //Draw Inventory
    DrawHUD_Inventory();

    // This is just temporary text
    glColor3d(.1, .1, .1);
    GLUTDrawText(R3Point(GLUTwindow_width / 2 - 10, GLUTwindow_height - 70, 0), "I am a dock!");
}

void DrawHUD_Hearts() 
{
    int x = GLUTwindow_width;
    int y = GLUTwindow_height;

    glColor3d(.9, .1, .1);

    glPushMatrix();
    glTranslatef(.25 * x, .9 * y, 0);

    for (int i = 0; i < Main_Character.MaxHealth; i++) 
    {
        if (i >= Main_Character.Health)
            glColor3d(.7, .7, .7);

        glBegin(GL_QUADS);
        glVertex2f(0, 0); 
        glVertex2f(x/64, - y/64); 
        glVertex2f(x/32, 0); 
        glVertex2f(x/64, y/64); 
        glEnd();
        glTranslatef(5 * x / 128, 0, 0);
    }

    glPopMatrix();
    glColor3d(.7, .7, .7);
}

void DrawHUD_Inventory() 
{
    int x = GLUTwindow_width;
    int y = GLUTwindow_height;
    int boxWidth = .0525 * x;
    int boxHeight = .0625 * y;

    glPushMatrix();
    glTranslatef(.1875 * x, .99 * y, 0.);

    for (int i = 4; i <= 12; i++) 
    {
        glTranslatef(x / 16, 0, 0.);
        glBegin(GL_QUADS);
        glVertex2f(0, 0); 
        glVertex2f(0, -boxHeight); 
        glVertex2f(boxWidth, -boxHeight); 
        glVertex2f(boxWidth, 0); 
        glEnd();
    }

    glPopMatrix(); 
}

void MakeMaterials(void) 
{
    //Load materials
    default_material = new R3Material();
    default_material->ka = R3Rgb(0.2, 0.2, 0.2, 1);
    default_material->kd = R3Rgb(0.5, 0.5, 0.5, 1);
    default_material->ks = R3Rgb(0.5, 0.5, 0.5, 1);
    default_material->kt = R3Rgb(0.0, 0.0, 0.0, 1);
    default_material->emission = R3Rgb(0, 0, 0, 1);
    default_material->shininess = 10;
    default_material->indexofrefraction = 1;
    default_material->texture = NULL;
    default_material->id = 0;

    alldirt_material = new R3Material();
    alldirt_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    alldirt_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    alldirt_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    alldirt_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    alldirt_material->emission = R3Rgb(0, 0, 0, 0);
    alldirt_material->shininess = 10;
    alldirt_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char alldirt[] = "input/alldirt.jpg";

    // Read texture image
    alldirt_material->texture = new R2Image();
    if (!alldirt_material->texture->Read(alldirt)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    alldirt_material->id = 5;

    dirt_material = new R3Material();
    dirt_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    dirt_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    dirt_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    dirt_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    dirt_material->emission = R3Rgb(0, 0, 0, 0);
    dirt_material->shininess = 10;
    dirt_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char dirt[] = "input/dirt.jpg";

    // Read texture image
    dirt_material->texture = new R2Image();
    if (!dirt_material->texture->Read(dirt)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    dirt_material->id = 2;

    branch_material = new R3Material();
    branch_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    branch_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    branch_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    branch_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    branch_material->emission = R3Rgb(0, 0, 0, 0);
    branch_material->shininess = 10;
    branch_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char branch[] = "input/branch.jpg";

    // Read texture image
    branch_material->texture = new R2Image();
    if (!branch_material->texture->Read(branch)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    //branch_material->texture = NULL;
    branch_material->id = 1;

    grass_material = new R3Material();
    grass_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    grass_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    grass_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    grass_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    grass_material->emission = R3Rgb(0, 0, 0, 0);
    grass_material->shininess = 10;
    grass_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char grass[] = "input/grass.jpg";

    // Read texture image
    grass_material->texture = new R2Image();
    if (!grass_material->texture->Read(grass)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    grass_material->id = 3;

    leaf_material = new R3Material();
    leaf_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    leaf_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    leaf_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    leaf_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    leaf_material->emission = R3Rgb(0, 0, 0, 0);
    leaf_material->shininess = 10;
    leaf_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char leaf[] = "input/leaf.jpg";

    // Read texture image
    leaf_material->texture = new R2Image();
    if (!leaf_material->texture->Read(leaf)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    leaf_material->id = 4;

	stone_material = new R3Material();
    stone_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    stone_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    stone_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    stone_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    stone_material->emission = R3Rgb(0, 0, 0, 0);
    stone_material->shininess = 10;
    stone_material->indexofrefraction = 1;
	
    //	char buffer[] = "input/checker.bmp";
    char stone[] = "input/stone.jpg";
	
    // Read texture image
    stone_material->texture = new R2Image();
    if (!stone_material->texture->Read(stone)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    stone_material->id = 5;
	
    heart_material = new R3Material();
    heart_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    heart_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    heart_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    heart_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    heart_material->emission = R3Rgb(0, 0, 0, 0);
    heart_material->shininess = 10;
    heart_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char heart[] = "input/alldirt.jpg";

    // Read texture image
    heart_material->texture = new R2Image();
    if (!heart_material->texture->Read(heart)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    heart_material->id = 9;

    empty_heart_material = new R3Material();
    empty_heart_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
    empty_heart_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
    empty_heart_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
    empty_heart_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
    empty_heart_material->emission = R3Rgb(0, 0, 0, 0);
    empty_heart_material->shininess = 10;
    empty_heart_material->indexofrefraction = 1;

    //	char buffer[] = "input/checker.bmp";
    char empty_heart[] = "input/grass.jpg";

    // Read texture image
    empty_heart_material->texture = new R2Image();
    if (!empty_heart_material->texture->Read(empty_heart)) {
        fprintf(stderr, "Unable to read texture from file");
        //	return 0;
    }	
    empty_heart_material->id = 10;
}

////////////////////////////////////////////////////////////
// SCENE DRAWING CODE
////////////////////////////////////////////////////////////

void DrawShape(R3Shape *shape)
{
    // Check shape type
    if (shape->type == R3_BOX_SHAPE) { }
    else if (shape->type == R3_SPHERE_SHAPE) shape->sphere->Draw();
    else if (shape->type == R3_CYLINDER_SHAPE) shape->cylinder->Draw();
    else if (shape->type == R3_CONE_SHAPE) shape->cone->Draw();
    else if (shape->type == R3_MESH_SHAPE) shape->mesh->Draw();
    else if (shape->type == R3_SEGMENT_SHAPE) shape->segment->Draw();
    else if (shape->type == R3_BLOCK_SHAPE)
    {
        //if (shape->block->getBlockType() != AIR_BLOCK)
        //    shape->block->Draw();
        if (shape->block->getBlockType() == LEAF_BLOCK) 
        {
            LoadMaterial(leaf_material);
            shape->block->getBox().Draw();
        }
        else if (shape->block->getBlockType() == DIRT_BLOCK) 
        {
            if (shape->block->getUpper()->getBlockType() == AIR_BLOCK) {
                LoadMaterial(dirt_material);
                shape->block->getBox().DrawFace(0);
                shape->block->getBox().DrawFace(1);
                shape->block->getBox().DrawFace(2);
                LoadMaterial(grass_material);
                shape->block->getBox().DrawFace(3);
                LoadMaterial(dirt_material);
                shape->block->getBox().DrawFace(4);
                shape->block->getBox().DrawFace(5);
            }
            else {
                LoadMaterial(alldirt_material);
                shape->block->getBox().Draw();
            }
        }
        else if (shape->block->getBlockType() == BRANCH_BLOCK) 
        {
            LoadMaterial(branch_material);
            shape->block->getBox().Draw();
        }
		else if (shape->block->getBlockType() == STONE_BLOCK) {
			LoadMaterial(stone_material);
			shape->block->getBox().Draw();
		}
    }

    else fprintf(stderr, "Unrecognized shape type: %d\n", shape->type);
}

void DrawShape(R3Node *node)
{
    // Check shape type
    if (node->shape->type == R3_BOX_SHAPE) 
        node->shape->box->Draw();
    else if (node->shape->type == R3_SPHERE_SHAPE) node->shape->sphere->Draw();
    else if (node->shape->type == R3_CYLINDER_SHAPE)node->shape->cylinder->Draw();
    else if (node->shape->type == R3_CONE_SHAPE) node->shape->cone->Draw();
    else if (node->shape->type == R3_MESH_SHAPE) node->shape->mesh->Draw();
    else if (node->shape->type == R3_SEGMENT_SHAPE) node->shape->segment->Draw();
    else if (node->shape->type == R3_BLOCK_SHAPE)
    {
        //if (shape->block->getBlockType() != AIR_BLOCK)
        //  shape->block->Draw();
        if (node->shape->block->getBlockType() == LEAF_BLOCK) 
        {
            //  LoadMaterial(water_material);
            node->shape->block->getBox().Draw();
        }
        else if (node->shape->block->getBlockType() == WATER_BLOCK) 
        {
            //	  LoadMaterial(water_material);
            node->shape->block->getBox().Draw();
        }
        else if (node->shape->block->getBlockType() == DIRT_BLOCK) 
        {
            //	  LoadMaterial(dirt_material);
            node->shape->block->getBox().Draw();
        }
    }

    else fprintf(stderr, "Unrecognized shape type: %d\n", node->shape->type);
}

void LoadMatrix(R3Matrix *matrix)
{
    // Multiply matrix by top of stack
    // Take transpose of matrix because OpenGL represents vectors with 
    // column-vectors and R3 represents them with row-vectors
    R3Matrix m = matrix->Transpose();
    glMultMatrixd((double *) &m);
}

void LoadMaterial(R3Material *material) 
{
    GLfloat c[4];
    // Check if same as current
    static R3Material *current_material = NULL;

    if (material == current_material) return;
    current_material = material;

    // Compute "opacity"
    double opacity = 1 - material->kt.Luminance();

    // Load ambient
    c[0] = material->ka[0];
    c[1] = material->ka[1];
    c[2] = material->ka[2];
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

    // Load diffuse
    c[0] = material->kd[0];
    c[1] = material->kd[1];
    c[2] = material->kd[2];
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

    // Load specular
    c[0] = material->ks[0];
    c[1] = material->ks[1];
    c[2] = material->ks[2];
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

    // Load emission
    c[0] = material->emission.Red();
    c[1] = material->emission.Green();
    c[2] = material->emission.Blue();
    c[3] = opacity;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

    // Load shininess
    c[0] = material->shininess;
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, c[0]);

    // Load texture
    if (material->texture) 
    {
        if (material->texture_index <= 0) 
        {
            // Create texture in OpenGL
            GLuint texture_index;
            glGenTextures(1, &texture_index);
            material->texture_index = (int) texture_index;
            glBindTexture(GL_TEXTURE_2D, material->texture_index); 
            R2Image *image = material->texture;
            int npixels = image->NPixels();
            R2Pixel *pixels = image->Pixels();
            GLfloat *buffer = new GLfloat [ 4 * npixels ];
            R2Pixel *pixelsp = pixels;
            GLfloat *bufferp = buffer;
            for (int j = 0; j < npixels; j++) 
            { 
                *(bufferp++) = pixelsp->Red();
                *(bufferp++) = pixelsp->Green();
                *(bufferp++) = pixelsp->Blue();
                *(bufferp++) = pixelsp->Alpha();
                pixelsp++;
            }
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            //glTexImage2D(GL_TEXTURE_2D, 0, 4, image->Width(), image->Height(), 0, GL_RGBA, GL_FLOAT, buffer);
			gluBuild2DMipmaps( GL_TEXTURE_2D, 3, image->Width(), image->Height(), GL_RGBA, GL_FLOAT, buffer);
            delete [] buffer;
        }

        // Select texture
        glBindTexture(GL_TEXTURE_2D, material->texture_index); 
        glEnable(GL_TEXTURE_2D);
    }
    else 
    {
        glDisable(GL_TEXTURE_2D);
    }

    // Enable blending for transparent surfaces
    if (opacity < 1) 
    {
        glDepthMask(false);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }
    else 
    {
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glDepthMask(true);
    }
}

void LoadCamera(R3Camera *camera)
{
	towards.Reset(0, 0, -1);
  float n[16], *nn = n;

  // Set projection transformation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(2 * 180.0 * camera->yfov / M_PI, 
          (GLdouble) GLUTwindow_width /(GLdouble) GLUTwindow_height, 
          0.01, 10000);

  // Set camera transformation
  R3Point e = camera->eye;

  glMatrixMode(GL_MODELVIEW);

  glRotatef(rot[1] * 180 / M_PI, 1., 0., 0);
  glRotatef(rot[0] * 180 / M_PI, 0., 1., 0);
  glTranslated(-e[0], -e[1], -e[2]);

  // Get the current transformation matrix
  glGetFloatv(GL_MODELVIEW_MATRIX, n);

  R3Matrix rotation = R3Matrix(nn);
  towards.Transform(rotation);
  towards.Normalize();
}

void LoadLights(R3Scene *scene)
{
    GLfloat buffer[4];

    // Load ambient light
    static GLfloat ambient[4];
    ambient[0] = scene->ambient[0];
    ambient[1] = scene->ambient[1];
    ambient[2] = scene->ambient[2];
    ambient[3] = 1;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    // Load scene lights
    for (int i = 0; i < (int) scene->lights.size(); i++) {
        R3Light *light = scene->lights[i];
        int index = GL_LIGHT0 + i;

        // Temporarily disable light
        glDisable(index);

        // Load color
        buffer[0] = light->color[0];
        buffer[1] = light->color[1];
        buffer[2] = light->color[2];
        buffer[3] = 1.0;
        glLightfv(index, GL_DIFFUSE, buffer);
        glLightfv(index, GL_SPECULAR, buffer);

        // Load attenuation with distance
        buffer[0] = light->constant_attenuation;
        buffer[1] = light->linear_attenuation;
        buffer[2] = light->quadratic_attenuation;
        glLightf(index, GL_CONSTANT_ATTENUATION, buffer[0]);
        glLightf(index, GL_LINEAR_ATTENUATION, buffer[1]);
        glLightf(index, GL_QUADRATIC_ATTENUATION, buffer[2]);

        // Load spot light behavior
        buffer[0] = 180.0 * light->angle_cutoff / M_PI;
        buffer[1] = light->angle_attenuation;
        glLightf(index, GL_SPOT_CUTOFF, buffer[0]);
        glLightf(index, GL_SPOT_EXPONENT, buffer[1]);

        // Load positions/directions
        if (light->type == R3_DIRECTIONAL_LIGHT) {
            // Load direction
            buffer[0] = -(light->direction.X());
            buffer[1] = -(light->direction.Y());
            buffer[2] = -(light->direction.Z());
            buffer[3] = 0.0;
            glLightfv(index, GL_POSITION, buffer);
        }
        else if (light->type == R3_POINT_LIGHT) {
            // Load position
            buffer[0] = light->position.X();
            buffer[1] = light->position.Y();
            buffer[2] = light->position.Z();
            buffer[3] = 1.0;
            glLightfv(index, GL_POSITION, buffer);
        }
        else if (light->type == R3_SPOT_LIGHT) {
            // Load position
            buffer[0] = light->position.X();
            buffer[1] = light->position.Y();
            buffer[2] = light->position.Z();
            buffer[3] = 1.0;
            glLightfv(index, GL_POSITION, buffer);

            // Load direction
            buffer[0] = light->direction.X();
            buffer[1] = light->direction.Y();
            buffer[2] = light->direction.Z();
            buffer[3] = 1.0;
            glLightfv(index, GL_SPOT_DIRECTION, buffer);
        }
        else if (light->type == R3_AREA_LIGHT) {
            // Load position
            buffer[0] = light->position.X();
            buffer[1] = light->position.Y();
            buffer[2] = light->position.Z();
            buffer[3] = 1.0;
            glLightfv(index, GL_POSITION, buffer);

            // Load direction
            buffer[0] = light->direction.X();
            buffer[1] = light->direction.Y();
            buffer[2] = light->direction.Z();
            buffer[3] = 1.0;
            glLightfv(index, GL_SPOT_DIRECTION, buffer);
        }
        else {
            fprintf(stderr, "Unrecognized light type: %d\n", light->type);
            return;
        }

        // Enable light
        glEnable(index);
    }
}

void DrawNode(R3Scene *scene, R3Node *node)
{

    // Push transformation onto stack
    glPushMatrix();
    LoadMatrix(&node->transformation);

    // Load material
    if (node->material) LoadMaterial(node->material);


    // Draw shape
    if (node->shape) 
    {
        // Draw face
        DrawShape(node->shape);
        // If this is the current selected block, highlight it
		
        if (currentSelection == node)
        {
            glDisable(GL_LIGHTING);
            glColor3d(0., 0., 0.);
            glLineWidth(15);
            glPolygonMode(GL_FRONT, GL_LINE);
            DrawShape(node->shape);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glLineWidth(1);
            glEnable(GL_LIGHTING);
        }
    }

    // Draw children nodes
    for (int i = 0; i < (int) node->children.size(); i++) 
        DrawNode(scene, node->children[i]);

    // Restore previous transformation
    glPopMatrix();

    // Show bounding box
    if (show_bboxes) {
        GLboolean lighting = glIsEnabled(GL_LIGHTING);
        glDisable(GL_LIGHTING);
        node->bbox.Outline();
        if (lighting) glEnable(GL_LIGHTING);
    }
}

void DrawScene(R3Scene *scene) 
{
    // Draw nodes recursively
    DrawNode(scene, scene->root);

    /* ADDED: draw the new array of nodes */
    for (int dz = 0; dz < CHUNK_Z; dz++)
        for (int dy = 0; dy < CHUNK_Y; dy++)
            for (int dx = 0; dx < CHUNK_X; dx++)
                DrawNode(scene, scene->chunk[dx][dy][dz]); 
    //what the fuck is scene here for
}

////////////////////////////////////////////////////////////
// GLUT USER INTERFACE CODE
////////////////////////////////////////////////////////////

void GLUTMainLoop(void)
{
    // Run main loop -- never returns 
    glutMainLoop();
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

void GLUTSaveImage(const char *filename)
{ 
    // Create image
    R2Image image(GLUTwindow_width, GLUTwindow_height);

    // Read screen into buffer
    GLfloat *pixels = new GLfloat [ 3 * GLUTwindow_width * GLUTwindow_height ];
    glReadPixels(0, 0, GLUTwindow_width, GLUTwindow_height, GL_RGB, GL_FLOAT, pixels);

    // Load pixels from frame buffer
    GLfloat *pixelsp = pixels;
    for (int j = 0; j < GLUTwindow_height; j++) {
        for (int i = 0; i < GLUTwindow_width; i++) {
            double r = (double) *(pixelsp++);
            double g = (double) *(pixelsp++);
            double b = (double) *(pixelsp++);
            R2Pixel pixel(r, g, b, 1);
            image.SetPixel(i, j, pixel);
        }
    }

    // Write image to file
    image.Write(filename);

    // Delete buffer
    delete [] pixels;
}

void GLUTStop(void)
{
    // Destroy window 
    glutDestroyWindow(GLUTwindow);

    // Delete scene
    delete scene;

    // Exit
    exit(0);
}

void GLUTResize(int w, int h)
{
    // Resize window
    glViewport(0, 0, w, h);

    // Resize camera vertical field of view to match aspect ratio of viewport
    camera.yfov = atan(tan(camera.xfov) * (double) h/ (double) w); 

    // Remember window size 
    GLUTwindow_width = w;
    GLUTwindow_height = h;

    // Redraw
    glutPostRedisplay();
}

void GLUTRedraw(void)
{

    // Time stuff
    current_time = GetTime();
    // program just started up?
    if (previous_time == 0) previous_time = current_time;
    double delta_time = current_time - previous_time;
    FPS = (int)1.0/delta_time;
    

    // Clear window 
    glClearColor(background[0], background[1], background[2], background[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Initialize selected block
    AlignReticle();

    // Load camera
    LoadCamera(&camera);

    // Load scene lights
    LoadLights(scene);

    // Draw scene surfaces
    if (show_faces) {
        glEnable(GL_LIGHTING);
        DrawScene(scene);
    }

    // Draw scene edges
    if (show_edges) {
        glDisable(GL_LIGHTING);
        glColor3d(0., 0., 0.);
        glLineWidth(3);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        DrawScene(scene);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1);
    }

    // Save image
    if (save_image) {
        char image_name[256];
        static int image_number = 1;
        for (;;) {
            sprintf(image_name, "image%d.jpg", image_number++);
            FILE *fp = fopen(image_name, "r");
            if (!fp) break; 
            else fclose(fp);
        }
        GLUTSaveImage(image_name);
        printf("Saved %s\n", image_name);
        save_image = 0;
    }

    // Quit here so that can save image before exit
    if (quit) {
        if (output_image_name) GLUTSaveImage(output_image_name);
        //write new scene based on changes

        if (!scene->WriteChunk(input_scene_name))
        {
            fprintf(stderr, "WARNING: Couldn't save new scene!!!\n");
        }
        GLUTStop();
    }

    // Get into 2D mode
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, GLUTwindow_width, GLUTwindow_height, 0, 0, 1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING); 

    DrawHUD();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); 

    // Swap buffers 
    glutSwapBuffers();
    
    previous_time = current_time;
}    

void GLUTPassiveMotion(int x, int y)
{
    if (!CAPTURE_MOUSE) 
    {
        GLUTmouse[0] = x;
        GLUTmouse[1] = y;
        return;
    }

    // Invert y coordinate
    y = GLUTwindow_height - y;

    // Compute mouse movement
    int dx = x - GLUTmouse[0];
    int dy = y - GLUTmouse[1];

    if (x < 30 || x > GLUTwindow_width - 30 || 
            y < 30 || y > GLUTwindow_height - 30)
    {
        glutWarpPointer(GLUTwindow_width / 2, GLUTwindow_height / 2);
        GLUTmouse[0] = x;
        GLUTmouse[1] = y;
        glutPostRedisplay();
        return;
    }

    if (x == GLUTwindow_width / 2 && y == GLUTwindow_height / 2)
    {
        GLUTmouse[0] = x;
        GLUTmouse[1] = y;
        glutPostRedisplay();
        return;
    }

    // Compute ratios to approximate angle of rotation
    double vx = (double) dx / (double) GLUTwindow_width;
    double vy = (double) dy / (double) GLUTwindow_height;

    // Update and clamp x-rotation and y-rotation
    rot[0] = WRAP(rot[0] + vx, -M_2PI, M_PI);
    rot[1] = CLAMP(rot[1] - vy, -M_PI / 2, M_PI / 2);

    GLUTmouse[0] = x;
    GLUTmouse[1] = y;
    glutPostRedisplay();
}

void GLUTMouse(int button, int state, int x, int y)
{
    // Invert y coordinate
    y = GLUTwindow_height - y;

    // Process mouse button event
    if (state == GLUT_DOWN) 
    {
        if (button == GLUT_LEFT_BUTTON) 
        {
						//printf("left click\n");
            if (CAPTURE_MOUSE == false) 
            {
                glutSetCursor(GLUT_CURSOR_NONE);
                CAPTURE_MOUSE = true;
            }
			if (currentSelection != NULL) {
				currentSelection->shape->block->health--;
				if (currentSelection->shape->block->health <= 0) {
				//	printf("out of health\n");
					RemoveBlock();
				}
			}

        }
        else if (button == GLUT_MIDDLE_BUTTON) 
        {
        }
        else if (button == GLUT_RIGHT_BUTTON) 
        {
			AddBlock();
        }
    }

    // Remember button state 
    int b = (button == GLUT_LEFT_BUTTON) ? 0 : ((button == GLUT_MIDDLE_BUTTON) ? 1 : 2);
    GLUTbutton[b] = (state == GLUT_DOWN) ? 1 : 0;

    // Remember modifiers 
    GLUTmodifiers = glutGetModifiers();

    // Remember mouse position 
    GLUTmouse[0] = x;
    GLUTmouse[1] = y;
    glutPostRedisplay();
}

void GLUTMouseEntry(int state)
{
    // Some window managers do not report accurate callbacks for mouse leave and
    // enters. Fortunately, X does!

    switch (state)
    {
        case GLUT_ENTERED: 
            break;
        case GLUT_LEFT:    
            break;
    }
}

void GLUTSpecial(int key, int x, int y)
{
    // Invert y coordinate
    y = GLUTwindow_height - y;

    // Process keyboard button event 
    switch (key) {
        case GLUT_KEY_F1:
            save_image = 1;
            break;
    }

    // Remember mouse position 
    GLUTmouse[0] = x;
    GLUTmouse[1] = y;

    // Remember modifiers 
    GLUTmodifiers = glutGetModifiers();

    // Redraw
    glutPostRedisplay();
}

void GLUTKeyboard(unsigned char key, int x, int y)
{
    // Invert y coordinate
    y = GLUTwindow_height - y;

    // Process keyboard button event 
    switch (key) 
    {
  /*      case 'B':
        case 'b':
            AddBlock();
            break;*/

        case 'C':
        case 'c':
            show_camera = !show_camera;
            break;

        case 'E':
        case 'e':
            show_edges = !show_edges;
            break;

        case 'F':
        case 'f':
            show_faces = !show_faces;
            break;

        case 'L':
        case 'l':
            show_lights = !show_lights;
            break;

        case 'Q':
        case 'q':
            quit = 1;
            break;

        case 27: // ESCAPE
            CAPTURE_MOUSE = false;
            // Restore cursor
            glutSetCursor(GLUT_CURSOR_INHERIT);
            break;

        case 'w': 
            InterpolateMotion(&(camera.eye), 
                    -(cos(rot[0]) * R3posz_point - sin(rot[0]) * R3posx_point));
            break;

        case 's': 
            InterpolateMotion(&(camera.eye), 
                    (cos(rot[0]) * R3posz_point - sin(rot[0]) * R3posx_point));
            break;

        case 'd': 
            InterpolateMotion(&(camera.eye), 
                    (sin(rot[0]) * R3posz_point + cos(rot[0]) * R3posx_point).Vector());
            break;

        case 'a': 
            InterpolateMotion(&(camera.eye), 
                    -(sin(rot[0]) * R3posz_point + cos(rot[0]) * R3posx_point).Vector());
            break;

        case ' ': 
            printf("camera %g %g %g  %g %g %g  %g  %g %g \n",
                    camera.eye[0], camera.eye[1], camera.eye[2], 
                    rot[0], rot[1], rot[2],
                    camera.xfov, camera.neardist, camera.fardist); 
            break; 
    }

    // Remember mouse position 
    GLUTmouse[0] = x;
    GLUTmouse[1] = y;

    // Remember modifiers 
    GLUTmodifiers = glutGetModifiers();

    // Redraw
    glutPostRedisplay();
}

void GLUTCommand(int cmd)
{
    // Execute command
    switch (cmd) {
        case DISPLAY_FACE_TOGGLE_COMMAND: show_faces = !show_faces; break;
        case DISPLAY_EDGE_TOGGLE_COMMAND: show_edges = !show_edges; break;
        case DISPLAY_BBOXES_TOGGLE_COMMAND: show_bboxes = !show_bboxes; break;
        case DISPLAY_LIGHTS_TOGGLE_COMMAND: show_lights = !show_lights; break;
        case DISPLAY_CAMERA_TOGGLE_COMMAND: show_camera = !show_camera; break;
        case SAVE_IMAGE_COMMAND: save_image = 1; break;
        case QUIT_COMMAND: quit = 1; break;
    }

    // Mark window for redraw
    glutPostRedisplay();
}

void GLUTCreateMenu(void)
{
  /*  // Display sub-menu
    int display_menu = glutCreateMenu(GLUTCommand);
    glutAddMenuEntry("Faces (F)", DISPLAY_FACE_TOGGLE_COMMAND);
    glutAddMenuEntry("Edges (E)", DISPLAY_EDGE_TOGGLE_COMMAND);
    glutAddMenuEntry("Bounding boxes (B)", DISPLAY_BBOXES_TOGGLE_COMMAND);
    glutAddMenuEntry("Lights (L)", DISPLAY_LIGHTS_TOGGLE_COMMAND);
    glutAddMenuEntry("Camera (C)", DISPLAY_CAMERA_TOGGLE_COMMAND);

    // Main menu
    glutCreateMenu(GLUTCommand);
    glutAddSubMenu("Display", display_menu);
    glutAddMenuEntry("Save Image (F1)", SAVE_IMAGE_COMMAND);
    glutAddMenuEntry("Quit", QUIT_COMMAND);

    // Attach main menu to right mouse button
    glutAttachMenu(GLUT_RIGHT_BUTTON);*/
}

void GLUTInit(int *argc, char **argv)
{
    // Open window 
    glutInit(argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(GLUTwindow_width, GLUTwindow_height);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // | GLUT_STENCIL
    GLUTwindow = glutCreateWindow("OpenGL Viewer");

    // Initialize GLUT callback functions 
    glutReshapeFunc(GLUTResize);
    glutDisplayFunc(GLUTRedraw);
    glutKeyboardFunc(GLUTKeyboard);
    glutSpecialFunc(GLUTSpecial);
    glutMouseFunc(GLUTMouse);
    glutPassiveMotionFunc(GLUTPassiveMotion);
    glutEntryFunc(GLUTMouseEntry);

    // Initialize graphics modes 
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Create menus
    GLUTCreateMenu();

    // Initialize OpenGL drawing modes
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(true);

    //Initialize Character
    Main_Character = R3Character();
}

////////////////////////////////////////////////////////////
// SCENE READING
////////////////////////////////////////////////////////////

R3Scene *ReadScene(const char *filename)
{
    // Allocate scene
    R3Scene *scene = new R3Scene();
    if (!scene) {
        fprintf(stderr, "Unable to allocate scene\n");
        return NULL;
    }

    // Read file
    if (!scene->Read(filename)) {
        fprintf(stderr, "Unable to read scene from %s\n", filename);
        return NULL;
    }

    // Remember initial camera
    camera = scene->camera;

    // Set up default properties of camera
    camera.towards = R3negz_vector;
    camera.eye = R3zero_point + 2. * R3posy_vector;
    camera.up = R3posy_vector;
    camera.right = R3posx_vector;

    // Return scene
    return scene;
}

////////////////////////////////////////////////////////////
// PROGRAM ARGUMENT PARSING
////////////////////////////////////////////////////////////

int ParseArgs(int argc, char **argv)
{
    // Innocent until proven guilty
    int print_usage = 0;

    // Parse arguments
    argc--; argv++;
    while (argc > 0) {
        if ((*argv)[0] == '-') {
            if (!strcmp(*argv, "-help")) { print_usage = 1; }
            else if (!strcmp(*argv, "-exit_immediately")) { quit = 1; }
            else if (!strcmp(*argv, "-output_image")) { argc--; argv++; output_image_name = *argv; }
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
        else {
            if (!input_scene_name) input_scene_name = *argv;
            else { fprintf(stderr, "Invalid program argument: %s", *argv); exit(1); }
            argv++; argc--;
        }
    }

    // Check input_scene_name
    if (!input_scene_name || print_usage) {
        printf("Usage: rayview <input.scn> [-maxdepth <int>] [-v]\n");
        return 0;
    }

    // Return OK status 
    return 1;
}

////////////////////////////////////////////////////////////
// OPENAL CODE 
////////////////////////////////////////////////////////////

#ifdef __linux__

ALboolean LoadALData()
{
    // Variables to load into.
    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;

    // Load wav data into a buffer
    alGenBuffers(1, &Buffer);
    
    if(alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    alutLoadWAVFile((ALbyte*) "sounds/ninth.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffer, format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);

    // Bind the buffer with the source
    alGenSources(1, &Source);

    if(alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    alSourcei (Source, AL_BUFFER,   Buffer   );
    alSourcef (Source, AL_PITCH,    1.0      );
    alSourcef (Source, AL_GAIN,     1.0      );
    alSourcefv(Source, AL_POSITION, SourcePos);
    alSourcefv(Source, AL_VELOCITY, SourceVel);
    alSourcei (Source, AL_LOOPING,  AL_TRUE  );

    if(alGetError() == AL_NO_ERROR)
        return AL_TRUE;
    return AL_FALSE;
}

void SetListenerValues()
{
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
}

void KillALData()
{
    alDeleteBuffers(1, &Buffer);
    alDeleteSources(1, &Source);
    alutExit();
}

#endif

////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    // Initialize GLUT
    GLUTInit(&argc, argv);

#ifdef __linux__
    alutInit(NULL, 0);
    alGetError();

    if(LoadALData() == AL_FALSE)
        printf("Error loading WAV sound data.");

    SetListenerValues();
    
    // Bind KillALData to run at exit
    atexit(KillALData);

    // Begin sound playback
    alSourcePlay(Source);
#endif

    MakeMaterials();
    // Parse program arguments
    if (!ParseArgs(argc, argv)) exit(1);

    // Read scene
    scene = ReadScene(input_scene_name);
    if (!scene) exit(-1);

    // Run GLUT interface
    GLUTMainLoop();

    // Return success 
    return 0;
}


