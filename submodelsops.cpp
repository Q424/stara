//---------------------------------------------------------------------------


#pragma hdrstop

#include "system.hpp"
#include "classes.hpp"
#include "submodelsops.h"
#include "model3d.h"
#include "Globals.h"
#include "Texture.h"
#include "timer.h"
#include "qutils.h"

#include <stdio.h>

GLuint bbscreen;
GLuint idig01, idig02, idig03, idig04, idig05, idig06, idig07, idig08, idig09, idig10;
AnsiString sdig01, sdig02, sdig03, sdig04, sdig05, sdig06, sdig07, sdig08, sdig09, sdig10;

AnsiString currprzebieg;


double bbtime = 0;
int r = 0;
double dt;
int x = 0;

void TSubModel::setreklam3in1(int id)
{

 dt = Timer::GetDeltaTime();

if (smID > 500 && smID < 524)
    {

     if (QGlobal::rekrot_timepause == false) QGlobal::rekrot3_time += 0.1*dt;                        // CZAS LECI ...

     if (QGlobal::rekrot_step == 1 && QGlobal::rekrot3_time >  30.0) {QGlobal::rekrot_timepause = true; QGlobal::rekrot_step = 2;}      // JEZELI CZAS = 9 TO PAUZA I KROK 2
     if (QGlobal::rekrot_step == 2) QGlobal::rekrot3_rot += 0.02;                                                    // KROK 2 - OBRACANIE
     if (QGlobal::rekrot_step == 2 && QGlobal::rekrot3_rot > 120.0) {QGlobal::rekrot_timepause = false; QGlobal::rekrot_step = 3;}        // JEZELI OSIAGNIE 60* WLACZ CZAS

     if (QGlobal::rekrot_step == 3 && QGlobal::rekrot3_time > 60.0) {QGlobal::rekrot_timepause = true; QGlobal::rekrot_step = 4;}      // JEZELI CZAS = 9 TO PAUZA I KROK 2
     if (QGlobal::rekrot_step == 4) QGlobal::rekrot3_rot += 0.02;
     if (QGlobal::rekrot_step == 4 && QGlobal::rekrot3_rot > 240.0) {QGlobal::rekrot_timepause = false; QGlobal::rekrot_step = 5;}

     if (QGlobal::rekrot_step == 5 && QGlobal::rekrot3_time > 90.0) {QGlobal::rekrot_timepause = true; QGlobal::rekrot_step = 6;}      // JEZELI CZAS = 9 TO PAUZA I KROK 2
     if (QGlobal::rekrot_step == 6) QGlobal::rekrot3_rot += 0.02;
     if (QGlobal::rekrot_step == 6 && QGlobal::rekrot3_rot > 360.0) {QGlobal::rekrot_timepause = false;}

     if (QGlobal::rekrot_step == 6 && QGlobal::rekrot3_rot > 360.0) {QGlobal::rekrot3_rot = 0.0; QGlobal::rekrot_step = 1; QGlobal::rekrot3_time = 0.0;}

     //SetRotateXYZ(vector3 (0.0,0.0, 0.0 ));

     glRotatef(QGlobal::rekrot3_rot,0,0,1);

    }

}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// BILLBOARD ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void TSubModel::setbillboardtex(int id, AnsiString nodename, int num)
{
/*
 dt = Timer::GetDeltaTime();
 bbtime += dt;

      if (name == "bbscreen")   // EKRANOWY BILLBOARD (PODPIAC GRABIENIE KLATEK Z AVI)
          {
           if (!mapsloaded)
           if (FileExists("data\\nodes\\" + nodename + ".txt"))
               {
                SL = new TStringList;
                SL->LoadFromFile("data\\nodes\\" + nodename + ".txt");
                mapsloaded = true;
               }
                       
           if (bbtime > 0000) r = 0;
           if (bbtime > 6000) r = 1;
           if (bbtime > 12000) bbtime = 0;

           if (mapsloaded) bbscreen = TTexturesManager::GetTextureID(SL->Strings[r].c_str());
           else bbscreen = TTexturesManager::GetTextureID("textures\\ip\\noise.bmp");
           
           glBindTexture(GL_TEXTURE_2D, bbscreen);

          }
   */
}

