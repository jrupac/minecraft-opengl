// Source file for raytracing code


// Include files

#include "R3Scene.h"
#include "raytrace.h"
using namespace std;

#define PRINT_POINT(x) printf("%lf %lf %lf\n", (x).X(), (x).Y(), (x).Z());
#define SQ(x) (x) * (x)
// The fudge factor
#define EPS 0.00001

static int MAX_DEPTH;

////////////////////////////////////////////////////////////////////////
// Create image from scene
//
// This is the main ray tracing function called from raypro
// 
// "width" and "height" indicate the size of the ray traced image
//   (keep these small during debugging to speed up your code development cycle)
//
// "max_depth" indicates the maximum number of secondary reflections/transmissions to trace for any ray
//   (i.e., stop tracing a ray if it has already been reflected max_depth times -- 
//   0 means direct illumination, 1 means one bounce, etc.)
//
// "num_primary_rays_per_pixel" indicates the number of random rays to generate within 
//   each pixel during antialiasing.  This argument can be ignored if antialiasing is not implemented.
//
// "num_distributed_rays_per_intersection" indicates the number of secondary rays to generate
//   for each surface intersection if distributed ray tracing is implemented.  
//   It can be ignored otherwise.
// 
////////////////////////////////////////////////////////////////////////

R2Image *RenderImage(R3Scene *scene, int width, int height, int max_depth,
  int num_primary_rays_per_pixel, int num_distributed_rays_per_intersection)
{
  // Allocate  image
  R2Image *image = new R2Image(width, height);
  if (!image) {
    fprintf(stderr, "Unable to allocate image\n");
    return NULL;
  }
  
  // Set depth to be at least one
  MAX_DEPTH = MAX(max_depth, 0) + 1;
  // Let's do this!
  RayCast(scene, image, width, height);

  // Return image
  return image;
}

void RayCast(R3Scene *scene, R2Image *image, int width, int height)
{
  R3Camera c = scene->camera;
  // Pre-compute corner points of view plane
  R3Point P1x = c.eye + c.towards - tan(c.xfov) * c.right - tan(c.yfov) * c.up,
          P2x = c.eye + c.towards + tan(c.xfov) * c.right - tan(c.yfov) * c.up,
          P1y = P1x,
          P2y = c.eye + c.towards - tan(c.xfov) * c.right + tan(c.yfov) * c.up,
          corner = P1x;

  // Iterate over all pixels in the image
  for (int i = 0; i < height; i++) 
  {
    for (int j = 0; j < width; j++) 
    {
      R3Point Px = P1x + ((j + 0.5) / width) * (P2x - P1x);
      R3Point Py = P1y + ((i + 0.5) / height) * (P2y - P1y);
      R3Vector V = (Px + Py) - (corner + c.eye);
      V.Normalize();
      
      // This is the ray coming out of the camera
      R3Ray ray = R3Ray(c.eye, V);
      
      // Obtain pixel from raytracing
      R3Intersection intersect = IntersectScene(ray, scene, scene->root);
      R3Rgb p = ComputeRadiance(scene, ray, intersect, 0);

      // Write pixel to image
      image->Pixel(j, i) = p;   
    }
  }
}

R3Intersection IntersectSphere(R3Ray ray, R3Sphere sphere)
{
  R3Intersection intersect;
  double tca, thc, d2, r = sphere.Radius();
  R3Point center = sphere.Center(), start = ray.Start();
  R3Vector L = center - start, vector = ray.Vector();

  if (((tca = L.Dot(vector)) < EPS) || (d2 = L.Dot(L) - SQ(tca)) > SQ(r) - EPS) 
      intersect.hit = false;
  else
  {
      thc = sqrt(SQ(r) - d2);
      intersect.hit = true;
      intersect.t = tca - thc;
      
      if (intersect.t < EPS)
        intersect.t += 2. * thc;
      intersect.position = start + intersect.t * vector;
      intersect.normal = intersect.position - center;
      intersect.normal.Normalize();
  }

  return intersect;
}

