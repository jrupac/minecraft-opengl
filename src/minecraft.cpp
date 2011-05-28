///////////////////////////////////////////////////////////////////////////////
//                         MINECRAFT - v.0.0.1                               //
//                             Written by:                                   //
//                             Rohan Bansal                                  //
//                            Dmitry Drutskoy                                //
//                            Ajay Roopakalu                                 //
//                              Sarah Tang                                   //
//                                                                           //
//                              Forked by:                                   //
//                            Ajay Roopakalu                                 //
//                                                                           //
// This game is a tribute to famous Minecraft game written by Markus "Notch" // 
// Persson and Jens Bergensten. That game, written in Java, is still still   //
// being actively developed, having first launched in May of 2009. Here is   //
// our tribute to the game, in C++ and OpenGL.                               //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// INCLUDE FILES
///////////////////////////////////////////////////////////////////////////////

#include "minecraft.h"

///////////////////////////////////////////////////////////////////////////////
// Program arguments 
///////////////////////////////////////////////////////////////////////////////

static char *input_scene_name = NULL;

///////////////////////////////////////////////////////////////////////////////
// Display Variables
///////////////////////////////////////////////////////////////////////////////

static enum ANTIALIAS antialias = HIGH;
static enum GAMESTATE state = STARTMENU;
static bool CAPTURE_MOUSE = false;
static bool show_faces = true;
static bool show_edges = false;
static bool quit = false;
static bool toSave = false;
static unsigned int FPS = 0;
static unsigned int currentLevel = 0;
static unsigned int num_creatures_to_make = 0;
static unsigned int MAX_num_creatures = 250;
static double previous_time = 0;
static double current_time = 0;
static double previousLevelTime = 0;
static double distanceToRenderCreature = 30;
static double distanceToUpdateCreature = 20;
static double AlignReticleDistance = 10;
static double timeBetweenLevels = 20;
static R3Rgb background = R3Rgb(0.529, 0.807, 0.980, 1.);
static R3Scene *scene = NULL;
static R3Vector currentNormal;
static R3Vector rot;	
static VecCreature creatures;
static VecCreatureIt currentSelectedCreatureIt;
static R3Character *mainCharacter;
static map<int, const jitter_point *> j;

///////////////////////////////////////////////////////////////////////////////
// Extern Variables
///////////////////////////////////////////////////////////////////////////////

enum CULLING culling = FULL; 
R3Node *currentSelection = NULL;
R3Camera camera = R3Camera();
double dotProductCutOff = 0.0;

///////////////////////////////////////////////////////////////////////////////
// GLUT Variables 
///////////////////////////////////////////////////////////////////////////////

static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTmodifiers = 0;

///////////////////////////////////////////////////////////////////////////////
// OpenAL Buffers
///////////////////////////////////////////////////////////////////////////////

#ifdef  __linux__

ALuint Buffer;
ALuint Source;
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  
                          0.0, 1.0, 0.0 };

#endif

///////////////////////////////////////////////////////////////////////////////
// HELPER METHODS
///////////////////////////////////////////////////////////////////////////////

R3Index getChunkCoordinates(R3Point p) 
{ 
  return scene->getIndex(p);
}

bool LegalPositions(R3Point *start, R3Vector direction, R3Index *c, bool isCharacter)
{
  double magnitude = direction.Length();
  double index = 1.0;
  R3Index coords;
  
  // Check every block along the way
  do 
  {
    coords = (*c) = getChunkCoordinates((*start) + direction / magnitude);
    
    if (isCharacter)
    {
      if (!coords.current || !coords.current->chunk[coords.x][coords.y-2][coords.z]->shape->block->walkable)
        return false;
    }
    else
    {
      if (!coords.current || !coords.current->chunk[coords.x][coords.y][coords.z]->shape->block->walkable)
        return false;
    }

  } while (++index < magnitude);

  return true;
}

R3Vector InterpolateMotion(R3Point *start, R3Vector direction, bool isCharacter)
{
	R3Point initial = *start;
	R3Index coords;
	int fallIndex = -1;

	// Check if every position from here to there is legal; otherwise, don't move
  if (!LegalPositions(start, direction, &coords, isCharacter))
      return R3zero_vector;

  // Update position
	*start += direction;

  // Update world
	if (isCharacter)	
		scene->UpdateScene(*start);

	// Starting from below character, keep going down until you find something
	// you can't walk through 
	for (int i = (isCharacter) ? coords.y - 2 : coords.y - 1; i >= 0; i--)
	{
		R3Node *current = coords.current->chunk[coords.x][i][coords.z];
		R3Block *curBlock = current->shape->block;

		if (curBlock->walkable)
			fallIndex = i;
		else if (fallIndex != -1)
			break;
	}	

	for (int i = (!isCharacter) ? coords.y : coords.y - 1; i >= 0; i--)
	{
		R3Node *current = coords.current->chunk[coords.x][i][coords.z];
		R3Block *curBlock = current->shape->block;

		if (curBlock->walkable)
			fallIndex = i;
		else if (fallIndex != -1)
			break;
	}	

	// The art of falling
	if (fallIndex != -1) 
  { 
		if (isCharacter) 
			(*start) -= (coords.y - fallIndex - 2) * R3posy_vector;
    else
			(*start) -= (coords.y - fallIndex) * R3posy_vector;
	}

	return (*start) - initial;
}

