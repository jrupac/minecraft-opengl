// Source file for mesh class



// Include files

#include "R3.h"
#include "cos426_opengl.h"


////////////////////////////////////////////////////////////
// MESH CONSTRUCTORS/DESTRUCTORS
////////////////////////////////////////////////////////////

R3Mesh::
R3Mesh(void)
  : bbox(R3null_box)
{
}



R3Mesh::
R3Mesh(const R3Mesh& mesh)
  : bbox(R3null_box)
{
  // Create vertices
  for (int i = 0; i < mesh.NVertices(); i++) {
    R3MeshVertex *v = mesh.Vertex(i);
    CreateVertex(v->position, v->normal, v->texcoords);
  }

  // Create faces
  for (int i = 0; i < mesh.NFaces(); i++) {
    R3MeshFace *f = mesh.Face(i);
    vector<R3MeshVertex *> face_vertices;
    for (unsigned int j = 0; j < f->vertices.size(); j++) {
      R3MeshVertex *ov = f->vertices[j];
      R3MeshVertex *nv = Vertex(ov->id);
      face_vertices.push_back(nv);
    }
    CreateFace(face_vertices);
  }
}



R3Mesh::
~R3Mesh(void)
{
  // Delete faces
  for (int i = 0; i < NFaces(); i++) {
    R3MeshFace *f = Face(i);
    delete f;
  }

  // Delete vertices
  for (int i = 0; i < NVertices(); i++) {
    R3MeshVertex *v = Vertex(i);
    delete v;
  }
}



////////////////////////////////////////////////////////////
// MESH PROPERTY FUNCTIONS
////////////////////////////////////////////////////////////

R3Point R3Mesh::
Center(void) const
{
  // Return center of bounding box
  return bbox.Centroid();
}



double R3Mesh::
Radius(void) const
{
  // Return radius of bounding box
  return bbox.DiagonalRadius();
}



////////////////////////////////////////////////////////////
// MESH PROCESSING FUNCTIONS
////////////////////////////////////////////////////////////

void R3Mesh::
Translate(double dx, double dy, double dz)
{
  // Translate the mesh by adding a 
  // vector (dx,dy,dz) to every vertex

  // This is implemented for you as an example 

  // Create a translation vector
  R3Vector translation(dx, dy, dz);

  // Update vertices
  for (unsigned int i = 0; i < vertices.size(); i++) {
    R3MeshVertex *vertex = vertices[i];
    vertex->position.Translate(translation);
  }

  // Update mesh data structures
  Update();
}




void R3Mesh::
Scale(double sx, double sy, double sz)
{
  // Scale the mesh by increasing the distance 
  // from every vertex to the origin by a factor 
  // given for each dimension (sx, sy, sz)

  // This is implemented for you as an example 

  // Update vertices
  for (unsigned int i = 0; i < vertices.size(); i++) {
    R3MeshVertex *vertex = vertices[i];
    vertex->position[0] *= sx;
    vertex->position[1] *= sy;
    vertex->position[2] *= sz;
  }

  // Update mesh data structures
  Update();
}




void R3Mesh::
Rotate(double angle, const R3Line& axis)
{
  // Rotate the mesh counter-clockwise by an angle 
  // (in radians) around a line axis

  // This is implemented for you as an example 

  // Update vertices
  for (unsigned int i = 0; i < vertices.size(); i++) {
    R3MeshVertex *vertex = vertices[i];
    vertex->position.Rotate(axis, angle);
  }

  // Update mesh data structures
  Update();
}




void R3Mesh::
RandomNoise(double factor)
{
  // Add noise of a random amount and direction 
  // to the position of every vertex, where the 
  // input parameter "factor" should be multiplied by
  // the average length of the edges attached to the
  // vertex to determine its maximum displacement
  // (i.e., displacement distances should be between 
  // 0 and "factor * vertex->AverageEdgeLength()"

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "RandomNoise(%g) not implemented\n", factor);

  // Update mesh data structures
  Update();
}



void R3Mesh::
Inflate(double factor)
{
  // Move every vertex along its normal direction.
  // The input parameter "factor" should be multiplied by
  // the average length of the edges attached to the
  // vertex to determine the displacement of each 
  // vertex along its normal direction.  Note that factor
  // can be negative, which means that the vertex should
  // move in the direction opposite to the normal vector.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Inflate(%g) not implemented\n", factor);

  // Update mesh data structures
  Update();
}




