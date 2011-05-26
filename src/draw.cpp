#include "draw.h"

static unsigned int LODcutoff = 40;

void DrawSceneFullOptimization(R3Scene *scene) 
{
	bool isSelected = false;
	int curChunkXLeft, curChunkXRight, curChunkZLeft, curChunkZRight;
	int left, right, back, forward;

	R3Index currentPosition = getChunkCoordinates(camera.eye);
	int charBlockX = currentPosition.x + (CHUNK_X)*(CHUNKS-1)/2;
	int charBlockZ = currentPosition.z + (CHUNK_Z)*(CHUNKS-1)/2;
	int charBlockY = currentPosition.y;

	double viewX = camera.towards[0];
	double viewZ = camera.towards[2];
	double viewY = camera.towards[1];

	int faceCount = 0;

	// It's okay, it's okay, we do culling here.
	for (int dChunkX = 0; dChunkX < CHUNKS; dChunkX++)
	{
		for (int dChunkZ = 0; dChunkZ < CHUNKS; dChunkZ++)
		{
			for (int dz = 0; dz < CHUNK_Z; dz++)
			{
				int blockDistZ = ((CHUNK_Z) * dChunkZ + dz) - charBlockZ;
        double blockDistZSq = SQ(blockDistZ);

				for (int dy = 0; dy < CHUNK_Y; dy++)
				{
					int blockDistY = dy - charBlockY;
          double blockDistYSq = SQ(blockDistY);

					for (int dx = 0; dx < CHUNK_X; dx++)
					{
						int blockDistX = ((CHUNK_X) * dChunkX + dx) - charBlockX;
						double totDist = sqrt(blockDistZSq + blockDistYSq + SQ(blockDistX)) - 2;

						if (!scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz]->shape->block->transparent && 
                blockDistX * viewX + blockDistY * viewY + blockDistZ * viewZ >= dotProductCutOff * totDist)
						{

							// Terrible black magic is about to happen
							curChunkXLeft = dChunkX;
							curChunkXRight = dChunkX;
							curChunkZLeft = dChunkZ;
							curChunkZRight = dChunkZ;
							left = dx - 1;
							right = dx + 1;
							back = dz - 1;
							forward = dz + 1;

							if (dx == 0 && dChunkX > 0)
							{
								left = CHUNK_X - 1;
								curChunkXLeft--;
							}
							if (dx == CHUNK_X - 1 && dChunkX < CHUNKS - 1)
							{
								right = 0;
								curChunkXRight++;
							}
							if (dz == 0 && dChunkZ > 0)
							{
								back = CHUNK_Z - 1;
								curChunkZLeft--;
							}
							if (dz == CHUNK_Z - 1 && dChunkZ < CHUNKS - 1)
							{
								forward = 0;
								curChunkZRight++;
							}

							R3Node *node = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz];
							R3Block *block = node->shape->block;
							isSelected = (currentSelection == node);

							double distance = R3Distance(camera.eye, block->box.Centroid());
							bool tooFar = distance > LODcutoff;

							if (tooFar) 
							{
								glDisable(GL_TEXTURE_2D);
								glDisable(GL_LIGHTING);
							}

							// Face 3; this is the the top face, set the material every time
							if (dy == CHUNK_Y - 1 || (dy + 1 < CHUNK_Y && scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy + 1][dz]->shape->block->transparent))
							{
								if (tooFar) 
									FindColor(block, true);
								else 
									FindMaterial(block, true);
								block->Draw(3, isSelected);
								faceCount++;
							}

							// Face 0; first face decides if others get material or solid color
							if (left >= 0 && scene->terrain[curChunkXLeft][dChunkZ]->chunk[left][dy][dz]->shape->block->transparent)
							{
								// Only swap materials for the dirt block
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);

								block->Draw(0, isSelected);
								faceCount++;
							}

							// Face 1
							if (right < CHUNK_X && scene->terrain[curChunkXRight][dChunkZ]->chunk[right][dy][dz]->shape->block->transparent)
							{
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);
								block->Draw(1, isSelected);
								faceCount++;
							}

							// Face 2
							if (dy - 1 > 0 && scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy - 1][dz]->shape->block->transparent)
							{
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);
								block->Draw(2, isSelected);
								faceCount++;
							}

							// Face 4
							if (back >= 0 && scene->terrain[dChunkX][curChunkZLeft]->chunk[dx][dy][back]->shape->block->transparent)
							{
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);
								block->Draw(4, isSelected);
								faceCount++;
							}

							// Face 5
							if (forward < CHUNK_Z && scene->terrain[dChunkX][curChunkZRight]->chunk[dx][dy][forward]->shape->block->transparent)
							{
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);
								block->Draw(5, isSelected);
								faceCount++;
							}

							if (tooFar)
							{
								glEnable(GL_TEXTURE_2D);
								glEnable(GL_LIGHTING);
							}
						} //end view frustrum check
					} // end dx
				} // end dy
			} // end dz
		} // end chunk z
	} // end chunk x
}

