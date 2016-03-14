//---------------------------------------------------------------------------

#include "system.hpp"
#include "classes.hpp"
#include "opengl/glew.h"

#pragma hdrstop

#include "env_sun.h"
#include "Globals.h"

//---------------------------------------------------------------------------
GLfloat  lightPos[4] = { 0.0f,  0.0f, 0.0f, 1.0f };

double rotx =0;
double roty =0;
double skyrottime = 0;

TSun::~TSun()
{
};

TSun::TSun()
{
};

TSun::LOADCONFIG()
{
 AnsiString LINE, TEST, PAR, asModel;
 TStringList *SKYCFG;
 SKYCFG = new TStringList;
//-- SKYCFG->LoadFromFile(Global::asAPPDIR + "data\\skyconf.txt");

 WriteLog("LOADING SUN CONFIG...");

 SUNL1_DIST = 48000;
 SUNL2_DIST = 45000;
 SUNL3_DIST = 37000;
 SUNL4_DIST = 35000;
 SUNL5_DIST = 32000;
 SUNL6_DIST = 40000;

 SUNL1_RADI = 3300/2;
 SUNL2_RADI = 1600/2;
 SUNL3_RADI = 5000/2;
 SUNL4_RADI = 5000/2;
 SUNL5_RADI = 12000/2;
 SUNL6_RADI = 12000/2;

 SUNL1_TEXT = 7;
 SUNL2_TEXT = 6;
 SUNL3_TEXT = 5;
 SUNL4_TEXT = 4;
 SUNL5_TEXT = 4;
 SUNL6_TEXT = 3;

 asModel = "sun.t3d";
 
 if (!mdSUN) mdSUN = TModelsManager::GetModel(asModel.c_str());

 if (mdSUN)  WriteLog("SUN OK");
 for (int l= 0; l < SKYCFG->Count; l++)
      {
       LINE = SKYCFG->Strings[l];
       TEST = LINE.SubString(1, LINE.Pos(":"));
       PAR =  LINE.SubString(LINE.Pos(":")+2, 255);
       //WriteLog(TEST + ", [" + PAR + "]");

       if (TEST == "SUNL1_DIST:") SUNL1_DIST = StrToFloat(PAR);
       if (TEST == "SUNL1_RADI:") SUNL1_RADI = StrToFloat(PAR);
       if (TEST == "SUNL1_TEXT:") SUNL1_TEXT = StrToInt(PAR);

       if (TEST == "SUNL2_DIST:") SUNL2_DIST = StrToFloat(PAR);
       if (TEST == "SUNL2_RADI:") SUNL2_RADI = StrToFloat(PAR);
       if (TEST == "SUNL2_TEXT:") SUNL2_TEXT = StrToInt(PAR);

       if (TEST == "SUNL3_DIST:") SUNL3_DIST = StrToFloat(PAR);
       if (TEST == "SUNL3_RADI:") SUNL3_RADI = StrToFloat(PAR);
       if (TEST == "SUNL3_TEXT:") SUNL3_TEXT = StrToInt(PAR);

       if (TEST == "SUNL4_DIST:") SUNL4_DIST = StrToFloat(PAR);
       if (TEST == "SUNL4_RADI:") SUNL4_RADI = StrToFloat(PAR);
       if (TEST == "SUNL4_TEXT:") SUNL4_TEXT = StrToInt(PAR);

       if (TEST == "SUNL5_DIST:") SUNL5_DIST = StrToFloat(PAR);
       if (TEST == "SUNL5_RADI:") SUNL5_RADI = StrToFloat(PAR);
       if (TEST == "SUNL5_TEXT:") SUNL5_TEXT = StrToInt(PAR);

       if (TEST == "SUNL6_DIST:") SUNL6_DIST = StrToFloat(PAR);
       if (TEST == "SUNL6_RADI:") SUNL6_RADI = StrToFloat(PAR);
       if (TEST == "SUNL6_TEXT:") SUNL6_TEXT = StrToInt(PAR);
      }

      delete SKYCFG;
}


  // Global::asSky zlezne od wpisu w konfigu, asSky = 1 gdy skyenabled yes w przeciwnym wypadku 0
  // nastepnie w czasie Ground.Init asSky przyjmujw zmienna z pliku scenerii czyli plik t3d nieba
