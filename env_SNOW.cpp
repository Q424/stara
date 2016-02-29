#include <gl/glew.h>
#include <gl/glut.h>
#include <gl/glext.h>
#include <gl/glaux.h>
#include "World.h"
#include "qutils.h"
#include "env_snow.h"


int triangle_list, sphere_list, polygon_list;
int ini_snow    = 95000;
int size;
GLfloat fog_density;
GLint   fog_mode;

float firex,firey,firez;
float campx,campy,campz,campalfa;
//NeHe's Particle Engine
int   nofParts;
float slowdown;
float xspeed;
float yspeed;

snows *snow = new snows[ini_snow];

void Polygon(void){
    polygon_list = glGenLists(1);
    glPushMatrix();
        glNewList(polygon_list,GL_COMPILE);
            glBegin(GL_POLYGON);
                glNormal3f  (0.0f,1.0f,0.0f);
                glTexCoord3f(0.0f,0.0f,0.0f);
                glVertex3f  (0.0f,0.0f,0.0f);
                glTexCoord3f(1.0f,0.0f,0.0f);
                glVertex3f  (1.0f,0.0f,0.0f);
                glTexCoord3f(1.0f,1.0f,0.0f);
                glVertex3f  (1.0f,1.0f,0.0f);
                glTexCoord3f(0.0f,1.0f,0.0f);
                glVertex3f  (0.0f,1.0f,0.0f);
            glEnd();
        glEndList();
    glPopMatrix();
    glFlush();
}

bool __fastcall TSnow::Init(int snowflakesnum, float size)
{

 sqrsize = size;
 nofSnow = snowflakesnum;

   for (int i=0;i<nofSnow;i++){
      snow[i].active = true;
      snow[i].x      = getRandomMinMax( -sqrsize, sqrsize ); //float(rand()%size);
      snow[i].y      = 50.0f-(float)(rand()%50);
      snow[i].z      = getRandomMinMax( -sqrsize, sqrsize ); //float(rand()%size);
      snow[i].xg     =-0.2f + float(rand()%20)/10.0f;
      snow[i].yg     = 0.1f + float(rand()%10)/10.0f;
      snow[i].zg     = 0.1f + float(rand()%10)/10.0f;
      snow[i].alfa   = 0.0f;
      snow[i].alfa_inc = float(rand()%100)/10.0f;
   }


 Polygon();
}

bool __fastcall TSnow::Render()
{
  vector3 gridalign = togridalign(); // AKTUALNA POZYCJA KAMERY WYROWNANA DO KOMORKI

  GLboolean blendEnabled;
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glGetBooleanv(GL_BLEND, &blendEnabled);
 // glEnable(GL_TEXTURE_2D);
 // glColor4f(fade[0]+1.0f,fade[1]+1.0f,fade[2]+1.0f,0.5f);

   glBlendFunc(GL_SRC_ALPHA,GL_ONE);


   glBindTexture(GL_TEXTURE_2D, QGlobal::texturetab[2]);
   glColor4f(0.7f, 0.7f, 0.7f, 0.5f);
   fog_density       = 0.015f;
   fog_mode          = GL_EXP2;
//   glDisable(GL_FOG);

//   glFogf (GL_FOG_DENSITY,fog_density);                                         //it should make a nice effect
//   glHint (GL_FOG_HINT, GL_DONT_CARE);

   float x, y, z;

   for (int i=0;i<nofSnow;i++){
      glPushMatrix();

      x    = snow[i].x    += snow[i].xg;
      y    = snow[i].y    -= snow[i].yg;
      z    = snow[i].z    += snow[i].zg;

      float alfa = snow[i].alfa += snow[i].alfa_inc;

      glTranslatef(x,y,z);
      glRotatef(alfa,0.0f,1.0f,0.0f);
      glScalef(0.5f,0.5f,1.0f);
      glCallList(polygon_list);



      if (snow[i].y < -5.0f) snow[i].active = false;
      if (snow[i].active == false){
          snow[i].active   = true;
          snow[i].x        = gridalign.x + getRandomMinMax( -sqrsize, sqrsize ); //(float)(rand()%size);
          snow[i].y        = 60.0f;
          snow[i].z        = gridalign.z + getRandomMinMax( -sqrsize, sqrsize ); //(float)(rand()%size)-200;
          snow[i].xg       =-0.05f + float(rand()%20)/15.0f;
          snow[i].yg       = 0.1f + float(rand()%10)/15.0f;
          snow[i].zg       = -0.15f + float(rand()%10)/15.0f;
          snow[i].alfa     = 0.0f;
          snow[i].alfa_inc = float(rand()%100)/10.0f;
      }
      glPopMatrix();
   }

 glEnable(GL_BLEND);
 glBlendFunc(blendSrc, blendDst);
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
// glEnable(GL_FOG);
 
/*
  vector3 gridalign = togridalign(); // AKTUALNA POZYCJA KAMERY WYROWNANA DO KOMORKI
  float x, y, z;
  if (m == 1) glColor4f(0.2f + lightning[0], 0.2f + lightning[1], 0.2f + lightning[2], 0.3f);

  GLboolean blendEnabled;
  GLboolean lightEnabled;
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glGetBooleanv(GL_BLEND, &blendEnabled);
  glGetBooleanv(GL_LIGHTING, &lightEnabled);

//  glShadeModel(GL_FLAT);
//glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
//glDisable(GL_LIGHTING);
  glLineWidth(rf);
  glEnable(GL_TEXTURE_2D);
  glColor4f(0.9, 0.9, 1.0, 0.6);
//glBlendFunc(GL_ONE, GL_ONE_MINUS_DST_COLOR);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
  glBindTexture(GL_TEXTURE_2D, Global::texturetab[7]);
    Randomize();
	//rand()%1000;
	for (int i=0; i<d; i++)
	{
		x= gridalign.x + (random_float[rand()%31]*50);
		y=               (random_float[rand()%31]*50);
		z= gridalign.z + (random_float[rand()%31]*50);

glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
    glVertex3f(x+0.0f,  y+0.1f, z+0.0f);              // Top
    glVertex3f(x+-0.1f, y+-0.2f, z+0.0f);              // Bottom Left
    glVertex3f(x+0.1f,  y+-0.2f, z+0.0f);              // Bottom Right
glEnd();
		glBegin(GL_LINES);
			glVertex3f(x, y, z);
			glVertex3f(x+s, y+s, z+s); 
		glEnd();
	}
	glEnable(GL_FOG);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(blendSrc, blendDst);
        if (!blendEnabled) glDisable(GL_BLEND); else glEnable(GL_BLEND);
        if (!lightEnabled) glDisable(GL_LIGHTING); else glEnable(GL_LIGHTING);

 */
 return true;
}



