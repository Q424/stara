#include <gl/glew.h>
#include <gl/glut.h>
#include <gl/glext.h>
#include <gl/glaux.h>
#include "World.h"
#include "qutils.h"
#include "env_snow.h"


int triangle_list, sphere_list, polygon_list;
int ini_snow = 251000;
int size;
GLfloat fog_density;
GLint fog_mode;

float firex,firey,firez;
float campx,campy,campz,campalfa;
//NeHe's Particle Engine
int   nofParts;
float slowdown;
float xspeed;
float yspeed;

snows *snow = new snows[ini_snow];
//int type = 2;

// ***********************************************************************************************************
//
void Polygon(void)
{
    polygon_list = glGenLists(1);

 //   glPushMatrix();
        glNewList(polygon_list,GL_COMPILE);

       // if (type == 0) // POLYGONS
            {
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
           }


        glEndList();
   // glPopMatrix();
    glFlush();
}


// ***********************************************************************************************************
// INICJALIZACJA CZASTEK SNIEGU, Funkcja ustawiajaca wlasnosci sniegu odczytywane z pliku scenerii
// parametry wejsciowe:
// - ilosc platkow:             max 15000
// - obszar padania:            max 2000m
// - rozmiar platka:            bez ograniczenia
// - typ obiektu platka:        1-triangle, 2-polygon, 3-line, 4-point
// - losowy kolor platka od:    0.0 - 1.0
// - losowy kolor platka do:    0.0 - 1.0
// - kolorowe czy jednolite:    true/false
// - czy teksturowac obiekt:    true/false
// - czy odbiera swiatlo:       true/false
// - rodzaj blendowania:        1,2
// ***********************************************************************************************************
bool TSnow::Init(int stype, int sobjt, int sflakesnum, float sarea, float sbaseh, float ssize, float srcf, float srct, float sraf, float srat, bool scolor, bool stex, bool slight, int sbf)
{
 objt = sobjt;
 type = stype;
 blendf = sbf;
 color = scolor;
 text = stex;
 light = slight;
 size = ssize;
 sqrsize = sarea;
 nofSnow = sflakesnum;
 rcf = srcf;
 rct = srct;
 raf = sraf;
 rat = srat;
 baseh = sbaseh;

   for (int i=0;i<nofSnow;i++){
      snow[i].active   = true;
      snow[i].x        = getRandomMinMax( -sqrsize, sqrsize ); //float(rand()%size);
      snow[i].y        = 50.0f-(float)(rand()%50);
      snow[i].z        = getRandomMinMax( -sqrsize, sqrsize ); //float(rand()%size);
      snow[i].xg       =-0.2f + float(rand()%20)/10.0f;
      snow[i].yg       = 0.1f + float(rand()%10)/10.0f;
      snow[i].zg       = 0.1f + float(rand()%10)/10.0f;
      snow[i].alfa     = 0.0f;
      snow[i].alfa_inc = float(rand()%100)/10.0f;
      snow[i].r        = getRandomMinMax( rcf, rct );
      snow[i].g        = getRandomMinMax( rcf, rct );
      snow[i].b        = getRandomMinMax( rcf, rct );
      snow[i].a        = getRandomMinMax( raf, rat );
   }

// Polygon();
}