void InterpolateJump(R3Point *start, R3Vector direction)
{
	R3Index coords = getChunkCoordinates((*start) + direction);
	int NUM_FRAMES_PER_JUMP = 12;

	// If the next location is not walkable...
	if (!(coords.current->chunk[coords.x][coords.y-2][coords.z]->shape->block->walkable))
	{
		// ...but the one above it is, then jump
		if (coords.current->chunk[coords.x][coords.y-1][coords.z]->shape->block->walkable)
		{
      // Smoother upwards movement
      for (int i = 0; i < NUM_FRAMES_PER_JUMP / 2; i++)
      {
        (*start) += R3posy_vector / (NUM_FRAMES_PER_JUMP / 2);
        GLUTRedraw();
      }

			*start += direction;
			scene->UpdateScene(*start);
		}
		return;
	}

	// What goes up...
	for (int i = 0; i < NUM_FRAMES_PER_JUMP / 2; i++)
	{
		(*start) += R3posy_vector / (NUM_FRAMES_PER_JUMP / 2);
		GLUTRedraw();
	}
	// ...must come down.
	for (int i = 0; i < NUM_FRAMES_PER_JUMP / 2; i++)
	{
		(*start) -= R3posy_vector / (NUM_FRAMES_PER_JUMP / 2);
		GLUTRedraw();
	}

}

///////////////////////////////////////////////////////////////////////////////
// GAME LOGIC CODE
///////////////////////////////////////////////////////////////////////////////

void AlignReticle()
{
	currentSelection = NULL;
	currentSelectedCreatureIt = creatures.end();
	R3Ray ray = R3Ray(camera.eye, camera.towards);
	VecCreatureIt it;
	double smallest = DBL_MAX;
	double dt = 0.0;
	R3Intersection intersect;
	R3Intersection closestIntersect;
  // Cache previously seen block for improved performance
  R3Node *cached = NULL;

	intersect.hit = false;
	closestIntersect.hit = false;

  // Loop until you go beyond the boundaries of the currently loaded map
	while (true)
	{
    dt += .5;
		R3Index cur = getChunkCoordinates(ray.Point(dt));
    
    // Break if you reach too far with this ray
		if (!cur.current || dt > AlignReticleDistance)
			break;

		R3Node *curNode = cur.current->chunk[cur.x][cur.y][cur.z];
    
    // Check if the current node is the same as the previous
    if (curNode == cached)
      continue;
    cached = curNode;
    
    // Temporary solution for stale pointers causing segfaults
    try 
    {
      // Intersect with the first non-transparent box
      if (!curNode->shape->block->transparent)
      {
        closestIntersect = IntersectBox(ray, curNode->shape->block->getBox());
        smallest = closestIntersect.t;
        closestIntersect.node = curNode;
        break;
      }
    } catch(...) { }
	}

	// Find if the reticle is on a creature
	for (it = creatures.begin(); it < creatures.end(); it++)
	{
		intersect = IntersectBox(ray, (*it)->box);
		if (intersect.hit && intersect.t < smallest) 
		{
			smallest = intersect.t;
			currentSelectedCreatureIt = it;
		}
	}

	// If we've hit a creature, don't update currentNormal and currentSelection
	if (currentSelectedCreatureIt != creatures.end())
		return;

	// Set closest intersection of the terrain
	if (closestIntersect.hit) 
	{
		currentSelection = closestIntersect.node;
		currentNormal = closestIntersect.normal;
	}
}	

void AddBlock(int block)
{
	// If no selection, don't add a block
	if (!currentSelection)
		return;

	R3Block *currentBlock = currentSelection->shape->block;
	R3Node *added = NULL;

	//If selection is a block that cannot be built on, return (ie. leaf)
	if (currentBlock->blockType == LEAF_BLOCK)
		return;

	R3Point p = currentBlock->box.Centroid();
	R3Index i;

	if (currentNormal.Y() == 1.0)
		p[1]++;
	if (currentNormal.Y() == -1.0)
		p[1]--;
	else if (currentNormal.X() == 1.0)
		p[0]++;
	else if (currentNormal.X() == -1.0)
		p[0]--;
	else if (currentNormal.Z() == 1.0)
		p[2]++;
	else if (currentNormal.Z() == -1.0)
		p[2]--;

	R3Point bottom = p;
	bottom[1]--;

	R3Index j = scene->getIndex(bottom);
	R3Node *bottomNode = j.current->chunk[j.x][j.y][j.z];

	if (block == DIRT_BLOCK || block == SAND_BLOCK) 
  {
		while (bottomNode->shape->block->getBlockType() == AIR_BLOCK) 
    {
			p[1]--;
			bottom[1]--;
			j = scene->getIndex(bottom);
			bottomNode = j.current->chunk[j.x][j.y][j.z];
		}
	}

	i = scene->getIndex(p);
	added = i.current->chunk[i.x][i.y][i.z];

	if (added) 
	{
		// Add new block only if new block is an air block
		if (added->shape->block->blockType == AIR_BLOCK) 
			added->shape->block->changeBlock(block);
	}
}

