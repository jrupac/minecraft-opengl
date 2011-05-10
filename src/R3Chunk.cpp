#include "R3Chunk.h"

void R3Chunk::MakeMountain(int types[CHUNK_X][CHUNK_Y][CHUNK_Z], int height)
{
  int cutoffX = 2*CHUNK_X/8;
  int cutoffZ = 2*CHUNK_Z/8;
  // pick something in middle
  int locX = rand()%(cutoffX) + 3*CHUNK_X/8;
  int locZ = rand()%(cutoffZ) + 3*CHUNK_Z/8;
  int locY;
  
  // find top Y value
  for (int yCheck = 0; yCheck < CHUNK_Y; yCheck++)
  {
    if (types[locX][yCheck][locZ] == AIR_BLOCK)
    {
      locY = yCheck;
      break;
    }
  }
  if (locY - 1 < 0)
    return; // just in case
  //fprintf(stderr, "new mountain @ %d %d\n", locX, locZ);
  
  // initialize visited array
  int final[CHUNK_X][CHUNK_Z];
  for (int i = 0; i < CHUNK_X; i++)
    for (int j = 0; j < CHUNK_Z; j++)
      final[i][j] = locY - 1;
   
  final[locX][locZ] = locY - 1 + height;
  
  // make things stone under mountain
  for (int yCheck = 1; yCheck < CHUNK_Y && yCheck < locY + height - 1; yCheck++)
  {
    types[locX][yCheck][locZ] = STONE_BLOCK;
  }
  
  // top off with dirt
  if (locY + height - 1 <= CHUNK_Y)
    types[locX][locY + height - 1][locZ] = DIRT_BLOCK;
    
  // recurse
  if (height > 0)
  {
    GrowMountain(types, locX, locZ, 1);
  
  }
  
}

void R3Chunk::GrowMountain(int types[CHUNK_X][CHUNK_Y][CHUNK_Z], int x, int z, int d)
{
  for (int dx = -1*d; dx <= d; dx++)
  {
    for (int dz = -1*d; dz <= d; dz++)
    {
      if ((abs(dx) < d && abs(dz) < d) || (x + dx) < 0 || (x + dx) >= CHUNK_X || (z + dz) < 0 || (z + dz) >= CHUNK_Z)
        continue;
        
      int maxY = 0;
      int curX = x + dx;
      int curZ = z + dz;
      for (int xAround = -1; xAround <= 1; xAround++)
      {
        for (int zAround = -1; zAround <= 1; zAround++)
        {
          if (abs(xAround + zAround) != 1 || curX + xAround < 0 || curX + xAround >= CHUNK_X || curZ + zAround < 0 || curZ + zAround >= CHUNK_Z)
            continue;
            
          for (int yCheck = 0; yCheck < CHUNK_Y; yCheck++)
          {
            if (types[curX + xAround][yCheck][curZ + zAround] == AIR_BLOCK)
            {
              if (maxY < yCheck - 1)
                maxY = yCheck - 1;
              break;
            }  // air block
          } // y check
        } // around z
      } // around x
      maxY -= rand()%2; // decrease with 50% prob
      /*int y;
      // find top Y value
      for (int yCheck = 0; yCheck < CHUNK_Y; yCheck++)
      {
        if (types[curX][yCheck][curZ] == AIR_BLOCK)
        {
          y = yCheck;
          break;
        }
      }*/
      
      // make things stone under mountain
      for (int yCheck = 1; yCheck < CHUNK_Y && yCheck < maxY; yCheck++)
      {
        types[curX][yCheck][curZ] = STONE_BLOCK;
      }
      
      // top off with dirt
      if (maxY <= CHUNK_Y)
        types[curX][maxY][curZ] = DIRT_BLOCK;
       
      
      
    } // dz
  } // dx
  
  // recurse
  if (d < CHUNK_X && d < CHUNK_Z)
  {
    GrowMountain(types, x, z, d + 1);
  }

}

