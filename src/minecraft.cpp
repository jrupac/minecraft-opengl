////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////

#include "R3/R3.h"
#include "R3Scene.h"
#include "raytrace.h"
#include "cos426_opengl.h"

// Zero is even
#define SIGN(x) ((x) >= 0 ? 1 : -1)

////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////

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
static R3Node *currentSelection = NULL;
static R3Rgb background = R3Rgb(0.529, 0.807, 0.980, 1.);
static float picker_height = 10;
static float picker_width = 10;
static bool CAPTURE_MOUSE = false;
static R3Vector rot;

// GLUT variables 

static int GLUTwindow = 0;
static int GLUTwindow_height = 512;
static int GLUTwindow_width = 512;
static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTmodifiers = 0;

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

////////////////////////////////////////////////////////////
// GAME LOGIC CODE
////////////////////////////////////////////////////////////

void AlignReticle()
{
  R3Ray ray = R3Ray(camera.eye, camera.towards);
  R3Intersection intersect = IntersectScene(ray, scene, scene->root);
  
  currentSelection = NULL;

  if (intersect.hit)
    currentSelection = intersect.node;
}

void AddBlock()
{
  // Force-update detection of current selection
  AlignReticle();
  
  // If no selection, don't add a block
  if (!currentSelection)
    return;
  
  // TODO: Check if there's a block already above it
  R3Box *lower = currentSelection->shape->box;

  // Create new block and add it to the scene graph
  R3Box *upper = new R3Box(lower->Min() + 2 * R3posy_vector, 
                           lower->Max() + 2 * R3posy_vector);
  
  R3Shape *shape = new R3Shape();
  shape->type = R3_BOX_SHAPE;
  shape->box = upper;
  shape->sphere = NULL;
  shape->cylinder = NULL;
  shape->cone = NULL;
  shape->mesh = NULL;
  shape->segment = NULL;

  R3Node *newNode = new R3Node();
  newNode->parent = scene->root;
  newNode->shape = shape;
  newNode->transformation = R3identity_matrix;
  newNode->bbox = *upper;
  newNode->selected = false;
  scene->root->children.push_back(newNode);
}

////////////////////////////////////////////////////////////
// SCENE DRAWING CODE
////////////////////////////////////////////////////////////

void DrawShape(R3Shape *shape)
{
  // Check shape type
  if (shape->type == R3_BOX_SHAPE) shape->box->Draw();
  else if (shape->type == R3_SPHERE_SHAPE) shape->sphere->Draw();
  else if (shape->type == R3_CYLINDER_SHAPE) shape->cylinder->Draw();
  else if (shape->type == R3_CONE_SHAPE) shape->cone->Draw();
  else if (shape->type == R3_MESH_SHAPE) shape->mesh->Draw();
  else if (shape->type == R3_SEGMENT_SHAPE) shape->segment->Draw();
  else if (shape->type == R3_BLOCK_SHAPE)
  {
    //if (shape->block->getBlockType() != AIR_BLOCK)
      shape->block->Draw();
  }
  
  else fprintf(stderr, "Unrecognized shape type: %d\n", shape->type);
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
  if (material->texture) {
    if (material->texture_index <= 0) {
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
      for (int j = 0; j < npixels; j++) { 
        *(bufferp++) = pixelsp->Red();
        *(bufferp++) = pixelsp->Green();
        *(bufferp++) = pixelsp->Blue();
        *(bufferp++) = pixelsp->Alpha();
        pixelsp++;
      }
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexImage2D(GL_TEXTURE_2D, 0, 4, image->Width(), image->Height(), 0, GL_RGBA, GL_FLOAT, buffer);
      delete [] buffer;
    }

    // Select texture
    glBindTexture(GL_TEXTURE_2D, material->texture_index); 
    glEnable(GL_TEXTURE_2D);
  }
  else {
    glDisable(GL_TEXTURE_2D);
  }

  // Enable blending for transparent surfaces
  if (opacity < 1) {
    glDepthMask(false);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
  }
  else {
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(true);
  }
}

