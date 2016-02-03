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
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath.h>

#pragma hdrstop

#include "Segment.h"
#include "Usefull.h"
#include "effects2d.h"
#include "Globals.h"

bool bNoise=true;
GLuint noise[30];
int actNoise=0;
int NOISERES = 1024;


void DrawCircle(float cx, float cy, float r, Color4 color)
{
     glDisable(GL_LIGHTING);
     glBegin(GL_LINE_LOOP);
     glColor4f(1.0f, 0.8f, 0.1f, 0.7f);
            
            for (int i = 0; i <= 360; i++)
            {
                float degInRad = DegToRad(i);
                glVertex2f(cx + ((float)cos(degInRad) * r), cy + ((float)sin(degInRad) * r));
            }

     glEnd();
     glEnable(GL_LIGHTING);
}


void DrawCircle(float cx, float cy, float r, int segments)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < segments; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(segments);//get the current angle

        float x = r * cos(theta);//calculate the x component
        float y = r * sin(theta);//calculate the y component

        glVertex2f(x + cx, y + cy);//output vertex

    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void DrawFilledCircle(float cx, float cy, float r, Color4 color)
{
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_TRIANGLE_FAN);
  //glColor4fv(fillcolor.ToFloatArray());
    glColor4f(1.0f, 0.8f, 0.1f, 0.7f);
    float y1 = cy;
    float x1 = cx;

    for (int i = 0; i <= 360; i++)
      {
       float degInRad = DegToRad(i);
       float x2 = cx + ((float)cos(degInRad) * r);
       float y2 = cy + ((float)sin(degInRad) * r);
       glVertex2f(cx,cy);
       glVertex2f(x1,y1);
       glVertex2f(x2,y2);
       y1=y2;
       x1=x2;
      }
    glEnd();
    glEnable(GL_LIGHTING);
}

void Disk( float x, float y, float r )
{
    glDisable(GL_LIGHTING);
    glBegin( GL_TRIANGLE_FAN );
        glVertex2f( x, y );
        for( float i = 0; i <= 2 * PI + 0.1; i += 0.1 )
        {
            glVertex2f( x + sin( i ) * r, y + cos( i ) * r );
        }
    glEnd();
    glEnable(GL_LIGHTING);
}


float roundf(float val)
{
 return ceil(val);
}

typedef struct
{
    float x;
    float y;

} Vector2f;

