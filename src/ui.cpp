#include "ui.h"

void DrawHUD(R3Character *Main_Character, bool dead, int FPS) 
{  
	glLineWidth(3);
	glColor3d(.1, .1, .1);

	if (!dead) 
  {
    static float picker_height = 10;
    static float picker_width = 10;

		// Draw reticle
		glBegin(GL_LINES);
		glVertex2f((GLUTwindow_width / 2) - picker_width, GLUTwindow_height / 2); 
		glVertex2f((GLUTwindow_width / 2) + picker_width, GLUTwindow_height / 2); 
		glEnd();
		glBegin(GL_LINES);
		glVertex2f(GLUTwindow_width / 2, (GLUTwindow_height / 2) - picker_height); 
		glVertex2f(GLUTwindow_width / 2, (GLUTwindow_height / 2) + picker_height); 
		glEnd();
	}

	glLineWidth(1);

	// Draw text
	char points[20];
	sprintf(points, "Gold: %d", Main_Character->number_gold);
	GLUTDrawText(R3Point(5, 13, 0), "MINECRAFT v.0.0.1");
	GLUTDrawText(R3Point(5, 30, 0), "C - display controls");
	
	GLUTDrawText(R3Point(400, 13, 0), "FPS: " );
	stringstream ss;
	ss << FPS;
	GLUTDrawText(R3Point(450, 13, 0), ss.str().c_str()); 
	
	GLUTDrawText(R3Point(10, 50, 0), points); 
	// Draw bottom pane
	glColor3d(.7, .7, .7);

	//Draw Hearts
	DrawHUD_Hearts(Main_Character);

	//Draw Inventory
	DrawHUD_Inventory(Main_Character);
}

void DrawHUD_Hearts(R3Character *Main_Character) 
{
	glDisable(GL_TEXTURE_2D);
	int x = GLUTwindow_width;
	int y = GLUTwindow_height;

	glColor3d(.9, .1, .1);

	glPushMatrix();
	glTranslatef(.25 * x, .9 * y, 0);

	for (int i = 0; i < Main_Character->MaxHealth; i++) 
	{
		if (i >= Main_Character->Health)
			glColor3d(1.0,1.0,1.0);

		glBegin(GL_QUADS);
      glVertex2f(0, 0); 
      glVertex2f(x/64, - y/64); 
      glVertex2f(x/32, 0); 
      glVertex2f(x/64, y/64); 
		glEnd();
		glTranslatef(5 * x / 128, 0, 0);
	}

	glPopMatrix();
	glColor3d(.7, .7, .7);
	glEnable(GL_TEXTURE_2D);
}

void DrawHUD_Inventory(R3Character *Main_Character) 
{
	int x = GLUTwindow_width;
	int y = GLUTwindow_height;
	int boxWidth = .0525 * x;
	int boxHeight = .0625 * y;
	int itemWidth = .2 * x;
	int itemHeight = .1875 * y;
	int materialsStart = DIRT_ICON;
	int i;

	glPushMatrix();
	glTranslatef(.20 * x, .99 * y, 0.);

	for (i = 0; i <= 4; i++) 
	{	
		if (Main_Character->number_items[i] > 0)
			LoadMaterial(materials[materialsStart]);
		else 
			LoadMaterial(materials[DEFAULT]);

		glTranslatef(x / 17, 0, 0.);
		glBegin(GL_QUADS);
      glNormal3d(0.0, 0.0, 1.0);
      glTexCoord2d(0, 1);
      glVertex2f(0, 0); 
      glTexCoord2d(0, 0);
      glVertex2f(0, -boxHeight); 
      glTexCoord2d(1, 0);
      glVertex2f(boxWidth, -boxHeight); 
      glTexCoord2d(1, 1);
      glVertex2f(boxWidth, 0); 
		glEnd();

		// Drawing the line
		if (Main_Character->item == i)
			glColor3f(1,1,1);
		else
			glColor3f(.4, .4, .4);

		glLineWidth(4);
    glBegin(GL_LINES);
      glVertex2f(0, 0); 
      glVertex2f(0, -boxHeight - 2); 
    glEnd();
    glBegin(GL_LINES);
      glVertex2f(0, -boxHeight); 
      glVertex2f(boxWidth+3, -boxHeight); 
    glEnd();
    glBegin(GL_LINES);
      glVertex2f(boxWidth, -boxHeight); 
      glVertex2f(boxWidth, 2); 
    glEnd();
    glBegin(GL_LINES);
      glVertex2f(boxWidth, 0); 
      glVertex2f(-2, 0); 
    glEnd();

		glColor3f(.7, .7, .7);
		materialsStart++;
	}

	glPopMatrix(); 

	//Draw currently held item
	glPushMatrix();
	glTranslatef(.75 * x, .9 * y, 0.);

	// Find correct material to load
  if (Main_Character->item != R3BLOCK_AIR) 
  {
    switch (Main_Character->item)
    {
      case R3BLOCK_DIRT:
        LoadMaterial(materials[GRASS]);
        break;
      case R3BLOCK_STONE:
        LoadMaterial(materials[STONE]);
        break;
      case R3BLOCK_WOOD:
        LoadMaterial(materials[WOOD]);
        break;
      case R3BLOCK_SAND:
        LoadMaterial(materials[SAND]);
        break;
      case R3BLOCK_OBSIDIAN:
        LoadMaterial(materials[OBSIDIAN]);
        break;
    }
		
    if (Main_Character->item == R3BLOCK_DIRT) 
      LoadMaterial(materials[GRASS]);

		glBegin(GL_QUADS);
		glNormal3d(0.0, 0.0, 1.0);
		glTexCoord2d(0, 1);
		glVertex2f(itemWidth * .05, -itemHeight * .15); 
		glTexCoord2d(1, 1);
		glVertex2f(-itemWidth * .25, itemHeight * .25);
		glTexCoord2d(1, 0);
		glVertex2f(itemWidth * .75, itemHeight * .50); 
		glTexCoord2d(0, 0);
		glVertex2f(itemWidth, itemHeight * .08); 
		glEnd();

		if (Main_Character->item == R3BLOCK_DIRT) 
      LoadMaterial(materials[DIRT]);

		glBegin(GL_QUADS);
		glNormal3d(0.0, 0.0, 1.0);
		glTexCoord2d(1, 1);
		glVertex2f(-itemWidth * .25, itemHeight * .25);
		glTexCoord2d(1, 0);
		glVertex2f(-itemWidth * .25, itemHeight);
		glTexCoord2d(0, 0);
		glVertex2f(itemWidth * .75, itemHeight); 
		glTexCoord2d(0, 1);
		glVertex2f(itemWidth * .75, itemHeight * .50); 
		glEnd();

		glBegin(GL_QUADS);
		glNormal3d(0.0, 0.0, 1.0);
		glTexCoord2d(1, 1);
		glVertex2f(itemWidth * .75, itemHeight * .50); 
		glTexCoord2d(1, 0);
		glVertex2f(itemWidth * .75, itemHeight);
		glTexCoord2d(0, 0);
		glVertex2f(itemWidth, itemHeight * .50); 
		glTexCoord2d(0, 1);
		glVertex2f(itemWidth, itemHeight * .08); 
		glEnd();

	}

	glPopMatrix();
}