void RemoveBlock() 
{
	// Don't destroy bedrock
	if (currentSelection->shape->block->dy == 0) 
			return;

	R3Node *lower = currentSelection;
	R3Block *lowerBlock = lower->shape->block;

	// Get block information for block above this one
	R3Point p = lowerBlock->box.Centroid();
	p[1]++;
	R3Index i;
	i = scene->getIndex(p);
  
	R3Node *upper = i.current->chunk[i.x][i.y][i.z];
	R3Block *upperBlock = upper->shape->block;

	if (!upper->shape->block->gravity) 
		lowerBlock->changeBlock(AIR_BLOCK);
	else 
	{
    // Update all blocks above this one
		while (upperBlock->gravity) 
		{
			lowerBlock->changeBlock(upperBlock->blockType);
			lower = upper;
			lowerBlock = lower->shape->block;

      // Get block information for block above this one
			R3Point p = upperBlock->box.Centroid();
			R3Index i;
			p[1]++;
			i = scene->getIndex(p);

			upper = i.current->chunk[i.x][i.y][i.z];
			upperBlock = upper->shape->block;
		}

		lowerBlock->changeBlock(AIR_BLOCK);
	}
}

static void inline RemoveCreature() 
{
	creatures.erase(currentSelectedCreatureIt);
}

void RemoveCreature(R3Creature *died) 
{
  creatures.erase(find(creatures.begin(), creatures.end(), died));
}

void MoveCharacter(R3Vector translated, double d) 
{
	translated.Normalize();
  InterpolateMotion(&(camera.eye), translated * d, true);
	GLUTRedraw();
}

void inline ChangeHealth(R3Character *character, int delta)
{
	character->Health += delta;
}

void ChangeHealth(R3Creature *creature, int delta)
{
	creature->Health += delta;

	if (creature->Health <= 0) 
  {
    switch (creature->creaturetype)
    {
      case R3COW_CREATURE:
        mainCharacter->Health = MIN(mainCharacter->Health + 2,
                                     mainCharacter->MaxHealth);
        break;
      case R3DEER_CREATURE:
        mainCharacter->Health = MIN(mainCharacter->Health + 1,
                                     mainCharacter->MaxHealth);
        break;
      default: 
        break;
    }

		RemoveCreature();
	}
}

void inline ChangeHealth(R3Block *block, int delta)
{
	block->health += delta;
}

///////////////////////////////////////////////////////////////////////////////
// SCENE DRAWING CODE
///////////////////////////////////////////////////////////////////////////////

static void inline DrawScene(R3Scene *scene)
{
  switch (culling)
  {
    case NONE:
      DrawSceneNone(scene);
      break;
    case VIEW:
      DrawSceneViewFrustrumOnly(scene);
      break;
    case OCCLUSION:
      DrawSceneOcclusionOnly(scene);
      break;
    case FULL:
      DrawSceneFullOptimization(scene);
      break;
  }
}

void FindMaterial(R3Block *block, bool isTop) 
{
	// Second argument specifies if top face is currently being drawn

  R3Block *upper;

  switch (block->getBlockType())
  {
    case LEAF_BLOCK:
      LoadMaterial(materials[LEAF]);
      break;
    case DIRT_BLOCK:
      if ((upper = block->getUpper()))
      {
        if (block->dy == 0)
          LoadMaterial(materials[BEDROCK]);
        else if (upper->getBlockType() == AIR_BLOCK) 
        {
          if (isTop)
            LoadMaterial(materials[GRASS]);
          else
            LoadMaterial(materials[DIRT]);
        }
        else LoadMaterial(materials[ALLDIRT]);
      }
      break;
    case WOOD_BLOCK:
      LoadMaterial(materials[WOOD]);
      break;
	  case STONE_BLOCK:
      LoadMaterial(materials[STONE]);
      break;
	  case SAND_BLOCK: 
      LoadMaterial(materials[SAND]);
      break;
	  case OBSIDIAN_BLOCK: 
      LoadMaterial(materials[OBSIDIAN]);
      break;
	  case GOLD_BLOCK:
      LoadMaterial(materials[GOLD]);
      break;
  }
}

void FindColor(R3Block *block, bool isTop) 
{
	// Second argument specifies if top face is currently being drawn
 
  R3Block *upper;

  switch (block->getBlockType())
  {
    case LEAF_BLOCK:
      glColor3f(0, 1, 0);
      break;
    case DIRT_BLOCK:
      if ((upper = block->getUpper()))
      {
        if (upper->getBlockType() == AIR_BLOCK) 
        {
          if (isTop)
            glColor3f(0., .1, 0.);
          else
            glColor3f(.549, .196, 0.);
        }
        else glColor3f(.737, .271, 0.075);
      }
      break;
    case WOOD_BLOCK:
      glColor3f(.737, .271, 0.075);
      break;
    case STONE_BLOCK:
      glColor3f(.2, .2, .2);
      break;
  }
}

