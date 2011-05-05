//Source file for the R3 scene graph class 



// Include files 
#include <string>
#include <sstream>

#include "R3Scene.h"


using namespace std;

R3Scene::
R3Scene(void)
  : bbox(R3null_box),
    background(0,0,0,1),
    ambient(0,0,0,1)
{
  // Setup default camera
  camera.eye = R3zero_point;
  camera.towards = R3negz_vector;
  camera.up = R3posy_vector;
  camera.right = R3posx_vector;
  camera.xfov = 0.0;
  camera.yfov = 0.0;
  camera.neardist = 0.01;
  camera.fardist = 100.0;

  // Create root node
  root = new R3Node();
  root->parent = NULL;
  root->transformation = R3identity_matrix;
  root->material = NULL;
  root->shape = NULL;
  root->bbox = R3null_box;
}

int R3Scene::
WriteChunk(const char *filename)
{
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "w")))
  {
    fprintf(stderr, "Unable to write to file %s", filename);
    return 0;
  }
  // assumes blocks
  R3Point chunkStart = chunk[0][0][0]->shape->block->getBox().Min();

  std:: string s;
  std:: stringstream out;
  out << "# chunk chunk_id mat_id\n# low_x low_y low_z\n" <<
         "# block_size (1 side)\n# 16x16x16 (z, y, x) block types\n";
  out << "chunk 1 -1       " << chunkStart[0] << " " << chunkStart[1] <<
         " " << chunkStart[2] << " " << "1\n";

  s = out.str();
  fputs(s.c_str(), fp);

  for (int dz = 0; dz < CHUNK_Z; dz++)
  {
    for (int dy = 0; dy < CHUNK_Y; dy++)
    {
      std:: string tempS;
      std:: stringstream tempOut;
      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        tempOut << chunk[dx][dy][dz]->shape->block->getBlockType() << " ";
      }
      tempOut << "\n";
      fputs(tempOut.str().c_str(), fp);
    }
    fputs("\n", fp);
  }

  return 1;

}