R3Intersection IntersectBox(R3Ray ray, R3Box box)
{
  R3Intersection intersect;
  intersect.hit = false;

  // Get box corner points 
  R3Point corners[8];
  corners[0] = box.Corner(0, 0, 0);
  corners[1] = box.Corner(0, 0, 1);
  corners[2] = box.Corner(0, 1, 1);
  corners[3] = box.Corner(0, 1, 0);
  corners[4] = box.Corner(1, 0, 0);
  corners[5] = box.Corner(1, 0, 1);
  corners[6] = box.Corner(1, 1, 1);
  corners[7] = box.Corner(1, 1, 0);

  // Get normals and ordering of vertices around each face
  static GLdouble normals[6][3] = {
    { -1.0, 0.0, 0.0 },
    { 1.0, 0.0, 0.0 },
    { 0.0, -1.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, -1.0 },
    { 0.0, 0.0, 1.0 }
  };
  static int surface_paths[6][4] = {
    { 3, 0, 1, 2 },
    { 4, 7, 6, 5 },
    { 0, 4, 5, 1 },
    { 7, 3, 2, 6 },
    { 3, 7, 4, 0 },
    { 1, 5, 6, 2 }
  };

  double minDist = DBL_MAX;

  // Iterate over all faces of the box
  for (int i = 0; i < 6; i++) 
  {
    // Construct the i-th face plane - only three points are necessary
    R3Plane plane = R3Plane(corners[surface_paths[i][0]],
                            corners[surface_paths[i][1]], 
                            corners[surface_paths[i][2]]);
    R3Vector n = R3Vector(normals[i]);

    // If ray is perpendicular to normal of plane, they will never intersect
    if (ray.Vector().Dot(n) > -EPS)
      continue;

    // Compute the minimum t-value of the intersection
    double t = -(ray.Start().Vector().Dot(n) + plane.D()) / 
               (ray.Vector().Dot(n));
    
    // If intersection is behind camera, it doesn't count
    if (t < EPS)
      continue;

    // Compute the point of intersection and the corners of the min distance 
    // plane; these define the extreme points of the box's face plane
    R3Point pt = ray.Start() + (t * ray.Vector()),
            minPt = corners[surface_paths[i][0]],
            maxPt = corners[surface_paths[i][2]];
  
    // Check if the point of intersection is inside the box's face plane
    R3Vector diff = (pt - minPt);
    R3Vector uvec = (corners[surface_paths[i][1]]-corners[surface_paths[i][0]]);
    R3Vector vvec = (corners[surface_paths[i][3]]-corners[surface_paths[i][0]]);
    double du = diff.Dot(uvec), dv = diff.Dot(vvec), 
           ul = uvec.Length(), vl = vvec.Length();

    if (du >= -EPS && dv >= -EPS && du - ul * ul <= EPS && dv - vl * vl <= EPS &&
        t < minDist)
    {
      intersect.position = pt;
      intersect.normal = normals[i];
      intersect.t = t;
      intersect.hit = true;
      minDist = t;
    }
  }
  
  return intersect;
}

static R3Intersection IntersectTri(R3Ray ray, R3MeshFace face)
{
  R3Intersection intersect;
  intersect.hit = false;
  R3Plane plane = face.plane;
  R3Vector n = plane.Normal(), vector = ray.Vector();
  R3Point a = ray.Start();
  double d;

  // If the ray vector and normal of the plane are perpendicular, they will
  // never intersect and if the dot product is positive, the plane faces away
  if ((d = vector.Dot(n)) > -EPS)
    return intersect;

  // Compute the minimum t-value of the intersection
  double t = -(a.Vector().Dot(n) + plane.D()) / d;
  
  // If intersection is behind camera, it doesn't count
  if (t < EPS)
    return intersect;

  R3Point p = a + t * vector;

  for (int i = 0; i < 3; i++) 
  {
    R3Vector V1 = face.vertices[i % 3]->position - p, 
             V2 = face.vertices[(i+1) % 3]->position - p;
    V2.Cross(V1);
    if (vector.Dot(V2) < EPS)
      return intersect;
  }

  intersect.hit = true;
  intersect.position = ray.Start() + t * ray.Vector();
  intersect.normal = n;
  intersect.normal.Normalize();
  intersect.t = t;

  return intersect;
}

