#include "R3Chunk.h"

R3Chunk::R3Chunk(void)
{
}

int R3Chunk::
GenerateChunk(int c_x, int c_z)
{
  chunk_x = c_x;
  chunk_z = c_z;
  //fprintf(stderr, "No chunk found: generating new chunk at chunk pos (%d, %d)\n", c_x, c_z);

  start_point = R3Point((double)c_x * 16.0 - 8.0, -CHUNK_Y / 2, 
                        (double)c_z * 16.0 - 8.0);
  end_point = start_point + R3Point(CHUNK_X, CHUNK_Y, CHUNK_Z);
  block_side = 1.0;

  // Create all blocks in this chunk
  for (int dy = 0; dy < CHUNK_Y; dy++)
  {
    for (int dz = 0; dz < CHUNK_Z; dz++)
    {
      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        int block_type;

        if (dy < CHUNK_Y / 2)
          block_type = DIRT_BLOCK;
        else 
          block_type = AIR_BLOCK;

        R3Point block_start = start_point + block_side * R3Point(dx, dy, dz);
        R3Point block_end = block_start + block_side * R3ones_point;

        // Create box
        R3Box box = R3Box(block_start, block_end);
        R3Block* block = new R3Block(box, block_type);
        block->dx = dx;
        block->dy = dy;
        block->dz = dz;

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
        node->shape = shape;
        node->bbox = box;
        node->selected = false;

        chunk[dx][dy][dz] = node;
      }
    }
  }

  // Compute the block above each block in the chunk
  for (int dz = 0; dz < CHUNK_Z; dz++)
  {
    for (int dy = 0; dy < CHUNK_Y; dy++)
    {
      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        if (dy == CHUNK_Y - 1)
          chunk[dx][dy][dz]->shape->block->setUpper(NULL);
        else 
          chunk[dx][dy][dz]->shape->block->setUpper(chunk[dx][dy+1][dz]->shape->block);
      }
    }
  }

  return 1;
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
        delete chunk[dx][dy][dz]->shape->block;
      //  delete chunk[dx][dy][dz]->material;
        delete chunk[dx][dy][dz];
      }
    }
  }
}

int R3Chunk::
ReadChunk(int xChunkCoord, int zChunkCoord)
{
  //fprintf(stderr, "Chunk found: reading old chunk at chunk pos (%d, %d)\n", xChunkCoord, zChunkCoord);

  std:: string name;
  std:: stringstream nameS;
  nameS << s_World << xChunkCoord << "." << zChunkCoord << ".scn";
  const char *filename = nameS.str().c_str();

  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "r"))) {
    fprintf(stderr, "Unable to open file %s", filename);
    return 0;
  }
  
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
      
      int read = fscanf(fp, "%d%d%lf%lf%lf%lf", &id, &m, &start_point[0],
                        &start_point[1], &start_point[2], &block_side);
      if (read != 6)
      {
        fprintf(stderr, "Unable to read chunk at command %d in file %s, read %i things\n", command_number, filename, read);
        return 0;
      }

      // Load every block into memory
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

            R3Point block_start = start_point + block_side * R3Point(dx, dy, dz);
            R3Point block_end = block_start + block_side * R3ones_point;

            // Create box
            R3Box box = R3Box(block_start, block_end);
            R3Block* block = new R3Block(box, block_type);
            block->dx = dx;
            block->dy = dy;
            block->dz = dz;

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
            node->shape = shape;
            node->bbox = box;
            node->selected = false;

            chunk[dx][dy][dz] = node;
          }
        }
      }

      // Compute the block above each block in the chunk
      for (int dz = 0; dz < CHUNK_Z; dz++)
      {
        for (int dy = 0; dy < CHUNK_Y; dy++)
        {
          for (int dx = 0; dx < CHUNK_X; dx++)
          {
            if (dy == CHUNK_Y - 1)
              chunk[dx][dy][dz]->shape->block->setUpper(NULL);
            else 
              chunk[dx][dy][dz]->shape->block->setUpper(chunk[dx][dy+1][dz]->shape->block);
          }
        }
      }
    }
  }
  
  // Compute chunk coordinates
  chunk_x = (int)(2 * start_point[0] / CHUNK_X) + 1; // -8*2/16 = -1
  chunk_z = (int)(2 * start_point[2] / CHUNK_Z) + 1; // -16*2/16 = -2

  // Compute global (x, y, z) coordinates
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

  // Assumes blocks
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