void LoadCamera(R3Camera *camera)
{
	camera->towards.Reset(0, 0, -1);
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
	camera->towards.Transform(rotation);
	camera->towards.Normalize();
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
	for (int i = 0; i < (int) scene->lights.size(); i++) 
  {
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
    switch (light->type)
    {
      case R3_DIRECTIONAL_LIGHT:
        // Load direction
        buffer[0] = -(light->direction.X());
        buffer[1] = -(light->direction.Y());
        buffer[2] = -(light->direction.Z());
        buffer[3] = 0.0;
        glLightfv(index, GL_POSITION, buffer);
        break;
      case R3_POINT_LIGHT:
        // Load position
        buffer[0] = light->position.X();
        buffer[1] = light->position.Y();
        buffer[2] = light->position.Z();
        buffer[3] = 1.0;
        glLightfv(index, GL_POSITION, buffer);
        break;
      case R3_SPOT_LIGHT:
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
        break;
      case R3_AREA_LIGHT:
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
        break;
      default:
        break;
    }

		// Enable light
		glEnable(index);
	}
}

void DrawCreatures() 
{
	VecCreatureIt it;

	for (it = creatures.begin(); it < creatures.end(); it++)
	{
		double d = R3Distance(camera.eye, (*it)->box.centroid);
		if (d > distanceToRenderCreature) 
      continue;
    float view[16];
	
    glGetFloatv(GL_MODELVIEW_MATRIX, view);
    
    R3Vector right = R3Vector(view[0], view[4], view[8]);
    R3Vector up = R3Vector(view[1], view[5], view[9]);
    
    right.Normalize();
    up.Normalize();
    
    R3Vector boardLook = camera.eye - (*it)->position;
    boardLook.Normalize();
    R3Vector boardRight = R3Vector(boardLook);
    boardRight.Cross(up);
    R3Vector boardUp = R3Vector(boardRight);
    boardUp.Cross(boardLook);
    
    R3Matrix billboard = R3Matrix(boardRight.X(), boardUp.X(), boardLook.X(), (*it)->position.X(),
                                  boardRight.Y(), boardUp.Y(), boardLook.Y(), (*it)->position.Y(),
                                  boardRight.Z(), boardUp.Z(), boardLook.Z(), (*it)->position.Z(),
                                  0, 0, 0, 1);
    
    glPushMatrix();
    LoadMatrix(&billboard);
      
    switch ((*it)->creaturetype)
    {
      case R3COW_CREATURE:
        LoadMaterial(materials[COW]);
        break;
      case R3DEER_CREATURE:
        LoadMaterial(materials[DEER]);
        break;
      case R3SUICIDE_CREATURE:
        LoadMaterial(materials[SUICIDE]);
        break;
    }
    
    glBegin(GL_QUADS);
      glNormal3d(0.0, 0.0, 1.0);
      glTexCoord2d(0, 1);
      glVertex2f(-0.5, -0.5);
      glTexCoord2d(0, 0);
      glVertex2f(0.5, -0.5);
      glTexCoord2d(1, 0);
      glVertex2f(0.5, 0.5); 
      glTexCoord2d(1, 1);
      glVertex2f(-0.5, 0.5); 
    glEnd();
    
    if (currentSelectedCreatureIt == it) 
    {
      glDisable(GL_LIGHTING);
      glColor3d(0., 0., 0.);

      glLineWidth(15);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glBegin(GL_QUADS);
        glVertex2f(-0.5, -0.5);
        glVertex2f(0.5, -0.5);
        glVertex2f(0.5, 0.5); 
        glVertex2f(-0.5, 0.5); 
      glEnd();

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1);
      glEnable(GL_LIGHTING);
    }

    glPopMatrix();
  }
}