R3Intersection IntersectMesh(R3Ray ray, R3Mesh mesh)
{
  R3Intersection intersect, curInt;
  int N = mesh.NFaces();
  double curT = DBL_MAX;
  intersect.hit = false;
  
  for(int i = 0; i < N; i++)
  {
    curInt = IntersectTri(ray, *mesh.Face(i));
    
    if (curInt.hit && curInt.t < curT)
    {
      curT = curInt.t;
      intersect = curInt;
    }
  }

  return intersect;
}

static double IntersectCap(R3Ray ray, R3Plane plane, double r2)
{
  R3Point start = ray.Start();
  R3Vector vector = ray.Vector(), n = plane.Normal();

  // If ray is perpendicular to normal of plane, they will never intersect
  if (vector.Dot(n) < EPS)
  {
    // Compute the minimum t-value of the intersection
    double t = -(start.Vector().Dot(n) + plane.D()) / 
               (vector.Dot(n));
    
    // If intersection is behind camera, it doesn't count
    if (t > EPS)
    {
      R3Point pt = start + (t * vector);

      // Now check if the intersection is inside the cap 
      if (pt.X() * pt.X() + pt.Z() * pt.Z() <= r2 + EPS)
          return t;
    }
  }

  return DBL_MAX;
}

R3Intersection IntersectCylinder(R3Ray ray, R3Cylinder cylinder)
{
  R3Intersection intersect;
  R3Point start = ray.Start(), center = cylinder.Center(), pt;
  R3Vector vector = ray.Vector();
  double r = cylinder.Radius(), h = cylinder.Height(), T = DBL_MAX;
  R3Plane plane;
  intersect.hit = false;

  // First compute intersection with sides
  double a = SQ(vector.X()) + SQ(vector.Z()),
         b = 2. * (start.X() * vector.X() + start.Z() * vector.Z()),
         c = SQ(start.X()) + SQ(start.Z()) - SQ(r);

  double dis = SQ(b) - 4 * a * c;

  // Only imaginary solutions => no intersections
  if (dis < EPS)
  {
    intersect.hit = false;
    return intersect;
  }

  double t1 = (-b + sqrt(dis)) / (2. * a),
         t2 = (-b - sqrt(dis)) / (2. * a); 

  // Both solutions are behind the camera
  if (MAX(t1, t2) < EPS)
  {
    intersect.hit = false;
    return intersect;
  }

  // If smaller t is negative, set to larger t
  T = MIN(t1, t2);
  if (T < EPS)
    T = MAX(t1, t2);

  // Ensure that the intersection is not below or above cylinder
  double yy = (start + T * vector).Y();
  if ((yy < center.Y() - h/2 - EPS) || (yy > center.Y() + h/2 + EPS))
    T = DBL_MAX;
  else
  {
    pt = start + T * vector;
    intersect.normal = pt - R3Point(center.X(), pt.Y(), center.Z());
  }

  // Now compute the caps
 
  // First the bottom cap
  plane = R3Plane(center + (h / 2) * R3negy_point, R3negy_vector);
  double tTop = IntersectCap(ray, plane, SQ(r));
  if (tTop < T)
  {
    T = tTop;
    intersect.normal = R3negy_vector;
  }

  // Then the top cap
  plane = R3Plane(center + (h / 2) * R3posy_point, R3posy_vector);

  double tBot = IntersectCap(ray, plane, SQ(r));
  if (tBot < T)
  {
    T = tTop;
    intersect.normal = R3posy_vector;
  }
  

  if (T == DBL_MAX)
    intersect.hit = false;
  else
  {
    intersect.hit = true;
    intersect.t = T;
    intersect.position = start + T * vector;
    intersect.normal.Normalize();
  }

  return intersect;
}