void R3Mesh::
Fun(void)
{
  // Warp a mesh using a non-linear mapping of your choice 
  // (examples are sine, bulge, swirl)

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Fun not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Smooth(void)
{
  // Smooth the mesh by moving every vertex to a position 
  // determined by a weighted average of its immediate neighbors 
  // (with weights determined by a Gaussian with sigma equal to
  // the average length of edges attached to the vertex, 
  // normalized such that the weights sum to one).

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Smooth not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Sharpen(void)
{
  // Accentuate details in the mesh by moving every vertex along
  // the opposite of the vector determined by a weighted average 
  // of its neighbors  (with weights determined by a Gaussian 
  // with sigma equal to the average length of edges attached 
  // to the vertex, normalized such that the weights sum to one).
  // This filter moves vertices by the vector exactly opposite from 
  // the one used for Smooth().

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Sharpen not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
SmoothBilateral(void)
{
  // Smooth the mesh using a bilateral filter as in 
  // [Jones et al, Siggraph 2003] or 
  // [Fleishman et al., Siggraph 2003]

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SmoothBilateral not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Truncate(double t)
{
  // For every vertex, create a new vertex a parameter t [0-1] 
  // of the way along each of its N attached edges, and then 
  // "chop off" the pyramid whose base is formed by the new vertices 
  // and whose apex is the original vertex, creating a new N-sided 
  // face covering the hole.  It is OK to assume that the input shape 
  // is convex for this feature.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Truncate(%g) not implemented\n", t);

  // Update mesh data structures
  Update();
}




void R3Mesh::
Bevel(double t)
{
  // For every edge, create a new face whose vertices are t [0-1] 
  // of the way along each of its attached edges.  This requires 
  // first truncating all vertices by t, creating new vertices t [0-1] 
  // of the way along each of new edges, and then "chopping off" a 
  // prism for each of the original edges, creating a new face
  // to triangulate the hole created for each edge.  It is OK to assume 
  // that the input shape is convex for this feature.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Bevel(%g) not implemented\n", t);

  // Update mesh data structures
  Update();
}




void R3Mesh::
SplitFaces(void)
{
  // Split every face into K+1 faces (where K is the number of vertices on the face).
  // Creating a new vertex at the midpoint of every edge, 
  // remove the original face, create a new face connnecting all the new vertices,
  // and create new triangular faces connecting each vertex of the original face
  // with the new vertices associated with its adjacent edges.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SplitFaces not implemented\n");

  // Update mesh data structures
  Update();
}



void R3Mesh::
StarFaces(double factor)
{
  // Split every face into N faces (where N is the number of vertices on the face).
  // That is, create a new vertex at the centroid of the face, 
  // remove the original face, 
  // create N new triangular faces connecting the new
  // vertex with each pair of adjacent vertices of the original face.
  // Position the new vertex at a point that is offset from the centroid
  // of the face along the normal vector by a distance equal to factor 
  // times the average edge length for the face.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "StarFaces(%g) not implemented\n", factor);

  // Update mesh data structures
  Update();
}



void R3Mesh::
SplitLongEdges(double max_edge_length)
{
  // Iteratively split edges longer than max_edge_length.  
  // Note that every edge split produces a new vertex at the 
  // edge midpoint and replaces the two adjacent faces with four.  
  // Edges  should be split repeatedly until there is none longer 
  // than the given threshold.  Note: an extra point will be given if 
  // longer edges are split first (which produces better shaped faces).

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SplitLongEdges not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
CollapseShortEdges(double min_edge_length)
{
  // Iteratively collapse edges shorter than min_edge_length.  
  // Note that every edge collapse merges two vertices into one 
  // and removes up to two faces (if the adjacent faces are triangles).  
  // Place the new vertex at the midpoint 
  // of the collapsed edge.  Note: an extra point will be given if 
  // shorter edges are collapsed first (which produces better 
  // shaped faces).

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "CollapseShortEdges not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
ClusterVertices(double grid_cell_size)
{
  // Simplify the mesh by clustering vertices residing in the same 
  // cell of a grid defined by x, y, and z spacing parameters.  
  // All vertices within the same grid cell should be merged 
  // into a single vertex, that vertex should be placed at the 
  // centroid of the cluster vertices, and all edges and faces 
  // that collapse as a result of the vertex merging should be removed. 

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "ClusterVertices not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Bezier(const R3Mesh& control_mesh, int M, int N)
{
  // Create a smooth mesh using uniform cubic Bezier patches.
  // The input file should have M*N vertices representing control points arranged
  // in a M by N array.  The output file should contain a fine triangular mesh with
  // 4M * 4N vertices representing the cubic Bezier surface implied by the control points.
  // That is, vertices at sixteen regular intervals of u and v on each 4x4 subset
  // of the control mesh should be generated using the tensor product uniform cubic
  // Bezier surface construction and connnected into triangles to form a polygonal
  // approximation of the smooth Bezier surface.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Bezier not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
BSpline(const R3Mesh& control_mesh, int M, int N)
{
  // Create a smooth mesh using uniform cubic BSpline patches.
  // The input file should have M*N vertices representing control points arranged
  // in a M by N array.  The output file should contain a fine triangular mesh with
  // 4M * 4N vertices representing the cubic BSpline surface implied by the control points.
  // That is, vertices at sixteen regular intervals of u and v on each 4x4 subset
  // of the control mesh should be generated using the tensor product uniform cubic
  // BSpline surface construction and connnected into triangles to form a polygonal
  // approximation of the smooth BSpline surface.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "BSpline not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
SubdivideLoop(void)
{
  // Subdivide every face using the method in SplitFaces.
  // Then, update the positions of all vertices according to the Loop subdivision weights.  
  // This only must work correctly for meshes with triangular faces.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SubdivideLooop not implemented\n");

  // Update mesh data structures
  Update();
}



void R3Mesh::
SubdivideCatmullClark(void)
{
  // Subdivide every N-sided face into N quads by creating a new vertex in the center of 
  // every face connected to new vertices at the center of every edge, and then update 
  // the positions of all vertices according to the Catmull-Clark subdivision weights. 
  // This only must work correctly for meshes with quadrilateral faces. 

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SubdivideCatmullClark not implemented\n");

  // Update mesh data structures
  Update();
}



void R3Mesh::
SurfaceOfRevolution(const R3Mesh& profile_curve, 
  const R3Line& axis_of_revolution, double rotation_angle_step)
{
  // Add new vertices and faces to the mesh by sweeping a profile curve 
  // around an axis of revolution.  The vertices representing the profile 
  // curve are provided in the passed mesh file (take the vertices of the 
  // mesh in order and ignore the faces).  The axis of revolution and 
  // rotation  angle step size are provided in the arguments.  New vertices 
  // should be created by successively rotating the original vertices around 
  // the axis by the step size and new faces should be constructed by 
  // connecting adjacent vertices to create a surface of revolution.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SurfaceOfRevolution not implemented\n");

  // Update mesh data structures
  Update();
}



void R3Mesh::
SurfaceSweep(const R3Mesh& crosssection_polygon, const R3Mesh& centerline_curve)
{
  // Create new vertices and faces by sweeping a polygon along a curve.  
  // The vertices representing a cross-section polygon are provided in 
  // the first input mesh file, and the vertices representing the sweep 
  // centerline curve are provided in the second mesh file (for both, take 
  // the vertices of the meshes in order and ignore the faces).  New vertices 
  // should be created by successively translating and rotating the vertices 
  // of the cross-section polygon to match the position and orientation of 
  // vertices/edges in the centerline, and new faces should be constructed 
  // by connecting adjacent vertices created during the sweep.  
  // Note: an extra 3 points will be awarded if your implementation avoids 
  // self-intersecting polygons in all cases.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "SurfaceSweep not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
FixHoles(void)
{
  // Create faces covering the holes of a mesh by connecting vertices 
  // on the boundary of every hole.  You should completely cover the hole, 
  // while doing your best to produce well-shaped faces 
  // (e.g., by connecting closer vertices first).  

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "FixHoles not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
FixCracks(double epsilon)
{
  // Merge boundary vertices and edges within a specified 
  // distance (epsilon) of one another.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "FixCracks not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
FixIntersections(void)
{
  // Insert edges at face-face intersections and discard 
  // the smaller part of the mesh "pinched" off by new edge loops.  
  // Note: this is hard.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "FixIntersections not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Intersect(const R3Mesh& mesh)
{
  // Intersect the solid implied by this mesh with another, 
  // keeping only the faces enclosing the intersection of the two solids.
  // This feature requires introducing edges at every face intersection 
  // and removing parts of the mesh that lie in the exterior of the 
  // solid object implied by either of the two meshes. 

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Intersect not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Subtract(const R3Mesh& mesh)
{
  // Subtract the solid implied by this mesh with another, 
  // keeping only the faces enclosing the difference of the two solids.
  // This feature requires introducing edges at every face intersection 
  // and removing parts of the mesh that lie in the interior of the 
  // solid object implied by the passed mesh.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Subtract not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Union(const R3Mesh& mesh)
{
  // Union  the solid implied by this mesh with another, 
  // keeping only the faces enclosing the union of the two solids.
  // This feature requires introducing edges at every face intersection 
  // and removing parts of the mesh that lie in the interior of the 
  // solid object implied by both of the two meshes. 

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Union not implemented\n");

  // Update mesh data structures
  Update();
}




void R3Mesh::
Crop(const R3Plane& plane)
{
  // Crop the input mesh to the positive side of the plane.  
  // This feature requires clipping each polygon crossing the plane, 
  // and discarding any part of any face on the negative side of the plane.

  // FILL IN IMPLEMENTATION HERE
  fprintf(stderr, "Crop not implemented\n");

  // Update mesh data structures
  Update();
}




////////////////////////////////////////////////////////////
// MESH ELEMENT CREATION/DELETION FUNCTIONS
////////////////////////////////////////////////////////////

R3MeshVertex *R3Mesh::
CreateVertex(const R3Point& position, const R3Vector& normal, const R2Point& texcoords)
{
  // Create vertex
  R3MeshVertex *vertex = new R3MeshVertex(position, normal, texcoords);

  // Update bounding box
  bbox.Union(position);

  // Set vertex ID
  vertex->id = vertices.size();

  // Add to list
  vertices.push_back(vertex);

  // Return vertex
  return vertex;
}



R3MeshFace *R3Mesh::
CreateFace(const vector<R3MeshVertex *>& vertices)
{
  // Create face
  R3MeshFace *face = new R3MeshFace(vertices);

  // Set face  ID
  face->id = faces.size();

  // Add to list
  faces.push_back(face);

  // Return face
  return face;
}



void R3Mesh::
DeleteVertex(R3MeshVertex *vertex)
{
  // Remove vertex from list
  for (unsigned int i = 0; i < vertices.size(); i++) {
    if (vertices[i] == vertex) {
      vertices[i] = vertices.back();
      vertices[i]->id = i;
      vertices.pop_back();
      break;
    }
  }

  // Delete vertex
  delete vertex;
}



void R3Mesh::
DeleteFace(R3MeshFace *face)
{
  // Remove face from list
  for (unsigned int i = 0; i < faces.size(); i++) {
    if (faces[i] == face) {
      faces[i] = faces.back();
      faces[i]->id = i;
      faces.pop_back();
      break;
    }
  }

  // Delete face
  delete face;
}



////////////////////////////////////////////////////////////
// UPDATE FUNCTIONS
////////////////////////////////////////////////////////////

void R3Mesh::
Update(void)
{
  // Update everything
  UpdateBBox();
  UpdateFacePlanes();
  UpdateVertexNormals();
  UpdateVertexCurvatures();
}



void R3Mesh::
UpdateBBox(void)
{
  // Update bounding box
  bbox = R3null_box;
  for (unsigned int i = 0; i < vertices.size(); i++) {
    R3MeshVertex *vertex = vertices[i];
    bbox.Union(vertex->position);
  }
}



void R3Mesh::
UpdateVertexNormals(void)
{
  // Update normal for every vertex
  for (unsigned int i = 0; i < vertices.size(); i++) {
    vertices[i]->UpdateNormal();
  }
}




void R3Mesh::
UpdateVertexCurvatures(void)
{
  // Update curvature for every vertex
  for (unsigned int i = 0; i < vertices.size(); i++) {
    vertices[i]->UpdateCurvature();
  }
}




void R3Mesh::
UpdateFacePlanes(void)
{
  // Update plane for all faces
  for (unsigned int i = 0; i < faces.size(); i++) {
    faces[i]->UpdatePlane();
  }
}



////////////////////////////////////////////////////////////////////////
// DRAWING FUNCTIONS
////////////////////////////////////////////////////////////////////////

void R3Mesh::
Draw(void) const
{
  // Draw all faces
  for (int i = 0; i < NFaces(); i++) {
    glBegin(GL_POLYGON);
    R3MeshFace *face = Face(i);
    const R3Vector& normal = face->plane.Normal();
    glNormal3d(normal[0], normal[1], normal[2]);
    for (unsigned int j = 0; j < face->vertices.size(); j++) {
      R3MeshVertex *vertex = face->vertices[j];
      const R3Point& p = vertex->position;
      glVertex3d(p[0], p[1], p[2]);
    }
    glEnd();
  }
}



void R3Mesh::
Outline(void) const
{
  // Draw mesh in wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  Draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}




////////////////////////////////////////////////////////////////////////
// I/O FUNCTIONS
////////////////////////////////////////////////////////////////////////

int R3Mesh::
Read(const char *filename)
{
  // Parse input filename extension
  const char *extension;
  if (!(extension = strrchr(filename, '.'))) {
    printf("Filename %s has no extension (e.g., .ply)\n", filename);
    return 0;
  }

  // Read file of appropriate type
  int status = 0;
  if (!strncmp(extension, ".ray", 4)) 
    status = ReadRay(filename);
  else if (!strncmp(extension, ".off", 4)) 
    status = ReadOff(filename);
  else if (!strncmp(extension, ".jpg", 4)) 
    status = ReadImage(filename);
  else if (!strncmp(extension, ".jpeg", 4)) 
    status = ReadImage(filename);
  else if (!strncmp(extension, ".bmp", 4)) 
    status = ReadImage(filename);
  else if (!strncmp(extension, ".ppm", 4)) 
    status = ReadImage(filename);
  else {
    fprintf(stderr, "Unable to read file %s (unrecognized extension: %s)\n", filename, extension);
    return 0;
  }

  // Update mesh data structures
  Update();

  // Return success
  return 1;
}



int R3Mesh::
Write(const char *filename)
{
  // Parse input filename extension
  const char *extension;
  if (!(extension = strrchr(filename, '.'))) {
    printf("Filename %s has no extension (e.g., .ply)", filename);
    return 0;
  }

  // Write file of appropriate type
  if (!strncmp(extension, ".ray", 4)) 
    return WriteRay(filename);
  else if (!strncmp(extension, ".off", 4)) 
    return WriteOff(filename);
  else {
    fprintf(stderr, "Unable to write file %s (unrecognized extension: %s)", filename, extension);
    return 0;
  }
}



////////////////////////////////////////////////////////////
// IMAGE FILE INPUT/OUTPUT
////////////////////////////////////////////////////////////

int R3Mesh::
ReadImage(const char *filename)
{
  // Create a mesh by reading an image file, 
  // constructing vertices at (x,y,luminance), 
  // and connecting adjacent pixels into faces. 
  // That is, the image is interpretted as a height field, 
  // where the luminance of each pixel provides its z-coordinate.

  // Read image
  R2Image *image = new R2Image();
  if (!image->Read(filename)) return 0;

  // Create vertices and store in arrays
  R3MeshVertex ***vertices = new R3MeshVertex **[image->Width() ];
  for (int i = 0; i < image->Width(); i++) {
    vertices[i] = new R3MeshVertex *[image->Height() ];
    for (int j = 0; j < image->Height(); j++) {
      double luminance = image->Pixel(i, j).Luminance();
      double z = luminance * image->Width();
      R3Point position((double) i, (double) j, z);
      R2Point texcoords((double) i, (double) j);
      vertices[i][j] = CreateVertex(position, R3zero_vector, texcoords);
    }
  }

  // Create faces
  vector<R3MeshVertex *> face_vertices;
  for (int i = 1; i < image->Width(); i++) {
    for (int j = 1; j < image->Height(); j++) {
      face_vertices.clear();
      face_vertices.push_back(vertices[i-1][j-1]);
      face_vertices.push_back(vertices[i][j-1]);
      face_vertices.push_back(vertices[i][j]);
      CreateFace(face_vertices);
      face_vertices.clear();
      face_vertices.push_back(vertices[i-1][j-1]);
      face_vertices.push_back(vertices[i][j]);
      face_vertices.push_back(vertices[i-1][j]);
      CreateFace(face_vertices);
    }
  }

  // Delete vertex arrays
  for (int i = 0; i < image->Width(); i++) delete [] vertices[i];
  delete [] vertices;

  // Delete image
  delete image;

  // Return success
  return 1;
}



////////////////////////////////////////////////////////////
// OFF FILE INPUT/OUTPUT
////////////////////////////////////////////////////////////

int R3Mesh::
ReadOff(const char *filename)
{
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "r"))) {
    fprintf(stderr, "Unable to open file %s\n", filename);
    return 0;
  }

  // Read file
  int nverts = 0;
  int nfaces = 0;
  int nedges = 0;
  int line_count = 0;
  int vertex_count = 0;
  int face_count = 0;
  char buffer[1024];
  char header[64];
  while (fgets(buffer, 1023, fp)) {
    // Increment line counter
    line_count++;

    // Skip white space
    char *bufferp = buffer;
    while (isspace(*bufferp)) bufferp++;

    // Skip blank lines and comments
    if (*bufferp == '#') continue;
    if (*bufferp == '\0') continue;

    // Check section
    if (nverts == 0) {
      // Read header keyword
      if (strstr(bufferp, "OFF")) {
        // Check if counts are on first line
        int tmp;
        if (sscanf(bufferp, "%s%d%d%d", header, &tmp, &nfaces, &nedges) == 4) {
          nverts = tmp;
        }
      }
      else {
        // Read counts from second line
        if ((sscanf(bufferp, "%d%d%d", &nverts, &nfaces, &nedges) != 3) || (nverts == 0)) {
          fprintf(stderr, "Syntax error reading header on line %d in file %s\n", line_count, filename);
          fclose(fp);
          return 0;
        }
      }
    }
    else if (vertex_count < nverts) {
      // Read vertex coordinates
      double x, y, z;
      if (sscanf(bufferp, "%lf%lf%lf", &x, &y, &z) != 3) {
        fprintf(stderr, "Syntax error with vertex coordinates on line %d in file %s\n", line_count, filename);
        fclose(fp);
        return 0;
      }

      // Create vertex
      CreateVertex(R3Point(x, y, z), R3zero_vector, R2zero_point);

      // Increment counter
      vertex_count++;
    }
    else if (face_count < nfaces) {
      // Read number of vertices in face 
      int face_nverts = 0;
      bufferp = strtok(bufferp, " \t");
      if (bufferp) face_nverts = atoi(bufferp);
      else {
        fprintf(stderr, "Syntax error with face on line %d in file %s\n", line_count, filename);
        fclose(fp);
        return 0;
      }

      // Read vertex indices for face
      vector<R3MeshVertex *> face_vertices;
      for (int i = 0; i < face_nverts; i++) {
        R3MeshVertex *v = NULL;
        bufferp = strtok(NULL, " \t");
        if (bufferp) v = Vertex(atoi(bufferp));
        else {
          fprintf(stderr, "Syntax error with face on line %d in file %s\n", line_count, filename);
          fclose(fp);
          return 0;
        }

        // Add vertex to vector
        face_vertices.push_back(v);
      }

      // Create face
      vector<R3MeshVertex *> tri_vertices;
      for (unsigned int j = 2; j < face_vertices.size(); j++) {
        tri_vertices.clear();
        tri_vertices.push_back(face_vertices[0]);
        tri_vertices.push_back(face_vertices[j-1]);
        tri_vertices.push_back(face_vertices[j]);
        CreateFace(tri_vertices);
      }

      // Increment counter
      face_count++;
    }
    else {
      // Should never get here
      fprintf(stderr, "Found extra text starting at line %d in file %s\n", line_count, filename);
      break;
    }
  }

  // Check whether read all vertices
  if ((vertex_count != nverts) || (NVertices() < nverts)) {
    fprintf(stderr, "Expected %d vertices, but read %d vertex lines and created %d vertices in file %s\n", 
      nverts, vertex_count, NVertices(), filename);
  }

  // Check whether read all faces
  if ((face_count != nfaces) || (NFaces() < nfaces)) {
    fprintf(stderr, "Expected %d faces, but read %d face lines and created %d faces in file %s\n", 
      nfaces, face_count, NFaces(), filename);
  }

  // Close file
  fclose(fp);

  // Return number of faces read
  return NFaces();
}



int R3Mesh::
WriteOff(const char *filename)
{
  // Open file
  FILE *fp = fopen(filename, "w");
  if (!fp) {
    fprintf(stderr, "Unable to open file %s\n", filename);
    return 0;
  }

  // Write header
  fprintf(fp, "OFF\n");
  fprintf(fp, "%d %d %d\n", NVertices(), NFaces(), 0);

  // Write vertices
  for (int i = 0; i < NVertices(); i++) {
    R3MeshVertex *vertex = Vertex(i);
    const R3Point& p = vertex->position;
    fprintf(fp, "%g %g %g\n", p.X(), p.Y(), p.Z());
    vertex->id = i;
  }

  // Write Faces
  for (int i = 0; i < NFaces(); i++) {
    R3MeshFace *face = Face(i);
    fprintf(fp, "3");
    for (unsigned int j = 0; j < face->vertices.size(); j++) {
      fprintf(fp, " %d", face->vertices[j]->id);
    }
    fprintf(fp, "\n");
  }

  // Close file
  fclose(fp);

  // Return number of faces
  return NFaces();
}



////////////////////////////////////////////////////////////
// RAY FILE INPUT/OUTPUT
////////////////////////////////////////////////////////////

int R3Mesh::
ReadRay(const char *filename)
{
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "r"))) {
    fprintf(stderr, "Unable to open file %s", filename);
    return 0;
  }

  // Read body
  char cmd[128];
  int polygon_count = 0;
  int command_number = 1;
  while (fscanf(fp, "%s", cmd) == 1) {
    if (!strcmp(cmd, "#vertex")) {
      // Read data
      double px, py, pz;
      double nx, ny, nz;
      double ts, tt;
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf", &px, &py, &pz, &nx, &ny, &nz, &ts, &tt) != 8) {
        fprintf(stderr, "Unable to read vertex at command %d in file %s", command_number, filename);
        return 0;
      }

      // Create vertex
      R3Point point(px, py, pz);
      R3Vector normal(nx, ny, nz);
      R2Point texcoords(ts, tt);
      CreateVertex(point, normal, texcoords);
    }
    else if (!strcmp(cmd, "#shape_polygon")) {
      // Read data
      int m, nverts;
      if (fscanf(fp, "%d%d", &m, &nverts) != 2) {
        fprintf(stderr, "Unable to read polygon at command %d in file %s", command_number, filename);
        return 0;
      }

      // Get vertices
      vector<R3MeshVertex *> face_vertices;
      for (int i = 0; i < nverts; i++) {
        // Read vertex id
        int vertex_id;
        if (fscanf(fp, "%d", &vertex_id) != 1) {
          fprintf(stderr, "Unable to read polygon at command %d in file %s", command_number, filename);
          return 0;
        }

        // Get vertex
        R3MeshVertex *v = Vertex(vertex_id);
        face_vertices.push_back(v);
      }

      // Create face
      vector<R3MeshVertex *> tri_vertices;
      for (unsigned int j = 2; j < face_vertices.size(); j++) {
        tri_vertices.clear();
        tri_vertices.push_back(face_vertices[0]);
        tri_vertices.push_back(face_vertices[j-1]);
        tri_vertices.push_back(face_vertices[j]);
        CreateFace(tri_vertices);
      }

      // Increment polygon counter
      polygon_count++;
    }
	
    // Increment command number
    command_number++;
  }

  // Close file
  fclose(fp);

  // Return number of faces created
  return polygon_count;
}



int R3Mesh::
WriteRay(const char *filename)
{
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "w"))) {
    fprintf(stderr, "Unable to open file %s", filename);
    return 0;
  }

  // Write vertices
  for (int i = 0; i < NVertices(); i++) {
    R3MeshVertex *vertex = Vertex(i);
    const R3Point& p = vertex->position;
    const R3Vector& n = vertex->normal;
    const R2Point& t = vertex->texcoords;
    fprintf(fp, "#vertex %g %g %g  %g %g %g  %g %g\n", p.X(), p.Y(), p.Z(), 
      n.X(), n.Y(), n.Z(), t.X(), t.Y());
    vertex->id = i;
  }

  // Write faces
  for (int i = 0; i < NFaces(); i++) {
    R3MeshFace *face = Face(i);
    int nvertices = face->vertices.size();
    fprintf(fp, "#shape_polygon 0 %d ", nvertices);
    for (int j = 0; j < nvertices; j++) {
      R3MeshVertex *v = face->vertices[j];
      fprintf(fp, "%d ", v->id);
    }
    fprintf(fp, "\n");
  }

  // Close file
  fclose(fp);

  // Return number of faces written
  return NFaces();
}



////////////////////////////////////////////////////////////
// MESH VERTEX MEMBER FUNCTIONS
////////////////////////////////////////////////////////////

R3MeshVertex::
R3MeshVertex(void)
  : position(0, 0, 0),
    normal(0, 0, 0),
    texcoords(0, 0),
    curvature(0),
    id(0)
{
}



R3MeshVertex::
R3MeshVertex(const R3MeshVertex& vertex)
  : position(vertex.position),
    normal(vertex.normal),
    texcoords(vertex.texcoords),
    curvature(vertex.curvature),
    id(0)
{
}




R3MeshVertex::
R3MeshVertex(const R3Point& position, const R3Vector& normal, const R2Point& texcoords)
  : position(position),                    
    normal(normal),
    texcoords(texcoords),
    curvature(0),
    id(0)
{
}




double R3MeshVertex::
AverageEdgeLength(void) const
{
  // Return the average length of edges attached to this vertex
  // This feature should be implemented first.  To do it, you must
  // design a data structure that allows O(K) access to edges attached
  // to each vertex, where K is the number of edges attached to the vertex.

  // FILL IN IMPLEMENTATION HERE  (THIS IS REQUIRED)
  // BY REPLACING THIS ARBITRARY RETURN VALUE
  fprintf(stderr, "Average vertex edge length not implemented\n");
  return 0.12345;
}




void R3MeshVertex::
UpdateNormal(void)
{
  // Compute the surface normal at a vertex.  This feature should be implemented
  // second.  To do it, you must design a data structure that allows O(K)
  // access to faces attached to each vertex, where K is the number of faces attached
  // to the vertex.  Then, to compute the normal for a vertex,
  // you should take a weighted average of the normals for the attached faces, 
  // where the weights are determined by the areas of the faces.
  // Store the resulting normal in the "normal"  variable associated with the vertex. 
  // You can display the computed normals by hitting the 'N' key in meshview.

  // FILL IN IMPLEMENTATION HERE (THIS IS REQUIRED)
  // fprintf(stderr, "Update vertex normal not implemented\n");
}




void R3MeshVertex::
UpdateCurvature(void)
{
  // Compute an estimate of the Gauss curvature of the surface 
  // using a method based on the Gauss Bonet Theorem, which is described in 
  // [Akleman, 2006]. Store the result in the "curvature"  variable. 

  // FILL IN IMPLEMENTATION HERE
  // fprintf(stderr, "Update vertex curvature not implemented\n");
}





////////////////////////////////////////////////////////////
// MESH FACE MEMBER FUNCTIONS
////////////////////////////////////////////////////////////

R3MeshFace::
R3MeshFace(void)
  : vertices(),
    plane(0, 0, 0, 0),
    id(0)
{
}



R3MeshFace::
R3MeshFace(const R3MeshFace& face)
  : vertices(face.vertices),
    plane(face.plane),
    id(0)
{
}



R3MeshFace::
R3MeshFace(const vector<R3MeshVertex *>& vertices)
  : vertices(vertices),
    plane(0, 0, 0, 0),
    id(0)
{
  UpdatePlane();
}



double R3MeshFace::
AverageEdgeLength(void) const
{
  // Check number of vertices
  if (vertices.size() < 2) return 0;

  // Compute average edge length
  double sum = 0;
  R3Point *p1 = &(vertices.back()->position);
  for (unsigned int i = 0; i < vertices.size(); i++) {
    R3Point *p2 = &(vertices[i]->position);
    double edge_length = R3Distance(*p1, *p2);
    sum += edge_length;
    p1 = p2;
  }

  // Return the average length of edges attached to this face
  return sum / vertices.size();
}



double R3MeshFace::
Area(void) const
{
  // Check number of vertices
  if (vertices.size() < 3) return 0;

  // Compute area using Newell's method (assumes convex polygon)
  R3Vector sum = R3null_vector;
  const R3Point *p1 = &(vertices.back()->position);
  for (unsigned int i = 0; i < vertices.size(); i++) {
    const R3Point *p2 = &(vertices[i]->position);
    sum += p2->Vector() % p1->Vector();
    p1 = p2;
  }

  // Return area
  return 0.5 * sum.Length();
}



void R3MeshFace::
UpdatePlane(void)
{
  // Check number of vertices
  int nvertices = vertices.size();
  if (nvertices < 3) { 
    plane = R3null_plane; 
    return; 
  }

  // Compute centroid
  R3Point centroid = R3zero_point;
  for (int i = 0; i < nvertices; i++) 
    centroid += vertices[i]->position;
  centroid /= nvertices;
  
  // Compute best normal for counter-clockwise array of vertices using newell's method
  R3Vector normal = R3zero_vector;
  const R3Point *p1 = &(vertices[nvertices-1]->position);
  for (int i = 0; i < nvertices; i++) {
    const R3Point *p2 = &(vertices[i]->position);
    normal[0] += (p1->Y() - p2->Y()) * (p1->Z() + p2->Z());
    normal[1] += (p1->Z() - p2->Z()) * (p1->X() + p2->X());
    normal[2] += (p1->X() - p2->X()) * (p1->Y() + p2->Y());
    p1 = p2;
  }
  
  // Normalize normal vector
  normal.Normalize();
  
  // Update face plane
  plane.Reset(centroid, normal);
}