void DrawSceneViewFrustrumOnly(R3Scene *scene) 
{
	bool isSelected = false;

	R3Index currentPosition = getChunkCoordinates(camera.eye);
	int charBlockX = currentPosition.x + (CHUNK_X)*(CHUNKS-1)/2;
	int charBlockZ = currentPosition.z + (CHUNK_Z)*(CHUNKS-1)/2;
	int charBlockY = currentPosition.y;

	double viewX = camera.towards[0];
	double viewZ = camera.towards[2];
	double viewY = camera.towards[1];

	int faceCount = 0;

	// It's okay, it's okay, we do culling here.
	for (int dChunkX = 0; dChunkX < CHUNKS; dChunkX++)
	{
		for (int dChunkZ = 0; dChunkZ < CHUNKS; dChunkZ++)
		{
			for (int dz = 0; dz < CHUNK_Z; dz++)
			{
				int blockDistZ = ((CHUNK_Z) * dChunkZ + dz) - charBlockZ;
        double blockDistZSq = SQ(blockDistZ);

				for (int dy = 0; dy < CHUNK_Y; dy++)
				{
					int blockDistY = dy - charBlockY;
          double blockDistYSq = SQ(blockDistY);

					for (int dx = 0; dx < CHUNK_X; dx++)
					{
						int blockDistX = ((CHUNK_X) * dChunkX + dx) - charBlockX;
						double totDist = sqrt(blockDistZSq + blockDistYSq + SQ(blockDistX)) - 2;

						if (!scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz]->shape->block->transparent && 
                blockDistX * viewX + blockDistY * viewY + blockDistZ * viewZ >= dotProductCutOff * totDist)
						{
							R3Node *node = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz];
							R3Block *block = node->shape->block;
							isSelected = (currentSelection == node);

							double distance = R3Distance(camera.eye, block->box.Centroid());
							bool tooFar = distance > LODcutoff;

							if (tooFar) 
							{
								glDisable(GL_TEXTURE_2D);
								glDisable(GL_LIGHTING);
							}

							if (tooFar) 
								FindColor(block, true);
							else 
								FindMaterial(block, true);
							// Face 3; this is the the top face, set the material every time
							block->Draw(3, isSelected);
							faceCount++;

							// Face 0; first face decides if others get material or solid color
							// Only swap materials for the dirt block
							if (block->blockType == DIRT_BLOCK)
							{
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);
							}

							block->Draw(0, isSelected);
							faceCount++;

							// Face 1
							block->Draw(1, isSelected);
							faceCount++;

							// Face 2
							block->Draw(2, isSelected);
							faceCount++;

							// Face 4
							block->Draw(4, isSelected);
							faceCount++;

							// Face 5
							block->Draw(5, isSelected);
							faceCount++;

							if (tooFar)
							{
								glEnable(GL_TEXTURE_2D);
								glEnable(GL_LIGHTING);
							}

						} //end view frustrum check
					} // end dx
				} // end dy
			} // end dz
		} // end chunk z
	} // end chunk x

	//fprintf(stderr, "View Frustrum Culling Only: Rendered %d faces.\n", faceCount);
}