R3Intersection IntersectCone(R3Ray ray, R3Cone cone)
{
  R3Intersection intersect;
  R3Point start = ray.Start(), center = cone.Center(), pt;
  R3Vector vector = ray.Vector();
  double r = cone.Radius(), h = cone.Height(), 
         T = DBL_MAX, rh = SQ(r / h), ch = center.Y() + h / 2;
  R3Plane plane;
  intersect.hit = false;

  // First compute intersection with sides
  double a = SQ(vector.X()) + SQ(vector.Z()) - rh * SQ(vector.Y()),
         b = 2. * (start.X() * vector.X() + start.Z() * vector.Z() - 
                   rh * (start.Y() * vector.Y() - vector.Y() * ch)),
         c = SQ(start.X()) + SQ(start.Z()) - 
             rh * (SQ(start.Y()) - 2. * start.Y() * ch +  SQ(ch));

  double dis = SQ(b) - 4 * a * c;

  // Only imaginary solutions => no intersections
  if (dis < EPS)
  {
    intersect.hit = false;
    return intersect;
  }

  double t1 = (-b + sqrt(dis)) / (2. * a),
         t2 = (-b - sqrt(dis)) / (2. * a); 

  // Both solutions are behind the camera
  if (MAX(t1, t2) < EPS)
  {
    intersect.hit = false;
    return intersect;
  }

  // If smaller t is negative, set to larger t
  T = MIN(t1, t2);
  if (T < EPS)
    T = MAX(t1, t2);

  // Ensure that the intersection is not below or above cylinder
  double yy = (start + T * vector).Y();
  if ((yy < ch - h - EPS) || (yy > ch + EPS))
    T = DBL_MAX;
  else
  {
    pt = start + T * vector;
    intersect.normal = pt - R3Point(center.X(), 
                                    pt.Y() * (1. + rh) - rh * (center.Y() + h / 2), 
                                    center.Z());
  }

  // Now compute the bottom cap
  plane = R3Plane(center + (h/2) * R3negy_point, R3negy_vector);
  double tTop = IntersectCap(ray, plane, SQ(r));
  if (tTop < T)
  {
    T = tTop;
    intersect.normal = R3negy_vector;
  }

  if (T == DBL_MAX)
    intersect.hit = false;
  else
  {
    intersect.hit = true;
    intersect.t = T;
    intersect.position = start + T * vector;
    intersect.normal.Normalize();
  }

  return intersect;
}

static inline R3Intersection IntersectShape(R3Ray ray, R3Shape shape)
{
  switch(shape.type)
  {
    case R3_BOX_SHAPE:    
      return IntersectBox(ray, *shape.box); 
    case R3_SPHERE_SHAPE: 
      return IntersectSphere(ray, *shape.sphere); 
    case R3_MESH_SHAPE:   
      return IntersectMesh(ray, *shape.mesh); 
    case R3_CYLINDER_SHAPE: 
      return IntersectCylinder(ray, *shape.cylinder); 
    case R3_CONE_SHAPE: 
      return IntersectCone(ray, *shape.cone); 
    // Passive-aggressively handle everything else
    default:
      R3Intersection intersect;
      intersect.hit = false;
      return intersect;
  }
}

R3Intersection IntersectScene(R3Ray ray, R3Scene *scene, R3Node *node)
{
  double minT = DBL_MAX;
  R3Intersection intersect;
  intersect.hit = false;

  if (node->shape)
  {
    R3Intersection curInt = IntersectShape(ray, *(node->shape));

    if (curInt.hit && curInt.t < minT)
    {
      minT = curInt.t;
      intersect = curInt;
      intersect.node = node;
    }
  }
  
  int numChild = node->children.size();
  R3Ray curRay = R3Ray(ray);
  curRay.InverseTransform(node->transformation);

  for (int i = 0; i < numChild; i++) 
  {
    R3Intersection childInt = IntersectScene(curRay, scene, node->children[i]);
    if (childInt.hit && childInt.t < minT)
    {
      minT = childInt.t;
      intersect = childInt;
    }
  }

  intersect.position.Transform(node->transformation);
  intersect.normal.Transform(node->transformation);
  intersect.t = ray.T(intersect.position);
  return intersect;
}

