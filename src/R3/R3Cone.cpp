// Source file for the R3 cone class 



// Include files 

#include "R3.h"



// Public variables 

const R3Cone R3null_cone(R3Point(0.0, 0.0, 0.0), 0, 0);
const R3Cone R3zero_cone(R3Point(0.0, 0.0, 0.0), 0, 0);
const R3Cone R3unit_cone(R3Point(0.0, 0.5, 0.0), 1, 1);



R3Cone::
R3Cone(void)
{
}



R3Cone::
R3Cone(const R3Cone& cone)
  : center(cone.center),
    radius(cone.radius),
    height(cone.height)
{
}



R3Cone::
R3Cone(const R3Point& center, double radius, double height)
  : center(center),
    radius(radius),
    height(height)
{
}



double R3Cone::
Area(void) const
{
  // Return surface area of cone
  double s = sqrt((Height() * Height()) + (Radius() * Radius()));
  return (M_PI * Radius() * (Radius() + s));
}



double R3Cone::
Volume(void) const
{
  // Return volume of cone
  return (0.33333333 * M_PI * Radius() * Radius() * Height());
}



R3Box R3Cone::
BBox(void) const
{
  // Return bounding box of cone 
  R3Box bbox(R3null_box);
  bbox.Union(center + 0.5 * height * R3negy_vector + radius * R3negx_vector + radius * R3negz_vector);
  bbox.Union(center + 0.5 * height * R3posy_vector + radius * R3posx_vector + radius * R3posz_vector);
  return bbox;
}



void R3Cone::
Empty(void)
{
  // Empty cone
  *this = R3null_cone;
}


void R3Cone::
Translate(const R3Vector& vector)
{
  // Move cone 
  center.Translate(vector);
}



void R3Cone::
Reposition(const R3Point& center)
{
  // Set cone center
  this->center = center;
}



void R3Cone::
Resize(double radius, double height) 
{
  // Set cone radius
  this->radius = radius;
  this->height = height;
}



void R3Cone::
Draw(void) const
{
  // Draw cone
  glPushMatrix();
  glTranslated(center[0], center[1], center[2]);
  glRotated(-90, 1, 0, 0);
  glTranslated(0, 0, -0.5 * height);
  static GLUquadricObj *glu_cone = gluNewQuadric();
  gluQuadricTexture(glu_cone, GL_TRUE);
  gluQuadricNormals(glu_cone, (GLenum) GLU_SMOOTH);
  gluQuadricDrawStyle(glu_cone, (GLenum) GLU_FILL);
  gluQuadricOrientation(glu_cone, (GLenum) GLU_OUTSIDE);
  gluCylinder(glu_cone, radius, 0, height, 32, 8);
  gluQuadricOrientation(glu_cone, (GLenum) GLU_INSIDE);
  gluDisk(glu_cone, 0, radius, 32, 8);
  glPopMatrix();
}



void R3Cone::
Outline(void) const
{
  // Draw cone in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  Draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



void R3Cone::
Print(FILE *fp) const
{
  // Print min and max points
  fprintf(fp, "(%g %g %g) %g %g", center[0], center[1], center[2], radius, height);
}