void GenerateCreatures(int num_to_create) 
{

	if (creatures.size() > MAX_num_creatures) 
        return;

	//total number of horizontal chunks
	double numspaces = CHUNKS*CHUNKS*CHUNK_X*CHUNK_Z;

	double probability = 1 / numspaces;
	probability = num_to_create * probability;

	//loop through all chunks around you and find one that has two air above it
	for (int dChunkX = 0; dChunkX < CHUNKS; dChunkX++)
	{
		for (int dChunkZ = 0; dChunkZ < CHUNKS; dChunkZ++)
		{
			for (int dz = 0; dz < CHUNK_Z; dz++)
			{
				for (int dy = 0; dy < CHUNK_Y-2; dy++)
				{
					for (int dx = 0; dx < CHUNK_X; dx++)
					{
						R3Block *currentblock = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz]->shape->block;
						if(!currentblock->transparent) {
							R3Block *aboveblock = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy+1][dz]->shape->block;
							if(aboveblock->transparent) {
								R3Block *secondaboveblock = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy+2][dz]->shape->block;
								if(secondaboveblock->transparent) {
									int randomnum = (int)(RandomNumber() / probability);

									//CREATE A MONSTER!!
									if (randomnum == 0) 
                                    {
										int rand_creature = (int)(RandomNumber() * 3);

										// Create a point at the location of the centroid of the box
										R3Point newpoint(aboveblock->box.centroid);
										R3Creature *newcreature1;
										switch (rand_creature)
										{
										case 0: 
											newcreature1 = new R3Creature(newpoint, R3COW_CREATURE);
											break;
										case 1:    

											newcreature1 = new R3Creature(newpoint, R3DEER_CREATURE);
											break;
										case 2:

											newcreature1 = new R3Creature(newpoint, R3SUICIDE_CREATURE);
											break;
										}
										creatures.push_back(newcreature1);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void UpdateCharacter() 
{
	mainCharacter->position.Reset(camera.eye.X(), camera.eye.Y(), camera.eye.Z());

	if (state == WORLDBUILDER) 
		for (int i = 0; i < 5; i++) 
			mainCharacter->number_items[i] = INT_MAX;

	if (mainCharacter->Health <= 0)
    state = LOST;
}

void ModulateLighting()
{
	// Adjust the color of the first two lights (the global lighting) and the 
	// glClearColor based on the time of day

	static const R3Rgb dayColor = R3Rgb(1, 1, 1, 1);
	static const R3Rgb backgroundDayColor = R3Rgb(0.529, 0.807, 0.980, 1.);
	static const R3Rgb backgroundNightColor = R3Rgb(0., 0., 0.400, 1);
	static const R3Rgb nightColor = R3Rgb(.2, .2, .2, 1);
	static int nightIndex = 0;
	static bool isNight = false;
  static bool dontChange = false;
	const int nightLength = 2e3;
	const double FACTOR = 1e4;
	R3Rgb diff;

	for (unsigned int i = 0; i < 2; i++)
	{
		R3Light *light = scene->lights[i];

    diff = (isNight) ? dayColor - light->color : nightColor - light->color;

		if (nightIndex != 0 && ABS(diff[0]) < EPS && ABS(diff[1]) < EPS && ABS(diff[2]) < EPS)
		{
			isNight = !isNight;
			nightIndex = 0;
      dontChange = true;
		}
  
    if (nightIndex++ >= nightLength)
      dontChange = false;
    
    if (!dontChange)
      light->color += diff / FACTOR;
	}

  diff = (isNight) ? backgroundDayColor - background :
                     backgroundNightColor - background;
  
  if (!dontChange)
    background += diff / FACTOR;
}

////////////////////////////////////////////////////////////
// GLUT USER INTERFACE CODE
////////////////////////////////////////////////////////////

void GLUTMainLoop(void)
{
	// Run main loop -- never returns 
	glutMainLoop();
}

void GLUTIdleFunction(void) 
{
  if ((state != REGULAR) || (state == LOST)) 
		return;

	ModulateLighting();
	UpdateCharacter();

  if (state == WORLDBUILDER)
  {
    glutPostRedisplay();
    return;
  }

	if (GetTime() >= previousLevelTime + timeBetweenLevels) 
  {
		currentLevel++;
		previousLevelTime = GetTime();

		GenerateCreatures(num_creatures_to_make++);
	}
  
  R3Vector direction;

  for (unsigned int i = 0; i < creatures.size(); i++)
  {
    double creaturedist = R3Distance(mainCharacter->position, creatures[i]->position);

    if (creaturedist >= distanceToRenderCreature) 
    {
      RemoveCreature(creatures[i--]);
      continue;
    }

    direction = creatures[i]->UpdateCreature(mainCharacter);

    if (direction == R3zero_vector) 
      continue;
    if (creaturedist < distanceToUpdateCreature) 
      InterpolateMotion(&(creatures[i]->position), direction, false);
  }

	glutPostRedisplay();
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
	camera.yfov = atan(tan(camera.xfov) * (double) h / (double) w); 
	
  dotProductCutOff = cos(atan(sqrt(SQ(tan(camera.xfov)) + SQ(tan(camera.yfov)))));

	// Remember window size 
	GLUTwindow_width = w;
	GLUTwindow_height = h;
  GLUTaspect = GLUTwindow_width / GLUTwindow_height;

	// Redraw
	glutPostRedisplay();
}

void JitterPerspective(GLdouble fovy, GLdouble aspect, GLdouble near_p, 
                       GLdouble far_p, GLdouble pixdx, GLdouble pixdy, 
	                     GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
	static GLdouble left, right, bottom, top, xwsize, ywsize, cached = 0.;

  // Only recompute if aspect ratio is changed
  if (aspect != cached)
  {
    top = near_p * tan(fovy);
    bottom = -top;
    right = top * aspect;
    left = -right;
    xwsize = right - left;
    ywsize = top - bottom;
    cached = aspect;
  }

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLdouble dx = -(pixdx * xwsize / GLUTwindow_width + eyedx * near_p / focus);
	GLdouble dy = -(pixdy * ywsize / GLUTwindow_height + eyedy * near_p / focus);

	glFrustum (left + dx, right + dx, bottom + dy, top + dy, near_p, far_p);
}

static void GLUTRedrawNoAA(void)
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

  // Load scene lights
  LoadLights(scene);

  LoadCamera(&camera);

  // Clear screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw scene surfaces
  if (show_faces && (state == REGULAR)) 
  {
    glEnable(GL_LIGHTING);
    DrawScene(scene);
    DrawCreatures();
  }
  // Draw scene edges
  if (show_edges && (state == REGULAR)) 
  {
    glDisable(GL_LIGHTING);
    glColor3d(0., 0., 0.);
    glLineWidth(3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    DrawScene(scene);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1);
  }

  // Get into 2D mode
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  glOrtho (0, GLUTwindow_width, GLUTwindow_height, 0, 0, 1);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING); 

  switch (state)
  {
    case WORLDBUILDER:
    case REGULAR:
      DrawHUD(mainCharacter, false, FPS);
      break;
    case STARTMENU:
      DisplayStartMenu();
      break;
    case CONTROLS:
      DisplayControls();
      break;
    case LOST:
      DisplayDeathMenu();
      break;
    case WON:
      DisplayWonMenu();
      break;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING); 

  // Move back buffer to front and relish the fruits of your labor.
  glutSwapBuffers();

  // Quit here so that can save image before exit
  if (quit) 
  {
    if (toSave) 
    {
      // Write new scene based on changes
      if (!scene->WriteScene(input_scene_name))
        fprintf(stderr, "WARNING: Couldn't save new scene!!!\n");
    }

    GLUTStop();
  }

  previous_time = current_time;
}    

static void GLUTRedrawAA(void)
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

  // Load scene lights
  LoadLights(scene);

  // Clear accumulation buffer
  glClear(GL_ACCUM_BUFFER_BIT);

  // Iterate through the jitter array to write to accumulation buffer
  for (int jitter = 0; jitter < antialias; jitter++) 
  {
    // Jitter perspective
    JitterPerspective(camera.yfov, GLUTaspect, 0.01, 10000, 
        j[antialias][jitter].x, j[antialias][jitter].y, 
        0.0, 0.0, 1.0);

    camera.towards.Reset(0, 0, -1);
    float n[16], *nn = n;
    R3Point e = camera.eye;

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Re-transform
    glRotatef(rot[1] * 180 / M_PI, 1., 0., 0);
    glRotatef(rot[0] * 180 / M_PI, 0., 1., 0);
    glTranslated(-e[0], -e[1], -e[2]);

    // Get the current transformation matrix
    glGetFloatv(GL_MODELVIEW_MATRIX, n);

    R3Matrix rotation = R3Matrix(nn);
    camera.towards.Transform(rotation);
    camera.towards.Normalize();

    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw scene surfaces
    if (show_faces && (state == REGULAR)) 
    {
      glEnable(GL_LIGHTING);
      DrawScene(scene);
      DrawCreatures();
    }
    // Draw scene edges
    if (show_edges && (state == REGULAR)) 
    {
      glDisable(GL_LIGHTING);
      glColor3d(0., 0., 0.);
      glLineWidth(3);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      DrawScene(scene);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1);
    }

    // Get into 2D mode
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0, GLUTwindow_width, GLUTwindow_height, 0, 0, 1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING); 

    switch (state)
    {
      case WORLDBUILDER:
      case REGULAR:
        DrawHUD(mainCharacter, false, FPS);
        break;
      case STARTMENU:
        DisplayStartMenu();
        break;
      case CONTROLS:
        DisplayControls();
        break;
      case LOST:
        DisplayDeathMenu();
        break;
      case WON:
        DisplayWonMenu();
        break;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); 

    // Add back buffer (the one written to) to the accumulation buffer and
    // weight with a fraction of the jitter size
    glAccum(GL_ACCUM, 1.0 / antialias);
  }

  // Put the composited buffer together and write to back buffer
  glAccum (GL_RETURN, 1.0);

  // Move back buffer to front and relish the fruits of your labor.
  glutSwapBuffers();

  // Quit here so that can save image before exit
  if (quit) 
  {
    if (toSave) 
    {
      // Write new scene based on changes
      if (!scene->WriteScene(input_scene_name))
        fprintf(stderr, "WARNING: Couldn't save new scene!!!\n");
    }

    GLUTStop();
  }

  previous_time = current_time;
}    

void GLUTRedraw(void)
{
  if (antialias)
    GLUTRedrawAA();
  else GLUTRedrawNoAA();
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

	if (x < 60 || x > GLUTwindow_width - 60 || 
		y < 60 || y > GLUTwindow_height - 60)
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
	rot[0] = WRAP(rot[0] + vx, -M_2PI, M_2PI);
	rot[1] = CLAMP(rot[1] - vy, -M_PI / 2, M_PI / 2);

	GLUTmouse[0] = x;
	GLUTmouse[1] = y;
	glutPostRedisplay();
}

void GLUTMouse(int button, int mouseState, int x, int y)
{
	// Invert y coordinate
	y = GLUTwindow_height - y;

	// Process mouse button event
	if (mouseState == GLUT_DOWN) 
	{
		if (button == GLUT_LEFT_BUTTON) 
		{
			if (CAPTURE_MOUSE == false) 
			{
				glutSetCursor(GLUT_CURSOR_NONE);
				CAPTURE_MOUSE = true;
			}

			else if (state == LOST) 
        return;
			else if (state == STARTMENU) 
        state = REGULAR;
			else if (currentSelectedCreatureIt != creatures.end() && *currentSelectedCreatureIt != NULL) 
			{
				ChangeHealth((*currentSelectedCreatureIt), -1);
			}
			else if (currentSelection != NULL) 
			{
        if (state == WORLDBUILDER) 
          ChangeHealth(currentSelection->shape->block, -20);
        else 
          ChangeHealth(currentSelection->shape->block, -1);

				if (currentSelection->shape->block->health <= 0) 
        {
					int item = 8;
					int block = currentSelection->shape->block->getBlockType();

          switch (block)
          {
            case DIRT_BLOCK:
              item = R3BLOCK_DIRT;
              break;
            case WOOD_BLOCK:
              item = R3BLOCK_WOOD;
              break;
            case STONE_BLOCK:
              item = R3BLOCK_STONE;
              break;
            case SAND_BLOCK:
              item = R3BLOCK_SAND;
              break;
            case OBSIDIAN_BLOCK:
              item = R3BLOCK_OBSIDIAN;
              break;
            case GOLD_BLOCK:
              mainCharacter->number_gold++;
              break;
          }

					if (item < 8) 
					{
						mainCharacter->number_items[item]++;
						mainCharacter->item = item;
					}

					RemoveBlock();

					if ((mainCharacter->number_gold == 10) && (state != WORLDBUILDER)) 
            state = WON;
				}
			}
		}
		else if (button == GLUT_MIDDLE_BUTTON) 
		{
		}
		else if (button == GLUT_RIGHT_BUTTON) 
		{
			if ((state == LOST) || (state != REGULAR)) 
        return;

			int block;
			int item = mainCharacter->item;

			if (currentSelection) 
      {
				if (item < 8) 
				{
					if (mainCharacter->number_items[item] > 0) 
					{
            switch (item)
            {
              case R3BLOCK_DIRT:
                block = DIRT_BLOCK;
                break;
              case R3BLOCK_WOOD:
                block = WOOD_BLOCK;
                break;
              case R3BLOCK_STONE:
                block = STONE_BLOCK;
                break;
              case R3BLOCK_SAND:
                block = SAND_BLOCK;
                break;
              case R3BLOCK_OBSIDIAN:
                block = OBSIDIAN_BLOCK;
                break;
              default:
                block = R3BLOCK_AIR;
                break;
            }

						if (--mainCharacter->number_items[item] == 0)
							mainCharacter->item = R3BLOCK_AIR;
						AddBlock(block);
					}
				}
			}
		}
	}

	// Remember button state 
	int b = (button == GLUT_LEFT_BUTTON) ? 
          0 : 
          ((button == GLUT_MIDDLE_BUTTON) ? 1 : 2);
	GLUTbutton[b] = (mouseState == GLUT_DOWN) ? 1 : 0;

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
	switch (key) 
  {
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
	R3Vector difference;
	difference.SetY(0);

	// Invert y coordinate
	y = GLUTwindow_height - y;

  // Process keyboard button event 
  switch (key) 
  {
    case '1':
      if (state == STARTMENU) 
        break;
      if (mainCharacter->number_items[R3BLOCK_DIRT] >0)
        mainCharacter->item = R3BLOCK_DIRT;
      break;
    case '2':
      if (state == STARTMENU) 
        break;
      if (mainCharacter->number_items[R3BLOCK_STONE] >0)
        mainCharacter->item = R3BLOCK_STONE;
      break;
    case '3':
      if (state == STARTMENU) 
        break;
      if (mainCharacter->number_items[R3BLOCK_WOOD] >0)
        mainCharacter->item = R3BLOCK_WOOD;
      break;
    case '4':
      if (state == STARTMENU) 
        break;
      if (mainCharacter->number_items[R3BLOCK_SAND] >0)
        mainCharacter->item = R3BLOCK_SAND;
      break;
    case '5':
      if (state == STARTMENU) 
        break;
      if (mainCharacter->number_items[R3BLOCK_OBSIDIAN] >0)
        mainCharacter->item = R3BLOCK_OBSIDIAN;
      break;
    case 'C':
    case 'c':
      if (state == STARTMENU) 
      {
        switch (culling)
        {
          case NONE:
            culling = VIEW;
            break;
          case VIEW:
            culling = OCCLUSION;
            break;
          case OCCLUSION:
            culling = FULL;
            break;
          case FULL:
            culling = NONE;
        }
        break;
      }
      state = CONTROLS;
      break;
    case 'b':
    case 'B':
      if (state == STARTMENU) 
        break;
      state = REGULAR;
      break;
    case 'E':
    case 'e':
      show_edges = !show_edges;
      break;
    case 'F':
    case 'f':
      show_faces = !show_faces;
      break;
    case 'Q':
      toSave = true;
    case 'q':
      quit = true;
      break;
    case 27: // ESCAPE
      CAPTURE_MOUSE = false;
      // Restore cursor
      glutSetCursor(GLUT_CURSOR_INHERIT);
      break;
    case 'N':
    case 'n':
      if (!CAPTURE_MOUSE) 
        break;	
      if (state == STARTMENU) 
        state = WORLDBUILDER;
      break;
    case 'w': 
      if (state == STARTMENU) 
        break;
      if ((state == LOST) || (state != REGULAR)) 
        return;
      difference = InterpolateMotion(&(camera.eye), 
          -(cos(rot[0]) * R3posz_point - sin(rot[0]) * R3posx_point), true);
      break;

    case 's': 
      if (state == STARTMENU) 
        break;
      if ((state == LOST) || (state != REGULAR)) 
        return;
      difference = InterpolateMotion(&(camera.eye), 
          (cos(rot[0]) * R3posz_point - sin(rot[0]) * R3posx_point), true);
      break;
    case 'd': 
      if (state == STARTMENU) 
        break;
      if ((state == LOST) || (state != REGULAR)) 
        return;
      difference = InterpolateMotion(&(camera.eye), 
          (sin(rot[0]) * R3posz_point + cos(rot[0]) * R3posx_point).Vector(), true);
      break;
    case 'a': 
      if (state == STARTMENU) 
        break;
      if ((state == LOST) || (state != REGULAR)) 
        return;
      difference = InterpolateMotion(&(camera.eye), 
          -(sin(rot[0]) * R3posz_point + cos(rot[0]) * R3posx_point).Vector(), true);
      break;
    case ' ': 
      if (state == STARTMENU) 
        break;
      if ((state == LOST) || (state != REGULAR)) 
        return;
      InterpolateJump(&(camera.eye),     
          -(cos(rot[0]) * R3posz_point - sin(rot[0]) * R3posx_point));
      break;
    case 'i':
      if (state == STARTMENU) 
        break;
      printf("camera %g %g %g  %g %g %g  %g  %g %g \n",
          camera.eye[0], camera.eye[1], camera.eye[2], 
          rot[0], rot[1], rot[2],
          camera.xfov, camera.neardist, camera.fardist); 
      break;
  }

	// If you fall too much, you lose health
	if (difference.Y() < -1.f)
		ChangeHealth(mainCharacter, -1);

	// Remember mouse position 
	GLUTmouse[0] = x;
	GLUTmouse[1] = y;

	// Remember modifiers 
	GLUTmodifiers = glutGetModifiers();

	// Redraw
	glutPostRedisplay();
}

void GLUTInit(int *argc, char **argv)
{
	// Open window 
	glutInit(argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(GLUTwindow_width, GLUTwindow_height);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_ACCUM | GLUT_DEPTH);

	GLUTwindow = glutCreateWindow("Minecraft v.0.0.1");
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);

	// Initialize GLUT callback functions 
	glutReshapeFunc(GLUTResize);
	glutDisplayFunc(GLUTRedraw);
	glutKeyboardFunc(GLUTKeyboard);
	glutSpecialFunc(GLUTSpecial);
	glutMouseFunc(GLUTMouse);
	glutPassiveMotionFunc(GLUTPassiveMotion);
	glutEntryFunc(GLUTMouseEntry);
	glutIdleFunc(GLUTIdleFunction);

	// Initialize graphics modes 
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);


	// Initialize OpenGL drawing modes
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(true);
	glClearAccum(0.0, 0.0, 0.0, 0.0);

	//Initialize Character    
	mainCharacter = new R3Character();


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
	camera.xfov = .5;

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
			else if (!strcmp(*argv, "-exit_immediately")) { quit = true; }
			else if (!strcmp(*argv, "-no_culling")) { argc--; argv++; culling = NONE;}
			else if (!strcmp(*argv, "-view_frustrum_culling")) { argc--; argv++; culling = VIEW;}
			else if (!strcmp(*argv, "-occlusion_culling")) { argc--; argv++; culling = OCCLUSION;}
			else if (!strcmp(*argv, "-full_culling")) { argc--; argv++; culling = FULL;}
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
	if (!input_scene_name || print_usage) 
  {
		printf("Usage: rayview <input.scn> [-no_culling -view_frustrum_culling -occlusion-_culling -full_culling (default)][-maxdepth <int>] [-v]\n");
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
	// Load wav data into a buffer
	alGenBuffers(1, &Buffer);

	if (alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	Buffer = alutCreateBufferFromFile(s_WAVFile);

	// Bind the buffer with the source
	alGenSources(1, &Source);

	if (alGetError() != AL_NO_ERROR)
		return AL_FALSE;

	alSourcei (Source, AL_BUFFER,   Buffer   );
	alSourcef (Source, AL_PITCH,    1.0      );
	alSourcef (Source, AL_GAIN,     1.0      );
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_LOOPING,  AL_TRUE  );

	if (alGetError() == AL_NO_ERROR)
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
  // Parse program arguments
	if (!ParseArgs(argc, argv)) 
    exit(1);

	// Set up jitter map
	j[2] = j2;
	j[4] = j4;

	// Initialize GLUT
	GLUTInit(&argc, argv);

#ifdef __linux__
	alutInit(NULL, 0);
	alGetError();

	if (LoadALData() == AL_FALSE)
		fprintf(stderr, "Error loading WAV sound data.\n");

	SetListenerValues();

	// Bind KillALData to run at exit
	atexit(KillALData);

	// Begin sound playback
	alSourcePlay(Source);
#endif
  
  // Load all initial materials
	MakeMaterials(materials);

	// Read scene
	if (!(scene = ReadScene(input_scene_name))) 
    exit(-1);

	// Run GLUT interface
	GLUTMainLoop();

	// Return success 
	return 0;
}

