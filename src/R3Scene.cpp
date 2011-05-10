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
  root->bbox = R3null_box;
  
  for (int xChunks = 0; xChunks < CHUNKS; xChunks++)
    for (int zChunks = 0; zChunks < CHUNKS; zChunks++)
      terrain[xChunks][zChunks] = new R3Chunk();
}

R3Index R3Scene::
getIndex(R3Point p)
{
  R3Index newIndex;
  int chunkX = (CHUNKS - 1) / 2;
  int chunkZ = (CHUNKS - 1) / 2;
  int chunksAway = 0;

  newIndex.x = (int)(p[0] - terrain[chunkX][chunkZ]->start_point[0]);
  newIndex.y = (int)(p[1] + CHUNK_Y / 2);
  newIndex.z = (int)(p[2] - terrain[chunkX][chunkZ]->start_point[2]);
  newIndex.current = NULL;
  
  //fprintf(stderr, "first guess at index: point is %d %d %d \n", newIndex.x, newIndex.y, newIndex.z);
  
  while (newIndex.x < 0)
  {
    newIndex.x += CHUNK_X;
    --chunkX;
    if (++chunksAway > 1)
      return newIndex;
  }
  chunksAway = 0;
  while (newIndex.x > CHUNK_X - 1)
  {
    newIndex.x -= CHUNK_X;
    ++chunkX;
    if (++chunksAway > 1)
      return newIndex;
  }
  chunksAway = 0;
  while (newIndex.z < 0)
  {
    newIndex.z += CHUNK_Z;
    --chunkZ;
    if (++chunksAway > 1)
      return newIndex;
  }
  chunksAway = 0;
  while (newIndex.z > CHUNK_Z - 1)
  {
    newIndex.z -= CHUNK_Z;
    ++chunkZ;
    if (++chunksAway > 1)
      return newIndex;
  }
  
  newIndex.current = terrain[chunkX][chunkZ];
 // fprintf(stderr, "ChunkX is %d and ChunkZ is %d\n", chunkX, chunkZ);

  return newIndex;
}

R3Chunk* R3Scene::
LoadChunk(int x_chunk, int z_chunk)
{
 // fprintf(stderr, "Loading chunk: %d, %d.\n", x_chunk, z_chunk);
  /*fprintf(stderr, "Current generated chunks are: ");
  set< pair<int, int> >:: iterator it;
  for ( it=generatedChunks.begin() ; it != generatedChunks.end(); it++ )
    fprintf(stderr, "(%d, %d) ", it->first, it->second);
  fprintf(stderr, "\n");*/


  
  R3Chunk* newCh = new R3Chunk();
  pair <int, int> checkPair (x_chunk, z_chunk);

  // If we do not find a chunk at this index, generate a new one. Otherwise, 
  // load the existing one from disk and return it
  if (generatedChunks.find(checkPair) == generatedChunks.end()) 
  {
     if (!newCh->GenerateChunk(x_chunk, z_chunk))
     {
       fprintf(stderr, "Unable to generate chunk: %d, %d!!!\n", x_chunk, z_chunk);
       return NULL;
     }
     else
     {
       pair<int, int> newChunk (x_chunk, z_chunk);
       generatedChunks.insert(newChunk);
     }
  }
  else
  {
    int failure = 10;
    while (!newCh->ReadChunk(x_chunk, z_chunk) && failure > 0)
    {
      fprintf(stderr, "Unable to open chunk file: %d, %d!!!\n %d tries remaining \n", x_chunk, z_chunk, failure);
      //newCh->GenerateChunk(x_chunk, z_chunk);
      failure--;
      //return NULL;
    }
    if (failure == 1)
    {
      fprintf(stderr, "Fuck it - making new chunk. \n");
      newCh->GenerateChunk(x_chunk, z_chunk);
    }
  }

  return newCh;
}

