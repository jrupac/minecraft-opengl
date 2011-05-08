#include "R3Chunk.h"

R3Chunk::R3Chunk(void)
{
}

int R3Chunk::
GenerateChunk(int c_x, int c_z)
{
  fprintf(stderr, "No chunk found: generating new chunk at chunk pos (%d, %d)\n", c_x, c_z);
  //THIS IS GONNA BE TERRIBLE
  //REMOVE ME PLEASE I JUST WANT TO DIE
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

  R3Material *group_materials[max_depth] = { NULL };
  group_materials[0] = default_material;
  int depth = 0;

  // Load other textures
	R3Material *leaf_material = new R3Material();
	leaf_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	leaf_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	leaf_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	leaf_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	leaf_material->emission = R3Rgb(0, 0, 0, 0);
	leaf_material->shininess = 10;
	leaf_material->indexofrefraction = 1;

	//	char buffer[] = "input/checker.bmp";
	char leaf[] = "input/leaf.jpg";

	// Read texture image
	leaf_material->texture = new R2Image();
	if (!leaf_material->texture->Read(leaf)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	leaf_material->id = 1;
	group_materials[1] = leaf_material;
	
	R3Material *dirt_material = new R3Material();
	dirt_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	dirt_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	dirt_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	dirt_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	dirt_material->emission = R3Rgb(0, 0, 0, 0);
	dirt_material->shininess = 10;
	dirt_material->indexofrefraction = 1;

	//	char buffer[] = "input/checker.bmp";
	char dirt[] = "input/dirt.jpg";

	// Read texture image
	dirt_material->texture = new R2Image();
	if (!dirt_material->texture->Read(dirt)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	dirt_material->id = 2;
	group_materials[2] = dirt_material;
	
	R3Material *grass_material = new R3Material();
	grass_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	grass_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	grass_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	grass_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	grass_material->emission = R3Rgb(0, 0, 0, 0);
	grass_material->shininess = 10;
	grass_material->indexofrefraction = 1;

	//	char buffer[] = "input/checker.bmp";
	char grass[] = "input/grass.jpg";
	//REMOVE ME PLEASE DONE
	
	R3Material *material = group_materials[0];
  //fuck it
  start_point = R3Point((double)c_x*16.0 - 8.0, -CHUNK_Y/2, (double)c_z*16.0 - 8.0);
  end_point = R3Point((double)c_x*16.0 + 8.0, -CHUNK_Y/2, (double)c_z*16.0 + 8.0);
  block_side = 1.0;
  
  for (int dy = 0; dy < CHUNK_Y; dy++)
  {
    for (int dz = 0; dz < CHUNK_Z; dz++)
    {
      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        int block_type = 1;
         if (dy < CHUNK_Y/2)
          block_type = 0;
         

            R3Point block_start = start_point + R3Point(dx * block_side,
                dy * block_side, dz * block_side);
            R3Point block_end = block_start + R3Point(block_side, block_side,
                block_side);

            // Create box
            R3Box box = R3Box(block_start, block_end);
            R3Block* block = new R3Block(box, block_type);

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

            if (block_type == LEAF_BLOCK) 
              material = group_materials[1];
           
            if (block_type == DIRT_BLOCK)
              material = group_materials[3];

            node->material = material;
            node->shape = shape;
            node->bbox = box;
            node->selected = false;

            chunk[dx][dy][dz] = node;
			  node->shape->block->dx = dx;
			  node->shape->block->dy = dy;
			  node->shape->block->dz = dz;
      }
    }
  }
  // Set adjacent chunks
      for (int dz = 0; dz < CHUNK_Z; dz++)
      {
        for (int dy = 0; dy < CHUNK_Y; dy++)
        {
          for (int dx = 0; dx < CHUNK_X; dx++)
          {
            if (dy == (CHUNK_Y -1))
              chunk[dx][dy][dz]->shape->block->setUpper(NULL);
            else if (dy != (CHUNK_Y -1))
              chunk[dx][dy][dz]->shape->block->setUpper(chunk[dx][dy+1][dz]->shape->block);
          }
        }
      }
}

void R3Chunk::
DeleteChunk()
{
  for (int dz = 0; dz < CHUNK_Z; dz++)
  {
    for (int dy = 0; dy < CHUNK_Y; dy++)
    {
      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        //delete chunk[dz][dy][dz]->shape->block; //clear some memory
      }
    }
  }
}

