/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#include "system.hpp"
#include "classes.hpp"
#pragma hdrstop

#include "effects2d.h"
#include "Globals.h"

#include <stdio.h>
#include <iostream>
#include <fstream>


bool bNoise=true;
GLuint noise[30];
int actNoise=0;
int NOISERES = 1024;


// *****************************************************************************
//
// *****************************************************************************
void fillRandom2(unsigned char* buffer, int size)
{
   for (int i=0;i<size*size;i++)
	{
		int x;
		x=rand()%2000;
		if (x>255) x=0;
		buffer[(i)*3]   =   x;
		buffer[(i)*3+1] =   x;
		buffer[(i)*3+2] =   x;
	}

}

void fillRandom(unsigned char* buffer, int size)
{
	for (int i=0;i<size*size;i++)
	{
		int x;
		x=rand()%750;
		if (x>255) x=0;
		buffer[(i)*3]   =   x;
		//x=myRand(2000);
		if (x>255) x=0;
		buffer[(i)*3+1] =   x;
		//x=myRand(2000);
		if (x>255) x=0;
		buffer[(i)*3+2] =   x;
	}

}

void generatenoisetex()
{
 unsigned char* data = (unsigned char *)HeapAlloc(GetProcessHeap(), 0, NOISERES*NOISERES*3*sizeof(unsigned char));
 for(int i=0;i<30;i++)
   {
		fillRandom(data,NOISERES);
		glGenTextures(1, &noise[i]);
		glBindTexture(GL_TEXTURE_2D, noise[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3,NOISERES, NOISERES, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   }
}


// *****************************************************************************
// Noise effect like old tele bad signal screen 
// *****************************************************************************
void drawNoise(int bnoise, float alpha)
{
  GLboolean blendEnabled;
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glGetBooleanv(GL_BLEND, &blendEnabled);

     //	glViewport(0,0,640,480);

     //	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	/*glBindTexture(GL_TEXTURE_2D, c7logo);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(0);
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(1, 1, 1, 1.0f);
		glTexCoord2f(0,0);
		glVertex2f(0, 460*0.8f);
		glTexCoord2f(1,0);
		glVertex2f(140*0.8f, 460*0.8f);
		glTexCoord2f(0,1);
		glVertex2f(0, 500*0.8f);
		glTexCoord2f(1,1);
		glVertex2f(140*0.8f, 500*0.8f);
	glEnd();
	glDepthMask(1);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
//	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
*/
	


	glDisable(GL_LIGHTING);
	//glDisable(GL_CULL_FACE);
  /*
	glDisable(GL_CULL_FACE);

	glPointSize(7.0f);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	Ortho();
	glBegin(GL_POINTS);
		for(int y=104; y<499; y+=3) 
		{
			for(int x=0; x<800; x+=6)
			{
				int c = (rand()%2)*128;
				//int a = rand() % 180;
				glColor4ub(c,c,c, 50);
				glVertex2f(x*0.8f, y*0.8f);
				x+=rand()%3;
			}
			//y+=rand()%(400- (int(noise)>399?399:int(noise)));
		}
	glEnd();
	Proj();
        */
	if (bnoise == 1)
	{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,noise[actNoise]);
	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(0.85, 0.85, 0.85, alpha);
		glTexCoord2f(1,0);
		glVertex2f(1280.0f, 1.0f);

		glTexCoord2f(0,0);
		glVertex2f(0.0f, 1.0f);

		glTexCoord2f(1,1);
		glVertex2f(1280.0f, 1024.0f);

		glTexCoord2f(0,1);
		glVertex2f(0.0f, 1024.0f);
	glEnd();

	actNoise++;
	actNoise=actNoise%20;
	}
       //	glDisable(GL_BLEND);
	
	//glDepthFunc(GL_ALWAYS);
	//glColor4f(1.0f,1.0f,1.0f,1.0f);

        /*
  //	glBindTexture(GL_TEXTURE_2D,c7logo);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,0);
		glVertex2f(640.0f, 400.0f);
		glTexCoord2f(0,0);
		glVertex2f(0.0f, 400.0f);
		glTexCoord2f(1,1);
		glVertex2f(640.0f, 480.0f);
		glTexCoord2f(0,1);
		glVertex2f(0.0f, 480.0f);

	glEnd();
          */

 /*
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.0f,0.0f,0.0f,1.0f);	
	glBegin(GL_TRIANGLE_STRIP);
		//glColor3f(0, 0, 0.2f);
		glVertex2f(0, 0);
		glVertex2f(800*0.8f, 0);
//		glColor3f(0.3f, 0.2f, 0.5f);
		glVertex2f(0, 100*0.8f);
		glVertex2f(800*0.8f, 100*0.8f);
	glEnd();
 */

        glDepthFunc(GL_LEQUAL);
      //glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
        glBlendFunc(blendSrc, blendDst);
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