static inline R3Rgb ComputeIrradiance(R3Light light, R3Intersection intersect)
{
  double d, theta;
  R3Vector L;

  switch(light.type)
  {
    case R3_POINT_LIGHT:
      d = R3Distance(intersect.position, light.position);
      return light.color / (light.constant_attenuation + 
                            light.linear_attenuation * d + 
                            light.quadratic_attenuation * d * d);
    case R3_SPOT_LIGHT:
      d = R3Distance(intersect.position, light.position);
      L = R3Vector(intersect.position - light.position);
      L.Normalize();
      theta = acos(L.Dot(light.direction));
      if (theta > light.angle_cutoff)
        return R3Rgb();
      return light.color * pow(cos(theta), light.angle_attenuation) / 
             (light.constant_attenuation + light.linear_attenuation * d +
              light.quadratic_attenuation * d * d);
    case R3_DIRECTIONAL_LIGHT:
      //if (light.direction.Dot(intersect.normal) < 0.0)
        return light.color;
      //return R3Rgb();
    case R3_AREA_LIGHT:
    case R3_NUM_LIGHT_TYPES:
      return light.color;
  }
  
  // Control should never reach here
  return light.color;
}

R3Rgb ComputeRadiance(R3Scene *scene, R3Ray ray, R3Intersection intersect, int depth)
{
  // Recursion termination condition
  if (depth == MAX_DEPTH)
    return R3Rgb();

  // If there is no hit, return the background of the environment
  if (!intersect.hit) 
    return scene->background;

  R3Vector N = intersect.normal,
           //V = R3Vector(ray.Start() - intersect.position);
           V = -ray.Vector();
  R3Material *mat = intersect.node->material;
  
  if (!mat)
    return scene->background;

  int numLights = scene->lights.size();
  V.Normalize();

  // Ambient
  R3Rgb p = R3Rgb(mat->ka * scene->ambient + mat->emission);

  if (N.Dot(V) < EPS)
    return p;

  // Iterate over every light
  for (int i = 0; i < numLights; i++) 
  {
    R3Vector L = R3Vector(scene->lights[i]->position - intersect.position);
    if (scene->lights[i]->type == R3_DIRECTIONAL_LIGHT)
      L = -scene->lights[i]->direction;

    // Create the shadow ray
    R3Ray shadowRay = R3Ray(intersect.position + EPS * L, L);
    // Obtain distance from intersection point to light position
    double dist = L.Length();
    // Compute intersection of reverse ray and check if it intersects something
    // before hitting the light's position; if so, don't add contribution of
    // this light at this pixel
    R3Intersection revInt = IntersectScene(shadowRay, scene, scene->root);
    if (revInt.hit && revInt.t < dist)
      continue;

    // Diffuse
    if (N.Dot(L) >= -EPS)
    {
      L /= dist;
      R3Rgb IL = ComputeIrradiance(*(scene->lights[i]), intersect);
      R3Vector R = (2. * L.Dot(N) * N) - L; 
      R.Normalize();
        
      p += mat->kd * N.Dot(L) * IL;
      // Specular
      if (V.Dot(R) >= -EPS)
        p += mat->ks * pow(V.Dot(R), mat->shininess) * IL;
    }
  }

  // Create specular ray
  R3Vector Ref = (2 * V.Dot(N) * N) - V;
  R3Ray specularRay = R3Ray(intersect.position + EPS * Ref, Ref);
  // Add the color returned by the recursive call
  p += mat->ks * ComputeRadiance(scene, specularRay, 
                                 IntersectScene(specularRay, scene, scene->root),
                                 depth + 1);

  // Do transmission by first computing the intersection on the opposite side 
  // of the surface
  R3Intersection other = IntersectScene(R3Ray(intersect.position + EPS * ray.Vector(), 
                                              ray.Vector()), 
                                        scene, scene->root);
  R3Ray transmissionRay = R3Ray(other.position + EPS * ray.Vector(), ray.Vector());
  p += mat->kt * ComputeRadiance(scene, transmissionRay, 
                                 IntersectScene(transmissionRay, scene, scene->root),
                                 depth + 1);

  return p;
}