int R3Scene::
UpdateScene(R3Point loc)
{
//  fprintf(stderr, "Updating location: new is (%f, %f, %f)\n", loc[0], loc[1], loc[2]);

  // Get middle chunk of those loaded in memory
  R3Chunk *cur = terrain[(CHUNKS - 1) / 2][(CHUNKS - 1) / 2];

  R3Point low = cur->start_point;
  R3Point high = cur->end_point;
  int oldChunkX = cur->chunk_x; 
  int oldChunkZ = cur->chunk_z; 

  //fprintf(stderr, "Old chunk x is %d and z is %d\n", oldChunkX, oldChunkZ);
  
  // X-directions
  while (loc[0] < low[0]) // moved out of lower bounds for x
  {
    // Get chunk coordinates of chunk to the left of the leftmost one loaded in
    // memory before the update
    int xChunkLoc = terrain[0][0]->chunk_x - 1;

    fprintf(stderr, "MOVING LEFT IN X\n");

    for (int zChunks = 0; zChunks < CHUNKS; zChunks++)
    {
      // Delete rightmost row of currently loaded chunks
      terrain[CHUNKS - 1][zChunks]->DeleteChunk();
      
      // Set the second-to-last row to the last row
      for (int xChunks = CHUNKS - 1; xChunks > 0 ; xChunks--)
        terrain[xChunks][zChunks] = terrain[xChunks - 1][zChunks];
      
      // Create new leftmost row
      terrain[0][zChunks] = LoadChunk(xChunkLoc, oldChunkZ + zChunks - (CHUNKS - 1) / 2);
    }
    
    // Reset variables after this new chunk loading
    cur = terrain[(CHUNKS - 1) / 2][(CHUNKS - 1 ) / 2];
    low = cur->start_point;
    high = cur->end_point;
    oldChunkX = cur->chunk_x; 
    oldChunkZ = cur->chunk_z; 
  }
  
  while (loc[0] > high[0]) // moved out of higher bounds for x
  {
    // Get chunk coordinates of chunk to the right of the rightmost one loaded in
    // memory before the update
    int xChunkLoc = terrain[CHUNKS - 1][0]->chunk_x + 1;

    fprintf(stderr, "MOVING RIGHT IN X\n");

    for (int zChunks = 0; zChunks < CHUNKS; zChunks++)
    {
      // Delete leftmost row of currently loaded chunks
      terrain[0][zChunks]->DeleteChunk();

      // Set the second-to-last row to the last row
      for (int xChunks = 0; xChunks < CHUNKS - 1; xChunks++)
        terrain[xChunks][zChunks] = terrain[xChunks + 1][zChunks];

      // Create new rightmost row
      terrain[CHUNKS - 1][zChunks] = LoadChunk(xChunkLoc,  oldChunkZ + zChunks - (CHUNKS - 1) / 2);
    }
    
    // Reset variables after this new chunk loading
    cur = terrain[(CHUNKS - 1) / 2][(CHUNKS - 1 ) / 2];
    low = cur->start_point;
    high = cur->end_point;
    oldChunkX = cur->chunk_x; 
    oldChunkZ = cur->chunk_z;
  }
  
  // Z-directions
  while (loc[2] < low[2]) // moved out of lower bounds for z
  {
    // Get chunk coordinates of chunk lower than the lowest one loaded in
    // memory before the update
    int zChunkLoc = terrain[0][0]->chunk_z - 1;

    fprintf(stderr, "MOVING CLOSER IN Z\n");

    for (int xChunks = 0; xChunks < CHUNKS; xChunks++)
    {
      // Delete highest row of currently loaded chunks
      terrain[xChunks][CHUNKS - 1]->DeleteChunk();

      // Set the second-to-last row to the last row
      for (int zChunks = CHUNKS - 1; zChunks > 0 ; zChunks--)
        terrain[xChunks][zChunks] = terrain[xChunks][zChunks - 1];

      // Create new lowest row
      terrain[xChunks][0] = LoadChunk(oldChunkX + xChunks - (CHUNKS - 1) / 2, zChunkLoc);
    }
    
    // Reset variables after this new chunk loading
    cur = terrain[(CHUNKS - 1) / 2][(CHUNKS - 1 ) / 2];
    low = cur->start_point;
    high = cur->end_point;
    oldChunkX = cur->chunk_x; 
    oldChunkZ = cur->chunk_z;
  }

  while (loc[2] > high[2]) // moved out of higher bounds for z
  {
    // Get chunk coordinates of chunk higher than the highest one loaded in
    // memory before the update
    int zChunkLoc = terrain[0][CHUNKS-1]->chunk_z + 1;

    fprintf(stderr, "MOVING FURTHER IN Z\n");

    for (int xChunks = 0; xChunks < CHUNKS; xChunks++)
    {
      // Delete lowest row of currently loaded chunks
      terrain[xChunks][0]->DeleteChunk();

      // Set the second-to-last row to the last row
      for (int zChunks = 0; zChunks < CHUNKS - 1; zChunks++)
        terrain[xChunks][zChunks] = terrain[xChunks][zChunks + 1];

      // Create new highest row
      terrain[xChunks][CHUNKS - 1] = LoadChunk(oldChunkX + xChunks - (CHUNKS - 1) / 2, zChunkLoc);
    }
    
    // Reset variables after this new chunk loading
    cur = terrain[(CHUNKS - 1) / 2][(CHUNKS - 1 ) / 2];
    low = cur->start_point;
    high = cur->end_point;
    oldChunkX = cur->chunk_x; 
    oldChunkZ = cur->chunk_z;
  }
  
  return 1;
}

