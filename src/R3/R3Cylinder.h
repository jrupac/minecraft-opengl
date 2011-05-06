// Include file for the R3 cylinder class 



// Class definition 

class R3Cylinder {
 public:
  // Constructor functions
  R3Cylinder(void);
  R3Cylinder(const R3Cylinder& cylinder);
  R3Cylinder(const R3Point& center, double radius, double height);

  // Cylinder propetry functions/operators
  const R3Point& Center(void) const;
  double Radius(void) const;
  double Height(void) const;
  R3Segment Axis(void) const;

  // Shape propetry functions/operators
  double Area(void) const;
  double Volume(void) const;
  R3Box BBox(void) const;

  // Manipulation functions/operators
  void Empty(void);
  void Translate(const R3Vector& vector);
  void Reposition(const R3Point& center);
  void Resize(double radius, double height);

  // Output functions
  void Draw(void) const;
  void Outline(void) const;
  void Print(FILE *fp = stdout) const;

 private:
  R3Point center;
  double radius;
  double height;
};



// Public variables 

extern const R3Cylinder R3null_cylinder;
extern const R3Cylinder R3zero_cylinder;
extern const R3Cylinder R3unit_cylinder;



// Inline functions 

inline const R3Point& R3Cylinder::
Center(void) const
{
  // Return center point
  return center;
}



inline double R3Cylinder::
Radius(void) const
{
  // Return cylinder radius
  return radius;
}



inline double R3Cylinder::
Height(void) const
{
  // Return cylinder height
  return height;
}



inline R3Segment R3Cylinder::
Axis(void) const
{
  // Return cylinder axis
  return R3Segment(center + 0.5 * height * R3negy_vector, center + 0.5 * height * R3posy_vector);
}



