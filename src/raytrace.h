// Include file for ray tracing code

#include <cmath>
#include <float.h>
#include "R2/R2.h"
#include "R3/R3.h"

// Basic comparison macros
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

struct R3Intersection
{
    bool hit;
    R3Node *node;
    R3Point position;
    R3Vector normal;
    double t;
};

R2Image *RenderImage(R3Scene *scene, int width, int height, int max_depth, 
  int num_primary_rays_per_pixel, int num_distributed_rays_per_intersection);

void RayCast(R3Scene *scene, R2Image *image, int width, int height);

R3Intersection IntersectSphere(R3Ray ray, R3Sphere sphere);

R3Intersection IntersectBox(R3Ray ray, R3Box box);

R3Intersection IntersectMesh(R3Ray ray, R3Mesh mesh);

R3Intersection IntersectCylinder(R3Ray ray, R3Cylinder cylinder);

R3Intersection IntersectCone(R3Ray ray, R3Cone cone);

R3Intersection IntersectScene(R3Ray ray, R3Scene *scene, R3Node *node);

R3Rgb ComputeRadiance(R3Scene *scene, R3Ray ray, R3Intersection intersect, int depth);