void LoadCamera(R3Camera *camera)
{
  glPushMatrix();
  // Set projection transformation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(2 * 180.0 * camera->yfov / M_PI, 
                 (GLdouble) GLUTwindow_width /(GLdouble) GLUTwindow_height, 
                 0.01, 10000);

  // Set camera transformation
  R3Point e = camera->eye;
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glRotatef(rot[1] * 180 / M_PI, 1., 0., 0);
  glRotatef(rot[0] * 180 / M_PI, 0., 1., 0);
  glTranslated(-e[0], -e[1], -e[2]);
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
    if (node == currentSelection)
    {
      glDisable(GL_LIGHTING);
      glColor3d(0., 0., 0.);
      glLineWidth(5);
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

  // Draw bottom pane
  glColor3d(.7, .7, .7);

  glBegin(GL_QUADS);
      glVertex2f((GLUTwindow_width / 2) - 200, GLUTwindow_height - 100); 
      glVertex2f((GLUTwindow_width / 2) - 200, GLUTwindow_height - 40); 
      glVertex2f((GLUTwindow_width / 2) + 200, GLUTwindow_height - 40); 
      glVertex2f((GLUTwindow_width / 2) + 200, GLUTwindow_height - 100); 
  glEnd();
  
  // This is just temporary text
  glColor3d(.1, .1, .1);
  GLUTDrawText(R3Point(GLUTwindow_width / 2 - 10, GLUTwindow_height - 70, 0), "I am a dock!");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING); 

  // Swap buffers 
  glutSwapBuffers();
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

  // Transform camera RTU frame by appropriate angles
  rot[0] += vx;
  if (abs(rot[0]) > 2 * M_PI)
      rot[0] = 0.;
  rot[1] -= vy;
  if (abs(rot[1]) > 2 * M_PI)
      rot[1] = 0.;

  GLUTmouse[0] = x;
  GLUTmouse[1] = y;
  glutPostRedisplay();
}

void GLUTMouse(int button, int state, int x, int y)
{
    fprintf(stderr, "HERE\n");
  // Invert y coordinate
  y = GLUTwindow_height - y;
  
  // Process mouse button event
  if (state == GLUT_DOWN) 
  {
    if (button == GLUT_LEFT_BUTTON) 
    {
      if (CAPTURE_MOUSE == false) 
      {
        glutSetCursor(GLUT_CURSOR_NONE);
        CAPTURE_MOUSE = true;
      }
    }
    else if (button == GLUT_MIDDLE_BUTTON) 
    {
    }
    else if (button == GLUT_RIGHT_BUTTON) 
    {
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
    case 'B':
    case 'b':
      AddBlock();
      break;

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
      camera.eye.SetX(camera.eye.X() + camera.towards.X());
      camera.eye.SetZ(camera.eye.Z() + camera.towards.Z());
      break;

    case 's': 
      camera.eye.SetX(camera.eye.X() - camera.towards.X());
      camera.eye.SetZ(camera.eye.Z() - camera.towards.Z());
      break;

   case 'd': 
      camera.eye += camera.right;
      break;

   case 'a': 
     camera.eye -= camera.right;
     break;

    case ' ': 
      printf("camera %g %g %g  %g %g %g  %g %g %g  %g  %g %g \n",
             camera.eye[0], camera.eye[1], camera.eye[2], 
             camera.towards[0], camera.towards[1], camera.towards[2], 
             camera.up[0], camera.up[1], camera.up[2], 
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
  // Display sub-menu
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
  glutAttachMenu(GLUT_RIGHT_BUTTON);
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
  camera.eye = R3zero_point + 1. * R3posy_vector;
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
// MAIN
////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  // Initialize GLUT
  GLUTInit(&argc, argv);

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

