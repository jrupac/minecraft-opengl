// Include file for mesh class




////////////////////////////////////////////////////////////
// DEPENDENCY INCLUDE FILES
////////////////////////////////////////////////////////////

#include <vector>
#include "R2/R2.h"
#include "R3/R3.h"
using namespace std;



////////////////////////////////////////////////////////////
// MESH VERTEX DECLARATION
////////////////////////////////////////////////////////////

struct R3MeshVertex {
  // Constructors
  R3MeshVertex(void);
  R3MeshVertex(const R3MeshVertex& vertex);
  R3MeshVertex(const R3Point& position, const R3Vector& normal, const R2Point& texcoords);

  // Property functions
  double AverageEdgeLength(void) const;

  // Update functions
  void UpdateNormal(void);
  void UpdateCurvature(void);

  // Data
  R3Point position;
  R3Vector normal;
  R2Point texcoords;
  double curvature;
  int id; 
};



////////////////////////////////////////////////////////////
// MESH FACE DECLARATION
////////////////////////////////////////////////////////////

struct R3MeshFace {
  // Constructors
  R3MeshFace(void);
  R3MeshFace(const R3MeshFace& face);
  R3MeshFace(const vector <R3MeshVertex *>& vertices);

  // Property functions
  double AverageEdgeLength(void) const;
  double Area(void) const;

  // Update functions
  void UpdatePlane(void);

  // Data
  vector<R3MeshVertex *> vertices;
  R3Plane plane;
  int id;
};



////////////////////////////////////////////////////////////
// MESH CLASS DECLARATION
////////////////////////////////////////////////////////////

struct R3Mesh {
  // Constructors
  R3Mesh(void);
  R3Mesh(const R3Mesh& mesh);
  ~R3Mesh(void);

  // Properties
  R3Point Center(void) const;
  double Radius(void) const;

  // Vertex and face access functions
  int NVertices(void) const;
  R3MeshVertex *Vertex(int k) const;
  int NFaces(void) const;
  R3MeshFace *Face(int k) const;

  // Transformations
  void Translate(double dx, double dy, double dz);
  void Scale(double sx, double sy, double sz);
  void Rotate(double angle, const R3Line& axis);

  // Warps
  void RandomNoise(double factor);
  void Inflate(double factor);
  void Fun(void);

  // Filters
  void Smooth(void);
  void SmoothBilateral(void);
  void Sharpen(void);

  // Erosion
  void Truncate(double t);
  void Bevel(double t);

  // Remeshing
  void SplitFaces(void);
  void StarFaces(double offset = 0);
  void SplitLongEdges(double max_edge_length);
  void CollapseShortEdges(double min_edge_length);
  void ClusterVertices(double grid_cell_size);

  // Smooth surface construction
  void Bezier(const R3Mesh& control_mesh, int m, int n);
  void BSpline(const R3Mesh& control_mesh, int m, int n);
  void SubdivideLoop(void);
  void SubdivideCatmullClark(void);

  // Topological fixup
  void FixHoles(void);
  void FixCracks(double epsilon);
  void FixIntersections(void);

  // Geometry construction
  void SurfaceOfRevolution(const R3Mesh& profile_curve, 
    const R3Line& axis_of_revolution, double rotation_angle_step);
  void SurfaceSweep(const R3Mesh& crosssection_polygon, 
    const R3Mesh& centerline_curve);

  // Boolean operations
  void Intersect(const R3Mesh& mesh);
  void Subtract(const R3Mesh& mesh);
  void Union(const R3Mesh& mesh);
  void Crop(const R3Plane& plane);

  // File input/output 
  int Read(const char *filename);
  int ReadRay(const char *filename);
  int ReadOff(const char *filename);
  int ReadImage(const char *filename);
  int Write(const char *filename);
  int WriteRay(const char *filename);
  int WriteOff(const char *filename);

  // Drawing functions
  void Draw(void) const;
  void Outline(void) const;

  // Low-level creation functions
  R3MeshVertex *CreateVertex(const R3Point& position, 
    const R3Vector& normal, const R2Point& texcoords);
  R3MeshFace *CreateFace(const vector <R3MeshVertex *>& vertices);
  void DeleteVertex(R3MeshVertex *vertex);
  void DeleteFace(R3MeshFace *face);

  // Update functions
  void Update(void);
  void UpdateBBox(void);
  void UpdateFacePlanes(void);
  void UpdateVertexNormals(void);
  void UpdateVertexCurvatures(void);

  // Data
  vector<R3MeshVertex *> vertices;
  vector<R3MeshFace *> faces;
  R3Box bbox;
};



////////////////////////////////////////////////////////////
// MESH INLINE FUNCTIONS
////////////////////////////////////////////////////////////

inline int R3Mesh::
NVertices(void) const
{
  // Return number of vertices in mesh
  return vertices.size();
}



inline R3MeshVertex *R3Mesh::
Vertex(int k) const
{
  // Return kth vertex of mesh
  return vertices[k];
}



inline int R3Mesh::
NFaces(void) const
{
  // Return number of faces in mesh
  return faces.size();
}



inline R3MeshFace *R3Mesh::
Face(int k) const
{
  // Return kth face of mesh
  return faces[k];
}