int R3Chunk::
ReadChunk(int xChunkCoord, int zChunkCoord)
{

  fprintf(stderr, "Chunk found: reading old chunk at chunk pos (%d, %d)\n", xChunkCoord, zChunkCoord);

  std:: string name;
  std:: stringstream nameS;
  nameS << "world/chunks/chunk" << xChunkCoord << "." << zChunkCoord << ".scn";
  const char *filename = nameS.str().c_str();
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

  R3Material *group_materials[max_depth] = { NULL };
  group_materials[0] = default_material;
  int depth = 0;

  // Load other textures
	R3Material *leaf_material = new R3Material();
	leaf_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	leaf_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	leaf_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	leaf_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	leaf_material->emission = R3Rgb(0, 0, 0, 0);
	leaf_material->shininess = 10;
	leaf_material->indexofrefraction = 1;

	//	char buffer[] = "input/checker.bmp";
	char leaf[] = "input/leaf.jpg";

	// Read texture image
	leaf_material->texture = new R2Image();
	if (!leaf_material->texture->Read(leaf)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	leaf_material->id = 1;
	group_materials[1] = leaf_material;
	
	R3Material *dirt_material = new R3Material();
	dirt_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	dirt_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	dirt_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	dirt_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	dirt_material->emission = R3Rgb(0, 0, 0, 0);
	dirt_material->shininess = 10;
	dirt_material->indexofrefraction = 1;

	//	char buffer[] = "input/checker.bmp";
	char dirt[] = "input/dirt.jpg";

	// Read texture image
	dirt_material->texture = new R2Image();
	if (!dirt_material->texture->Read(dirt)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	dirt_material->id = 2;
	group_materials[2] = dirt_material;
	
	R3Material *grass_material = new R3Material();
	grass_material->ka = R3Rgb(0.0, 0.0, 0.0, 0.0);
	grass_material->kd = R3Rgb(0.5, 0.5, 0.5,0.0);
	grass_material->ks = R3Rgb(0.5, 0.5, 0.5,0.0);
	grass_material->kt = R3Rgb(0.0, 0.0, 0.0,0.0);
	grass_material->emission = R3Rgb(0, 0, 0, 0);
	grass_material->shininess = 10;
	grass_material->indexofrefraction = 1;

	//	char buffer[] = "input/checker.bmp";
	char grass[] = "input/grass.jpg";

	// Read texture image
	grass_material->texture = new R2Image();
	if (!grass_material->texture->Read(grass)) {
		fprintf(stderr, "Unable to read texture from file");
		//	return 0;
	}	
	grass_material->id = 3;
	group_materials[3] = dirt_material;
	
  
  // Read body
  char cmd[128];
  int command_number = 1;
  while (fscanf(fp, "%s", cmd) == 1) {
    if (cmd[0] == '#') {
      // Comment -- read everything until end of line
      do { cmd[0] = fgetc(fp); } while ((cmd[0] >= 0) && (cmd[0] != '\n'));
    }
    else if (!strcmp(cmd, "chunk")) {

      // Read data
      int id, m;
      //double block_side;
      //R3Point start_point;
      
      int read = fscanf(fp, "%d%d%lf%lf%lf%lf", &id, &m, &start_point[0],
                &start_point[1], &start_point[2], &block_side);
      if (read != 6)
      {
        fprintf(stderr, "Unable to read chunk at command %d in file %s, read %i things\n", command_number, filename, read);
        return 0;
      }

      // Get material
      R3Material *material = group_materials[0];

      //start = start_point;
      

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

            R3Point block_start = start_point + R3Point(dx * block_side,
                dy * block_side, dz * block_side);
            R3Point block_end = block_start + R3Point(block_side, block_side,
                block_side);

            // Create box
            R3Box box = R3Box(block_start, block_end);
            R3Block* block = new R3Block(box, block_type);

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

            if (block_type == LEAF_BLOCK) 
              material = group_materials[1];
           
            if (block_type == DIRT_BLOCK)
              material = group_materials[3];

            node->material = material;
            node->shape = shape;
            node->bbox = box;
            node->selected = false;

            chunk[dx][dy][dz] = node;
			  node->shape->block->dx = dx;
			  node->shape->block->dy = dy;
			  node->shape->block->dz = dz;
          }
        }
      }

      // Set adjacent chunks
      for (int dz = 0; dz < CHUNK_Z; dz++)
      {
        for (int dy = 0; dy < CHUNK_Y; dy++)
        {
          for (int dx = 0; dx < CHUNK_X; dx++)
          {
            if (dy == (CHUNK_Y -1))
              chunk[dx][dy][dz]->shape->block->setUpper(NULL);
            else if (dy != (CHUNK_Y -1))
              chunk[dx][dy][dz]->shape->block->setUpper(chunk[dx][dy+1][dz]->shape->block);
          }
        }
      }
    }
  }
  chunk_x = (int)(2*start_point[0]/CHUNK_X) + 1; // -8*2/16 = -1
  chunk_z = (int)(2*start_point[2]/CHUNK_Z) + 1; // -16*2/16 = -2
  end_point = start_point + R3Point(CHUNK_X, CHUNK_Y, CHUNK_Z);
  return 1;
}




int R3Chunk::
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