TSun::Init()
{
  LOADCONFIG();

  SSRX = 0.0;  // SUN ROTATE X
  SSRY = 0.0;  // SUN ROTATE Y
  SSRZ = 0.0;  // SUN ROTATE Z

  sunrottime = 0;

  WriteLog(" ");
};


TSun::Render_luneplane(double size, int tid)
{
            //--glBindTexture(GL_TEXTURE_2D, Global::texturetab[tid]);          // SUN/MOON LUNE
		glBegin(GL_QUADS);

			// Front face
			glNormal3f( 0.0f, 0.0f, 0.5f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size,  size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size,  size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size,  size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size,  size);
			// Back face
			glNormal3f( 0.0f, 0.0f,-0.5f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, -size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-size,  size, -size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( size,  size, -size);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( size, -size, -size);

		glEnd();
}


 double user_theta  = 0;
 double user_height = 0;

 void drawSphere(double r, int lats, int longs) {
     int i, j;
     for(i = 0; i <= lats; i++) {
         double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
         double z0  = sin(lat0);
         double zr0 =  cos(lat0);

         double lat1 = M_PI * (-0.5 + (double) i / lats);
         double z1 = sin(lat1);
         double zr1 = cos(lat1);

         glBegin(GL_QUAD_STRIP);
         for(j = 0; j <= longs; j++) {
             double lng = 2 * M_PI * (double) (j - 1) / longs;
             double x = cos(lng);
             double y = sin(lng);

             glNormal3f(x * zr0, y * zr0, z0);
             glVertex3f(x * zr0, y * zr0, z0);
             glNormal3f(x * zr1, y * zr1, z1);
             glVertex3f(x * zr1, y * zr1, z1);
         }
         glEnd();
     }
 }


TSun::Render_1(double dt)
{

if (mdSUN)
 {
//--  glPushMatrix();
//--  glDisable(GL_DEPTH_TEST);
//--  glTranslatef(Global::lightPos[0], Global::lightPos[1], Global::lightPos[2]);

//drawSphere( 50, 10, 10);

//--  glColor4f(0.9, 0.6, 0.3, 0.5);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//--  glEnable(GL_BLEND);
//--  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
//--  glScalef(33, 33, 33);

//--  mdSUN->Render(100,0);
//--  mdSUN->RenderAlpha(100,0);
//--  glClear(GL_DEPTH_BUFFER_BIT);
//--  glPopMatrix();

  glLightfv(GL_LIGHT0, GL_POSITION, Global::lightPos);
 }



// SUN PIVOT ^^^^^^^^^^^^^^^^^^^^^^

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    glPushMatrix();
    glTranslatef(Global::lightPos[0],Global::lightPos[1],Global::lightPos[2]);
//  glutSolidSphere((30200000.0/100)/2, 64, 64);
    glColor4f(0.9,0.8,0.1,0.7);
//-    glutSolidSphere((20200000.0/100)/2, 64, 64);


//BILLBOARDING
    matrix4x4 mat; //potrzebujemy wspó³rzêdne przesuniêcia œrodka uk³adu wspó³rzêdnych submodelu
    glGetDoublev(GL_MODELVIEW_MATRIX,mat.getArray()); //pobranie aktualnej matrycy
    float3 gdzie=float3(mat[3][0],mat[3][1],mat[3][2]); //pocz¹tek uk³adu wspó³rzêdnych submodelu wzglêdem kamery
    glLoadIdentity(); //macierz jedynkowa
    glTranslatef(gdzie.x,gdzie.y,gdzie.z); //pocz¹tek uk³adu zostaje bez zmian
    glRotated(atan2(gdzie.x,gdzie.z)*180.0/M_PI,0.0,1.0,0.0); //jedynie obracamy w pionie o k¹t

    // SPHERE 2
//    glPushMatrix();
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
//    glColor4f(1.0, 0.4, 0.0, 0.8);
//    glTranslatef(0, 0, SUNL5_DIST);    // 700000000.0,935124000,149600000000
//-    glutSolidSphere((200000.0/2), 64, 64);
//    glPopMatrix();



  //  glEnable(GL_TEXTURE_2D);
  //  glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);                                // JEZELI TO TU JEST TO W SKY.CPP glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

    // LUNE 1
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0.0, 0.0, SUNL1_DIST);
    glColor4f(0.9,0.7,0.4,0.8);
    Render_luneplane(SUNL1_RADI, SUNL1_TEXT);      // big glow
    glPopMatrix();

    // LUNE 2
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glPushMatrix();
    glTranslatef(0.0, 0.0, SUNL2_DIST);
    glColor4f(0.9,0.7,0.4,0.8);
    Render_luneplane(SUNL2_RADI/2, SUNL2_TEXT);         // 8
    glPopMatrix();

    // LUNE 3
    glPushMatrix();
    glTranslatef(0.0, 0.0, SUNL3_DIST);
    glColor4f(0.8,0.5,0.4,0.7);
    Render_luneplane(SUNL3_RADI/2, SUNL3_TEXT);              // DAC ZWIEKSZANIE RADIUSA PRZY ZACHODZIE
    glPopMatrix();

    // LUNE 4
    glTranslatef(0.0, 0.0, SUNL4_DIST);
    glPushMatrix();
    glColor4f(0.8,0.5,0.4,0.7);
    Render_luneplane(SUNL4_RADI/2, SUNL4_TEXT);
    glPopMatrix();
/*
    // LUNE 5
    glTranslatef(0.0, 0.0, SUNL5_DIST);
    glPushMatrix();
    glColor4f(0.9,0.3,0.6,1.0);
    Render_luneplane(SUNL5_RADI/2, SUNL5_TEXT);
    glPopMatrix();
*/
/*
    // LUNE 6
    glTranslatef(0.0, 0.0, SUNL6_DIST);
    glPushMatrix();
    glColor4f(0.9,0.6,0.3,1.0);
    Render_luneplane(SUNL6_RADI, SUNL6_TEXT);
    glPopMatrix();
*/


    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

}