int R3Scene::
Read(const char *filename, R3Node *node)
{
  bool foundChunks = false;
  //bool foundChar = false;

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

  // Provide these two lights by default
  
  // Create first directional light
  R3Light *light = new R3Light();
  R3Vector direction(-3,-4,-5);
  direction.Normalize();
  light->type = R3_DIRECTIONAL_LIGHT;
  light->color = R3Rgb(1, 1, 1, 1);
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
  light->color = R3Rgb(1, 1, 1, 1);
  light->position = R3Point(0, 0, 0);
  light->direction = direction;
  light->radius = 0;
  light->constant_attenuation = 0;
  light->linear_attenuation = 0;
  light->quadratic_attenuation = 0;
  light->angle_attenuation = 0;
  light->angle_cutoff = M_PI;
  lights.push_back(light);
	
  // Read body
  char cmd[128];
  int command_number = 1;
  while (fscanf(fp, "%s", cmd) == 1) {
    if (cmd[0] == '#') {
      // Comment -- read everything until end of line
      do { cmd[0] = fgetc(fp); } while ((cmd[0] >= 0) && (cmd[0] != '\n'));
    }
    else if (!strcmp(cmd, "chunk")) {

      // Read which chunks we generated, one at a time
      int x, z;
      if (fscanf(fp, "%d%d", &x, &z) != 2)
      {
        fprintf(stderr, "wrong chunk declaration in scene read\n");
        return 0;
      }

      foundChunks = true;
      pair <int, int> addChunk (x, z);
      generatedChunks.insert(addChunk);
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

  // Provide completely non-default chunks :)
  //if (foundChunks == false) {
    for (int xChunks = 0; xChunks < CHUNKS; xChunks++)
    {
      for (int zChunks = 0; zChunks < CHUNKS; zChunks++)
      {
        int xChunkCoord = xChunks - (CHUNKS - 1) / 2;
        int zChunkCoord = zChunks - (CHUNKS - 1) / 2;
        terrain[xChunks][zChunks] = LoadChunk(xChunkCoord, zChunkCoord);
      }
    }
  //}
  /*if (!UpdateScene(R3Point(0, 0, 0)))
  {
    fprintf(stderr, "Fucked up.\n");
    return 0;
  }*/

  // Close file
  fclose(fp);

  // Return success
  return 1;
}

int R3Scene::
WriteScene(const char* filename)
{
  fprintf(stderr, "Saving entire scene ");
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "w")))
  {
    fprintf(stderr, "Unable to write to scene %s", filename);
    return 0;
  }
  
  //write which chunks to look for:
  set< pair<int, int> >:: iterator it;
  for ( it=generatedChunks.begin() ; it != generatedChunks.end(); it++ )
  {
    fprintf(stderr, "Saving chunk: (%d, %d) ", it->first, it->second);
    //std:: stringstream out;
    //out << "chunk " << it->first << " " << it->second << "\n";
    //fputs(out.str().c_str(), fp);
    fprintf(fp, "chunk %d %d\n", it->first, it->second);
    
  }
  
  for (int dx = 0; dx < CHUNKS; dx++)
  {
    for (int dz = 0; dz < CHUNKS; dz++)
    {
      terrain[dx][dz]->DeleteChunk();
    }
  }
  fclose(fp);
  
  return 1;
  
}

