// Include file for the R3 cone class 



// Class definition 

class R3Cone {
 public:
  // Constructor functions
  R3Cone(void);
  R3Cone(const R3Cone& cone);
  R3Cone(const R3Point& center, double radius, double height);

  // Cone propetry functions/operators
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

extern const R3Cone R3null_cone;
extern const R3Cone R3zero_cone;
extern const R3Cone R3unit_cone;



// Inline functions 

inline const R3Point& R3Cone::
Center(void) const
{
  // Return center point
  return center;
}



inline double R3Cone::
Radius(void) const
{
  // Return cone radius
  return radius;
}



inline double R3Cone::
Height(void) const
{
  // Return cone height
  return height;
}



inline R3Segment R3Cone::
Axis(void) const
{
  // Return cone axis
  return R3Segment(center + 0.5 * height * R3negy_vector, center + 0.5 * height * R3posy_vector);
}