TSun::Render_2(double dt)
{


    // SUN PIVOT ^^^^^^^^^^^^^^^^^^^^^^
    glDisable(GL_FOG);
    glEnable(GL_BLEND);						// You should already know what this does
//glBlendFunc(GL_SRC_ALPHA, GL_ONE);				// You should already know what this does
    glEnable(GL_TEXTURE_2D);					// You should already know what this does
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslatef(0.0,0.0,0.0);
    glRotated(QGlobal::SUNROTX,1,0,0);         // LEWO/PRAWO
    glRotated(QGlobal::SUNROTY,0,1,0);         // DOL/GORA

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_FOG);
}


// ***********************************************************************************************************
// FUNKCJA WEJSCIOWA RENDEROWANIA SLONCA, wywolywana w TWorld::Render()
// ***********************************************************************************************************
TSun::Render(double dt)
{
 glLightfv(GL_LIGHT0, GL_POSITION, Global::lightPos);

 if (Global::lightPos[1] > -350000)
 {
  GLboolean blendEnabled;
  GLint blendSrc;
  GLint blendDst;

  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glDisable(GL_FOG);

  sunrottime += dt;

//     if (sunrottime > 0.1)
//         {
//          Global::SUNROTX -= (sunrottime/8);
//          Global::SUNROTY -= (sunrottime/8);
//          sunrottime = 0;
//         }
         
  Render_1(dt);
  glEnable(GL_FOG);
  glBlendFunc(blendSrc, blendDst);
  }
};



//---------------------------------------------------------------------------

#pragma package(smart_init)