void TSubModel::setmirrorstex(int id, AnsiString nodename, int num)
{
/*
      if ( name == "mirror_r" || name == "mirror_l" || name == "mirror_a" || name == "cscreen" ||
           name == "preview01" || name == "preview02"  || name == "preview03"  || name == "preview04")
          {
            if (Global::MIRROR_R_ACTIVE && name == "mirror_r") glBindTexture(GL_TEXTURE_2D, Global::mirrorR);
            if (Global::MIRROR_L_ACTIVE && name == "mirror_l") glBindTexture(GL_TEXTURE_2D, Global::mirrorL);
            //if (name == "mirror_a") glBindTexture(GL_TEXTURE_2D, Global::mirrorA);
            //if (name == "cscreen")  glBindTexture(GL_TEXTURE_2D, Global::mirrorA);

            //if (name == "preview01")  glBindTexture(GL_TEXTURE_2D, Global::secucamciew[0]);
            //if (name == "preview02")  glBindTexture(GL_TEXTURE_2D, Global::secucamciew[1]);
            //if (name == "preview03")  glBindTexture(GL_TEXTURE_2D, Global::secucamciew[2]);
            //if (name == "preview04")  glBindTexture(GL_TEXTURE_2D, Global::secucamciew[3]);
          }
*/
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// NUMER ROZJAZDU NA ZWROTNIKU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void TSubModel::setswitchnumber(int id, AnsiString nodename, int num)
{
      if (pName == "swnum")
          {
           sdig01 = AnsiString("data\\zwr\\" + nodename.SubString(9,2) + ".bmp");
           //idig01 = TTexturesManager::GetTextureID(sdig01.c_str());
           glBindTexture(GL_TEXTURE_2D, idig01);
          }

}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// USTAWIANIE PRZEBYTEGO DYSTANSU NA HASLERZE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void TSubModel::sethaslerdistance(int id, AnsiString nodename, int num)
{
      //if (name == "hs1") glBindTexture(GL_TEXTURE_2D, Global::ddigit01);
      //if (name == "hs2") glBindTexture(GL_TEXTURE_2D, Global::ddigit02);
      //if (name == "hs3") glBindTexture(GL_TEXTURE_2D, Global::ddigit03);
      //if (name == "hs4") glBindTexture(GL_TEXTURE_2D, Global::ddigit04);
      //if (name == "hs5") glBindTexture(GL_TEXTURE_2D, Global::ddigit05);
      //if (name == "hs6") glBindTexture(GL_TEXTURE_2D, Global::ddigit06);
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ZEGAR DIODOWY (SEGMENTOWY) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void TSubModel::setclockdigital(int id, AnsiString nodename, int num)
{
      //if (name == "ccd1") { glBindTexture(GL_TEXTURE_2D, Global::cdigit01); }
      //if (name == "ccd2") { glBindTexture(GL_TEXTURE_2D, Global::cdigit02); }
      //if (name == "ccd3") { glBindTexture(GL_TEXTURE_2D, Global::cdigit03); }
      //if (name == "ccd4") { glBindTexture(GL_TEXTURE_2D, Global::cdigit04); }
      //if (name == "ccd5") { glBindTexture(GL_TEXTURE_2D, Global::cdigit05); }
      //if (name == "ccd6") { glBindTexture(GL_TEXTURE_2D, Global::cdigit06); }
      //if (name == "ccd7") { glBindTexture(GL_TEXTURE_2D, Global::cdigit07); }
      //if (name == "ccd8") { glBindTexture(GL_TEXTURE_2D, Global::cdigit08); }
}


void TSubModel::setvechcomputer(int id, AnsiString nodename, int num)
{
//
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// WAGA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

void TSubModel::getvechicleweight(int id, AnsiString nodename, int num)
{
/*
AnsiString tmp;
int x;

if (nodename == Global::asnearestobj)
    {
if (Global::waga_tor == "wagax")
    {
     AnsiString asweight =  Global::waga_masa;


     x = asweight.Pos(".");                                                     // POBIERANIE MIEJSCA SEPARATORA DZIESIETNEGO

     if (x>0) tmp = asweight.SubString(1,x+1) + "kg_______";                    // JEZELI Z PRZECINKIEM
       else tmp = asweight + "kg_________";

     asweight = tmp;


      if (Name == "diga") // DIGIT 01 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig01 = AnsiString("data\\" + asweight.SubString(1,1) + ".bmp");
           idig01 = TTexturesManager::GetTextureID(sdig01.c_str());
           glBindTexture(GL_TEXTURE_2D, idig01);
          }
      if (Name == "digb") // DIGIT 02 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig02 = AnsiString("data\\" + asweight.SubString(2,1) + ".bmp");
           if (x == 2) sdig02 = AnsiString("data\\,.bmp");
           idig02 = TTexturesManager::GetTextureID(sdig02.c_str());
           glBindTexture(GL_TEXTURE_2D, idig02);
          }
      if (Name == "digc") // DIGIT 03 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig03 = AnsiString("data\\" + asweight.SubString(3,1) + ".bmp");
           if (x == 3) sdig03 = AnsiString("data\\,.bmp");
           idig03 = TTexturesManager::GetTextureID(sdig03.c_str());
           glBindTexture(GL_TEXTURE_2D, idig03);
          }
      if (Name == "digd") // DIGIT 04 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig04 = AnsiString("data\\" + asweight.SubString(4,1) + ".bmp");
           if (x == 4) sdig04 = AnsiString("data\\,.bmp");
           idig04 = TTexturesManager::GetTextureID(sdig04.c_str());
           glBindTexture(GL_TEXTURE_2D, idig04);
          }
      if (Name == "dige") // DIGIT 05 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig05 = AnsiString("data\\" + asweight.SubString(5,1) + ".bmp");
           if (x == 5) sdig05 = AnsiString("data\\,.bmp");
           idig05 = TTexturesManager::GetTextureID(sdig05.c_str());
           glBindTexture(GL_TEXTURE_2D, idig05);
          }
      if (Name == "digf") // DIGIT 06 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig06 = AnsiString("data\\" + asweight.SubString(6,1) + ".bmp");
           if (x == 6) sdig06 = AnsiString("data\\,.bmp");
           idig06 = TTexturesManager::GetTextureID(sdig06.c_str());
           glBindTexture(GL_TEXTURE_2D, idig06);
          }
      if (Name == "digg") // DIGIT 07 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig07 = AnsiString("data\\" + asweight.SubString(7,1) + ".bmp");
           if (x == 7) sdig07 = AnsiString("data\\,.bmp");
           idig07 = TTexturesManager::GetTextureID(sdig07.c_str());
           glBindTexture(GL_TEXTURE_2D, idig07);
          }
      if (Name == "digh") // DIGIT 08 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig08 = AnsiString("data\\" + asweight.SubString(8,1) + ".bmp");
           idig08 = TTexturesManager::GetTextureID(sdig08.c_str());
           glBindTexture(GL_TEXTURE_2D, idig08);
          }
      if (Name == "digi") // DIGIT 09 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig09 = AnsiString("data\\" + asweight.SubString(9,1) + ".bmp");
           idig09 = TTexturesManager::GetTextureID(sdig09.c_str());
           glBindTexture(GL_TEXTURE_2D, idig09);
          }
      if (Name == "digj") // DIGIT 10 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          {
           sdig10 = AnsiString("data\\" + asweight.SubString(10,1) + ".bmp");
           idig10 = TTexturesManager::GetTextureID(sdig10.c_str());
           glBindTexture(GL_TEXTURE_2D, idig10);
          }

     }
 }

 */
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
