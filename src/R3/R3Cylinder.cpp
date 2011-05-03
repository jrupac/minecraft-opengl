// Source file for the R3 cylinder class 



// Include files 

#include "R3.h"



// Public variables 

const R3Cylinder R3null_cylinder(R3Point(0.0, 0.0, 0.0), 0, 0);
const R3Cylinder R3zero_cylinder(R3Point(0.0, 0.0, 0.0), 0, 0);
const R3Cylinder R3unit_cylinder(R3Point(0.0, 0.5, 0.0), 1, 1);



R3Cylinder::
R3Cylinder(void)
{
}



R3Cylinder::
R3Cylinder(const R3Cylinder& cylinder)
  : center(cylinder.center),
    radius(cylinder.radius),
    height(cylinder.height)
{
}



R3Cylinder::
R3Cylinder(const R3Point& center, double radius, double height)
  : center(center),
    radius(radius),
    height(height)
{
}



double R3Cylinder::
Area(void) const
{
  // Return surface area of cylinder
  return (2.0 * M_PI * Radius() * (Radius() + Height()));
}



double R3Cylinder::
Volume(void) const
{
  // Return volume of cylinder
  return (M_PI * Radius() * Radius() * Height());
}



R3Box R3Cylinder::
BBox(void) const
{
  // Return bounding box of cylinder 
  R3Box bbox(R3null_box);
  bbox.Union(center + 0.5 * height * R3negy_vector + radius * R3negx_vector + radius * R3negz_vector);
  bbox.Union(center + 0.5 * height * R3posy_vector + radius * R3posx_vector + radius * R3posz_vector);
  return bbox;
}



void R3Cylinder::
Empty(void)
{
  // Empty cylinder
  *this = R3null_cylinder;
}


void R3Cylinder::
Translate(const R3Vector& vector)
{
  // Move cylinder 
  center.Translate(vector);
}



void R3Cylinder::
Reposition(const R3Point& center)
{
  // Set cylinder center
  this->center = center;
}



void R3Cylinder::
Resize(double radius, double height) 
{
  // Set cylinder radius
  this->radius = radius;
  this->height = height;
}




void R3Cylinder::
Draw(void) const
{
  // Draw cylinder
  glPushMatrix();
  glTranslated(center[0], center[1], center[2]);
  glRotated(-90, 1, 0, 0);
  glTranslated(0, 0, -0.5 * height);
  static GLUquadricObj *glu_cylinder = gluNewQuadric();
  gluQuadricTexture(glu_cylinder, GL_TRUE);
  gluQuadricNormals(glu_cylinder, (GLenum) GLU_SMOOTH);
  gluQuadricDrawStyle(glu_cylinder, (GLenum) GLU_FILL);
  gluQuadricOrientation(glu_cylinder, (GLenum) GLU_OUTSIDE);
  gluCylinder(glu_cylinder, radius, radius, height, 32, 8);
  gluQuadricOrientation(glu_cylinder, (GLenum) GLU_INSIDE);
  gluDisk(glu_cylinder, 0, radius, 32, 8);
  gluQuadricOrientation(glu_cylinder, (GLenum) GLU_OUTSIDE);
  glTranslated(0, 0, height);
  gluDisk(glu_cylinder, 0, radius, 32, 8);
  glPopMatrix();
}



void R3Cylinder::
Outline(void) const
{
  // Draw cylinder in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  Draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



void R3Cylinder::
Print(FILE *fp) const
{
  // Print min and max points
  fprintf(fp, "(%g %g %g) %g %g", center[0], center[1], center[2], radius, height);
}