void DrawSceneOcclusionOnly(R3Scene *scene) 
{
	bool isSelected = false;
	int curChunkXLeft, curChunkXRight, curChunkZLeft, curChunkZRight;
	int left, right, back, forward;
	int faceCount = 0;

	// It's okay, it's okay, we do culling here.
	for (int dChunkX = 0; dChunkX < CHUNKS; dChunkX++)
	{
		for (int dChunkZ = 0; dChunkZ < CHUNKS; dChunkZ++)
		{
			for (int dz = 0; dz < CHUNK_Z; dz++)
			{
				for (int dy = 0; dy < CHUNK_Y; dy++)
				{
					for (int dx = 0; dx < CHUNK_X; dx++)
					{
						if (!scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz]->shape->block->transparent)
						{
							// Terrible black magic is about to happen
							curChunkXLeft = dChunkX;
							curChunkXRight = dChunkX;
							curChunkZLeft = dChunkZ;
							curChunkZRight = dChunkZ;
							left = dx - 1;
							right = dx + 1;
							back = dz - 1;
							forward = dz + 1;

							if (dx == 0 && dChunkX > 0)
							{
								left = CHUNK_X - 1;
								curChunkXLeft--;
							}
							if (dx == CHUNK_X - 1 && dChunkX < CHUNKS - 1)
							{
								right = 0;
								curChunkXRight++;
							}
							if (dz == 0 && dChunkZ > 0)
							{
								back = CHUNK_Z - 1;
								curChunkZLeft--;
							}
							if (dz == CHUNK_Z - 1 && dChunkZ < CHUNKS - 1)
							{
								forward = 0;
								curChunkZRight++;
							}

							R3Node *node = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz];
							R3Block *block = node->shape->block;
							isSelected = (currentSelection == node);

							double distance = R3Distance(camera.eye, block->box.Centroid());
							bool tooFar = distance > LODcutoff;

							if (tooFar) 
							{
								glDisable(GL_TEXTURE_2D);
								glDisable(GL_LIGHTING);
							}

							// Face 3; this is the the top face, set the material every time
							if (dy + 1 < CHUNK_Y && scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy + 1][dz]->shape->block->transparent)

							{
								if (tooFar) 
									FindColor(block, true);
								else 
									FindMaterial(block, true);
								block->Draw(3, isSelected);
								faceCount++;
							}

							FindMaterial(block, false);

							// Face 0; first face decides if others get material or solid color
							if (left >= 0 && scene->terrain[curChunkXLeft][dChunkZ]->chunk[left][dy][dz]->shape->block->transparent)
							{
								// Only swap materials for the dirt block
								if (block->blockType == DIRT_BLOCK)
								{
									if (tooFar) 
										FindColor(block, false);
									else 
										FindMaterial(block, false);
								}

								block->Draw(0, isSelected);
								faceCount++;
							}

							// Face 1
							if (right < CHUNK_X && scene->terrain[curChunkXRight][dChunkZ]->chunk[right][dy][dz]->shape->block->transparent)
							{
								// Only swap materials for the dirt block
								if (block->blockType == DIRT_BLOCK)
								{
									if (tooFar) 
										FindColor(block, false);
									else 
										FindMaterial(block, false);
								}

								block->Draw(1, isSelected);
								faceCount++;
							}

							// Face 2
							if (dy - 1 >= 0 && scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy - 1][dz]->shape->block->transparent)
							{
								// Only swap materials for the dirt block
								if (block->blockType == DIRT_BLOCK)
								{
									if (tooFar) 
										FindColor(block, false);
									else 
										FindMaterial(block, false);
								}

								block->Draw(2, isSelected);
								faceCount++;
							}

							// Face 4
							if (back >= 0 && scene->terrain[dChunkX][curChunkZLeft]->chunk[dx][dy][back]->shape->block->transparent)
							{
								// Only swap materials for the dirt block
								if (block->blockType == DIRT_BLOCK)
								{
									if (tooFar) 
										FindColor(block, false);
									else 
										FindMaterial(block, false);
								}

								block->Draw(4, isSelected);
								faceCount++;
							}

							// Face 5
							if (forward < CHUNK_Z && scene->terrain[dChunkX][curChunkZRight]->chunk[dx][dy][forward]->shape->block->transparent)
							{
								// Only swap materials for the dirt block
								if (block->blockType == DIRT_BLOCK)
								{
									if (tooFar) 
										FindColor(block, false);
									else 
										FindMaterial(block, false);
								}

								block->Draw(5, isSelected);
								faceCount++;
							}

							if (tooFar)
							{
								glEnable(GL_TEXTURE_2D);
								glEnable(GL_LIGHTING);
							}
						} //end view frustrum check
					} // end dx
				} // end dy
			} // end dz
		} // end chunk z
	} // end chunk x

	//fprintf(stderr, "Occlusion Culling Only: Rendered %d faces.\n", faceCount);
}

void DrawSceneNone(R3Scene *scene) 
{
	bool isSelected = false;
	int faceCount = 0;

	for (int dChunkX = 0; dChunkX < CHUNKS; dChunkX++)
	{
		for (int dChunkZ = 0; dChunkZ < CHUNKS; dChunkZ++)
		{
			for (int dz = 0; dz < CHUNK_Z; dz++)
			{
				for (int dy = 0; dy < CHUNK_Y; dy++)
				{
					for (int dx = 0; dx < CHUNK_X; dx++)
					{
						if (!scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz]->shape->block->transparent)
						{
							R3Node *node = scene->terrain[dChunkX][dChunkZ]->chunk[dx][dy][dz];
							R3Block *block = node->shape->block;
							isSelected = (currentSelection == node);

							double distance = R3Distance(camera.eye, block->box.Centroid());
							bool tooFar = distance > LODcutoff;

							if (tooFar) 
							{
								glDisable(GL_TEXTURE_2D);
								glDisable(GL_LIGHTING);
							}

							// Face 3; this is the the top face, set the material every time
							if (tooFar) 
								FindColor(block, true);
							else 
								FindMaterial(block, true);
							block->Draw(3, isSelected);
							faceCount++;

							// Face 0; first face decides if others get material or solid color
							// Only swap materials for the dirt block
							if (block->blockType == DIRT_BLOCK)
							{
								if (tooFar) 
									FindColor(block, false);
								else 
									FindMaterial(block, false);
							}
							block->Draw(0, isSelected);
							faceCount++;

							// Face 1
							block->Draw(1, isSelected);
							faceCount++;


							// Face 2
							block->Draw(2, isSelected);
							faceCount++;

							// Face 4
							block->Draw(4, isSelected);
							faceCount++;

							// Face 5
							block->Draw(5, isSelected);
							faceCount++;

							if (tooFar)
							{
								glEnable(GL_TEXTURE_2D);
								glEnable(GL_LIGHTING);
							}
						} //view frustrum culling
					} // end dx
				} // end dy
			} // end dz
		} // end chunk z
	} // end chunk x

	//fprintf(stderr, "No culling: Rendered %d faces.\n", faceCount);
}