int R3Scene::
Read(const char *filename, R3Node *node)
{
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "r"))) {
    fprintf(stderr, "Unable to open file %s", filename);
    return 0;
  }

  // Create array of materials
  vector<R3Material *> materials;

  // Create default material
  R3Material *default_material = new R3Material();
  default_material->ka = R3Rgb(0.2, 0.2, 0.2, 1);
  default_material->kd = R3Rgb(0.5, 0.5, 0.5, 1);
  default_material->ks = R3Rgb(0.5, 0.5, 0.5, 1);
  default_material->kt = R3Rgb(0.0, 0.0, 0.0, 1);
  default_material->emission = R3Rgb(0, 0, 0, 1);
  default_material->shininess = 10;
  default_material->indexofrefraction = 1;
  default_material->texture = NULL;
  default_material->id = 0;

  // Create stack of group information
  const int max_depth = 1024;
  R3Node *group_nodes[max_depth] = { NULL };
  R3Material *group_materials[max_depth] = { NULL };
  group_nodes[0] = (node) ? node : root;
  group_materials[0] = default_material;
  int depth = 0;

  // Read body
  char cmd[128];
  int command_number = 1;
  while (fscanf(fp, "%s", cmd) == 1) {
    if (cmd[0] == '#') {
      // Comment -- read everything until end of line
      do { cmd[0] = fgetc(fp); } while ((cmd[0] >= 0) && (cmd[0] != '\n'));
    }
    else if (!strcmp(cmd, "tri")) {
      // Read data
      int m;
      R3Point p1, p2, p3;
      if (fscanf(fp, "%d%lf%lf%lf%lf%lf%lf%lf%lf%lf", &m, 
        &p1[0], &p1[1], &p1[2], &p2[0], &p2[1], &p2[2], &p3[0], &p3[1], &p3[2]) != 10) {
        fprintf(stderr, "Unable to read triangle at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at tri command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create mesh
      R3Mesh *mesh = new R3Mesh();
      vector<R3MeshVertex *> vertices;
      vertices.push_back(mesh->CreateVertex(p1, R3zero_vector, R2zero_point));
      vertices.push_back(mesh->CreateVertex(p2, R3zero_vector, R2zero_point));
      vertices.push_back(mesh->CreateVertex(p3, R3zero_vector, R2zero_point));
      mesh->CreateFace(vertices);

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_MESH_SHAPE;
      shape->box = NULL;
      shape->sphere = NULL;
      shape->cylinder = NULL;
      shape->cone = NULL;
      shape->mesh = mesh;
      shape->segment = NULL;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = R3null_box;
      node->bbox.Union(p1);
      node->bbox.Union(p2);
      node->bbox.Union(p3);

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "box")) {
      // Read data
      int m;
      R3Point p1, p2;
      
      R3Point center, del = R3Point(1., 1., 1.);
      fscanf(fp, "%d%lf%lf%lf", &m, &center[0], &center[1], &center[2]);
      p1 = (center - del).Point();
      p2 = (center + del);

      //if (fscanf(fp, "%d%lf%lf%lf%lf%lf%lf", &m, &p1[0], &p1[1], &p1[2], &p2[0], &p2[1], &p2[2]) != 7) {
        //fprintf(stderr, "Unable to read box at command %d in file %s\n", command_number, filename);
        //return 0;
      //}

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at box command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create box
      R3Box *box = new R3Box(p1, p2);

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_BOX_SHAPE;
      shape->box = box;
      shape->sphere = NULL;
      shape->cylinder = NULL;
      shape->cone = NULL;
      shape->mesh = NULL;
      shape->segment = NULL;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = *box;
      node->selected = false;

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }






/*                 NEW ADDITION: CHUNK                     */
    else if (!strcmp(cmd, "chunk")) {
      // Read data
      int id, m;
      double block_side;
      R3Point start_point;
      
      //R3Point center, del = R3Point(1., 1., 1.);
      int read = fscanf(fp, "%d%d%lf%lf%lf%lf", &id, &m, &start_point[0],
                &start_point[1], &start_point[2], &block_side);
      if (read != 6)
      {
        fprintf(stderr, "Unable to read chunk at command %d in file %s, read %i things\n", command_number, filename, read);
        return 0;
      }
      //p1 = (center - del).Point();
      //p2 = (center + del);

      //if (fscanf(fp, "%d%lf%lf%lf%lf%lf%lf", &m, &p1[0], &p1[1], &p1[2], &p2[0], &p2[1], &p2[2]) != 7) {
        //fprintf(stderr, "Unable to read box at command %d in file %s\n", command_number, filename);
        //return 0;
      //}

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at box command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      for (int dz = 0; dz < CHUNK_Z; dz++)
      {
        for (int dy = 0; dy < CHUNK_Y; dy++)
        {
          for (int dx = 0; dx < CHUNK_X; dx++)
          {
            int block_type;
            if (fscanf(fp, "%d", &block_type) != 1)
            {
              fprintf(stderr, "Unable to read chunk block at command %d in file %s\n", command_number, filename);
              return 0;
            }
            else
              fprintf(stderr, "Chunk block %d, %d, %d read with type %d\n", dx, dy, dz, block_type);

            //fprintf(stderr, "AIR_BLOCK has type %d\n", AIR_BLOCK);
            R3Point block_start = start_point + R3Point(dx * block_side,
                                dy * block_side, dz * block_side);
            R3Point block_end = block_start + R3Point(block_side, block_side,
                                block_side);
            // Create box
            R3Box box = R3Box(block_start, block_end);
            R3Block* block = new R3Block(box, block_type);

            //fprintf(stderr, "scanning block %d, %d, %d of type %f\n", dx, dy,
            //                    dz, block_side);
            // Create shape
            R3Shape *shape = new R3Shape();
            shape->type = R3_BLOCK_SHAPE;
            shape->box = NULL;
            shape->sphere = NULL;
            shape->cylinder = NULL;
            shape->cone = NULL;
            shape->mesh = NULL;
            shape->segment = NULL;
            shape->block = block;

            // Create shape node
            R3Node *node = new R3Node();
            node->transformation = R3identity_matrix;
            node->material = material;
            node->shape = shape;
            node->bbox = box;
            node->selected = false;

            // Insert node
            //group_nodes[depth]->bbox.Union(node->bbox);
            //group_nodes[depth]->children.push_back(node);
            //node->parent = group_nodes[depth];
            //               block_side);
            chunk[dx][dy][dz] = node;
          }
        }
      }
          }
/*                CHUNK DONE                      */






    else if (!strcmp(cmd, "sphere")) {
      // Read data
      int m;
      R3Point c;
      double r;
      if (fscanf(fp, "%d%lf%lf%lf%lf", &m, &c[0], &c[1], &c[2], &r) != 5) {
        fprintf(stderr, "Unable to read sphere at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at sphere command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create sphere
      R3Sphere *sphere = new R3Sphere(c, r);

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_SPHERE_SHAPE;
      shape->box = NULL;
      shape->sphere = sphere;
      shape->cylinder = NULL;
      shape->cone = NULL;
      shape->mesh = NULL;
      shape->segment = NULL;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = sphere->BBox();

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "cylinder")) {
      // Read data
      int m;
      R3Point c;
      double r, h;
      if (fscanf(fp, "%d%lf%lf%lf%lf%lf", &m, &c[0], &c[1], &c[2], &r, &h) != 6) {
        fprintf(stderr, "Unable to read cylinder at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at cyl command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create cylinder
      R3Cylinder *cylinder = new R3Cylinder(c, r, h);

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_CYLINDER_SHAPE;
      shape->box = NULL;
      shape->sphere = NULL;
      shape->cylinder = cylinder;
      shape->cone = NULL;
      shape->mesh = NULL;
      shape->segment = NULL;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = cylinder->BBox();

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "mesh")) {
      // Read data
      int m;
      char meshname[256];
      if (fscanf(fp, "%d%s", &m, meshname) != 2) {
        fprintf(stderr, "Unable to parse mesh command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at cone command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Get mesh filename
      char buffer[2048];
      strcpy(buffer, filename);
      char *bufferp = strrchr(buffer, '/');
      if (bufferp) *(bufferp+1) = '\0';
      else buffer[0] = '\0';
      strcat(buffer, meshname);

      // Create mesh
      R3Mesh *mesh = new R3Mesh();
      if (!mesh) {
        fprintf(stderr, "Unable to allocate mesh\n");
        return 0;
      }

      // Read mesh file
      if (!mesh->Read(buffer)) {
        fprintf(stderr, "Unable to read mesh: %s\n", buffer);
        return 0;
      }

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_MESH_SHAPE;
      shape->box = NULL;
      shape->sphere = NULL;
      shape->cylinder = NULL;
      shape->cone = NULL;
      shape->mesh = mesh;
      shape->segment = NULL;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = mesh->bbox;

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "cone")) {
      // Read data
      int m;
      R3Point c;
      double r, h;
      if (fscanf(fp, "%d%lf%lf%lf%lf%lf", &m, &c[0], &c[1], &c[2], &r, &h) != 6) {
        fprintf(stderr, "Unable to read cone at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at cone command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create cone
      R3Cone *cone = new R3Cone(c, r, h);

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_CONE_SHAPE;
      shape->box = NULL;
      shape->sphere = NULL;
      shape->cylinder = NULL;
      shape->cone = cone;
      shape->mesh = NULL;
      shape->segment = NULL;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = cone->BBox();

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "line")) {
      // Read data
      int m;
      R3Point p1, p2;
      if (fscanf(fp, "%d%lf%lf%lf%lf%lf%lf", &m, &p1[0], &p1[1], &p1[2], &p2[0], &p2[1], &p2[2]) != 7) {
        fprintf(stderr, "Unable to read line at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at line command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create segment
      R3Segment *segment = new R3Segment(p1, p2);

      // Create shape
      R3Shape *shape = new R3Shape();
      shape->type = R3_SEGMENT_SHAPE;
      shape->box = NULL;
      shape->sphere = NULL;
      shape->cylinder = NULL;
      shape->cone = NULL;
      shape->mesh = NULL;
      shape->segment = segment;
      shape->block = NULL;

      // Create shape node
      R3Node *node = new R3Node();
      node->transformation = R3identity_matrix;
      node->material = material;
      node->shape = shape;
      node->bbox = segment->BBox();

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "begin")) {
      // Read data
      int m;
      double matrix[16];
      if (fscanf(fp, "%d%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &m, 
        &matrix[0], &matrix[1], &matrix[2], &matrix[3], 
        &matrix[4], &matrix[5], &matrix[6], &matrix[7], 
        &matrix[8], &matrix[9], &matrix[10], &matrix[11], 
        &matrix[12], &matrix[13], &matrix[14], &matrix[15]) != 17) {
        fprintf(stderr, "Unable to read begin at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[depth];
      if (m >= 0) {
        if (m < (int) materials.size()) {
          material = materials[m];
        }
        else {
          fprintf(stderr, "Invalid material id at cone command %d in file %s\n", command_number, filename);
          return 0;
        }
      }

      // Create new group node
      R3Node *node = new R3Node();
      node->transformation = R3Matrix(matrix);
      node->material = NULL;
      node->shape = NULL;
      node->bbox = R3null_box;

      // Push node onto stack
      depth++;
      group_nodes[depth] = node;
      group_materials[depth] = material;
    }
    else if (!strcmp(cmd, "end")) {
      // Pop node from stack
      R3Node *node = group_nodes[depth];
      depth--;

      // Transform bounding box
      node->bbox.Transform(node->transformation);

      // Insert node
      group_nodes[depth]->bbox.Union(node->bbox);
      group_nodes[depth]->children.push_back(node);
      node->parent = group_nodes[depth];
    }
    else if (!strcmp(cmd, "material")) {
      // Read data
      R3Rgb ka, kd, ks, kt, e;
      double n, ir;
      char texture_name[256];
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%s", 
          &ka[0], &ka[1], &ka[2], &kd[0], &kd[1], &kd[2], &ks[0], &ks[1], &ks[2], &kt[0], &kt[1], &kt[2], 
          &e[0], &e[1], &e[2], &n, &ir, texture_name) != 18) {
        fprintf(stderr, "Unable to read material at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Create material
      R3Material *material = new R3Material();
      material->ka = ka;
      material->kd = kd;
      material->ks = ks;
      material->kt = kt;
      material->emission = e;
      material->shininess = n;
      material->indexofrefraction = ir;
      material->texture = NULL;

      // Read texture
      if (strcmp(texture_name, "0")) {
        // Get texture filename
        char buffer[2048];
        strcpy(buffer, filename);
        char *bufferp = strrchr(buffer, '/');
        if (bufferp) *(bufferp+1) = '\0';
        else buffer[0] = '\0';
        strcat(buffer, texture_name);

        // Read texture image
        material->texture = new R2Image();
        if (!material->texture->Read(buffer)) {
          fprintf(stderr, "Unable to read texture from %s at command %d in file %s\n", buffer, command_number, filename);
          return 0;
        }
      }

      // Insert material
      materials.push_back(material);
    }
    else if (!strcmp(cmd, "dir_light")) {
      // Read data
      R3Rgb c;
      R3Vector d;
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf", 
        &c[0], &c[1], &c[2], &d[0], &d[1], &d[2]) != 6) {
        fprintf(stderr, "Unable to read directional light at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Normalize direction
      d.Normalize();

      // Create light
      R3Light *light = new R3Light();
      light->type = R3_DIRECTIONAL_LIGHT;
      light->color = c;
      light->position = R3Point(0, 0, 0);
      light->direction = d;
      light->radius = 0;
      light->constant_attenuation = 0;
      light->linear_attenuation = 0;
      light->quadratic_attenuation = 0;
      light->angle_attenuation = 0;
      light->angle_cutoff = M_PI;

      // Insert light
      lights.push_back(light);
    }
    else if (!strcmp(cmd, "point_light")) {
      // Read data
      R3Rgb c;
      R3Point p;
      double ca, la, qa;
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &c[0], &c[1], &c[2], &p[0], &p[1], &p[2], &ca, &la, &qa) != 9) {
        fprintf(stderr, "Unable to read point light at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Create light
      R3Light *light = new R3Light();
      light->type = R3_POINT_LIGHT;
      light->color = c;
      light->position = p;
      light->direction = R3Vector(0, 0, 0);
      light->radius = 0;
      light->constant_attenuation = ca;
      light->linear_attenuation = la;
      light->quadratic_attenuation = qa;
      light->angle_attenuation = 0;
      light->angle_cutoff = M_PI;

      // Insert light
      lights.push_back(light);
    }
    else if (!strcmp(cmd, "spot_light")) {
      // Read data
      R3Rgb c;
      R3Point p;
      R3Vector d;
      double ca, la, qa, sc, sd;
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", 
        &c[0], &c[1], &c[2], &p[0], &p[1], &p[2], &d[0], &d[1], &d[2], &ca, &la, &qa, &sc, &sd) != 14) {
        fprintf(stderr, "Unable to read point light at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Normalize direction
      d.Normalize();

      // Create light
      R3Light *light = new R3Light();
      light->type = R3_SPOT_LIGHT;
      light->color = c;
      light->position = p;
      light->direction = d;
      light->radius = 0;
      light->constant_attenuation = ca;
      light->linear_attenuation = la;
      light->quadratic_attenuation = qa;
      light->angle_attenuation = sd;
      light->angle_cutoff = sc;

      // Insert light
      lights.push_back(light);
    }
    else if (!strcmp(cmd, "area_light")) {
      // Read data
      R3Rgb c;
      R3Point p;
      R3Vector d;
      double radius, ca, la, qa;
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", 
        &c[0], &c[1], &c[2], &p[0], &p[1], &p[2], &d[0], &d[1], &d[2], &radius, &ca, &la, &qa) != 13) {
        fprintf(stderr, "Unable to read area light at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Normalize direction
      d.Normalize();

      // Create light
      R3Light *light = new R3Light();
      light->type = R3_AREA_LIGHT;
      light->color = c;
      light->position = p;
      light->direction = d;
      light->radius = radius;
      light->constant_attenuation = ca;
      light->linear_attenuation = la;
      light->quadratic_attenuation = qa;
      light->angle_attenuation = 0;
      light->angle_cutoff = M_PI;

      // Insert light
      lights.push_back(light);
    }
    else if (!strcmp(cmd, "camera")) {
      // Read data
      double px, py, pz, dx, dy, dz, ux, uy, uz, xfov, neardist, fardist;
      if (fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &px, &py, &pz, &dx, &dy, &dz, &ux, &uy, &uz, &xfov, &neardist, &fardist) != 12) {
        fprintf(stderr, "Unable to read camera at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Assign camera
      camera.eye = R3Point(px, py, pz);
      camera.towards = R3Vector(dx, dy, dz);
      camera.towards.Normalize();
      camera.up = R3Vector(ux, uy, uz);
      camera.up.Normalize();
      camera.right = camera.towards % camera.up;
      camera.right.Normalize();
      camera.up = camera.right % camera.towards;
      camera.up.Normalize();
      camera.xfov = xfov;
      camera.yfov = xfov;
      camera.neardist = neardist;
      camera.fardist = fardist;
    }
    else if (!strcmp(cmd, "include")) {
      // Read data
      char scenename[256];
      if (fscanf(fp, "%s", scenename) != 1) {
        fprintf(stderr, "Unable to read include command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Get scene filename
      char buffer[2048];
      strcpy(buffer, filename);
      char *bufferp = strrchr(buffer, '/');
      if (bufferp) *(bufferp+1) = '\0';
      else buffer[0] = '\0';
      strcat(buffer, scenename);

      // Read scene from included file
      if (!Read(buffer, group_nodes[depth])) {
        fprintf(stderr, "Unable to read included scene: %s\n", buffer);
        return 0;
      }
    }
    else if (!strcmp(cmd, "background")) {
      // Read data
      double r, g, b;
      if (fscanf(fp, "%lf%lf%lf", &r, &g, &b) != 3) {
        fprintf(stderr, "Unable to read background at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Assign background color
      background = R3Rgb(r, g, b, 1);
    }
    else if (!strcmp(cmd, "ambient")) {
      // Read data
      double r, g, b;
      if (fscanf(fp, "%lf%lf%lf", &r, &g, &b) != 3) {
        fprintf(stderr, "Unable to read ambient at command %d in file %s\n", command_number, filename);
        return 0;
      }

      // Assign ambient color
      ambient = R3Rgb(r, g, b, 1);
    }
    else {
      fprintf(stderr, "Unrecognized command %d in file %s: %s\n", command_number, filename, cmd);
      return 0;
    }
	
    // Increment command number
    command_number++;
  }

  // Update bounding box
  bbox = root->bbox;

  // Provide default camera
  if (camera.xfov == 0) {
    double scene_radius = bbox.DiagonalRadius();
    R3Point scene_center = bbox.Centroid();
    camera.towards = R3Vector(0, 0, -1);
    camera.up = R3Vector(0, 1, 0);
    camera.right = R3Vector(1, 0, 0);
    camera.eye = scene_center - 3 * scene_radius * camera.towards;
    camera.xfov = 0.25;
    camera.yfov = 0.25;
    camera.neardist = 0.01 * scene_radius;
    camera.fardist = 100 * scene_radius;
  }

  // Provide default lights
  if (lights.size() == 0) {
    // Create first directional light
    R3Light *light = new R3Light();
    R3Vector direction(-3,-4,-5);
    direction.Normalize();
    light->type = R3_DIRECTIONAL_LIGHT;
    light->color = R3Rgb(1,1,1,1);
    light->position = R3Point(0, 0, 0);
    light->direction = direction;
    light->radius = 0;
    light->constant_attenuation = 0;
    light->linear_attenuation = 0;
    light->quadratic_attenuation = 0;
    light->angle_attenuation = 0;
    light->angle_cutoff = M_PI;
    lights.push_back(light);

    // Create second directional light
    light = new R3Light();
    direction = R3Vector(3,2,3);
    direction.Normalize();
    light->type = R3_DIRECTIONAL_LIGHT;
    light->color = R3Rgb(0.5, 0.5, 0.5, 1);
    light->position = R3Point(0, 0, 0);
    light->direction = direction;
    light->radius = 0;
    light->constant_attenuation = 0;
    light->linear_attenuation = 0;
    light->quadratic_attenuation = 0;
    light->angle_attenuation = 0;
    light->angle_cutoff = M_PI;
    lights.push_back(light);
  }

  // Close file
  fclose(fp);

  // Return success
  return 1;
}