// ***********************************************************************************************************
// RENDEROWANIE PLATKOW SNIEGU
// ***********************************************************************************************************
bool TSnow::Render()
{
 if (QGlobal::bRENDERSNOW)
  {
   vector3 gridalign = togridalign(); // AKTUALNA POZYCJA KAMERY WYROWNANA DO KOMORKI
   GLfloat sizes[2];
   GLboolean blendEnabled;
   GLint blendSrc;
   GLint blendDst;
   GLfloat x, y, z;
   glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
   glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
   glGetBooleanv(GL_BLEND, &blendEnabled);
   glDepthMask(0);

 //float quadratic[] =  { 1.0f, 0.0f, 0.01f };
 //glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
 //glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );
 //glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, sizes);
   if (objt == sf_point) glEnable( GL_POINT_SPRITE_ARB );
   if (objt == sf_point) glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 3.0f );
   if (objt == sf_point) glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, 7.0f );
   if (objt == sf_point) glPointSize(2.5);
   if (objt == sf_line ) glLineWidth(5);
   if (objt != sf_tri) text = false;

   if (objt == sf_tri) glDisable(GL_CULL_FACE);                       // wylaczenie automatycznego usuwania niewidocznych scian obiektu
   if (objt == sf_tri) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);      // wlaczenie rysowania tlnych i przednich scian

   if (blendf == 1) glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   if (blendf == 2) glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
   if (blendf == 3) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


   if (!text) glDisable(GL_TEXTURE_2D);                                   // Bez tego czastepczki zaczynaly sie zbyt daleko od kamery
   if ( text) glBindTexture(GL_TEXTURE_2D, QGlobal::texturetab[2]);
   if (!light) glDisable(GL_LIGHTING);
   //glShadeModel(GL_SMOOTH);

   for (int i=0; i<nofSnow; i++)
     {

      x    = snow[i].x    += snow[i].xg;
      y    = snow[i].y    -= snow[i].yg;
      z    = snow[i].z    += snow[i].zg;

      float alfa = snow[i].alfa += snow[i].alfa_inc;

    //float pointsize = 33.0f;
    //float xDist = Global::pCamera->Pos.x - x;
    //float yDist = Global::pCamera->Pos.y - y;
    //float zDist = Global::pCamera->Pos.z - z;
    //float CamDistToEmitter = sqrt(SQR(zDist)+SQR(yDist)+SQR(xDist));
    //if (CamDistToEmitter < 0.1f) //avoid too big particles
    //CamDistToEmitter = 0.1f;
    //glPointSize(pointsize / CamDistToEmitter);


      //glPushMatrix();
      //glTranslatef(x,y,z);
      //glRotatef(alfa,0.0f,1.0f,0.0f);
      //glScalef(0.75f,0.75f,1.0f);
      //glCallList(polygon_list);


      if (snow[i].y < Global::pCamera->Pos.y -10.0f) snow[i].active = false;
      if (snow[i].active == false)
      {
          if (!color) glColor4f(0.7f, 0.7f, 0.7f, snow[i].a);
          if ( color) glColor4f(snow[i].r, snow[i].g, snow[i].b, snow[i].a);
          if (type==1) glColor4f(0.2f, 0.2f, 0.4f, snow[i].a);

          snow[i].active   = true;
          snow[i].x        = gridalign.x + getRandomMinMax( -sqrsize, sqrsize ); //(float)(rand()%size);
          snow[i].y        = Global::pCamera->Pos.y + baseh;
          snow[i].z        = gridalign.z + getRandomMinMax( -sqrsize, sqrsize ); //(float)(rand()%size)-200;
          snow[i].xg       =-0.050f + float(rand()%5)/15.0f;
          snow[i].yg       = 0.100f + float(rand()%10)/11.0f;
          snow[i].zg       =-0.050f + float(rand()%4)/10.0f;
          snow[i].alfa     = 0.000f;
          snow[i].alfa_inc = float(rand()%100)/10.0f;
          snow[i].r        = getRandomMinMax( rcf, rct );
          snow[i].g        = getRandomMinMax( rcf, rct );
          snow[i].b        = getRandomMinMax( rcf, rct );
          snow[i].a        = getRandomMinMax( raf, rat );
      }
      
      if (objt == sf_tri)  //1
      {
        glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
      //glVertex3f(x+0.0f,  y+size, z+0.0f);              // Top
      //glVertex3f(x+-size, y+-size, z+0.0f);              // Bottom Left
      //glVertex3f(x+size,  y+-size, z+0.0f);              // Bottom Right

        glTexCoord2f(0, 0); glVertex3f(x+0.0f,  y+size, z+0.0f);
        glTexCoord2f(1, 0); glVertex3f(x+-size, y+-size, z+0.0f);
        glTexCoord2f(0.5, 1); glVertex3f(x+size,  y+-size, z+0.0f);
        glEnd();
      }
      if (objt == sf_line)  //3
      {
        glBegin(GL_LINES);
        glVertex3f(x, y, z);
        glVertex3f(x+size, y+size, z+size);
        glEnd();
      }
      if (objt == sf_point) //4
      {
        glBegin(GL_POINTS);
        glVertex3f(x, y, z);
        glEnd();
      }

    //glPopMatrix();
   }
  glShadeModel(GL_SMOOTH);
//glDisable( GL_POINT_SPRITE_ARB );
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_CULL_FACE );
  glEnable( GL_LIGHTING );
  glEnable( GL_BLEND );
  glDepthMask(1);
  glBlendFunc(blendSrc, blendDst);
  glPolygonMode(GL_FRONT, GL_FILL);

  return true;
 }
}


