void R3Chunk::GrowTree(int types[CHUNK_X][CHUNK_Y][CHUNK_Z], int x, int z, int y, int recDepth)
{
  // recursive method: base case - check for valid location and put leaf
  if (x > 0 && x < CHUNK_X && y > 0 && y < CHUNK_Y && z > 0 && z < CHUNK_Z && types[x][y][z] == AIR_BLOCK)
    types[x][y][z] = LEAF_BLOCK;
  else
    return;
  // recursive - if depth is > 1 recurse
  if (recDepth > 0)
  {
    GrowTree(types, x - 1, z - 1, y, recDepth - 1);
    GrowTree(types, x    , z - 1, y, recDepth - 1);
    GrowTree(types, x + 1, z - 1, y, recDepth - 1);
    GrowTree(types, x - 1, z    , y, recDepth - 1);
    GrowTree(types, x + 1, z    , y, recDepth - 1);
    GrowTree(types, x - 1, z + 1, y, recDepth - 1);
    GrowTree(types, x    , z + 1, y, recDepth - 1);
    GrowTree(types, x + 1, z + 1, y, recDepth - 1);
  }
}

void R3Chunk::MakeTree(int types[CHUNK_X][CHUNK_Y][CHUNK_Z], int count)
{
    int * locX = new int[count];
    int *  locZ = new int[count];
    int *  locY= new int[count];
    
    // pick unique locations
    for (int c = 0; c < count; c++)
    {
      // check for uniqueness
      bool created = false;
      while (!created)
      {
        locX[c] = rand()%CHUNK_X;
        locZ[c] = rand()%CHUNK_Z;
        for (int prev = 0; prev < c; prev++)
        {
          if (abs(locX[prev] - locX[c]) < 2 || abs(locZ[prev] - locZ[c]) < 2)
            created = false;
          else
            created = true;
        }
        if (c == 0)
          created = true;
        
      }
   //   fprintf(stderr, "new tree @ %d, %d\n", locX[c], locZ[c]);
      // find top Y value
      for (int yCheck = 0; yCheck < CHUNK_Y; yCheck++)
      {
        if (types[locX[c]][yCheck][locZ[c]] == AIR_BLOCK)
        {
          locY[c] = yCheck;
          break;
        }
      }
      //now make the tree     
      types[locX[c]][locY[c]][locZ[c]] = WOOD_BLOCK;
      if (locY[c] + 1 < CHUNK_Y)
        types[locX[c]][locY[c] + 1][locZ[c]] = WOOD_BLOCK;
      else continue;
      if (locY[c] + 2 < CHUNK_Y)
      {
        types[locX[c]][locY[c] + 2][locZ[c]] = WOOD_BLOCK;
        GrowTree(types, locX[c] - 1, locZ[c] - 1, locY[c] +2, 2);
        GrowTree(types, locX[c]    , locZ[c] - 1, locY[c] +2, 2);
        GrowTree(types, locX[c] + 1, locZ[c] - 1, locY[c] +2, 2);
        GrowTree(types, locX[c] - 1, locZ[c]    , locY[c] +2, 2);
        GrowTree(types, locX[c] + 1, locZ[c]    , locY[c] +2, 2);
        GrowTree(types, locX[c] - 1, locZ[c] + 1, locY[c] +2, 2);
        GrowTree(types, locX[c]    , locZ[c] + 1, locY[c] +2, 2);
        GrowTree(types, locX[c] + 1, locZ[c] + 1, locY[c] +2, 2);
        
      }
      else continue; 
      if (locY[c] + 3 < CHUNK_Y)
      {
        types[locX[c]][locY[c] + 3][locZ[c]] = WOOD_BLOCK;
        GrowTree(types, locX[c] - 1, locZ[c] - 1, locY[c] +3, 1);
        GrowTree(types, locX[c]    , locZ[c] - 1, locY[c] +3, 1);
        GrowTree(types, locX[c] + 1, locZ[c] - 1, locY[c] +3, 1);
        GrowTree(types, locX[c] - 1, locZ[c]    , locY[c] +3, 1);
        GrowTree(types, locX[c] + 1, locZ[c]    , locY[c] +3, 1);
        GrowTree(types, locX[c] - 1, locZ[c] + 1, locY[c] +3, 1);
        GrowTree(types, locX[c]    , locZ[c] + 1, locY[c] +3, 1);
        GrowTree(types, locX[c] + 1, locZ[c] + 1, locY[c] +3, 1);
        
      }
      else continue;
      if (locY[c] + 4 < CHUNK_Y)
      {
        types[locX[c]][locY[c] + 4][locZ[c]] = WOOD_BLOCK;
        GrowTree(types, locX[c] - 1, locZ[c] - 1, locY[c] +4, 0);
        GrowTree(types, locX[c]    , locZ[c] - 1, locY[c] +4, 0);
        GrowTree(types, locX[c] + 1, locZ[c] - 1, locY[c] +4, 0);
        GrowTree(types, locX[c] - 1, locZ[c]    , locY[c] +4, 0);
        GrowTree(types, locX[c] + 1, locZ[c]    , locY[c] +4, 0);
        GrowTree(types, locX[c] - 1, locZ[c] + 1, locY[c] +4, 0);
        GrowTree(types, locX[c]    , locZ[c] + 1, locY[c] +4, 0);
        GrowTree(types, locX[c] + 1, locZ[c] + 1, locY[c] +4, 0);
        
      }
      if (locY[c] + 5 < CHUNK_Y)
      {
        types[locX[c]][locY[c] + 5][locZ[c]] = LEAF_BLOCK;
      }
    }
     
}