void RoundRectW( int x, int y, int width, int height, int radius, int resolution, Color4 color)
{

y = height + y;

float step = ( 2.0f * M_PI ) / resolution,
      angle = 0.0f,
      x_offset,
      y_offset;

int i = 0;

unsigned int index = 0,
segment_count = ( int )( resolution / 4 );

Vector2f *top_left             = ( Vector2f * ) malloc( segment_count * sizeof( Vector2f ) ),
         *bottom_left         = ( Vector2f * ) malloc( segment_count * sizeof( Vector2f ) ),
         *top_right             = ( Vector2f * ) malloc( segment_count * sizeof( Vector2f ) ),
         *bottom_right         = ( Vector2f * ) malloc( segment_count * sizeof( Vector2f ) ),
          bottom_left_corner = { x + radius,
                                 y - height + radius }; 

while( i != segment_count )
{
    x_offset = cos( angle );
    y_offset = sin( angle );


    top_left[ index ].x = bottom_left_corner.x - 
                          ( x_offset * radius );
    top_left[ index ].y = ( height - ( radius * 2.0f ) ) + 
                            bottom_left_corner.y - 
                          ( y_offset * radius );


    top_right[ index ].x = ( width - ( radius * 2.0f ) ) + 
                             bottom_left_corner.x + 
                           ( x_offset * radius );
    top_right[ index ].y = ( height - ( radius * 2.0f ) ) + 
                             bottom_left_corner.y -
                           ( y_offset * radius );


    bottom_right[ index ].x = ( width - ( radius * 2.0f ) ) +
                                bottom_left_corner.x + 
                              ( x_offset * radius );
    bottom_right[ index ].y = bottom_left_corner.y + 
                              ( y_offset * radius );


    bottom_left[ index ].x = bottom_left_corner.x - 
                             ( x_offset * radius );
    bottom_left[ index ].y = bottom_left_corner.y +
                             ( y_offset * radius );


    top_left[ index ].x = roundf( top_left[ index ].x );
    top_left[ index ].y = roundf( top_left[ index ].y );


    top_right[ index ].x = roundf( top_right[ index ].x );
    top_right[ index ].y = roundf( top_right[ index ].y );


    bottom_right[ index ].x = roundf( bottom_right[ index ].x );
    bottom_right[ index ].y = roundf( bottom_right[ index ].y );


    bottom_left[ index ].x = roundf( bottom_left[ index ].x );
    bottom_left[ index ].y = roundf( bottom_left[ index ].y );

    angle -= step;

    ++index;

    ++i;
}
/*
glDisable(GL_LIGHTING);
glDisable( GL_FOG );
glDisable( GL_TEXTURE_2D );
glColor4f(0.4f, 0.4f, 0.4f, 0.6f);

glBegin( GL_TRIANGLE_STRIP );
{
    // Top
    {
        i = 0;
        while( i != segment_count )
        {
            glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
            glVertex2i( top_left[ i ].x, top_left[ i ].y );

            glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
            glVertex2i( top_right[ i ].x, top_right[ i ].y );

            ++i;
        }
    }


    // In order to stop and restart the strip.
    glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
    glVertex2i( top_right[ 0 ].x, top_right[ 0 ].y );

    glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
    glVertex2i( top_right[ 0 ].x, top_right[ 0 ].y );


    // Center
    {
        glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
        glVertex2i( top_right[ 0 ].x, top_right[ 0 ].y );
        glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
        glVertex2i( top_left[ 0 ].x, top_left[ 0 ].y );


        glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
        glVertex2i( bottom_right[ 0 ].x, bottom_right[ 0 ].y );
         glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
        glVertex2i( bottom_left[ 0 ].x, bottom_left[ 0 ].y );
    }


    // Bottom
    i = 0;
    while( i != segment_count )
    {
        glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
        glVertex2i( bottom_right[ i ].x, bottom_right[ i ].y );
        glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
        glVertex2i( bottom_left[ i ].x, bottom_left[ i ].y );

        ++i;
    }    
}
glEnd();
 */

glDisable(GL_LIGHTING);
glDisable( GL_FOG );
glDisable( GL_TEXTURE_2D );
//glColor4f(color.r, color.g, color.b, color.o);
glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
glBegin( GL_LINE_STRIP );
// Border
{
    i = ( segment_count - 1 );
    while( i > -1 )
    {    
        glVertex2i( top_left[ i ].x,
                    top_left[ i ].y );

        --i;
    }


    i = 0;
    while( i != segment_count )
    {    
        glVertex2i( bottom_left[ i ].x,
                    bottom_left[ i ].y );

        ++i;
    }


    i = ( segment_count - 1 );
    while( i > -1 )
    {    
        glVertex2i( bottom_right[ i ].x,
                    bottom_right[ i ].y );

        --i;
    }


    i = 0;
    while( i != segment_count )
    {    
        glVertex2i( top_right[ i ].x,
                    top_right[ i ].y );

        ++i;
    }


    // Close the border.
    glVertex2i( top_left[ ( segment_count - 1 ) ].x,
                top_left[ ( segment_count - 1 ) ].y );
}
glEnd();



//glColor4f(color.r, color.g, color.b, color.o);

glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
glBegin( GL_LINES );

//glColor4f( 0.0f, 1.0f, 1.0f, 1.0f );
//glColor4f( 0.0f, 0.5f, 1.0f, 1.0f );

// Separator
{
    // Top bar
    glVertex2i( top_right[ 0 ].x,
                top_right[ 0 ].y );

    glVertex2i( top_left[ 0 ].x,
                top_left[ 0 ].y );    


    // Bottom bar
    glVertex2i( bottom_left[ 0 ].x,
                bottom_left[ 0 ].y );    

    glVertex2i( bottom_right[ 0 ].x,
                bottom_right[ 0 ].y );    
}
glEnd();

glEnable( GL_TEXTURE_2D );
glEnable( GL_LIGHTING );

free( top_left );
free( bottom_left );
free( top_right );
free( bottom_right );
}

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
