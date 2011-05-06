// Include file for the R3 sphere class 

#include "cos426_opengl.h"

// Class definition 

class R3Sphere {
 public:
  // Constructor functions
  R3Sphere(void);
  R3Sphere(const R3Sphere& sphere);
  R3Sphere(const R3Point& center, double radius);

  // Sphere property functions/operators
  const R3Point& Center(void) const;
  const double Radius(void) const;
  const bool IsEmpty(void) const;

  // Shape property functions/operators
  double Area(void) const;
  double Volume(void) const;
  R3Box BBox(void) const;

  // Manipulation functions/operators
  void Empty(void);
  void Translate(const R3Vector& vector);
  void Reposition(const R3Point& center);
  void Resize(double radius);

  // Output functions
  void Draw(void) const;
  void Outline(void) const;
  void Print(FILE *fp = stdout) const;

 private:
  R3Point center;
  double radius;
};



// Public variables 

extern const R3Sphere R3null_sphere;
extern const R3Sphere R3zero_sphere;
extern const R3Sphere R3unit_sphere;



// Inline functions 

inline const R3Point& R3Sphere::
Center(void) const
{
  // Return sphere center
  return center;
}



inline const double R3Sphere::
Radius(void) const
{
  // Return sphere radius
  return radius;
}



inline const bool R3Sphere::
IsEmpty(void) const
{
  // Return whether the sphere is null
  return (radius < 0.0);
}