R3Chunk::R3Chunk(void)
{
}



int R3Chunk::
GenerateChunk(int c_x, int c_z)
{
  chunk_x = c_x;
  chunk_z = c_z;
  //fprintf(stderr, "No chunk found: generating new chunk at chunk pos (%d, %d)\n", c_x, c_z);

  start_point = R3Point((double)c_x * CHUNK_X - (double)CHUNK_X/2, -CHUNK_Y / 2, 
                        (double)c_z * CHUNK_Z - (double)CHUNK_Z/2);
  end_point = start_point + R3Point(CHUNK_X, CHUNK_Y, CHUNK_Z);
  block_side = 1.0;
  
  /* Chunk types:
  0 - flat 1d2 trees, 1d2 bushes
  1 - 1d4 little mounds 1d2 - 1 trees, 1d2 - 1 bushes 
  2 - hill - 1d2 - 1 trees, 1d2 - 1 bushes
  3 - sand pit - 0 trees, 0 bushes
  */
  int chunkType = rand()%4;
  int trees, bushes;
  //start block always the same!!!
  if (c_x == 0 && c_z == 0)
  {
    chunkType = 0;
    trees = 0;
    bushes = 0;
  } 
  else if (chunkType == 0)
  {
    trees = rand()%4 + 1;
    //bushes = rand()%2 + 1;
    bushes = 0;
  }
  else if (chunkType == 1 || chunkType == 2)
  {
    trees = rand()%4;
    //bushes = rand()%2;
    bushes = 0;
  }
  else
  {
    trees = 0;
    bushes = 0;
  }
  
  int types[CHUNK_X][CHUNK_Y][CHUNK_Z];

  for (int dx = 0; dx < CHUNK_X; dx++)
  {
    for (int dy = 0; dy < CHUNK_Y; dy++)
    {
      for (int dz = 0; dz < CHUNK_Z; dz++)
      {
        if (dy == 0) // 0
          types[dx][dy][dz] = OBSIDIAN_BLOCK;
        else if (dy > 0 && dy <= CHUNK_Y/8) // 1 to 2
          types[dx][dy][dz] = STONE_BLOCK;
        else if (dy > CHUNK_Y/8 && dy <= (CHUNK_Y/4 + 1)) // 3 to 5
        {
          int prob = rand()%(dy - CHUNK_Y/8 + 1);
			if (prob == 0) {
				int goldprob = rand()%20;
				if (goldprob == 0) 
					types[dx][dy][dz] = GOLD_BLOCK;
			else 
				types[dx][dy][dz] = STONE_BLOCK;
			}
          else
            types[dx][dy][dz] = DIRT_BLOCK;
        }
        else if (dy > (CHUNK_Y/4 + 1) && dy < CHUNK_Y/2) //5 to 7
          types[dx][dy][dz] = DIRT_BLOCK;
        else // 9 to 15
         types[dx][dy][dz] = AIR_BLOCK;
      }
    }
  }
  
  // Do things based on chunk type  
  if (chunkType == 0) // Flat area - make 1-2 trees, 1-2 bushes
  {
    MakeTree(types, trees); 
    
  }
  if (chunkType == 2) // a few hills
  {
    MakeMountain(types, 4);
  
  }
  
  

  // Create all blocks in this chunk
  for (int dy = 0; dy < CHUNK_Y; dy++)
  {
    for (int dz = 0; dz < CHUNK_Z; dz++)
    {
      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        int block_type = types[dx][dy][dz];

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
  //std:: string s;
  //std:: stringstream out;
  //out << s_World << chunk_x << "." << chunk_z << ".scn";
  char filename[64];
  sprintf(filename, "%s%d.%d.scn", s_World, chunk_x, chunk_z);
  WriteChunk(filename);
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

  //std:: string name;
  //std:: stringstream nameS;
  char filename[1024];
  sprintf(filename, "%s%d.%d.scn", s_World, xChunkCoord, zChunkCoord);
  
  //nameS << s_World << xChunkCoord << "." << zChunkCoord << ".scn";
  
  // Open file
  FILE *fp;
  if (!(fp = fopen(filename, "r"))) {
    fprintf(stderr, "Unable to open file %s\n", filename);
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
              fprintf(stderr, "Unable to read chunk block at zyx(%d, %d, %d) in file %s\n", dz, dy, dx, filename);
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
  //chunk_x = (int)(2 * start_point[0] / CHUNK_X) + 1; // -8*2/16 = -1
  //chunk_z = (int)(2 * start_point[2] / CHUNK_Z) + 1; // -16*2/16 = -2
  chunk_x = xChunkCoord;
  chunk_z = zChunkCoord;

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

  //std:: string s;
  //std:: stringstream out;
  //char temp[1024];
  fprintf(fp, "# chunk chunk_id mat_id\n# low_x low_y low_z\n# block_size (1 side)\n# 16x16x16 (z, y, x) blocktypes\n");
  fprintf(fp, "chunk 1 -1    %f %f %f   1\n", chunkStart[0], chunkStart[1], chunkStart[2]);
  //out << "# chunk chunk_id mat_id\n# low_x low_y low_z\n" <<
  //       "# block_size (1 side)\n# 16x16x16 (z, y, x) block types\n";
  //out << "chunk 1 -1       " << chunkStart[0] << " " << chunkStart[1] <<
  //       " " << chunkStart[2] << " " << "1\n";

  //s = out.str();
  //fputs(temp, fp);

  for (int dz = 0; dz < CHUNK_Z; dz++)
  {
    for (int dy = 0; dy < CHUNK_Y; dy++)
    {
      //std:: string tempS;
      //std:: stringstream tempOut;

      for (int dx = 0; dx < CHUNK_X; dx++)
      {
        //tempOut << chunk[dx][dy][dz]->shape->block->getBlockType() << " ";
        fprintf(fp, "%i ", chunk[dx][dy][dz]->shape->block->getBlockType());
      }
      fprintf(fp, "\n");

    }

    //fputs("\n", fp);
    fprintf(fp, "\n");
  }
  
  fclose(fp);

  return 1;
}
