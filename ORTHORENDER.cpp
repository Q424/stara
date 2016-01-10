//---------------------------------------------------------------------------
/*

    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak, Maciej Czapkiewicz and others

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include <fstream>
#include <string>

#include "opengl/glew.h"   // ZAWSZE JAKO PIERWSZY MODUL OPENGLOWY
#include "glfont.c"
#include "system.hpp"
#include "classes.hpp"
#include "sysutils.hpp"
#include "shellapi.h"
#include "Console.h"
#include "dynobj.h"
#include "train.h"
#include "bitmap_Font.h"
#include "screen.h"
//#include <gl/gl.h>
//#include <gl/glu.h>
//#include <GL/glaux.h>
//#include "opengl/glext.h"
//#include "opengl/glut.h"
//#include <vfw.h>	// Header file for video for windows
#pragma hdrstop

#include "Timer.h"
#include "mtable.hpp"
#include "Sound.h"
#include "World.h"
#include "logs.h"
#include "Globals.h"
#include "Camera.h"
#include "Driver.h"
#include "Event.h"
#include "Train.h"
#include "freetype.h"		// Header for our little font library.
//#include "screenshot.h"                     // QUEUEDZIU 170806
#include "qutils.h"

int mtype;
bool wdebugger;
bool cmdexecute;
AnsiString DESTLOK;
double promptblink = 0;
TStringList *textfromfile;
TStringList *execcommands;
TStringList *nodesinmain;
AnsiString CDATE;
int cl, lc;
int nn;
bool iniloaded = false;
bool floaded = false;
bool startsound1 = false;
bool startsound2 = false;
bool tmp;
Font *BFONT;


int GWW, GWH, PBX, PBY;
int ypos;
static POINT mouse;
AnsiString desc;

AnsiString selscn;
AnsiString selvch;
AnsiString seltrs;
AnsiString seltrk;
int selscnid = 0;
int selvchid = 0;
int seltrsid = 0;
int seltrkid = 0;

int sitscn = 0;
int sittrs = 0;
int sittrk = 0;

double irailmaxdistance;
double ipodsmaxdistance;
int isnowflakesnum;

int resolutionid = 6;
int sfrequencyid = 3;
int selsw;
int selsh;
AnsiString resolutionxy;
AnsiString currloading;
AnsiString currloading_b;
AnsiString currloading_bytes;

bool trainsetsect;
bool bnodesmain = false;

AnsiString _TRAINSETNAME;

GLuint kartaobiegu;

TStringList *MODESLIST;

bool bcheckfreq = false;
bool startpassed = false;

float bkgalpha = 0.2;
float ltrans = 0.99;

float emm1[]= { 1,1,1,0 };
float emm2[]= { 0,0,0,0 };

// CONSOLE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool showpointer = true;

AnsiString state = "";



float LDR_COLOR_R;
float LDR_COLOR_G;
float LDR_COLOR_B;
float LDR_STR_1_R;
float LDR_STR_1_G;
float LDR_STR_1_B;
float LDR_STR_1_A;
float LDR_TBACK_R;
float LDR_TBACK_G;
float LDR_TBACK_B;
float LDR_TBACK_A;
float LDR_PBARLEN;
float LDR_PBAR__R;
float LDR_PBAR__G;
float LDR_PBAR__B;
float LDR_PBAR__A;
float LDR_LOGOVIS;
float LDR_MLOGO_X;
float LDR_MLOGO_Y;
float LDR_MLOGO_A;
float LDR_DESCVIS;
float LDR_BRIEF_X;
float LDR_BRIEF_Y;
AnsiString LDR_STR_LOAD;
AnsiString LDR_STR_FRST;

 

// *****************************************************************************
// LOADING Q FEATURES CONFIG ***************************************************
// *****************************************************************************

bool __fastcall TWorld::LOADQCONFIG()
{
 AnsiString LINE, TEST, PAR;
 QGlobal::CONFIG->LoadFromFile(QGlobal::asAPPDIR + "data\\config.txt");

 WriteLog("LOADING Q FEATURES CONFIG...");

 for (int l= 0; l < QGlobal::CONFIG->Count; l++)
      {
       LINE = QGlobal::CONFIG->Strings[l];
       TEST = LINE.SubString(1, LINE.Pos(":"));
       PAR =  LINE.SubString(LINE.Pos(":")+2, 255);
       WriteLog(TEST + ", [" + PAR + "]");

       // if (TEST == "SUN:") Global::bsun = StrToBool(PAR);
       // if (TEST == "CLD:") Global::bRenderSky = StrToBool(PAR);
     }
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// POBIERANIE DOSTEPNYCH ROZDZIELCZOSCI DLA ZADANEJ CZESTOTLIWOSCI ^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


/*
bool listmodes(int HZ)
{
if (Global::bQueuedAdvLog) WriteLog("bool listmodes(" + AnsiString(HZ) + ")");
TDevMode   DevMode;
BOOL		bRetVal;
int cnt = 0;
int LP = 0;

AnsiString str1, str2, str3, str4, str5, str6;

MODESLIST= new TStringList;
MODESLIST->Clear();
do
{
    bRetVal = ::EnumDisplaySettings(NULL, cnt, &DevMode);
    cnt++;
    if (bRetVal)
    {
     str1 = DevMode.dmPelsWidth;
     str2 = DevMode.dmPelsHeight;
     str3 = DevMode.dmBitsPerPel ;
     str4 = DevMode.dmDisplayFrequency;
     str5 = DevMode.dmDriverVersion;
     str6 = DevMode.dmPanningWidth;

     if (str3 == IntToStr(Global::iBpp) && str4 == IntToStr(HZ))
         {
          LP++;
          MODESLIST->Add(str1 + "/" + str2);
         }
    }

    }
     while (bRetVal);

     if (MODESLIST->Text != "") resolutionid = 0;
     if (MODESLIST->Text != "") resolutionxy = MODESLIST->Strings[resolutionid];

       for (int i=0; i<MODESLIST->Count-1; i++)
            {
             if ( MODESLIST->Strings[i] == AnsiString(Global::iWindowWidth) + "/" + AnsiString(Global::iWindowHeight)) resolutionid = i+1;
             resolutionxy = MODESLIST->Strings[resolutionid];
            }
}
  */
bool checkfreq()
{
/*
if (Global::bQueuedAdvLog) WriteLog("bool checkfreq()");

if (Global::bAdjustScreenFreq) Global::iScreenFreq = Global::iScreenFreqA;

 if (Global::iScreenFreq == 60) sfrequencyid = 0;
 if (Global::iScreenFreq == 70) sfrequencyid = 1;
 if (Global::iScreenFreq == 72) sfrequencyid = 2;
 if (Global::iScreenFreq == 75) sfrequencyid = 3;
 if (Global::iScreenFreq == 80) sfrequencyid = 4;

 if (sfrequencyid == 0) Global::iScreenFreq = 60;
 if (sfrequencyid == 1) Global::iScreenFreq = 70;
 if (sfrequencyid == 2) Global::iScreenFreq = 72;
 if (sfrequencyid == 3) Global::iScreenFreq = 75;
 if (sfrequencyid == 4) Global::iScreenFreq = 80;
 if (sfrequencyid == 5) Global::iScreenFreq = 85;
 if (sfrequencyid == 6) Global::iScreenFreq = 90;
 if (sfrequencyid == 7) Global::iScreenFreq = 120;

//ShowMessage(AnsiString(Global::iScreenFreq));
 listmodes(Global::iScreenFreq);

 bcheckfreq = true;
 */
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// USTAWIENIE ROZDZIELCZOSCI Z EU07.CFG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool SetConfigResolution()
{
if (QGlobal::bQueuedAdvLog) WriteLog("bool SetConfigResolution()");
if (QGlobal::bQueuedAdvLog) WriteLog("modes for current freq: " + AnsiString(MODESLIST->Count));
 for (int i=0; i<MODESLIST->Count; i++)
      {
       if (QGlobal::bQueuedAdvLog) WriteLog(AnsiString(i));
       if (MODESLIST->Strings[i] == AnsiString(Global::iWindowWidth) + "/" + AnsiString(Global::iWindowHeight) ) resolutionid = i; // USTAWIENIE IDa
      }
      if (MODESLIST->Text != "") resolutionxy = MODESLIST->Strings[resolutionid]; // DODANIE DO EDITBOXA

      selsw = StrToInt(resolutionxy.SubString(1, resolutionxy.Pos("/")-1));      // WYCIAGNIECIE SW
      selsh = StrToInt(resolutionxy.SubString(resolutionxy.Pos("/")+1, 255));    // WYCIAGNIECIE SH
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// NASTEPNA ROZDZIELCZOSC ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool ScreenResolutionAdd()
{
  if (MODESLIST->Text != "") if (resolutionid < MODESLIST->Count-1) resolutionid++;
  if (MODESLIST->Text != "") resolutionxy = MODESLIST->Strings[resolutionid];

 selsw = StrToInt(resolutionxy.SubString(1, resolutionxy.Pos("/")-1));
 selsh = StrToInt(resolutionxy.SubString(resolutionxy.Pos("/")+1, 255));
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// POPRZEDNIA ROZDZIELCZOSC ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool ScreenResolutionRem()
{
 if (MODESLIST->Text != "") if (resolutionid > 0) resolutionid--;
 if (MODESLIST->Text != "") resolutionxy = MODESLIST->Strings[resolutionid];

 selsw = StrToInt(resolutionxy.SubString(1, resolutionxy.Pos("/")-1));
 selsh = StrToInt(resolutionxy.SubString(resolutionxy.Pos("/")+1, 255));
}


bool ScreenFrequencyAdd()
{
/*
if (!Global::bAdjustScreenFreq)
    {
     if (sfrequencyid < 9) sfrequencyid++;
     if (sfrequencyid == 0) Global::iScreenFreq = 60;
     if (sfrequencyid == 1) Global::iScreenFreq = 70;
     if (sfrequencyid == 2) Global::iScreenFreq = 72;
     if (sfrequencyid == 3) Global::iScreenFreq = 75;
     if (sfrequencyid == 4) Global::iScreenFreq = 80;
     if (sfrequencyid == 5) Global::iScreenFreq = 85;
     if (sfrequencyid == 6) Global::iScreenFreq = 90;
     if (sfrequencyid == 7) Global::iScreenFreq = 120;

     listmodes(Global::iScreenFreq);
    }
    */
}

bool ScreenFrequencyRem()
{
/*
if (!Global::bAdjustScreenFreq)
    {
 if (sfrequencyid > 0) sfrequencyid--;
 if (sfrequencyid == 0) Global::iScreenFreq = 60;
 if (sfrequencyid == 1) Global::iScreenFreq = 70;
 if (sfrequencyid == 2) Global::iScreenFreq = 72;
 if (sfrequencyid == 3) Global::iScreenFreq = 75;
 if (sfrequencyid == 4) Global::iScreenFreq = 80;
 if (sfrequencyid == 5) Global::iScreenFreq = 85;
 if (sfrequencyid == 6) Global::iScreenFreq = 90;
 if (sfrequencyid == 7) Global::iScreenFreq = 120;

 listmodes(Global::iScreenFreq);
 }
 */
}

bool CHANGECONSIST()
{
/*
 AnsiString APPDIR, x;
 int tracknamelen;
 bool trackexist = false;

 APPDIR = ExtractFilePath(ParamStr(0));

 tracknamelen = seltrk.Length();

 for (int i=0; i<Form3->RESCN->Lines->Count; i++)
      {
       x = Form3->RESCN->Lines->Strings[i];

       if ( x.SubString(1,14+tracknamelen) == "trainset none " + seltrk)
           {
            Form3->RESCN->Lines->Delete(i);
            Form3->RESCN->Lines->Add( Form3->ETRAINSET->Text );
            Form3->RESCN->Lines->SaveToFile(APPDIR + "SCENERY\\" + selscn);
            trackexist = true;
           }
            else
           {
            trackexist = false;
           }

      }

    if (trackexist == false) Form3->RESCN->Lines->Add( Form3->ETRAINSET->Text );
    if (trackexist == false) Form3->RESCN->Lines->SaveToFile(APPDIR + "SCENERY\\" + selscn);
    */
 return true;
}

bool ADDLOCOMOTIVE()
{
 //ShowMessage("AL");
 return true;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// FUNKCJA SPRAWDZAJACA CZY ISTNIEJE PLIK CHARAKTERYSTYKI POJAZDU ^^^^^^^^^^^^^^
// SKANOWANIE W GLOWNYM PLIKU SCN ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool checknodes()
{
 TStringList *noexvechicles;
 AnsiString APPDIR, filetocheck;
 AnsiString node, dynname, maxdist, mindist, type, subdir, texture, chk, body;
 bool noexist = false;

 APPDIR = ExtractFilePath(ParamStr(0));

 noexvechicles = new TStringList;
 noexvechicles->Clear();
 noexvechicles->Add("LISTA NIEISTNIEJACYCH POJAZDOW:");
 noexvechicles->Add(" ");

 for (int lc=0; lc<nodesinmain->Count; lc++)
      {
        node = nodesinmain->Strings[lc];

        node.SubString(1, node.Pos(" ")-1);
        node.Delete(1, node.Pos(" "));
        maxdist = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
        mindist = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
        dynname = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
        type = node.SubString(1, node.Pos(" ")-1);    node.Delete(1, node.Pos(" "));
        subdir = node.SubString(1, node.Pos(" ")-1);  node.Delete(1, node.Pos(" "));
        texture = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
        chk = node.SubString(1, node.Pos(" ")-1);     node.Delete(1, node.Pos(" "));

        filetocheck = APPDIR + "dynamic\\" + subdir + "\\" + chk + ".mmd";

        if (FileExists(filetocheck) == false)
            {
             noexist = true;
             noexvechicles->Add(filetocheck);
            }
      }
     //if (noexist) ShowMessage(noexvechicles->Text);
 return true;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// FUNKCJA SPRAWDZAJACA CZY ISTNIEJE PLIK CHARAKTERYSTYKI POJAZDU ^^^^^^^^^^^^^^
// SKANOWANIE W PLIKU SKLADU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool checkconsist(AnsiString file)
{
 TStringList *noexvechicles;
 AnsiString dynname, maxdist, mindist, type, subdir, texture, chk, body;
 AnsiString APPDIR, filetocheck, line, node;
 string wers;
 bool noexist = false;
 int nodes = 0;

 APPDIR = ExtractFilePath(ParamStr(0));

 noexvechicles = new TStringList;
 noexvechicles->Clear();
 noexvechicles->Add("LISTA NIEISTNIAJECYCH POJAZDOW:");
 noexvechicles->Add(" ");

 ifstream in(file.c_str());

 while(getline(in, wers))
       {
        node = wers.c_str();

        if (node.SubString(1,4) == "node")
            {
             node.SubString(1, node.Pos(" ")-1);
             node.Delete(1, node.Pos(" "));
             maxdist = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
             mindist = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
             dynname = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
             type = node.SubString(1, node.Pos(" ")-1);    node.Delete(1, node.Pos(" "));
             subdir = node.SubString(1, node.Pos(" ")-1);  node.Delete(1, node.Pos(" "));
             texture = node.SubString(1, node.Pos(" ")-1); node.Delete(1, node.Pos(" "));
             chk = node.SubString(1, node.Pos(" ")-1);     node.Delete(1, node.Pos(" "));
             filetocheck = APPDIR + "dynamic\\" + subdir + "\\" + chk + ".chk";

             if (FileExists(filetocheck) == false)
                 {
                  noexist = true;
                  noexvechicles->Add(filetocheck);
                 }
            }
       }
     //if (noexist) ShowMessage(noexvechicles->Text);
  return true;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// FUNKCJA SPRAWDZAJACA CZY JEST MOZLIWOSC JAZDY POJAZDEM
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool isdriveable(AnsiString chk)
{
 bool driveable = false;

 if (LowerCase(chk) == "sn61") driveable =true;
 if (LowerCase(chk) == "6d") driveable =true;
 if (LowerCase(chk) == "ls800") driveable =true;
 if (LowerCase(chk) == "4e") driveable =true;
 if (LowerCase(chk) == "4e-n") driveable =true;
 if (LowerCase(chk) == "4e1") driveable =true;
 if (LowerCase(chk) == "104e") driveable =true;
 if (LowerCase(chk) == "104e1") driveable =true;
 if (LowerCase(chk) == "303e") driveable =true;
 if (LowerCase(chk) == "303e-n") driveable =true;
 if (LowerCase(chk) == "303e-m") driveable =true;
 if (LowerCase(chk) == "201e") driveable =true;
 if (LowerCase(chk) == "201e-w") driveable =true;
 if (LowerCase(chk) == "201e-n") driveable =true;
 if (LowerCase(chk) == "203e-a") driveable =true;
 if (LowerCase(chk) == "203e-b") driveable =true;
 if (LowerCase(chk) == "ls150") driveable =true;
 if (LowerCase(chk) == "44e-04") driveable =true;
 if (LowerCase(chk) == "6ba") driveable =true;
 if (LowerCase(chk) == "6bb") driveable =true;
 if (LowerCase(chk) == "060da") driveable =true;
 if (LowerCase(chk) == "060db-a") driveable =true;
 if (LowerCase(chk) == "411d") driveable =true;
 if (LowerCase(chk) == "3e") driveable =true;
 if (LowerCase(chk) == "3e-n") driveable =true;
 if (LowerCase(chk) == "3e1") driveable =true;
 if (LowerCase(chk) == "3e2") driveable =true;
 if (LowerCase(chk) == "3e2-n") driveable =true;
 if (LowerCase(chk) == "4e_b") driveable =true;
 if (LowerCase(chk) == "4e-zez") driveable =true;
 if (LowerCase(chk) == "4e-ep_b") driveable =true;
 if (LowerCase(chk) == "4e-ep") driveable =true;
 if (LowerCase(chk) == "4e-ep-zez_B") driveable =true;
 if (LowerCase(chk) == "4e-ep-zez") driveable =true;
 if (LowerCase(chk) == "6baii") driveable =true;
 if (LowerCase(chk) == "6bsii") driveable =true;
 if (LowerCase(chk) == "6bbii") driveable =true;
 if (LowerCase(chk) == "st44-u") driveable =true;
 if (LowerCase(chk) == "t448p1") driveable =true;

 if (LowerCase(chk) == "q001") driveable =true;

 return driveable;
}

AnsiString gettrainsetname(AnsiString trainset)
{
 AnsiString trainsetname;

 trainset.SubString(1, trainset.Pos(" ")-1);
 trainset.Delete(1, trainset.Pos(" "));
 trainsetname = trainset.SubString(1, trainset.Pos(" ")-1);
 trainset.Delete(1, trainset.Pos(" "));

 return trainsetname;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// getdynamicname() - ZWRACA NAZWE POJAZDU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

AnsiString getdynamicname(AnsiString node)
{
 AnsiString dynname, maxdist, mindist, type, subdir, texture, chk, body;

 node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));

 maxdist = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 mindist = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 dynname = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 type = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 subdir = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 texture = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 chk = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));

 return dynname;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// getdynamicchk() - ZWRACA CHK POJAZDU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

AnsiString getdynamicchk(AnsiString node)
{
 AnsiString dynname, maxdist, mindist, type, subdir, texture, chk, body;

 node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));

 maxdist = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 mindist = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 dynname = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 type = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 subdir = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 texture = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));
 chk = node.SubString(1, node.Pos(" ")-1);
 node.Delete(1, node.Pos(" "));

 return chk;
}




// *****************************************************************************
//
// *****************************************************************************

void TWorld::ShowHints(void)
{
//Ra: nie u¿ywaæ tego, bo Ÿle dzia³a
 //Q: popsulem do konca bo i tak nie dzialalo
 //  glBindTexture(GL_TEXTURE_2D, 0);
 //  glColor4f(0.3f,1.0f,0.3f,1.0f);
 //  glLoadIdentity();
 //  glTranslatef(0.0f,0.0f,-0.50f);


   //Global::debuginfo1="Uruchamianie lokomotywy - pomoc dla niezaawansowanych";
/*
   if(TestFlag(Controlled->MoverParameters->SecuritySystem.Status,s_ebrake))
      {
        Global::debuginfo1 = "Gosciu, ale refleks to ty masz szachisty. Teraz zaczekaj.";
        Global::debuginfo2 = "W tej sytuacji czuwak mozesz zbic dopiero po zatrzymaniu pociagu. ";
        if(Controlled->MoverParameters->Vel==0)
        Global::debuginfo3 = "   (mozesz juz nacisnac spacje)";
      }
   else
   if(TestFlag(Controlled->MoverParameters->SecuritySystem.Status,s_alarm))
      {
        Global::debuginfo1 ="Natychmiast zbij czuwak, bo pociag sie zatrzyma!";
        Global::debuginfo2 ="   (szybko nacisnij spacje!)";
      }
   else
   if(TestFlag(Controlled->MoverParameters->SecuritySystem.Status,s_aware))
      {
        Global::debuginfo1 ="Zbij czuwak, zeby udowodnic, ze nie spisz :) ";
        Global::debuginfo2 ="   (nacisnij spacje)";
      }
   else
   if(Controlled->MoverParameters->FuseFlag)
      {
        Global::debuginfo1 ="Czlowieku, delikatniej troche! Gdzie sie spieszysz?";
        Global::debuginfo2 ="Wybilo Ci bezpiecznik nadmiarowy, teraz musisz wlaczyc go ponownie.";
        Global::debuginfo3 ="   ('N', wczesniej nastawnik i boczniki na zero -> '-' oraz '*' do oporu)";

      }
   else
   if (Controlled->MoverParameters->V==0)
   {
      if (!(Controlled->MoverParameters->PantFrontVolt||Controlled->MoverParameters->PantRearVolt))
         {
         Global::debuginfo1 ="Jezdziles juz kiedys lokomotywa? Pierwszy raz? Dobra, to zaczynamy.";
         Global::debuginfo2 ="No to co, trzebaby chyba podniesc pantograf?";
         Global::debuginfo3 ="   (wcisnij 'shift+P' - przedni, 'shift+O' - tylny)";
         }
      else
      if (!Controlled->MoverParameters->Mains)
         {
         Global::debuginfo1 ="Dobra, mozemy uruchomic glowny obwod lokomotywy.";
         Global::debuginfo2 ="   (wcisnij 'shift+M')";
         }
      else
      if (!Controlled->MoverParameters->ConverterAllow)
         {
         Global::debuginfo1 ="Teraz wlacz przetwornice.";
         Global::debuginfo2 ="   (wcisnij 'shift+X')";
         }
      else
      if (!Controlled->MoverParameters->CompressorAllow)
         {
         Global::debuginfo1 ="Teraz wlacz sprezarke.";
         Global::debuginfo2 ="   (wcisnij 'shift+C')";
         }
      else
      if (Controlled->MoverParameters->ActiveDir==0)
         {
         Global::debuginfo1 ="Ustaw nawrotnik na kierunek, w ktorym chcesz jechac.";
         Global::debuginfo2 ="   ('d' - do przodu, 'r' - do tylu)";
         }
      else
      if (Controlled->GetFirstDynamic(1)->MoverParameters->BrakePress>0.1)
         {
         Global::debuginfo1 ="Odhamuj sklad i zaczekaj az Ci powiem - to moze troche potrwac.";
         Global::debuginfo2 ="   ('.' na klawiaturze numerycznej)";
         }
      else
      if (Controlled->MoverParameters->BrakeCtrlPos!=0)
         {
         Global::debuginfo1 ="Przelacz kran hamulca w pozycje 'jazda'.";
         Global::debuginfo2 ="   ('4' na klawiaturze numerycznej)";
         }
      else
      if (Controlled->MoverParameters->MainCtrlPos==0)
         {
         Global::debuginfo1 ="Teraz juz mozesz ruszyc ustawiajac pierwsza pozycje na nastawniku.";
         Global::debuginfo2 ="   (jeden raz '+' na klawiaturze numerycznej)";
         }
      else
      if((Controlled->MoverParameters->MainCtrlPos>0)&&(Controlled->MoverParameters->ShowCurrent(1)!=0))
         {
         Global::debuginfo1 ="Dobrze, mozesz teraz wlaczac kolejne pozycje nastawnika.";
         Global::debuginfo2 ="   ('+' na klawiaturze numerycznej, tylko z wyczuciem)";
         }
      if((Controlled->MoverParameters->MainCtrlPos>1)&&(Controlled->MoverParameters->ShowCurrent(1)==0))
         {
         Global::debuginfo1 ="Spieszysz sie gdzies? Zejdz nastawnikiem na zero i probuj jeszcze raz!";
         Global::debuginfo2 ="   (teraz do oporu '-' na klawiaturze numerycznej)";
         }
   }
   else
   {
      Global::debuginfo1 ="Aby przyspieszyc mozesz wrzucac kolejne pozycje nastawnika.";
      if(Controlled->MoverParameters->MainCtrlPos==28)
      {Global::debuginfo1 ="Przy tym ustawienu mozesz bocznikowac silniki - sprobuj: '/' i '*' ";}
      if(Controlled->MoverParameters->MainCtrlPos==43)
      {Global::debuginfo1 ="Przy tym ustawienu mozesz bocznikowac silniki - sprobuj: '/' i '*' ";}

      Global::debuginfo2 ="Aby zahamowac zejdz nastawnikiem do 0 ('-' do oporu) i ustaw kran hamulca";
      Global::debuginfo3 ="w zaleznosci od sily hamowania, jakiej potrzebujesz ('2', '5' lub '8' na kl. num.)";

      //else
      //if() OutText1="teraz mozesz ruszyc naciskajac jeden raz '+' na klawiaturze numerycznej";
      //else
      //if() OutText1="teraz mozesz ruszyc naciskajac jeden raz '+' na klawiaturze numerycznej";
   }
   //OutText3=FloatToStrF(Controlled->MoverParameters->SecuritySystem.Status,ffFixed,3,0);

   //renderpanview(0.80, 150, 1);
   RenderTUTORIAL(1);
   //Global::debuginfo3=FloatToStrF(Controlled->MoverParameters->SecuritySystem.Status,ffFixed,3,0);
    */
};



// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RENDEROWANIE TEKSTU IRC ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
bool __fastcall TWorld::RenderIRCEU07Text()
{

}
*/


bool __fastcall RenderMenuProgressBar(int x, int y)
{

}


void __fastcall setfontcolor()
{
      //oswietlenie kabiny
      GLfloat  ambientCabLight[4]= { 0.4f,  0.4f, 0.2f, 0.9f };
      GLfloat  diffuseCabLight[4]= { 0.4f,  0.4f, 0.2f, 1.0f };
      GLfloat  specularCabLight[4]= { 0.4f,  0.4f, 0.2f, 1.0f };


      glLightfv(GL_LIGHT0,GL_AMBIENT,ambientCabLight);
      glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseCabLight);
      glLightfv(GL_LIGHT0,GL_SPECULAR,specularCabLight);
}

void __fastcall prepare2d(int x, int y, float alpha)
{

         glEnable( GL_TEXTURE_2D);
         BFONT->Begin();
         glDisable(GL_TEXTURE_2D);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
         glColor4f(0,0,0,alpha);
         glLoadIdentity();
         glBegin(GL_QUADS);
         glVertex2i(1, y);                                                      // dol lewy
         glVertex2i(Global::iWindowWidth-1, y);                                 //dol prawy
         glVertex2i(Global::iWindowWidth-1, Global::iWindowHeight-1);           // gora
         glVertex2i(1, Global::iWindowHeight-1);                                // gora
         glEnd();

         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);
}

void __fastcall prepare2d()
{
    // RENDEROWANIE PLASKIE (GUI) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    glEnable( GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, GWW, GWH, 0, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
}


int startline;
int __fastcall lineplus(int pix)
{
 startline += pix;
 return startline;
}


bool __fastcall TWorld::renderpanview(float trans, int frameheight, int pans)
{
/*
    Global::iTextMode = 0;

    if (!floaded) BFONT = new Font();
    if (!floaded) BFONT->loadf("none");
    floaded = true;


   // RENDEROWANIE PLASKIE (GUI) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, GWW, GWH, 0, -100, 100);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity( );


    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    int i =1;
    int margin = 1;
    glEnable( GL_TEXTURE_2D);
    BFONT->Begin();

    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,Global::panviewtrans);
    //glBindTexture(GL_TEXTURE_2D, Global::texmenu_backg);
    if (pans > 0)
        {
         // GORNY
         glBegin(GL_QUADS);
         glVertex2i(0,GWH-frameheight);  // dol lewy
         glVertex2i(GWW-0,GWH-frameheight);  //dol prawy
         glVertex2i(GWW-0,GWH); // gora
         glVertex2i(0,GWH);    // gora
         glEnd();
        }
        
    if (pans == 2)
        {
         // DOLNY
         glBegin(GL_QUADS);
         glVertex2i(0,0);         // dol lewy
         glVertex2i(GWW-0,0);      //dol prawy
         glVertex2i(GWW-0,frameheight);
         glVertex2i(0,frameheight);
         glEnd();
        }
    BFONT->End();


    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FOG);
    */
}



bool __fastcall TWorld::rendercompass(float trans, int size, double angle)
{
/*
    // RENDEROWANIE PLASKIE (GUI) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

   glDisable(GL_FOG);
   glColor4f(0.8,0.8,0.8,0.99f);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);   // COBY CZARNE TLO USUNAC
   glEnable(GL_TEXTURE_2D);
   glPushMatrix();
glTranslatef((size/2), (size/2), 0);
glRotatef(-angle,0,0,1);
glTranslatef((-size/2), (-size/2), 0);
   glBindTexture(GL_TEXTURE_2D, Global::texcompass);
   glBegin( GL_QUADS );
   glTexCoord2f(1, 1); glVertex3i(1,  1,0);   // GORNY LEWY
   glTexCoord2f(1, 0); glVertex3i(1,  size,0);  // DOLY LEWY
   glTexCoord2f(0, 0); glVertex3i(size,size,0);  // DOLNY PRAWY
   glTexCoord2f(0, 1); glVertex3i(size,1,0);  // GORNY PRAWY
   glEnd( );
   glPopMatrix();

   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);      // COBY CZARNE TLO USUNAC
   glEnable(GL_TEXTURE_2D);
   glPushMatrix();
   glBindTexture(GL_TEXTURE_2D, Global::texcompassarr);
   glBegin( GL_QUADS );
   glTexCoord2f(1, 1); glVertex3i(1,  1,0);   // GORNY LEWY
   glTexCoord2f(1, 0); glVertex3i(1,  size,0);  // DOLY LEWY
   glTexCoord2f(0, 0); glVertex3i(size,size,0);  // DOLNY PRAWY
   glTexCoord2f(0, 1); glVertex3i(size,1,0);  // GORNY PRAWY
   glEnd( );
   glPopMatrix();
   glDisable(GL_BLEND);

   glEnable(GL_FOG);
   */
}


bool __fastcall TWorld::renderfadeoff(float trans)
{

if (QGlobal::fscreenfade > 0.01)
    {
    if (!floaded) BFONT = new Font();
    if (!floaded) BFONT->init("none");
    floaded = true;

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    int i =1;
    int margin = 1;
    glEnable( GL_TEXTURE_2D);
    BFONT->Begin();

    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,QGlobal::fscreenfade);

    // GORNY
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-1024);  // dol lewy
    glVertex2i(GWW-0,GWH-1024);  //dol prawy
    glVertex2i(GWW-0,GWH); // gora
    glVertex2i(0,GWH);    // gora
    glEnd();

    BFONT->End();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FOG);
    }
    else QGlobal::bFADEOFFP = true;
    if (QGlobal::fscreenfade > 0.01) QGlobal::fscreenfade -= 0.02;

}

/*
bool __fastcall TWorld::renderhitcolor(int r, int g, int b, int a)
{
    if (!floaded) BFONT = new Font();
    if (!floaded) BFONT->loadf("none");
    floaded = true;

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    int i =1;
    int margin = 1;
    glEnable( GL_TEXTURE_2D);
    BFONT->Begin();

    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);

    //glColor4f(Global::selcolor_r,Global::selcolor_g,Global::selcolor_b,Global::selcolor_a);
    glColor4ub(   Global::selcolor_r,Global::selcolor_g,Global::selcolor_b,Global::selcolor_a );
    // GORNY
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-1024);     // dol lewy
    glVertex2i(20-0,GWH-1024);  //dol prawy
    glVertex2i(20-0,GWH);       // gora
    glVertex2i(0,GWH);          // gora
    glEnd();

    BFONT->End();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FOG);
}
*/


bool __fastcall TWorld::RenderFPS()
{
QGlobal::bscrfilter = false;
glDisable( GL_FOG );
glDisable( GL_LIGHTING );
glPolygonMode(GL_FRONT, GL_FILL);
glLineWidth(0.001);

     float trans = 0.65;
     //if (!floaded) BFONT = new Font();
     //if (!floaded) BFONT->loadf("none");
     //floaded = true;

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    BFONT->Begin();
    glEnable(GL_BLEND);      // DO USUNIECIA
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,trans);

    // CIEMNE TLO NAPISOW
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-50);  // dol lewy
    glVertex2i(GWW-0,GWH-50);  //dol prawy
    glVertex2i(GWW-0,GWH); // gora
    glVertex2i(0,GWH);    // gora
    glEnd();
    BFONT->End();

    //glDisable(GL_BLEND);

         //prepare2d(0, 900, 0.9);

         BFONT->Begin();
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
         glColor4f(0,0,0,0.99);
         //glLoadIdentity();
         // TUTAJ TLO
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);

         glColor4f(0.5,0.5,0.5,0.95);
         BFONT->Print_xy_scale(30, (GWH-30), AnsiString("FPS " + FloatToStrF(QGlobal::fps,ffFixed,8,3)).c_str(), 1, 0.9, 0.9);
         //BFONT->Print_xy_scale(30, (GWH-45), AnsiString("THR " + IntToStr(Global::threadcycle)).c_str(), 1, 0.9, 0.9);
         BFONT->End();
         glLineWidth(0.001);

   glEnable(GL_LIGHT0);
   glEnable( GL_LIGHTING );
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RenderInformation() - WYSWIETLANIE ROZNYCH INFORMACJI ^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool __fastcall TWorld::RenderInformation(int type)
{
/*
    float Ambient[] = { 1,1,1,1 };
    float Diffuse[] = { 1,1,1,1 };
    float Specular[] = { 0,0,0,1 };

    glDisable( GL_FOG );
    glDisable( GL_LIGHTING );

//if (TWorld::cph < 10)
//    {
     //if (Global::objectid == 0) Global::objectidinfo = "";       // CO TO BYLO???

     mtype = type;
     int plusy1 = -5;
     int plusy2 = -10;

     if (!floaded) BFONT = new Font();
     if (!floaded) BFONT->loadf("none");
     floaded = true;

     glEnable(GL_TEXTURE_2D);


/*
    if (type == 0) // MENU GLOWNE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         //glEnable(GL_BLEND);
         glColor4f(1,1,1,1);
         //BFONT->Begin();
         //BFONT->Print_xy_scale(30, (Global::iWindowHeight)-45, AnsiString("Symulator Pojazdow Trakcyjnych MASZYNA EU07-424 ").c_str(), 1, 1.7, 1.7);
         //BFONT->Print_scale(95,3,AnsiString("FPS " + FloatToStrF(Global::fps,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->End();
        }

    if (type == 11) // USTAWIENIA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
         glEnable(GL_BLEND);
         glColor4f(0.5,0.5,0.5,0.90);
         glBindTexture(GL_TEXTURE_2D, Global::consolebackg);
         glBegin(GL_QUADS);
         glVertex2i(10, 930);                                           // dol lewy
         glVertex2i(Global::iWindowWidth-10, 930);                      //dol prawy
         glVertex2i(Global::iWindowWidth-10, Global::iWindowHeight-5);  // gora
         glVertex2i(10, Global::iWindowHeight-5);                       // gora
         glEnd();
         glDisable(GL_BLEND);

         glColor4f(1,1,1,0.95);
         BFONT->Begin();
         BFONT->Print_xy_scale(30, (Global::iWindowHeight)-45, AnsiString("USTAWIENIA...").c_str(), 1, 1.7, 1.7);
         BFONT->End();
        }

    if (type == 33) // INFORMACJE O PROGRAMIE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         glEnable(GL_BLEND);
         glColor4f(0.5,0.5,0.5,0.90f);
         glBindTexture(GL_TEXTURE_2D, Global::consolebackg);
         glBegin( GL_QUADS );
         glTexCoord2f(1, 1); glVertex3i(0,    1,    0);     // GORNY LEWY
         glTexCoord2f(1, 0); glVertex3i(0,    1024, 0);     // DOLY LEWY
         glTexCoord2f(0, 0); glVertex3i(1280, 1024, 0);     // DOLNY PRAWY
         glTexCoord2f(0, 1); glVertex3i(1280, 1,    0);     // GORNY PRAWY
         glEnd( );
         glDisable(GL_BLEND);

         glColor4f(1,1,1,0.95);
         BFONT->Begin();
         BFONT->Print_xy_scale(30, (Global::iWindowHeight)-45, AnsiString("O SYMULATORZE...").c_str(), 1, 1.8, 1.8);
         startline = 130;

         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Glowni autorzy symulatora:").c_str(), 0, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Marcin Marcin_EU Wozniak - glowny kod zrodlowy programu,").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Maciej McZapkie Czapkiewicz - fizyka; AI; modele 3D; tekstury; trasy; pomysly").c_str(), 1,0.8,0.8);
lineplus(15);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Pozostali autorzy:").c_str(), 0, 1.1, 1.1);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Olgierd Olo_EU Wiemann - konsultacje, modele 3D taboru, tekstury").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Arkadiusz Winger Slusarczyk - poprawki w kodzie, mirror, forum").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Lukasz nbmx_eu Kirchner - poprawki w kodzie").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Adam ABu Bugiel - programowanie, VD").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Maciej youBy Cierniak - poprawki fizyki hamowania").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Igor q Puchalski - programowanie grafiki, modele taboru, tekstury").c_str(), 1,0.8,0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Ra - programowanie grafiki, poprawki w kodzie").c_str(), 1,0.8,0.8);
lineplus(15);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Osoby ktore znaczaco pomogly przy projekcie:").c_str(), 0, 1.0, 1.0);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Mieczyslaw Mietols Michalski - pomoc merytoryczna, betatesty").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Dawid Dzyszla Najgiebauer - STV, HTV").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Jakub Kakish Krysztofiak - modele 3D").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Mateusz Myszor Bilinski - keymap").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Zbyszek ShaXbee Mandziejewicz - programy pomocnicze").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Jacek Jastrzab Jastrzebski").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Jaroslaw Chester Stawarz").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Przemyslaw Maron").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Adam Wojcieszyk").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Igor Przybylski").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Piotr Smolinski").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Adam Lagoda").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Artur Kopacz").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Mateusz mateu Matusik - trasy").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Grzegorz Hunter Durbajlo - trasy").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Mati_an - trasy").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Michal mKaczy Massel - kabiny taboru").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Jakub Profeta Manka - tabor").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Slawek Sakorius Wieczorek - tabor, trasy").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Bartosz ET22_RULZ  - fizyki pojazdow").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Olaf OLI_EU Schmeling - tabor").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Wlodzimierz EP08_015 - tabor").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Jaroslaw Jaras Krasuski").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Defiler - tekstury taboru").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Juliusz EU05 Wawrzek - tekstury taboru").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Bartosz Bart Matelski - tekstury taboru").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Kudlacz - tabor").c_str(), 1, 0.8, 0.8);
         BFONT->Print_xy_scale(32, (Global::iWindowHeight)-lineplus(15), AnsiString("Przemyslaw Bombardier - COS ").c_str(), 1, 0.8, 0.8);
         BFONT->End();
        }

    if (type == 44) // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         glColor4f(1,1,1,1);
         BFONT->Begin();
         BFONT->Print_xy_scale(30, (Global::iWindowHeight)-45, AnsiString("WYBOR SCENERII...").c_str(), 1, 1.7, 1.7);
         BFONT->End();
        }

    if (type == 66) // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         glDisable(GL_BLEND);
         glColor4f(1.0,1.1,0.2,0.99);
         BFONT->Begin();
         BFONT->Print_xy_scale(10, (Global::iWindowHeight)-45, AnsiString("Zrzut zapisano w " + Global::asSCREENSHOTFILE).c_str(), 1, 0.9, 0.9);
         BFONT->End();
        }


    if (type == 88) // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         glColor4f(1,1,1,0.9);
         BFONT->Begin();
         BFONT->Print_xy_scale(30, (Global::iWindowHeight)-45, AnsiString("Aby zakonczyc program, przycisnij klawisz [Y], [N] - kontynuowanie.").c_str(), 1, 1.1, 1.1);
         BFONT->End();
        }
 */
    if (type == 101) // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         glColor4f(0.4,0.9,0.3,0.95);


         //BFONT->Print_scale(2,2,AnsiString(Global::debuginfo1).c_str(), 1,1.0,1.0);
         //BFONT->Print_scale(2,4,AnsiString(Global::debuginfo2).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,6,AnsiString(Global::debuginfo3).c_str(), 1,0.9,0.9);
         BFONT->Begin();
          BFONT->Print_scale(2,0,AnsiString("1: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,1,AnsiString("2: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,2,AnsiString("3: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,3,AnsiString("4: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,4,AnsiString("5: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,5,AnsiString("6: ").c_str(), 1,0.9,0.9);
         BFONT->Print_xy_scale(10, (Global::iWindowHeight)-125, AnsiString("Zrzut zapisano w ").c_str() , 1, 0.9, 0.9);

         //--BFONT->Print_xy_scale(30, (GWH-30), AnsiString(Global::debuginfo1).c_str(), 1, 0.9, 0.9);
         //--BFONT->Print_xy_scale(30, (GWH-50), AnsiString(Global::debuginfo2).c_str(), 1, 0.9, 0.9);
         //--BFONT->Print_xy_scale(30, (GWH-70), AnsiString(Global::debuginfo3).c_str(), 1, 0.9, 0.9);
         BFONT->End();
        }


    if (type == 99) // LOADING ... ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
         int l =0;
         int g =0;

         glEnable( GL_TEXTURE_2D);
         BFONT->Begin();
         glDisable(GL_TEXTURE_2D);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
         glColor4f(LDR_TBACK_R, LDR_TBACK_G, LDR_TBACK_B, LDR_TBACK_A);
       //  glLoadIdentity();

       // TLO PROGRESSBARA I INFORMACJI O WCZYTYWANIU
         glBegin(GL_QUADS);
         glVertex2i(10, 50);                          // dol lewy
         glVertex2i(Global::iWindowWidth-10, 50);     //dol prawy
         glVertex2i(Global::iWindowWidth-10, 200-8);  // gora
         glVertex2i(10, 200-8);                       // gora
         glEnd();

         PBY = Global::iWindowHeight - (200-8);


         AnsiString x, scn;
         if (g==0) x= AnsiString(Global::szSceneryFile);
         if (g==0) l = x.Length();
         //scn = x.SubString(5,l-7)
         if (g==0) scn = x.SubString(1,l-4);
         g=1;
         if (scn == "$") tmp = true;
         if (scn == "$") scn= "...z pliku tymczasowego $";

         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);
         glColor4f(LDR_STR_1_R, LDR_STR_1_G, LDR_STR_1_B, LDR_STR_1_A);
         if (!QGlobal::bSCNLOADED) BFONT->Print_scale( 2,53,AnsiString(LDR_STR_LOAD).c_str(), 1, 1.3, 1.3);
         if (!QGlobal::bSCNLOADED) BFONT->Print_scale( 1,0,AnsiString("Wcisnij ESC aby przerwac").c_str(), 1, 0.6, 0.6);
         if ( QGlobal::bSCNLOADED) BFONT->Print_scale( 2,54,AnsiString("Wcisnij spacje aby rozpoczac...").c_str(), 1, 1.7, 1.7);

         BFONT->Print_scale( 2,55, AnsiString(IntToStr(QGlobal::iNODES) + ", " + currloading_b).c_str(), 1, 0.7, 0.7);     // current element
         if (!QGlobal::bfirstloadingscn) BFONT->Print_scale( 2,56,AnsiString(currloading).c_str(), 1, 0.7, 0.7);
         if (!QGlobal::bSCNLOADED) BFONT->Print_scale(40,59,AnsiString(AnsiString(scn)).c_str(), 1, 2.0, 2.0);      // NAZWA SCENERII
if (tmp) if (!QGlobal::bSCNLOADED) BFONT->Print_scale(40,60,AnsiString("$.scn - plik roboczy generowany przez Rainsted").c_str(), 1, 0.8, 0.8);

         glColor4f(0.5, 0.5, 0.5, 0.7);  // 09 07 02
         BFONT->Print_scale(75,63,AnsiString(AnsiString(QGlobal::asAPPVERS)).c_str(), 1, 0.7, 0.7);       // WERSJA APLIKACJI, DATA KOMPILACJI
         glColor4f(0.5, 0.5, 0.5, 0.7);
         BFONT->Print_scale(1,63,AnsiString(AnsiString(FormatFloat("0.000", (QGlobal::lsec/1000)))).c_str(), 1, 0.7, 0.7);

         // glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE);
       //   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //  glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);
        //glDisable(GL_BLEND);
        // glColor4f(0.2, 0.2, 0.9, 1.0);  // 09 07 02
        // BFONT->Print_scale(10,12,AnsiString(AnsiString("ssssssssssssssaaaaaaaa1234567890ABCDE")).c_str(), 1, 0.9, 0.9);       // WERSJA APLIKACJI, DATA KOMPILACJI
        //  BFONT->Print_scale( 10,14,AnsiString("Wcisnij ESC aby przerwac").c_str(), 1, 0.9, 0.9);
        // glEnable(GL_BLEND);
         //BFONT->End();

         if (QGlobal::bfirstloadingscn) BFONT->Print_scale( 2, 56, AnsiString(LDR_STR_FRST).c_str(), 1, 0.7, 0.7);
         BFONT->End();


         // Q: TEKST OPISU MISJI
         if (QGlobal::bloaderbriefing)
         {
         glColor4f(0.2, 0.2, 0.2, 0.9);
         int py = 200;

         freetype::print(our_font14, 55, Global::iWindowHeight-200, QGlobal::MBRIEF->Strings[0].c_str());   // Nazwa skladu ...
         py+=24;
         freetype::print(our_font12, 55, Global::iWindowHeight-py, QGlobal::MBRIEF->Strings[1].c_str());    // Prowadzimy ...
         py+=20;

         for (int l = 2; l<QGlobal::MBRIEF->Count -1; l++)
         {
          py+=14;
          freetype::print(our_font10, 55, Global::iWindowHeight-py, QGlobal::MBRIEF->Strings[l].c_str());
          }
         } 
        }

 /*
    if (type == 1) // FPS, PRZELACZNIKI ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
         prepare2d(0, 920, 0.4);
         glColor4f(0.9,0.9,0.9,0.8);
         i++;
           BFONT->Print_scale( 2,0,AnsiString("1: ").c_str(), 1,0.9,0.9);
           BFONT->Print_scale(97,2,AnsiString("FPS " + FloatToStrF(Global::fps,ffFixed,8,3)).c_str(), 1,1.4,1.4);
         //BFONT->Print_scale(85,4,AnsiString("TRI " + FloatToStrF(Global::crt,ffFixed,8,3)).c_str(), 1,0.9,0.9);
           BFONT->Print_scale(89,4,AnsiString("VER " + Global::asAPPDATE + " " + Global::asAPPSIZE).c_str(), 1,0.9,0.9);
         //--BFONT->Print_scale(89,5,AnsiString("LTI " + FormatFloat("0.000", (Global::lsec) / 1000) + "s").c_str(), 1,0.9,0.9);
         //BFONT->Print_scale( 2,5,AnsiString("TURNTABLE SLOT: " + IntToStr(Global::_turntable[0].currentslot)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale( 2,6,AnsiString("TURNTABLE DEGR: " + FloatToStr(Global::_turntable[0].rotatedeg)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(95,2,AnsiString("UPD " + FloatToStrF(Global::upd,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(95,3,AnsiString("REN " + FloatToStrF(Global::ren,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("OBJECTID: "  + IntToStr(Global::objectid) + " - " + Global::objectidinfo).c_str(), 0, 1.1, 1.1);
         //BFONT->Print_scale(2,i++,AnsiString("C2D " + IntToStr(Global::C2D)).c_str(), 1,0.9,0.9);

           BFONT->Print_scale(2,3,AnsiString("gtime " + Global::asGTIME).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,5,AnsiString("objid " + AnsiString(Global::objectid)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,7,AnsiString("objin " + Global::objectidinfo).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2, 9,AnsiString("nodeN " + Global::asnearestobj ).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,10,AnsiString("nodeM " + Global::asnearestinc ).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,11,AnsiString("nodeF " + Global::MODELFILE ).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,12,AnsiString("nodeT " + Global::MODELTRIS ).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,13,AnsiString("nodeS " + Global::MODELSUBS ).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,14,AnsiString("total " + IntToStr(Global::renderedtris) ).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2,4,AnsiString("SUN X " + FloatToStrF(Global::lightPos[0],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,5,AnsiString("SUN Y " + FloatToStrF(Global::lightPos[1],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,6,AnsiString("SUN Z " + FloatToStrF(Global::lightPos[2],ffFixed,8,3)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2, 8,AnsiString("SUN A " + FloatToStrF(Global::ambientDayLight[0],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2, 9,AnsiString("SUN A " + FloatToStrF(Global::ambientDayLight[1],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,10,AnsiString("SUN A " + FloatToStrF(Global::ambientDayLight[2],ffFixed,8,3)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2,12,AnsiString("SUN D " + FloatToStrF(Global::diffuseDayLight[0],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,13,AnsiString("SUN D " + FloatToStrF(Global::diffuseDayLight[1],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,14,AnsiString("SUN D " + FloatToStrF(Global::diffuseDayLight[2],ffFixed,8,3)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2,16,AnsiString("SUN S " + FloatToStrF(Global::specularDayLight[0],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,17,AnsiString("SUN S " + FloatToStrF(Global::specularDayLight[1],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,18,AnsiString("SUN S " + FloatToStrF(Global::specularDayLight[2],ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,8,AnsiString("currV " + Global::currspeed).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,9,AnsiString("delta " + FloatToStrF(Global::deltatime,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,10,AnsiString("vobrm " + FloatToStrF(Controlled->SM42_engrpm_cur1,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,14,AnsiString("infof " + Global::infof).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,15,AnsiString("infog " + Global::infog).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,16,AnsiString("infoh " + Global::infoh).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("ltime " + AnsiString(ltime)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("                          ").c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("cwinds " + AnsiString(windspeed)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("dwinds " + AnsiString(desirewindspeed)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("cwinda " + AnsiString(windangle)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("                          ").c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("gfog_s " + AnsiString(Global::fFogStart)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("gfog_e " + AnsiString(Global::fFogEnd)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("gfog_d " + AnsiString(Global::fFogDensity)).c_str(), 1,0.9,0.9);
         BFONT->End();
        }
      */
    if (type == 2) // INFORMACJE O LOKU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
         TMoverParameters *mvparam =Controlled->ControlledFind()->MoverParameters;;

         mvControlled=Controlled->ControlledFind()->MoverParameters;
         glDisable(GL_LIGHTING);
         BFONT->Begin();
         //setfontcolor();
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);
         glColor4f(0.9, 0.8, 0.4, 0.6);
         BFONT->Print_scale(2,0,AnsiString("2: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("INFORMACJE O LOKOMOTYWIE").c_str(), 0, 1.1, 1.1);
         i++;
         if (mvControlled != NULL) {
         //BFONT->Print_scale(2,i++,AnsiString("> vechicle type   " + AnsiString(mvControlled->MoverParameters->VechicleType)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> name            " + AnsiString(mvparam->Name)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> file            " + AnsiString(Controlled->MoverParameters->filename)).c_str(), 1, 0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> type            " + AnsiString(Controlled->MoverParameters->TypeName)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> train           " + AnsiString(Controlled->MoverParameters->TrainType)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> compressor r    " + FloatToStr(Controlled->MoverParameters->CompressorSpeed)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Vnax            " + FloatToStr(Controlled->MoverParameters->Vmax)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> mass            " + FloatToStr(Controlled->MoverParameters->Mass)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> mass            " + FloatToStr(Controlled->MoverParameters->TotalMass)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> pipe press      " + FloatToStrF(Controlled->MoverParameters->PipePress,  ffFixed,8,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> brake press     " + FloatToStrF(Controlled->MoverParameters->BrakePress,  ffFixed,8,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> CompressedVol   " + FloatToStrF(Controlled->MoverParameters->CompressedVolume,  ffFixed,8,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> velocity km/h   " + FloatToStrF(Controlled->MoverParameters->Vel,  ffFixed,8,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> velocity m/s    " + FloatToStrF(Controlled->MoverParameters->Vel*1000/3600,  ffFixed,8,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> first dynamic   " + AnsiString(Controlled->GetFirstDynamic(1)->MoverParameters->Name)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> last dynamic    " + AnsiString(Controlled->GetLastDynamic(1)->MoverParameters->Name)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> main ctrl pos   " + IntToStr(Controlled->MoverParameters->MainCtrlPos)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> scnd ctrl pos   " + IntToStr(Controlled->MoverParameters->ScndCtrlPos)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> train brake pos " + FloatToStrF(Controlled->MoverParameters->BrakeCtrlPos,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> local brake pos " + FloatToStrF(Controlled->MoverParameters->LocalBrakePos,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);

         BFONT->Print_scale(2,i++,AnsiString("> pantograf A     " + FloatToStrF(Controlled->MoverParameters->PantFrontUp,  ffFixed,8,1)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> pantograf B     " + FloatToStrF(Controlled->MoverParameters->PantRearUp,  ffFixed,8,1)).c_str(), 1,0.7, 0.7);
         i++;
         BFONT->Print_scale(2,i++,AnsiString("> Dl              " + FloatToStrF(Controlled->MoverParameters->dL,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> Fb              " + FloatToStrF(Controlled->MoverParameters->Fb,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> Ff              " + FloatToStrF(Controlled->MoverParameters->Ff,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> FTrain          " + FloatToStrF(Controlled->MoverParameters->FTrain,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> FStand          " + FloatToStrF(Controlled->MoverParameters->FStand,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> FTotal          " + FloatToStrF(Controlled->MoverParameters->FTotal,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Sand            " + FloatToStrF(Controlled->MoverParameters->Sand,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> COMP VOLUME     " + FloatToStrF(Controlled->MoverParameters->CompressedVolume,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> PANT VOLUME     " + FloatToStrF(Controlled->MoverParameters->PantVolume,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> enrot           " + FloatToStrF(Controlled->MoverParameters->enrot,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         i++;
         BFONT->Print_scale(2,i++,AnsiString("> Im              " + FloatToStrF(Controlled->MoverParameters->Im,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Itot            " + FloatToStrF(Controlled->MoverParameters->Itot,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Mm              " + FloatToStrF(Controlled->MoverParameters->Mm,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Mw              " + FloatToStrF(Controlled->MoverParameters->Mw,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Fw              " + FloatToStrF(Controlled->MoverParameters->Fw,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Ft              " + FloatToStrF(Controlled->MoverParameters->Ft,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Imin            " + FloatToStrF(Controlled->MoverParameters->Imin,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Imax            " + FloatToStrF(Controlled->MoverParameters->Imax,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Voltage         " + FloatToStrF(Controlled->MoverParameters->Voltage,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Rventrot        " + FloatToStrF(Controlled->MoverParameters->RventRot,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Rventmax        " + FloatToStrF(Controlled->MoverParameters->RVentnmax,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Rventcut        " + FloatToStrF(Controlled->MoverParameters->RVentCutOff,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> MaxBrakeForce   " + FloatToStrF(Controlled->MoverParameters->MaxBrakeForce,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> MaxBrakePress   " + FloatToStrF(Controlled->MoverParameters->MaxBrakePress,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> HighPipePress   " + FloatToStrF(Controlled->MoverParameters->HighPipePress,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> LowPipePress    " + FloatToStrF(Controlled->MoverParameters->LowPipePress,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
       //BFONT->Print_scale(2,i++,AnsiString("> DeltaPipePress  " + FloatToStrF(Controlled->MoverParameters->DeltaPipePress,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> BrakeDelayFlag  " + FloatToStrF(Controlled->MoverParameters->BrakeDelayFlag,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> Imax            " + FloatToStrF(Controlled->MoverParameters->Imax,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         i++;
         //BFONT->Print_scale(2,i++,AnsiString("> dDoorMoveLA     " + FloatToStrF(Controlled->dDoorMoveLA,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> dDoorMoveLB     " + FloatToStrF(Controlled->dDoorMoveLB,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> wycieraczki     " + FloatToStrF(Controlled->wycieraczki_tryb,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> wycieraczki zak " + FloatToStrF(Controlled->wycieraczki_zakres,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         i++;
//--         BFONT->Print_scale(2,i++,AnsiString("> track name      " + AnsiString(Controlled->MyTrack->asName)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> track vel       " + FloatToStrF(Controlled->MyTrack->fVelocity,  ffFixed,5,0)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> active cab      " + IntToStr(Controlled->MoverParameters->ActiveCab)).c_str(), 1,0.7, 0.7);
//--         BFONT->Print_scale(2,i++,AnsiString("> head lights L   " + IntToStr(Controlled->headlA_L)).c_str(), 1,0.7, 0.7);
//--         BFONT->Print_scale(2,i++,AnsiString("> head lights U   " + IntToStr(Controlled->headlA_U)).c_str(), 1,0.7, 0.7);
//--         BFONT->Print_scale(2,i++,AnsiString("> head lights R   " + IntToStr(Controlled->headlA_R)).c_str(), 1,0.7, 0.7);
//--         BFONT->Print_scale(2,i++,AnsiString("> head lights T   " + IntToStr(Global::iHLBLENDTYPE)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> texture         " + AnsiString(Controlled->texturefile)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> engine type     " + IntToStr(Controlled->MoverParameters->EngineType)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> engine pwr src  " + IntToStr(Controlled->MoverParameters->EnginePowerSource)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("                  ").c_str() , 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> radio power     " + FloatToStrF(Controlled->radiopower,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> radio volume    " + FloatToStrF(Controlled->radiovolume,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> radio chanid    " + IntToStr(Controlled->RADIOCHANID)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> radio freque    " + Controlled->RADIOCHANFR).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> music power     " + FloatToStrF(Controlled->musicpower,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> music volume    " + FloatToStrF(Controlled->musicvolume,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);

         //BFONT->Print_scale(2,i++,AnsiString("> SENSOR F POS X  " + FloatToStrF(Controlled->pSENSOR_F_POS.x,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> SENSOR F POS Y  " + FloatToStrF(Controlled->pSENSOR_F_POS.y,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> SENSOR F POS Z  " + FloatToStrF(Controlled->pSENSOR_F_POS.z,  ffFixed,8,3)).c_str(), 1,0.7, 0.7);
         //i++;
         //BFONT->Print_scale(2,i++,AnsiString("> TOTAL LOCO DIST " + Global::currprzebieg).c_str(), 1,0.7, 0.7);
         i++;
         vector3 pos= Controlled->GetPosition();

         BFONT->Print_scale(2,i++,AnsiString("> POS X     " + FloatToStrF(pos.x,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> POS Y     " + FloatToStrF(pos.y,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> POS Z     " + FloatToStrF(pos.z,  ffFixed,5,2)).c_str(), 1,0.7, 0.7);

         i++;
         BFONT->Print_scale(2,i++,AnsiString("> SEL R     " + FloatToStrF(QGlobal::selcolor[0],  ffFixed,5,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> SEL G     " + FloatToStrF(QGlobal::selcolor[1],  ffFixed,5,3)).c_str(), 1,0.7, 0.7);
         BFONT->Print_scale(2,i++,AnsiString("> SEL B     " + FloatToStrF(QGlobal::selcolor[2],  ffFixed,5,3)).c_str(), 1,0.7, 0.7);
         i++;
         //BFONT->Print_scale(2,i++,AnsiString("> MOUSX     " + IntToStr(Global::iMPX)).c_str(), 1,0.7, 0.7);
         //BFONT->Print_scale(2,i++,AnsiString("> MOUSY     " + IntToStr(Global::iMPY)).c_str(), 1,0.7, 0.7);
         double averS;

       //  averS = Controlled->MoverParameters->DistCounter / (Global::timepassed/60);

      //   BFONT->Print_scale(2,i++,AnsiString("> distance    " + FloatToStrF(Controlled->MoverParameters->DistCounter,  ffFixed,8,3)).c_str(), 1,0.9,0.9);
      //   BFONT->Print_scale(2,i++,AnsiString("> distance D  " + FloatToStrF(Controlled->MoverParameters->DistCounterD,  ffFixed,8,3)).c_str(), 1,0.9,0.9);
      //   BFONT->Print_scale(2,i++,AnsiString("> drive time  " + FloatToStrF(Controlled->MoverParameters->DriveTime,  ffFixed,8,3)).c_str(), 1,0.9,0.9);
      //   BFONT->Print_scale(2,i++,AnsiString("> STIME       " + FloatToStrF(Global::timepassed / 60,  ffFixed,5,2)).c_str(), 1,0.9,0.9);
      //   BFONT->Print_scale(2,i++,AnsiString("> AVERS       " + FloatToStrF(averS*60,  ffFixed,5,2)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2,i++,AnsiString("> R   " + AnsiString(Global::selcolor_r)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> G   " + AnsiString(Global::selcolor_g)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> B   " + AnsiString(Global::selcolor_b)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> A   " + AnsiString(Global::selcolor_a)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("                  ").c_str() , 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> mirror AR      " + FloatToStr(Controlled->mirror_ar_rot)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> mirror AL      " + FloatToStr(Controlled->mirror_al_rot)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2,i++,AnsiString("> vechicle rot Y  " + FloatToStrF(Controlled->dyn_rot_y,  ffFixed,5,3)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(2,i++,AnsiString("                  ").c_str() , 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("26 lightpos x     " + FloatToStr(LTS.sltxp )).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("27 lightpos y     " + FloatToStr(LTS.sltyp )).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("28 lightpos z     " + FloatToStr(LTS.sltzp )).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("29 lightdir x     " + FloatToStr(LTS.sltsdxp )).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("30 lightdir y     " + FloatToStr(LTS.sltsdyp )).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("31 lightdir z     " + FloatToStr(LTS.sltsdzp )).c_str(), 1,0.9,0.9);
      //   }
         BFONT->End();
        }
/*

    if (type == 3) // INFORMACJE O NAJBLIZSZYM POJEZDZIE ^^^^^^^^^^^^^^^^^^^^^^^
        {
        /*
         TDynamicObject *DYNOBJ;
         TGroundNode *tmp;
         vector3 dpos;
         tmp= Ground.FindDynamic(Global::asnearestdyn);

         int i =1;
         glDisable( GL_LIGHTING );
         prepare2d(0, 20, 0.4);
         glColor4f(0.9,0.9,0.9,0.8);
         BFONT->Print_scale(2,0,AnsiString("3: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("INFORMACJE O NAJBLIZSZYM POJEZDZIE").c_str(), 0, 1.1, 1.1);
         i++;
         BFONT->Print_scale(2,i++,AnsiString("> nearest         " + AnsiString(Global::asnearestdyn)).c_str(), 1,0.9,0.9);
         if (tmp)
             {
              DYNOBJ= tmp->DynamicObject;
              dpos = DYNOBJ->GetPosition();


         if (Global::bnearestengaged == true)
         {
         //BFONT->Print_scale(2,i++,AnsiString("> vechicle type   " + AnsiString(DYNOBJ->MoverParameters->VechicleType)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> name            " + AnsiString(DYNOBJ->MoverParameters->Name)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> file            " + AnsiString(DYNOBJ->MoverParameters->filename)).c_str(), 1, 0.9, 0.9);
         BFONT->Print_scale(2,i++,AnsiString("> type            " + AnsiString(DYNOBJ->MoverParameters->TypeName)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> train           " + AnsiString(DYNOBJ->MoverParameters->TrainType)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> compressor r    " + FloatToStr(DYNOBJ->MoverParameters->CompressorSpeed)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> Vnax            " + FloatToStr(DYNOBJ->MoverParameters->Vmax)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> mass            " + FloatToStr(DYNOBJ->MoverParameters->Mass)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> total mass      " + FloatToStr(DYNOBJ->MoverParameters->TotalMass)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> pipe press      " + FloatToStr(DYNOBJ->MoverParameters->PipePress)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> brake press     " + FloatToStr(DYNOBJ->MoverParameters->BrakePress)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> CompressedVol   " + FloatToStr(DYNOBJ->MoverParameters->CompressedVolume)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> velocity        " + FloatToStr(DYNOBJ->MoverParameters->Vel)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> first dynamic   " + AnsiString(DYNOBJ->GetFirstDynamic(1)->MoverParameters->Name)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> last dynamic    " + AnsiString(DYNOBJ->GetLastDynamic(1)->MoverParameters->Name)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> main ctrl pos   " + IntToStr(DYNOBJ->MoverParameters->MainCtrlPos)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> scnd ctrl pos   " + IntToStr(DYNOBJ->MoverParameters->ScndCtrlPos)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("> track name      " + AnsiString(DYNOBJ->MyTrack->aMaxTrackName)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> pantograf A     " + FloatToStrF(DYNOBJ->MoverParameters->PantFrontUp,ffFixed,5,0)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> pantograf B     " + FloatToStrF(DYNOBJ->MoverParameters->PantRearUp,ffFixed,5,0)).c_str(), 1,0.9,0.9);
         i++;
         BFONT->Print_scale(2,i++,AnsiString("> distance        " + FloatToStrF(DYNOBJ->MoverParameters->DistCounter,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         i++;
         //BFONT->Print_scale(2,i++,AnsiString("> texture         " + AnsiString(DYNOBJ->texturefile)).c_str(), 1,0.9,0.9);
         i++;
         BFONT->Print_scale(2,i++,AnsiString("> len to null N   " + AnsiString(Global::debuginfo1)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("> len to null P   " + AnsiString(Global::debuginfo2)).c_str(), 1,0.9,0.9);
       //BFONT->Print_scale(2,i++,AnsiString("> engine type     " + IntToStr(Controlled->MoverParameters->EngineType)).c_str(), 1,0.9,0.9);

       //BFONT->Print_scale(2,i++,AnsiString("> engine pwr src  " + IntToStr(Controlled->MoverParameters->EnginePowerSource)).c_str(), 1,0.9,0.9);
       //BFONT->Print_scale(2,i++,AnsiString("> distance D      " + FloatToStrF(DYNOBJ->MoverParameters->DistCounterD,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         i++;
       //BFONT->Print_scale(2,i++,AnsiString("> position x      " + FloatToStr(dpos.x)).c_str(), 1,0.9,0.9);
       //BFONT->Print_scale(2,i++,AnsiString("> position y      " + FloatToStr(dpos.y)).c_str(), 1,0.9,0.9);
       //BFONT->Print_scale(2,i++,AnsiString("> position z      " + FloatToStr(dpos.z)).c_str(), 1,0.9,0.9);
       //BFONT->Print_scale(2,i++,AnsiString("> modelrot y      " + FloatToStrF(DYNOBJ->dyn_rot_y,  ffFixed,5,3)).c_str(), 1,0.9,0.9);
         }
         }
         BFONT->End();
         */
         }
/*
    if (type == 4) // LISTA BRAKUJACYCH MODELI ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         float i =0.7;
         prepare2d(0 ,20, 0.4);
         //setfontcolor();
         glColor4f(0.9,0.9,0.9,0.8);
         BFONT->Print_scale(2,0,AnsiString("4: ").c_str(), 1,0.9,0.9);
         i++;
         BFONT->Print_scale(2,i++,AnsiString("LISTA BRAKUJACYCH MODELI").c_str(), 0, 1.0, 1.0);
         i++;
         i++;
         BFONT->Print_scale(2,60,AnsiString("Lista zostala zapisana w DATA\\LOGS\\").c_str(), 0, 0.9, 0.9);
         BFONT->Print_scale(2,62,AnsiString("TAB - PRZELACZANIE POMIEDZY TYPEM WYSWIETLANYCH INFORMACJI, WCISNIJ, ABY ZAMKNAC LISTE").c_str(), 0, 1.1, 1.1);

         //for (int lc=0; lc<Form3->Memo7->Lines->Count; lc++)
         //     {
         //      BFONT->Print_scale(2,i++,AnsiString(Form3->Memo7->Lines->Strings[lc]).c_str(), 1,0.9,0.9);
         //     }
         BFONT->End();
        }
 */
    if (type == 5) // INFORMACJE O NAJBLIZSZYM OBIEKCIE ^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
         glDisable( GL_LIGHTING );
       //prepare2d(0, 20, 0.4);
         BFONT->Begin();
         glColor4f(0.9,0.9,0.9,0.8);
         BFONT->Print_scale(2,0,AnsiString("5: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("INFORMACJE O OBIEKCIE: ").c_str(), 0, 1.1, 1.1);
         i++;
         i++;
       //BFONT->Print_scale(2,i++,AnsiString("CAMERA NUM     " + AnsiString(Global::holdCamera)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA POS X   " + FloatToStrF(Camera.Pos.x, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA POS Y   " + FloatToStrF(Camera.Pos.y, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA POS Z   " + FloatToStrF(Camera.Pos.z, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA LAT X   " + FloatToStrF(Camera.LookAt.x, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA LAT Y   " + FloatToStrF(Camera.LookAt.y, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA LAT Z   " + FloatToStrF(Camera.LookAt.z, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("CAMERA YAW X   " + FloatToStrF(Camera.Yaw*180.0f/M_PI, ffFixed, 8, 3)).c_str(), 1,0.9,0.9);
         i++;
       //BFONT->Print_scale(2,i++,AnsiString("HMPOST NUM:    " + AnsiString(Global::hmposts)).c_str(), 1,0.9,0.9);
       //BFONT->Print_scale(2,i++,AnsiString("HMPOST CHM     " + FloatToStrF(Global::hmpost_ckm, ffFixed, 8, 1)).c_str(), 1,0.9,0.9);

       /*
         if (Global::bnearestobjengaged == true)
         {
         i++;
         //BFONT->Print_scale(2,i++,AnsiString("01 file       " + AnsiString(Global::asNI_file)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("02 nodename   " + AnsiString(Global::asNI_name)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("03 submodels  " + AnsiString(Global::iNI_submodels)).c_str(), 1, 0.9, 0.9);
         //BFONT->Print_scale(2,i++,AnsiString("04 numtri     " + AnsiString(Global::iNI_numtri)).c_str(), 1, 0.9, 0.9);
         //BFONT->Print_scale(2,i++,AnsiString("05 numverts   " + AnsiString(Global::iNI_numverts)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("06 state      " + AnsiString(Global::iNI_state)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("07 type       " + AnsiString(Global::aNI_type)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("08 texture    " + AnsiString(Global::iNI_textureid)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("09 angle      " + AnsiString(Global::fNI_angle)).c_str(), 1,0.9,0.9);
         i++;
         //BFONT->Print_scale(2,i++,AnsiString("10 position x " + FloatToStr(Global::fNI_posx)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("11 position y " + FloatToStr(Global::fNI_posy)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("12 position z " + FloatToStr(Global::fNI_posz)).c_str(), 1,0.9,0.9);
         }
         */

         BFONT->End();
        }

        /*
    if (type == 6) // INFORMACJE O SKLADZIE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
         glDisable( GL_LIGHTING );
         prepare2d(0, 20, 0.4);
         //setfontcolor();
         glColor4f(0.9,0.9,0.9,0.8);
         BFONT->Print_scale(2,0,AnsiString("6: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("INFORMACJE O SKLADZIE POCIAGU: ").c_str(), 0, 1.1, 1.1);
         i++;
         i++;
         BFONT->Print_scale(2,i++,AnsiString("wagonow       " + AnsiString(Global::consistcars)).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("dlugosc       " + FloatToStrF(Global::consistlen, ffFixed, 8, 3) + AnsiString("m")).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("masa          " + FloatToStrF(Global::consistmass, ffFixed, 8, 3) + AnsiString("t")).c_str(), 1,0.9,0.9);

         i++;
         for (int lc=0; lc<Global::CONSISTA->Count; lc++)
              {
               BFONT->Print_scale(2,i++,AnsiString(Global::CONSISTA->Strings[lc]).c_str(), 1,0.9,0.9);
              }
         BFONT->End();
        }
          */


    if (type == 7) // KLAWISZOLOGIA POJAZDU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {

         int i =1;
         //prepare2d(0, 20, 0.4);

         glColor4f(0.9,0.9,0.9,0.95);
         BFONT->Print_scale(2,0,AnsiString("7: ").c_str(), 1,0.9,0.9);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);

         BFONT->Begin();
         BFONT->Print_scale(2,i++,AnsiString("OBSLUGA POJAZDU").c_str(), 0, 1.1, 1.1);


         i++;
         int py = 50;
         for (int lc=0; lc<QGlobal::LOKTUT->Count; lc++)
              {
             //  BFONT->Print_scale(2,i++,AnsiString(Global::LOKTUT->Strings[lc]).c_str(), 1,0.7,0.7);

               BFONT->Print_xy_scale(20, (1024-py), AnsiString(QGlobal::LOKTUT->Strings[lc]).c_str(), 1, 0.8, 0.8);
               py = py +15;
              }
         BFONT->End();
        }

    if (type == 8) // KLAWISZOLOGIA POJAZDU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
        // prepare2d(0, 20, 0.4);

         glColor4f(0.9,0.9,0.9,0.95);
         BFONT->Print_scale(2,0,AnsiString("8: ").c_str(), 1,0.9,0.9);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);

         BFONT->Begin();
         BFONT->Print_scale(2,i++,AnsiString("KLAWISZOLOGIA POJAZDU").c_str(), 0, 1.1, 1.1);


         i++;
         int py = 50;
         for (int lc=0; lc<QGlobal::LOKKBD->Count; lc++)
              {
               BFONT->Print_xy_scale(20, (1024-py), AnsiString(QGlobal::LOKKBD->Strings[lc]).c_str(), 1, 0.8, 0.8);
               py = py +15;
              }
         BFONT->End();
        }

   if (type == 100) // POTWIERDZENIE WYJSCIA Z PROGRAMU ^^^^^^^^^^^^^^^^^^^^^^^^^
        {

         int i =1;
         glDisable( GL_LIGHTING );
         prepare2d(0, 20, 0.4);
         glColor4f(0.9,0.9,0.9,0.95);
         //BFONT->Print_scale(2,0,AnsiString("7: ").c_str(), 1,0.9,0.9);
         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
         glEnable(GL_TEXTURE_2D);
         BFONT->Print_scale(2,i++,AnsiString("ZAMYKANIE PROGRAMU - POTWIERDZNIE ").c_str(), 0, 1.1, 1.1);

         BFONT->End();
        }


/*
    if (type == 8) // FPS< VERSJA, CZAS WCZYTYWANIA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         int i =1;
         glDisable( GL_LIGHTING );
         prepare2d(0, 920, 0.4);
         glColor4f(0.9,0.9,0.9,0.95);
           BFONT->Print_scale( 2,0,AnsiString("8: ").c_str(), 1,0.9,0.9);
           BFONT->Print_scale(97,2,AnsiString("FPS " + FloatToStrF(Global::fps,ffFixed,8,3)).c_str(), 1,1.4,1.4);
         //BFONT->Print_scale(85,4,AnsiString("TRI " + FloatToStrF(Global::crt,ffFixed,8,3)).c_str(), 1,0.9,0.9);
           BFONT->Print_scale(89,4,AnsiString("VER " + Global::asAPPDATE + " " + Global::asAPPSIZE).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(89,5,AnsiString("LTI " + FormatFloat("0.000", (Global::lsec) / 1000) + "s").c_str(), 1,0.9,0.9);
         //BFONT->Print_scale( 2,5,AnsiString("TURNTABLE SLOT: " + IntToStr(Global::_turntable[0].currentslot)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale( 2,6,AnsiString("TURNTABLE DEGR: " + FloatToStr(Global::_turntable[0].rotatedeg)).c_str(), 1,0.9,0.9);

         //BFONT->Print_scale(95,2,AnsiString("UPD " + FloatToStrF(Global::upd,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(95,3,AnsiString("REN " + FloatToStrF(Global::ren,ffFixed,8,3)).c_str(), 1,0.9,0.9);
         //BFONT->Print_scale(2,i++,AnsiString("OBJECTID: "  + IntToStr(Global::objectid) + " - " + Global::objectidinfo).c_str(), 0, 1.1, 1.1);
         //BFONT->Print_scale(2,i++,AnsiString("C2D " + IntToStr(Global::C2D)).c_str(), 1,0.9,0.9);

         BFONT->Print_scale(2,3,AnsiString("gtime " + Global::asGTIME).c_str(), 1,0.9,0.9);
         BFONT->End();
        }

    if (type == 9) // INFORMACJE O SKLADZIE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        {
         AnsiString MULTIP;
         if ( Global::iMultiplayer) MULTIP = "1";
         if (!Global::iMultiplayer) MULTIP = "0";
         int i =1;
         glDisable( GL_LIGHTING );
         prepare2d(0, 20, 0.4);
         //setfontcolor();
         glColor4f(0.9,0.9,0.9,0.8);
         BFONT->Print_scale(2,0,AnsiString("9: ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("INNE: ").c_str(), 0, 1.1, 1.1);
         i++;
         i++;
         BFONT->Print_scale(2,i++,AnsiString("version:        " + AnsiString("ROOT RELEASE ") + AnsiString(Global::asVersion) + " +Q ").c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("current:        " + AnsiString("   Q RELEASE Compilation ") + Global::asAPPDAT2 + ", release " + Global::asAPPCOMP ).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("Rendering mode: " + AnsiString(Global::bUseVBO?"VBO":"Display Lists") ).c_str(), 1,0.9,0.9);
         BFONT->Print_scale(2,i++,AnsiString("MULTIPLAYER:    " + MULTIP ).c_str(), 1,0.9,0.9);
         /*
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P01[s]) + "," ;
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 01:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P02[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 02:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P03[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 03:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P04[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 04:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P05[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 05:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P06[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 06:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P07[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 07:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P08[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 08:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P09[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 09:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P10[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 10:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P11[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 11:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P12[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 12:    " + state ).c_str(), 1,0.9,0.9);
         i++;
         state = "";
         for (int s =1; s<9; s++) state += IntToStr(Global::DEV_P13[s]) + ",";
         BFONT->Print_scale(2,i++,AnsiString("INPUT PORT 13:    " + state ).c_str(), 1,0.9,0.9);

         //for (int lc=0; lc<Global::CONSISTA->Count; lc++)
         //     {
         //      BFONT->Print_scale(2,i++,AnsiString(Global::CONSISTA->Strings[lc]).c_str(), 1,0.9,0.9);
         //     }

         BFONT->End();
        }
        */

}


AnsiString __fastcall booltostr(bool state)
{
 if (state == 1) return "yes";
 if (state == 0) return "no";
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// savesettings() - ZAPISYWANIE USTAWIEN DO PLIKU DATA/EU07.CFG ^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
bool __fastcall savesettings()
{

 Global::OTHERS->Clear();
 Global::OTHERS->Add("sceneryfile " + selscn);
 Global::OTHERS->Add("humanctrlvehicle " + selvch);
 Global::OTHERS->Add(" ");
 //Global::OTHERS->Add("screenmode " + IntToStr(resolutionid));
 Global::OTHERS->Add("width " + IntToStr(selsw));
 Global::OTHERS->Add("height " + IntToStr(selsh));
 Global::OTHERS->Add("sf " + IntToStr(Global::iScreenFreq));
 Global::OTHERS->Add("bpp 32");
 Global::OTHERS->Add("fullscreen " + booltostr(Global::bFullScreen));
 Global::OTHERS->Add("debugmode " + booltostr(Global::bDebugMode));
 Global::OTHERS->Add("soundenabled " + booltostr(Global::bSoundEnabled));
 Global::OTHERS->Add("renderalpha " + booltostr(Global::bRenderAlpha));
 Global::OTHERS->Add("physicslog no");
 Global::OTHERS->Add("debuglog " + booltostr(Global::bWriteLogEnabled));
 Global::OTHERS->Add("adjustscreenfreq " + booltostr(Global::bAdjustScreenFreq));
 Global::OTHERS->Add("mousescale 3.2 0.5 ");
 Global::OTHERS->Add("enabletraction " + booltostr(Global::bEnableTraction));
 Global::OTHERS->Add("loadtraction " + booltostr(Global::bLoadTraction));
 Global::OTHERS->Add("livetraction " + booltostr(Global::bLiveTraction));
 Global::OTHERS->Add("advdebug " + booltostr(Global::bQueuedAdvLog));
 Global::OTHERS->Add("pascallog " + booltostr(Global::bQueuedPascalLog));
 Global::OTHERS->Add("newaircouplers " + booltostr(Global::bnewAirCouplers));
 Global::OTHERS->Add("defaultext " +  AnsiString(Global::szDefaultExt.c_str()));

 Global::OTHERS->Add("mdlsfromcache " + booltostr(Global::bmdlsfromcache));
 Global::OTHERS->Add("normals " + booltostr(Global::seekfacenormal));
 Global::OTHERS->Add("make_q3d " + booltostr(Global::makeQ3D));
 Global::OTHERS->Add("bogiesrot " + booltostr(Global::bABuBogiesRoting));
 Global::OTHERS->Add("modelroll " + booltostr(Global::bABuModelRolling));
 Global::OTHERS->Add("modelshake " + booltostr(Global::bABuModelShaking));
 Global::OTHERS->Add("wheelrot " + booltostr(Global::bWheelRotating));
 Global::OTHERS->Add("skyenabled " + booltostr(Global::bRenderSky));
 Global::OTHERS->Add("semlenses " + booltostr(Global::bSemLenses));
 Global::OTHERS->Add("snowenabled " + booltostr(Global::bSnowEnabled));
 Global::OTHERS->Add("fogmaxdist " + booltostr(Global::bfogmaxdist));
 Global::OTHERS->Add("fogsky " + booltostr(Global::bskyfog));
 Global::OTHERS->Add("writedriwetape " + booltostr(Global::bwritetape));
 Global::OTHERS->Add("snowflakesnum " + IntToStr(isnowflakesnum));
 Global::OTHERS->Add("railmaxdistance " + FloatToStr(irailmaxdistance));
 Global::OTHERS->Add("podsmaxdistance " + FloatToStr(ipodsmaxdistance));
 Global::OTHERS->Add("sshotsubdir " + Global::asSSHOTSUBDIR);

 Global::OTHERS->SaveToFile("eu07.ini");
 
}
*/

bool __fastcall TWorld::menuinitctrls()
{
/*
 if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::menuinitctrls()");

      for (int LOOP=0; LOOP<63 ; LOOP++)
           {
            Global::menuctrls[LOOP].px = 0;
            Global::menuctrls[LOOP].py = 0;
            Global::menuctrls[LOOP].pz = 0;
            Global::menuctrls[LOOP].dx = 0;
            Global::menuctrls[LOOP].dy = 0;
            Global::menuctrls[LOOP].checked = 0;
            Global::menuctrls[LOOP].selected = 0;

            Global::menuctrls[LOOP].text = "";
            Global::menuctrls[LOOP].image = "";
            Global::menuctrls[LOOP].label = "";
            Global::menuctrls[LOOP].type = "";
            Global::menuctrls[LOOP].code = "";
            Global::menuctrls[LOOP].info = "";
           }
*/
}


bool __fastcall RenderMenuLineH(int lx, int ly, int rx, int ry, int cr, int cg, int cb)
{
 if (QGlobal::bQueuedAdvLog) WriteLog("bool __fastcall RenderMenuLineH(int lx, int ly, int rx, int ry, int cr, int cg, int cb)");

        glBegin(GL_LINES);
        glColor3f(cr,cb,cg);
        glVertex2f(lx,ly);
        glVertex2f(rx,ry);
        glEnd();
}

bool __fastcall TWorld::RenderMenuCheckBox(int w, int h, int x, int y, int ident, bool check, bool selected, AnsiString label)
{
 if (QGlobal::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenuCheckBox(int w, int h, int x, int y, int ident, bool check, bool selected, AnsiString label)");

 w=15;
 h=15;  /*
        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(0.8,0.8,0.8,0.96);
        if (!check) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox0);
        if (ident == 113 && Global::bFullScreen) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 114 && Global::bDebugMode) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 115 && Global::bSoundEnabled) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 117 && Global::bWriteLogEnabled) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 118 && Global::bLoadTraction) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 119 && Global::bLiveTraction) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 120 && Global::bQueuedAdvLog) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 121 && Global::bmdlsfromcache) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 122 && Global::bABuBogiesRoting) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 123 && Global::bABuModelRolling) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 124 && Global::bABuModelShaking) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 125 && Global::bWheelRotating) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 126 && Global::bRenderSky) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 127 && Global::bSemLenses) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 128 && Global::bSnowEnabled) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 129 && Global::bAdjustScreenFreq) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 130 && Global::bRenderCabFF) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 131 && Global::bfogmaxdist) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 132 && Global::bskyfog) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 133 && Global::bwritetape) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 134 && Global::sky1rot) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 135 && Global::sky2rot) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);
        if (ident == 136 && Global::makeQ3D) glBindTexture(GL_TEXTURE_2D, Global::texmenu_cbox1);

        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_TEXTURE_2D);
        BFONT->Begin();
        glScalef(0.5,0.5,0.5);
        //BFONT->Print_xy(x+30, (Global::iWindowHeight-y)-20, label.c_str(), 1);
        BFONT->Print_xy_scale(x+30, (Global::iWindowHeight-y)-15, label.c_str(), 1, 0.7, 0.7);
        BFONT->End();

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h)) MOUSEHIT(ident);
        if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h)) MOUSEMOV(ident, mouse.x, mouse.y);
        */
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RenderMenuButton() ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool __fastcall TWorld::RenderMenuButton(int w, int h, int x, int y, int ident, bool selected, AnsiString label)
{
/*
 if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenuButton(int w, int h, int x, int y, int ident, bool selected, AnsiString label)");

        if (x < 0) x = Global::iWindowWidth + x; // rightx = true;
        if (y < 0) y = Global::iWindowHeight + y; // bottomy = true;

        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(0.8,0.8,0.8,0.99);
        if (ident == 102) if (!startpassed) {SetConfigResolution(); startpassed = true;};        // MUSI BYC, BO INACZEJ BEDZIE PUSTY SCREEN

        if (ident == 101) glBindTexture(GL_TEXTURE_2D, Global::texmenu_setti_0);
        if (ident == 102) glBindTexture(GL_TEXTURE_2D, Global::texmenu_start_0);
        if (ident == 103) glBindTexture(GL_TEXTURE_2D, Global::texmenu_about_0);
        if (ident == 104) glBindTexture(GL_TEXTURE_2D, Global::texmenu_exitt_0);
        if (ident == 105) glBindTexture(GL_TEXTURE_2D, Global::texmenu_backw);
        if (ident == 111) glBindTexture(GL_TEXTURE_2D, Global::texmenu_leave);
        if (ident == 112) glBindTexture(GL_TEXTURE_2D, Global::texmenu_okeyy);
        if (ident == 174) glBindTexture(GL_TEXTURE_2D, Global::texmenu_leave);
        if (ident == 175) glBindTexture(GL_TEXTURE_2D, Global::texmenu_okeyy);
        if (ident == 176) glBindTexture(GL_TEXTURE_2D, Global::texmenu_chcon);
        if (ident == 177) glBindTexture(GL_TEXTURE_2D, Global::texmenu_adlok);
        if (ident == 200) glBindTexture(GL_TEXTURE_2D, Global::texmenu_okeyy);


        if (ident == 101) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))  glBindTexture(GL_TEXTURE_2D, Global::texmenu_setti_1);
        if (ident == 102) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))  glBindTexture(GL_TEXTURE_2D, Global::texmenu_start_1);
        if (ident == 103) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))  glBindTexture(GL_TEXTURE_2D, Global::texmenu_about_1);
        if (ident == 104) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))  glBindTexture(GL_TEXTURE_2D, Global::texmenu_exitt_1);

        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h)) MOUSEHIT(ident);
 */
}

bool __fastcall TWorld::RenderMenuPanel1(int w, int h, int x, int y, int ident, bool selected, AnsiString label, AnsiString backg)
{
/*
 if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenuPanel1(int w, int h, int x, int y, int ident, bool selected, AnsiString label, AnsiString backg)");

        GLuint tex;
        if (x < 0) x = Global::iWindowWidth + x; // rightx = true;
        if (y < 0) y = Global::iWindowHeight + y; // bottomy = true;

        tex = TTexturesManager::GetTextureID(backg.c_str());

        glColor4f(0.5,0.5,0.5,0.2f);
        glEnable( GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBindTexture(GL_TEXTURE_2D, kartaobiegu);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3i(x-4,   y-4,  0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x-4,   y+h+4,0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w+4, y+h+4,0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w+4, y-4,  0);  // GORNY PRAWY
        glEnd();

        //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        //glEnable(GL_BLEND);
        //glBegin( GL_QUADS );
        //glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        //glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        //glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        //glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        //glEnd( );
        //glDisable(GL_BLEND);
        */
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RenderMenuInputBox() ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool __fastcall TWorld::RenderMenuInputBox(int w, int h, int x, int y, int ident, bool selected, AnsiString label)
{
/*
 if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenuInputBox(int w, int h, int x, int y, int ident, bool selected, AnsiString label)");

        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_BLEND);

        if (x < 0) x = Global::iWindowWidth + x; // rightx = true;
        if (y < 0) y = Global::iWindowHeight + y; // bottomy = true;

        glBindTexture(GL_TEXTURE_2D, Global::texmenu_editb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);

        BFONT->Begin();
        BFONT->Print_xy_scale(x+w+15, (Global::iWindowHeight-y)-20, label.c_str(), 1, 0.7, 0.7);
        BFONT->End();
        if (Global::bAdjustScreenFreq) Global::iScreenFreq = Global::iScreenFreqA;
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_TEXTURE_2D);
        BFONT->Begin();
        if (ident == 140) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, resolutionxy.c_str(), 1, 0.6, 0.6);
        if (ident == 141) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStr(irailmaxdistance).c_str(), 1, 0.6, 0.6);
        if (ident == 142) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStr(ipodsmaxdistance).c_str(), 1, 0.6, 0.6);
        if (ident == 143) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, IntToStr(isnowflakesnum).c_str(), 1, 0.6, 0.6);
        if (ident == 144) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, "maszyna5\\", 1, 0.7, 0.7);
        if (ident == 150) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.skycolor_r, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 151) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.skycolor_g, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 152) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.skycolor_b, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 153) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.fogcolor_r, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 154) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.fogcolor_g, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 155) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.fogcolor_b, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 156) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.fogdst_min, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 157) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.fogdst_max, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 158) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightpos_x, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 159) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightpos_y, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 160) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightpos_z, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 161) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::fogmaxdistp, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 162) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::iScreenFreq, ffFixed,8,0).c_str(), 1, 0.6, 0.6);
        if (ident == 169) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::nfactor, ffFixed,8,0).c_str(), 1, 0.6, 0.6);

        if (ident == 163) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightamb_r, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 164) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightamb_g, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 165) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightamb_b, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 166) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightdif_r, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 167) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightdif_g, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        if (ident == 168) BFONT->Print_xy_scale(x+5, (Global::iWindowHeight-y)-18, FloatToStrF(Global::_atmodata.lightdif_b, ffFixed,8,2).c_str(), 1, 0.6, 0.6);
        BFONT->End();

        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);



        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))
            {
             if (ident == 140) ScreenResolutionAdd();
             if (ident == 141) irailmaxdistance +=10.0;
             if (ident == 142) ipodsmaxdistance +=10.0;
             if (ident == 143) isnowflakesnum +=1000;
             if (ident == 150) Global::_atmodata.skycolor_r += 0.01;
             if (ident == 151) Global::_atmodata.skycolor_g += 0.01;
             if (ident == 152) Global::_atmodata.skycolor_b += 0.01;
             if (ident == 153) Global::_atmodata.fogcolor_r += 0.01;
             if (ident == 154) Global::_atmodata.fogcolor_g += 0.01;
             if (ident == 155) Global::_atmodata.fogcolor_b += 0.01;
             if (ident == 156) Global::_atmodata.fogdst_min += 10.0;
             if (ident == 157) Global::_atmodata.fogdst_max += 10.0;
             if (ident == 158) Global::_atmodata.lightpos_x += 10.0;
             if (ident == 159) Global::_atmodata.lightpos_y += 10.0;
             if (ident == 160) Global::_atmodata.lightpos_z += 10.0;
             if (ident == 161) Global::fogmaxdistp += 10.0;
             if (ident == 162) ScreenFrequencyAdd();
             if (ident == 169) Global::nfactor += 1;
             if (ident == 163) Global::_atmodata.lightamb_r += 0.01;
             if (ident == 164) Global::_atmodata.lightamb_g += 0.01;
             if (ident == 165) Global::_atmodata.lightamb_b += 0.01;
             if (ident == 166) Global::_atmodata.lightdif_r += 0.01;
             if (ident == 167) Global::_atmodata.lightdif_g += 0.01;
             if (ident == 168) Global::_atmodata.lightdif_b += 0.01;

             Global::podsmaxdistance = ipodsmaxdistance;
             Global::railmaxdistance = irailmaxdistance;

             if (Global::_atmodata.skycolor_r > 0.9) Global::_atmodata.skycolor_r = 1.0;
             if (Global::_atmodata.skycolor_g > 0.9) Global::_atmodata.skycolor_g = 1.0;
             if (Global::_atmodata.skycolor_b > 0.9) Global::_atmodata.skycolor_b = 1.0;
             if (Global::_atmodata.fogcolor_r > 0.9) Global::_atmodata.fogcolor_r = 1.0;
             if (Global::_atmodata.fogcolor_g > 0.9) Global::_atmodata.fogcolor_g = 1.0;
             if (Global::_atmodata.fogcolor_b > 0.9) Global::_atmodata.fogcolor_b = 1.0;

             Global::AtmoColor[0] = Global::_atmodata.skycolor_r;
             Global::AtmoColor[1] = Global::_atmodata.skycolor_g;
             Global::AtmoColor[2] = Global::_atmodata.skycolor_b;
             Global::FogColor[0]  = Global::_atmodata.fogcolor_r;
             Global::FogColor[1]  = Global::_atmodata.fogcolor_g;
             Global::FogColor[2]  = Global::_atmodata.fogcolor_b;
             Global::fFogStart    = Global::_atmodata.fogdst_min;
             Global::fFogEnd      = Global::_atmodata.fogdst_max;
             Global::lightPos[0]  = Global::_atmodata.lightpos_x;
             Global::lightPos[1]  = Global::_atmodata.lightpos_y;
             Global::lightPos[2]  = Global::_atmodata.lightpos_z;
             Global::ambientDayLight[0] = Global::_atmodata.lightamb_r;
             Global::ambientDayLight[1] = Global::_atmodata.lightamb_g;
             Global::ambientDayLight[2] = Global::_atmodata.lightamb_b;
             Global::diffuseDayLight[0] = Global::_atmodata.lightdif_r;
             Global::diffuseDayLight[1] = Global::_atmodata.lightdif_g;
             Global::diffuseDayLight[2] = Global::_atmodata.lightdif_b;

             //glClearColor (Global::AtmoColor[0], Global::AtmoColor[1], Global::AtmoColor[2], 0.0);                  // Background Color
             glFogfv(GL_FOG_COLOR, Global::FogColor);				                                   // Set Fog Color
             glFogf(GL_FOG_START, Global::fFogStart);	        		// Fog Start Depth
             glFogf(GL_FOG_END, Global::fFogEnd);         			// Fog End Depth
             glDisable(GL_FOG);

             MOUSEHIT(ident);
            }


        if (Pressed(VK_RBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))
            {
             if (ident == 140) ScreenResolutionRem();
             if (ident == 141) irailmaxdistance -=10.0;
             if (ident == 142) ipodsmaxdistance -=10.0;
             if (ident == 143) isnowflakesnum -=1000;
             if (ident == 150) Global::_atmodata.skycolor_r -= 0.01;
             if (ident == 151) Global::_atmodata.skycolor_g -= 0.01;
             if (ident == 152) Global::_atmodata.skycolor_b -= 0.01;
             if (ident == 153) Global::_atmodata.fogcolor_r -= 0.01;
             if (ident == 154) Global::_atmodata.fogcolor_g -= 0.01;
             if (ident == 155) Global::_atmodata.fogcolor_b -= 0.01;
             if (ident == 156) Global::_atmodata.fogdst_min -= 10.0;
             if (ident == 157) Global::_atmodata.fogdst_max -= 10.0;
             if (ident == 158) Global::_atmodata.lightpos_x -= 10.0;
             if (ident == 159) Global::_atmodata.lightpos_y -= 10.0;
             if (ident == 160) Global::_atmodata.lightpos_z -= 10.0;
             if (ident == 161) Global::fogmaxdistp -= 10.0;
             if (ident == 169) Global::nfactor -= 1;
             if (ident == 162) ScreenFrequencyRem();
             if (ident == 163) Global::_atmodata.lightamb_r -= 0.01;
             if (ident == 164) Global::_atmodata.lightamb_g -= 0.01;
             if (ident == 165) Global::_atmodata.lightamb_b -= 0.01;
             if (ident == 166) Global::_atmodata.lightdif_r -= 0.01;
             if (ident == 167) Global::_atmodata.lightdif_g -= 0.01;
             if (ident == 168) Global::_atmodata.lightdif_b -= 0.01;


             Global::podsmaxdistance = ipodsmaxdistance;
             Global::railmaxdistance = irailmaxdistance;

             if (Global::_atmodata.skycolor_r < 0.0) Global::_atmodata.skycolor_r = 0.0;
             if (Global::_atmodata.skycolor_g < 0.0) Global::_atmodata.skycolor_g = 0.0;
             if (Global::_atmodata.skycolor_b < 0.0) Global::_atmodata.skycolor_b = 0.0;
             if (Global::_atmodata.fogcolor_r < 0.0) Global::_atmodata.fogcolor_r = 0.0;
             if (Global::_atmodata.fogcolor_g < 0.0) Global::_atmodata.fogcolor_g = 0.0;
             if (Global::_atmodata.fogcolor_b < 0.0) Global::_atmodata.fogcolor_b = 0.0;

             Global::AtmoColor[0] = Global::_atmodata.skycolor_r;
             Global::AtmoColor[1] = Global::_atmodata.skycolor_g;
             Global::AtmoColor[2] = Global::_atmodata.skycolor_b;
             Global::FogColor[0]  = Global::_atmodata.fogcolor_r;
             Global::FogColor[1]  = Global::_atmodata.fogcolor_g;
             Global::FogColor[2]  = Global::_atmodata.fogcolor_b;
             Global::fFogStart    = Global::_atmodata.fogdst_min;
             Global::fFogEnd      = Global::_atmodata.fogdst_max;
             Global::lightPos[0]  = Global::_atmodata.lightpos_x;
             Global::lightPos[1]  = Global::_atmodata.lightpos_y;
             Global::lightPos[2]  = Global::_atmodata.lightpos_z;
             Global::ambientDayLight[0] = Global::_atmodata.lightamb_r;
             Global::ambientDayLight[1] = Global::_atmodata.lightamb_g;
             Global::ambientDayLight[2] = Global::_atmodata.lightamb_b;
             Global::diffuseDayLight[0] = Global::_atmodata.lightdif_r;
             Global::diffuseDayLight[1] = Global::_atmodata.lightdif_g;
             Global::diffuseDayLight[2] = Global::_atmodata.lightdif_b;
             
//           glClearColor (Global::AtmoColor[0], Global::AtmoColor[1], Global::AtmoColor[2], 0.0);                  // Background Color
             glFogfv(GL_FOG_COLOR, Global::FogColor);				                                   // Set Fog Color
             glFogf(GL_FOG_START, Global::fFogStart);	        		                                  // Fog Start Depth
             glFogf(GL_FOG_END, Global::fFogEnd);         		                                	 // Fog End Depth
             glDisable(GL_FOG);
             
             MOUSEHIT(ident);
            }

             //if (resolutionid == 0) resolutionxy = "320x240";
             //if (resolutionid == 1) resolutionxy = "512x384";
             //if (resolutionid == 2) resolutionxy = "640x480";
             //if (resolutionid == 3) resolutionxy = "800x600";
             //if (resolutionid == 4) resolutionxy = "1024x768";
             //if (resolutionid == 5) resolutionxy = "1152x864";
             //if (resolutionid == 6) resolutionxy = "1280x1024";
             //if (resolutionid == 7) resolutionxy = "1600x1024";
             //if (resolutionid == 8) resolutionxy = "1600x1200";
             //if (resolutionid == 9) resolutionxy = "2048x1536";
 */
}

bool __fastcall TWorld::RenderMenuListBoxItem(int w, int h, int x, int y, int ident, int selid, bool selected, int item, AnsiString label)
{
/*
 if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenuListBoxItem(int w, int h, int x, int y, int ident, int selid, bool selected, int item, AnsiString label)");

 AnsiString str, line;
 h = 15;

if (ident == 171)  // SCENERY LIST
       {
        nodesinmain->Clear();

        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(0.6,0.4,0.3,0.5);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);


        if (selscnid == selid)  // SELECTOR
            {
             glBlendFunc(GL_SRC_ALPHA,GL_ONE);
             glEnable(GL_BLEND);
             glColor4f(1,1,1,0.7);
             glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
             glBegin( GL_QUADS );
             glTexCoord2f(0, 1); glVertex3i(x-2, y+0, 0);    // GORNY LEWY
             glTexCoord2f(0, 0); glVertex3i(x-2, y+h, 0);    // DOLY LEWY
             glTexCoord2f(1, 0); glVertex3i(x+w+2, y+h, 0);  // DOLNY PRAWY
             glTexCoord2f(1, 1); glVertex3i(x+w+2, y+0, 0);  // GORNY PRAWY
             glEnd( );
             glDisable(GL_BLEND);
            }

        glColor4f(1,0.4,0.5,1);
        BFONT->Begin();
        BFONT->Print_xy_scale(x, (Global::iWindowHeight-y)-15, AnsiString(SCNLST->Strings[item]).c_str(), 1, 0.8, 0.8);
        BFONT->End();

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))
        {
         AnsiString FNAME, x, b, c, t;
         string wers;
         int lines = 0;
         int trainsets = 0;
         trainsetsect = false;

         VCHLST->Clear();
         TRKLST->Clear();

         str = IntToStr(selid);
         line = str.SubString(2,2);
         selscn = SCNLST->Strings[StrToInt(line)] + ".scn";
         seltrk = SCNLST->Strings[StrToInt(line)] + ".txt";

         strcpy(Global::szSceneryFile, selscn.c_str());

         selscnid = selid;

         // WCZYTANIE LISTY TOROW DLA WYBRANEJ TRASY

         FNAME = Global::g___APPDIR + "SCENERY\\#tracks\\"  + seltrk;

         ifstream in1(FNAME.c_str());

          while(getline(in1, wers))
          {
           x = wers.c_str();
           TRKLST->Add(x);
          }




         // PARSOWANIE WYBRANEGO PLIKU SCN W POSZUKIWANIU DOSTEPNYCH POJAZDOW

         VCHLST->Add("ghostview");

         FNAME = Global::g___APPDIR + "SCENERY\\"  + selscn;

         //---Form3->RESCN->Lines->LoadFromFile(FNAME);   ---

         ifstream in2(FNAME.c_str());

          while(getline(in2, wers))
          {
           lines++;
           x = wers.c_str();

           if (x.SubString(1,9) == "FirstInit")
               {
                trainsetsect = true;
               }

           if (x.SubString(1,8) == "trainset")
               {
                trainsets++;
                _TRAINSETNAME = gettrainsetname(x);
                t = _TRAINSETNAME;
               }

           if (x.SubString(1,4) == "node" && trainsetsect)
               {
                b = wers.c_str();

                nodesinmain->Add(b);   // DODAJ NODE'a DO LISTY COBY POZNIEJ SPRAWDZIC WSZYSTKO

                AnsiString n = getdynamicname(b);
                AnsiString c = getdynamicchk(b);

                if (isdriveable(getdynamicchk(b))) VCHLST->Add(n);      //n - name, c - type, t - trainset name
               }

          }
          checknodes();  // SPRAWDZANIE W POSZUKIWANIU NIEISTNIEJACYH POJAZDOW
       }
    }


if (ident == 172)  // VECH LIST
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1.0,0.4,0.3,0.5);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);

        if (selvchid == selid)  // SELECTOR
            {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1,1,1,0.7);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x-2, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x-2, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w+2, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w+2, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);
            }

        glColor4f(1,0.4,0.5,1);
        BFONT->Begin();
        BFONT->Print_xy_scale(x, (Global::iWindowHeight-y)-15, AnsiString(VCHLST->Strings[item]).c_str(), 1, 0.8, 0.8);
        BFONT->End();

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))
        {
         str = IntToStr(selid);
         line = str.SubString(2,2);
         selvch = VCHLST->Strings[StrToInt(line)];
         selvchid = selid;
        }
    }


if (ident == 180)  // TRAINSETS LIST
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1.0,0.4,0.3,0.5);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);

        if (seltrsid == selid)   // SELECTOR
            {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1,1,1,0.7);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x-2, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x-2, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w+2, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w+2, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);
            }

        glColor4f(1,0.4,0.5,1);
        BFONT->Begin();
        BFONT->Print_xy_scale(x, (Global::iWindowHeight-y)-15, AnsiString(TRSLST->Strings[item]).c_str(), 1, 0.8, 0.8);
        BFONT->End();

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))
        {
         str = IntToStr(selid);
         line = str.SubString(2,2);
         seltrs = TRSLST->Strings[StrToInt(line)];
         seltrsid = selid;
       //Form3->ETRAINSET->Text = "trainset none " + seltrk + " 1.0 0.0 include #TRAINSETS/" + seltrs + ".trainset end endtrainset";

         if (FileExists("scenery\\#trainsets\\" + seltrs + ".jpg"))
             {
              kartaobiegu = TTexturesManager::GetTextureID(AnsiString("scenery\\#trainsets\\" + seltrs + ".jpg").c_str());
             }
             else
             {
              kartaobiegu = TTexturesManager::GetTextureID(AnsiString("scenery\\#trainsets\\unav.jpg").c_str());
             }
          checkconsist("scenery\\#trainsets\\" + seltrs + ".trainset");
        }
    }



if (ident == 181)  // TRACKS LIST
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1.0,0.4,0.3,0.5);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);

        if (seltrkid == selid)   // SELECTOR
            {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1,1,1,0.7);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxb);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x-2, y+0, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x-2, y+h, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w+2, y+h, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w+2, y+0, 0);  // GORNY PRAWY
        glEnd( );
        glDisable(GL_BLEND);
            }

        glColor4f(1,0.4,0.5,1);
        BFONT->Begin();
        BFONT->Print_xy_scale(x, (Global::iWindowHeight-y)-15, AnsiString(TRKLST->Strings[item]).c_str(), 1, 0.8, 0.8);
        BFONT->End();

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x) && (mouse.x <x+w) && (mouse.y > y) && (mouse.y <y+h))
        {
         str = IntToStr(selid);
         line = str.SubString(2,2);
         seltrk = TRKLST->Strings[StrToInt(line)];
         seltrkid = selid;
       //--Form3->ETRACK->Text = seltrk;
        }
    }
*/
}

bool __fastcall RenderMenuListBoxSBUD(int w, int h, int x, int y, int ident, int cap, int all, int dir)
{
/*
 if (Global::bQueuedAdvLog) WriteLog("bool __fastcall RenderMenuListBoxSBUD(int w, int h, int x, int y, int ident, int cap, int all, int dir)");

 if (dir == 1)
     {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1.0,0.4,0.3,0.8);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxu);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0,  y+(cap*16)+18, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0,  y+(cap*16)+32, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w,  y+(cap*16)+32, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w,  y+(cap*16)+18, 0);  // GORNY PRAWY
        glEnd( );

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x+0) && (mouse.x <x+w) && (mouse.y > y+(cap*16)+16) && (mouse.y <y+(cap*16)+32))
        {
         if (ident == 171) if (sitscn > 0) sitscn -= 1;
         if (ident == 180) if (sittrs > 0) sittrs -= 1;
         if (ident == 181) if (sittrk > 0) sittrk -= 1;
         Sleep(100);
        }
     }

 if (dir == 0)
     {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_BLEND);
        glColor4f(1.0,0.4,0.3,0.8);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxd);
        glBegin( GL_QUADS );
        glTexCoord2f(0, 1); glVertex3i(x+0,  y+(cap*16)+33, 0);  // GORNY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+0,  y+(cap*16)+49, 0);  // DOLY LEWY
        glTexCoord2f(1, 0); glVertex3i(x+w,  y+(cap*16)+49, 0);  // DOLNY PRAWY
        glTexCoord2f(1, 1); glVertex3i(x+w,  y+(cap*16)+33, 0);  // GORNY PRAWY
        glEnd( );

        if (Pressed(VK_LBUTTON)) if ((mouse.x > x+0) && (mouse.x <x+w) && (mouse.y > y+(cap*16)+32) && (mouse.y <y+(cap*16)+48))
        {
         if (ident == 171) if (sitscn+cap < all-1) sitscn += 1;
         if (ident == 180) if (sittrs+cap < all-1) sittrs += 1;
         if (ident == 181) if (sittrk+cap < all-1) sittrk += 1;
         Sleep(100);
        }
     }
*/
}

bool __fastcall TWorld::RenderMenuListBox(int w, int h, int x, int y, int ident, bool selected, int items, AnsiString label)
{
/*
  if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenuListBox(int w, int h, int x, int y, int ident, bool selected, int items, AnsiString label)");

        int l, itemid;

        if (x < 0) x = Global::iWindowWidth + x; // rightx = true;
        if (y < 0) y = Global::iWindowHeight + y; // bottomy = true;

if (ident == 171)  // SCENERY LIST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       {
        int yy;
        int sit = sitscn;           // START ITEM
        int tit = SCNLST->Count;    // TOTAL ITEMS
        int cap = 28;               // LB CAPACITY

        if (tit < cap) cap = tit-1;  // NA WSZELKI WYPADEK GDYBY BYLO MNIEJ TRAS NIZ CAPACITY

        yy = Global::iWindowHeight - (y+(cap*16)+32) - 20;

        glEnable( GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
        glColor4f(1,0.5,0.7,0.8f);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxa);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3i(x-4, 88,0);   // GORNY LEWY
        glTexCoord2f(1, 0); glVertex3i(x-4, Global::iWindowHeight-yy,0); // DOLY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+w+4, Global::iWindowHeight-yy,0);// DOLNY PRAWY
        glTexCoord2f(0, 1); glVertex3i(x+w+4, 88,0);  // GORNY PRAWY
        glEnd();
        glDisable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        BFONT->Begin();
        BFONT->Print_xy(x, (Global::iWindowHeight-y)-16, AnsiString(label).c_str(), 1);
        BFONT->End();

        RenderMenuListBoxSBUD(w, h, x, y, ident, cap, tit, 1);
        RenderMenuListBoxSBUD(w, h, x, y, ident, cap, tit, 0);

        itemid = 700+sit;

        if (sit+cap < tit)
         {
          for (int LOOP=sit; LOOP<sit+cap ; LOOP++)
               {
                y+=16;
                RenderMenuListBoxItem(w, h, x, y, ident, itemid, 0, LOOP, "");
                itemid+=1;
               }
           }
       }

if (ident == 172)  // VECHICLES LIST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       {
        l = VCHLST->Count;
        itemid = 800;

        int yy;

        yy = Global::iWindowHeight - (y+(l*16)) - 20;


        // LISTBOX BACKGROUND ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        glEnable( GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
        //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1,0.5,0.7,0.8f);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxa);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3i(x-4, 88,0);   // GORNY LEWY
        glTexCoord2f(1, 0); glVertex3i(x-4, Global::iWindowHeight-yy,0); // DOLY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+w+4, Global::iWindowHeight-yy,0);// DOLNY PRAWY
        glTexCoord2f(0, 1); glVertex3i(x+w+4, 88,0);  // GORNY PRAWY
        glEnd();
        glDisable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_TEXTURE_2D);
        BFONT->Begin();
        BFONT->Print_xy(x, (Global::iWindowHeight-y)-16, AnsiString(label).c_str(), 1);
        BFONT->End();


          for (int LOOP=0; LOOP<l ; LOOP++)
               {
                y+=16;
                RenderMenuListBoxItem(w, h, x, y, ident, itemid, 0, LOOP, "");
                itemid+=1;
               }
       }

if (ident == 180)  // TRAINSETS LIST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       {
        int sit = sittrs;           // START ITEM
        int tit = TRSLST->Count;    // TOTAL ITEMS
        int cap = 11;               // LB CAPACITY
        int yy;

        if (tit < cap) cap = tit-1;  // NA WSZELKI WYPADEK GDYBY BYLO MNIEJ TRAS NIZ CAPACITY

        yy = Global::iWindowHeight - (y+(cap*16)+32) - 20;

        // LISTBOX BACKGROUND ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        glEnable( GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
        glColor4f(1,0.5,0.7,0.8f);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxa);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3i(x-4, 88,0);   // GORNY LEWY
        glTexCoord2f(1, 0); glVertex3i(x-4, Global::iWindowHeight-yy,0); // DOLY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+w+4, Global::iWindowHeight-yy,0);// DOLNY PRAWY
        glTexCoord2f(0, 1); glVertex3i(x+w+4, 88,0);  // GORNY PRAWY
        glEnd();
        glDisable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_TEXTURE_2D);
        BFONT->Begin();
        BFONT->Print_xy(x, (Global::iWindowHeight-y)-16, AnsiString(label).c_str(), 1);
        BFONT->End();

        if (tit > 0) RenderMenuListBoxSBUD(w, h, x, y, ident, cap, tit, 1);
        if (tit > 0) RenderMenuListBoxSBUD(w, h, x, y, ident, cap, tit, 0);

        itemid = 800+sit;

        if (sit+cap < tit)
         {
         for (int LOOP=sit; LOOP<sit+cap ; LOOP++)
               {
                y+=16;
                RenderMenuListBoxItem(w, h, x, y, ident, itemid, 0, LOOP, "");
                itemid+=1;
               }
          }
       }

if (ident == 181)  // TRACKS LIST ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       {
        int yy;
        int sit = sittrk;           // START ITEM
        int tit = TRKLST->Count;    // TOTAL ITEMS
        int cap = 11;               // LB CAPACITY

        if (tit < cap) cap = tit-1;  // NA WSZELKI WYPADEK GDYBY BYLO MNIEJ TRAS NIZ CAPACITY

        yy = Global::iWindowHeight - (y+(cap*16)+32) - 20;

        // LISTBOX BACKGROUND ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        glEnable( GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
        glColor4f(1,0.5,0.7,0.8f);
        glBindTexture(GL_TEXTURE_2D, Global::texmenu_lboxa);
        glBegin(GL_QUADS);
        glTexCoord2f(1, 1); glVertex3i(x-4, y,0);   // GORNY LEWY
        glTexCoord2f(1, 0); glVertex3i(x-4, Global::iWindowHeight-yy,0); // DOLY LEWY
        glTexCoord2f(0, 0); glVertex3i(x+w+4, Global::iWindowHeight-yy,0);// DOLNY PRAWY
        glTexCoord2f(0, 1); glVertex3i(x+w+4, y,0);  // GORNY PRAWY
        glEnd();
        glDisable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        glEnable(GL_TEXTURE_2D);
        BFONT->Begin();
        BFONT->Print_xy(x, (Global::iWindowHeight-y)-16, AnsiString(label).c_str(), 1);
        BFONT->End();

        if (tit > 0) RenderMenuListBoxSBUD(w, h, x, y, ident, cap, tit, 1);
        if (tit > 0) RenderMenuListBoxSBUD(w, h, x, y, ident, cap, tit, 0);

        itemid = 900+sit;

        if (sit+cap < tit)
         {
         for (int LOOP=sit; LOOP<sit+cap ; LOOP++)
               {
                y+=16;
                RenderMenuListBoxItem(w, h, x, y, ident, itemid, 0, LOOP, "");
                itemid+=1;
               }
         }
     }
     */
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// MOUSEMOV() - OBSLUGA POZYCJI KURSORA PODCZAS PRZESUWANIA ^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/*
__fastcall TWorld::MOUSEMOV(int ID, int x, int y)
{
  if (Global::bQueuedAdvLog) WriteLog("__fastcall TWorld::MOUSEMOV(int ID, int x, int y)");

  if (ID == 113) desc = "ustawia tryb pelnoekranowy lub w oknie";
  if (ID == 115) desc = "dzwieki przestrzenne";
  if (ID == 117) desc = "logowanie przebiegu uruchamiania";
  if (ID == 118) desc = "renderowanie elementow sieci trakcyjnej (przewody, slupy, bramki, wyciegniki)";
  if (ID == 119) desc = "zaleznosc elektryki pojazdu od sieci trakcyjnej";
  if (ID == 120) desc = "szczegolowe logowanie silnika grafiki i fizyki (DEBUG)";
  if (ID == 121) desc = "nie wczytuj tych samych modeli obiektow jako nowe";
  if (ID == 124) desc = "efekt bujania mechanika w kabinie";
  if (ID == 125) desc = "animacje zestawow kolowych pojazdow";
  if (ID == 126) desc = "renderowanie nieba";
  if (ID == 127) desc = "rozmycie swiatel semaforow";
  if (ID == 128) desc = "animacja sniegu";
  if (ID == 130) desc = "renderowanie kabiny w trybie 'freefly' (nowe modele z oknami)";
  if (ID == 136) desc = "generowanie modeli Q3D (modele z policzonymi normalnymi)";
}
*/

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// MOUSEHIT() - OBSLUGA POZYCJI KURSORA PODCZAS KLIKNIECIA ^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

/*
__fastcall TWorld::MOUSEHIT(int ID)
{

if (!Global::SCNLOADED)
    {
     if (ID == 102) savesettings();                                             // MENU GLOWNE, [START]
     if (ID == 102) Global::LoadIniFile();                                      // MENU GLOWNE, [START]
     if (ID == 102) Global::bmenuon = false;                                    // tymczasowo
     if (ID == 102) Global::infotype = 0;                                       // tymczasowo

     if (ID == 101) SCNLST->Clear();
     if (ID == 101) dir(AnsiString(Global::g___APPDIR + "SCENERY\\").c_str(), true, "scn");
     if (ID == 101) SCNLST->Add( "z.scn" );

     if (ID == 101) {Global::menuselected = "sett";}                            // MENU GLOWNE, [USTAWIENIA]
     if (ID == 102) {Global::menuselected = "symul";}      //"start"            // MENU GLOWNE, [START]
     if (ID == 103) {Global::menuselected = "about";}                           // MENU GLOWNE, [O PROGRAMIE]
     if (ID == 104) {TWorld::kill = true;}                                      // MENU GLOWNE, [WYJSCIE]
     if (ID == 105) {Global::SCNLOADED = !Global::SCNLOADED;};

     if (ID == 112) savesettings();                                             // MENU SETTS, [OKEJ]
     if (ID == 113) Global::bFullScreen = !Global::bFullScreen;
     if (ID == 114) Global::bDebugMode = !Global::bDebugMode;
     if (ID == 115) Global::bSoundEnabled = !Global::bSoundEnabled;
     if (ID == 117) Global::bWriteLogEnabled = !Global::bWriteLogEnabled;
     if (ID == 118) Global::bLoadTraction = !Global::bLoadTraction;
     if (ID == 119) Global::bLiveTraction = !Global::bLiveTraction;
     if (ID == 120) Global::bQueuedAdvLog = !Global::bQueuedAdvLog;
     if (ID == 121) Global::bmdlsfromcache = !Global::bmdlsfromcache;
     if (ID == 122) Global::bABuBogiesRoting = !Global::bABuBogiesRoting;
     if (ID == 123) Global::bABuModelRolling = !Global::bABuModelRolling;
     if (ID == 124) Global::bABuModelShaking = !Global::bABuModelShaking;
     if (ID == 125) Global::bWheelRotating = !Global::bWheelRotating;
     if (ID == 126) Global::bRenderSky = !Global::bRenderSky;
     if (ID == 126) if (!Global::SKYINIT) Clouds.Init();
     if (ID == 127) Global::bSemLenses = !Global::bSemLenses;
     if (ID == 128) Global::bSnowEnabled = !Global::bSnowEnabled;
     if (ID == 129) Global::bAdjustScreenFreq = !Global::bAdjustScreenFreq;
     if (ID == 130) Global::bRenderCabFF = !Global::bRenderCabFF;
     if (ID == 131) Global::bfogmaxdist = !Global::bfogmaxdist;
     if (ID == 132) Global::bskyfog = !Global::bskyfog;
     if (ID == 133) Global::bwritetape = !Global::bwritetape;
     if (ID == 134) Global::sky1rot = !Global::sky1rot;
     if (ID == 135) Global::sky2rot = !Global::sky2rot;
     if (ID == 136) Global::makeQ3D = !Global::makeQ3D;

     if (ID == 111) {Global::menuselected = "main";}                            // MENU SETTS, [ANULUJ]
     if (ID == 112) {Global::menuselected = "main";}                            // MENU SETTS, [OKEJ]
     if (ID == 174) {Global::menuselected = "main";}                            // MENU START, [ANULUJ]
     if (ID == 200) {Global::menuselected = "main";}                            // MENU ABOUT, [OKEJ]
     if (ID == 175) savesettings();                                             // MENU START, [OKEJ]
     if (ID == 175) Global::LoadIniFile();                                      // MENU START, [OKEJ]
     if (ID == 175) {LOAD(TWorld::xhWnd, TWorld::xhDC);                         // MENU START, [OKEJ]


     if (Global::menuselected == "sett") Global::infotype = 11;
     if (Global::menuselected == "about") Global::infotype = 33;
     if (Global::menuselected == "start") Global::infotype = 0;

     ShowCursor(false);}
     //if (ID == 176) {CHANGECONSIST();};
     //if (ID == 177) {ADDLOCOMOTIVE();};

     Sleep(150);

 }

}
  */

// *****************************************************************************
// RenderMenu() - GLOWNA FUNKCJA RENDERUJACA MENU GLOWNE ***********************
// *****************************************************************************

bool __fastcall TWorld::RenderMenu()
{
/*
    AnsiString type;
    settscontainer mobj;
    int margin = 1;

    if (Global::bQueuedAdvLog) WriteLog("bool __fastcall TWorld::RenderMenu()");

    glPolygonMode(GL_FRONT, GL_FILL);
    
// checkfreq()
    if (!bnodesmain) nodesinmain = new TStringList;
    desc = "";

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    nodesinmain->Clear();
    bnodesmain = true;

  //if (!startsound1) PlaySound("data\\sounds\\menu.x", NULL, SND_ASYNC); startsound1 = true;

    if (!iniloaded)
        {
         Global::LoadIniFile(); iniloaded = true;
         irailmaxdistance = Global::railmaxdistance;
         ipodsmaxdistance = Global::podsmaxdistance;
         isnowflakesnum = Global::iSnowFlakesNum;
         selscn = Global::szSceneryFile;
         selvch = Global::asHumanCtrlVehicle;
        }

    if (!bcheckfreq) checkfreq();

    if (!startpassed) {SetConfigResolution(); startpassed = true;};        // MUSI BYC, BO INACZEJ BEDZIE PUSTY SCREEN


    glDisable( GL_LIGHT0 );
    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );


//-    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // TO GDY MENU BEDZIE RENDEROWANE PRZED ZALADOWANIEM SCENERII
//-    glClearColor (0.1, 0.2, 0.2, 1.0);                   // ELO GREEN


   // RENDEROWANIE PLASKIE (GUI) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, GWW, GWH, 0, -100, 100);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity( );


   // MENU BACKGROUND ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   glColor4f(0.1,0.1,0.1,0.97f);
   glEnable(GL_BLEND);


   if (Global::menuselected == "main" ) glColor4f(0.4, 0.4, 0.4, 0.45f);
   if (Global::menuselected == "about") glColor4f(0.4, 0.4, 0.4, 0.45f);
   if (Global::menuselected == "sett" ) glColor4f(0.4, 0.4, 0.4, 0.45f);
   
   glBindTexture(GL_TEXTURE_2D, Global::texmenu_backg);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 1); glVertex3i(    margin,      margin, 0);  // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i(    margin, GWH- margin, 0);  // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i(GWW-margin, GWH- margin, 0);  // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i(GWW-margin,      margin, 0);  // GORNY PRAWY
   glEnd();



   // HEADER LINE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   //-glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
   if (Global::menuselected == "main" ) glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   //-glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   //glEnable(GL_BLEND);
   //glColor4f(0.1,0.1,0.1,0.97f);
   //glBindTexture(GL_TEXTURE_2D, Global::texmenu_head1);
   //glBegin(GL_QUADS);
   //glTexCoord2f(0, 1); glVertex3i(    margin,     margin, 0);  // GORNY LEWY
   //glTexCoord2f(0, 0); glVertex3i(    margin, GWH-GWH+80, 0);  // DOLY LEWY
   //glTexCoord2f(1, 0); glVertex3i(GWW-margin, GWH-GWH+80, 0);  // DOLNY PRAWY
   //glTexCoord2f(1, 1); glVertex3i(GWW-margin,     margin, 0);  // GORNY PRAWY
   //glEnd();
   //glDisable(GL_BLEND);


   //RenderMenuLineH(10, GWH-10, GWW-10, GWH-10, 1, 0, 0);
   //RenderMenuLineH(10,     10, GWW-10,     10, 1, 0, 0);  // LINIA HEADER 1
   //RenderMenuLineH(10,     81, GWW-10,     81, 1, 0, 0);  // LINIA HEADER 2



//   if (Global::menuselected == "main") RenderInformation(0);
   if (Global::menuselected == "sett") RenderInformation(11);
   if (Global::menuselected == "about") RenderInformation(33);
   if (Global::menuselected == "start") RenderInformation(44);


   Global::sceneryloading = false; // WYMUSZONE
if (!Global::sceneryloading)
    {
    
     for (int LOOP=0; LOOP<Global::menuobjs ; LOOP++)
          {
           if (Global::menuselected == Global::menuctrls[LOOP].parent)
               {
                mobj = Global::menuctrls[LOOP];
                type = Global::menuctrls[LOOP].type;

                if (type == "button") RenderMenuButton(mobj.dx, mobj.dy, mobj.px, mobj.py, StrToInt(mobj.code), 0, "none");

                if (type == "editbox") RenderMenuInputBox(mobj.dx, mobj.dy, mobj.px, mobj.py, StrToInt(mobj.code), 0, mobj.label);

                if (type == "checkbox") RenderMenuCheckBox(mobj.dx, mobj.dy, mobj.px, mobj.py, StrToInt(mobj.code), 0, 0, mobj.label);

                if (type == "listbox") RenderMenuListBox(mobj.dx, mobj.dy, mobj.px, mobj.py, StrToInt(mobj.code), 0, 21, mobj.label);

                if (type == "panel1") RenderMenuPanel1(mobj.dx, mobj.dy, mobj.px, mobj.py, StrToInt(mobj.code), 0, "none", mobj.image);
               }
          }
    }



   // MOUSE CURSOR ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   GetCursorPos(&mouse);
   Global::MPX = mouse.x;
   Global::MPY = mouse.y;

   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, Global::texmenu_point);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i(Global::MPX+0,  Global::MPY+0,  0);           // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i(Global::MPX+0,  Global::MPY+30, 0);           // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i(Global::MPX+30, Global::MPY+30, 0);           // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i(Global::MPX+30, Global::MPY+0,  0);           // GORNY PRAWY
   glEnd( );
   glDisable(GL_BLEND);




   
   // INFORMACJE O PRZELACZNIKU W MENU USTAWIENIA
   glEnable( GL_TEXTURE_2D);
   BFONT->Begin();
   glColor4f(0.4,0.4,0.4,0.7);
   BFONT->Print_xy_scale(20, (Global::iWindowHeight)-1015, AnsiString(desc).c_str(), 1, 1.0, 1.0);
   BFONT->Print_xy_scale(950, (Global::iWindowHeight)-1015, AnsiString(Global::APPDATE + " " + Global::APPSIZE).c_str(), 1, 0.8, 0.8);
   BFONT->End();

   //WriteLog("MENU:");
   glEnable(GL_LIGHT0);
//-   glFlush();

   return true;
   */
}


bool __fastcall TWorld::RenderFPS(double alpha)
{

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );

    float trans = 0.35;
    float margin = 1;
    //if (!floaded) BFONT = new Font();
    //if (!floaded) BFONT->loadf("none");
    //floaded = true;

   //prepare2d();

    //glEnable( GL_TEXTURE_2D);
    //BFONT->Begin();

    //glDisable(GL_TEXTURE_2D);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glColor4f(0.1,0.1,0.1, Global::GUITUTOPAC);

    //glBegin(GL_QUADS);
    //glVertex2i(0,     GWH-1024);  // dol lewy
    //glVertex2i(GWW-1, GWH-1024);  // dol prawy
    //glVertex2i(GWW-1, GWH);       // gora
    //glVertex2i(0,     GWH);       // gora
    //glEnd();

     //BFONT->End();

     glColor4f(0.8f, 0.8f, 0.8f, 0.7f);
     freetype::print(our_font14, Global::iWindowWidth-90, Global::iWindowHeight-20, "FPS: %7.2f", QGlobal::fps);   // W CZASIE SYMULACJI

   //freetype::print(our_font14, 10, Global::iWindowHeight-20, "MAXRENDERDIST: %7.2f", QGlobal::worldmaxdist);
   //freetype::print(our_font14, 10, Global::iWindowHeight-40, "SUN Y POSITION: %7.2f", Global::lightPos[1]);
     glEnable(GL_TEXTURE_2D);
     glEnable(GL_FOG);
     glEnable( GL_LIGHTING );
}


bool __fastcall TWorld::RenderFILTER(double alpha)
{
    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;
    QGlobal::bTUTORIAL = false;
    QGlobal::bKEYBOARD = false;
    QGlobal::infotype = 0;
    float margin = 1;
    int pm = 0;
//    if (!floaded) BFONT = new Font();
//    if (!floaded) BFONT->loadf("none");
//    floaded = true;

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity( );

    glDisable(GL_DEPTH_TEST);
    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glEnable( GL_TEXTURE_2D);
//    BFONT->Begin();

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.20,0.20,0.20, alpha);
//    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, QGlobal::SCRFILTER);

//  glBindTexture(GL_TEXTURE_2D, Global::splashscreen);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(margin-pm,   margin,0);   // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(margin-pm,   Global::iWindowHeight-margin,0); // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+pm, Global::iWindowHeight-margin,0); // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+pm, margin,0);   // GORNY PRAWY
    glEnd( );

 //    BFONT->End();
    glColor4f(0.5,0.5,0.5, 0.25);
    freetype::print(our_font12, 20, Global::iWindowHeight-1000, "FPS: %7.2f", QGlobal::fps);
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_FOG );
    glEnable( GL_LIGHTING );
    glEnable( GL_DEPTH_TEST );
   return true;
}


bool __fastcall TWorld::RenderEXITQUERY(double alpha)
{
    QGlobal::bTUTORIAL = false;
    QGlobal::bKEYBOARD = false;
    QGlobal::infotype = 0;
    float margin = 1;

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glEnable( GL_TEXTURE_2D );

    glColor4f(0.1,0.1,0.1, 0.6);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(margin-0,   margin,0);   // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(margin-0,   Global::iWindowHeight-margin,0); // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+0, Global::iWindowHeight-margin,0); // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+0, margin,0);   // GORNY PRAWY
    glEnd( );

    glColor4f(0.8f, 0.8f, 0.8f, 0.7f);
    freetype::print(our_font18, 10, Global::iWindowHeight-20, "ZAMYKANIE PROGRAMU - POTWIERDZENIE ");
    freetype::print(our_font18, 120, Global::iWindowHeight-220, "Czy na pewno chesz zakonczyc dzialanie programu? [Y]/[N]");

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_FOG );
    glEnable( GL_LIGHTING );
}


bool __fastcall TWorld::RenderINFOPANEL(int num, double alpha)
{
  QGlobal::bTUTORIAL = false;
  QGlobal::bKEYBOARD = false;

  float margin = 1;
  int lc = QGlobal::CONSISTF->Count;
  int iWH = Global::iWindowHeight;

  glDisable( GL_LIGHTING );
  glDisable( GL_FOG );
  glEnable( GL_TEXTURE_2D );



  glColor4f(0.1,0.1,0.1, alpha);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
  glBegin( GL_QUADS );
  glTexCoord2f(0, 1); glVertex3i(margin-0,   margin, 0);   // GORNY LEWY
  glTexCoord2f(0, 0); glVertex3i(margin-0,   iWH-margin, 0); // DOLY LEWY
  glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+0, iWH-margin, 0); // DOLNY PRAWY
  glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+0, margin, 0);   // GORNY PRAWY
  glEnd( );

  // TLO BOCZNE
  glColor4f(0.0,0.0,0.0, 0.4);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
  glBegin( GL_QUADS );
  glTexCoord2f(0, 1); glVertex3i(margin-0,   40+margin, 0);   // GORNY LEWY
  glTexCoord2f(0, 0); glVertex3i(margin-0,   iWH-margin, 0); // DOLY LEWY
  glTexCoord2f(1, 0); glVertex3i(300-margin+0, iWH-margin, 0); // DOLNY PRAWY
  glTexCoord2f(1, 1); glVertex3i(300-margin+0, 40+margin, 0);   // GORNY PRAWY
  glEnd( );

  // ETYKIETA PANELU
  glColor4f(0.0,0.0,0.0, 0.4);
  glBegin( GL_QUADS );
  glTexCoord2f(0, 1); glVertex3i(margin-0,   margin, 0);   // GORNY LEWY
  glTexCoord2f(0, 0); glVertex3i(margin-0,   40, 0); // DOLY LEWY
  glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+0, 40-margin, 0); // DOLNY PRAWY
  glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+0, margin, 0);   // GORNY PRAWY
  glEnd( );

  
  if (num == 1)  // PANEL 1 - INFORMACJE O SKLADZIE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   {
    int posy = 60;
    for (int l = 0; l<lc; l++)
    {
     glColor4f(0.9f, 0.7f, 0.1f, 0.7f);
     freetype::print(our_font16, 10, Global::iWindowHeight-20, AnsiString("INFORMACJE O SKLADZIE POCIAGU " + Controlled->asTrainNumber).c_str());
     freetype::print(our_font10, 10, Global::iWindowHeight-posy, QGlobal::CONSISTF->Strings[l].c_str());
     posy+= 20;
    }
   }

  if (num == 2)  // PANEL 2 - INFORMACJE O STACJI ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   {
    int posy = 160;
    glColor4f(0.9f, 0.7f, 0.1f, 0.7f);
    freetype::print(our_font16, 10, Global::iWindowHeight-20, "INFORMACJE O STACJI");

    if (QGlobal::iSTATIONPOSINTAB > -1)    // QGlobal::iSTATIONPOSINTAB wyciagane jest w TWorld::Update()
     {
       freetype::print(our_font16, 10, Global::iWindowHeight-20, "INFORMACJE O STACJI");
       freetype::print(our_font10, 20, iWH- 60, AnsiString("ST Name     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].Name).c_str());
       freetype::print(our_font10, 20, iWH- 80, AnsiString("ST Info     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].Info).c_str());
       freetype::print(our_font10, 20, iWH-100, AnsiString("ST Type     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].Type).c_str());
       freetype::print(our_font10, 20, iWH-120, AnsiString("ST SubT     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].SubType).c_str());
       freetype::print(our_font10, 20, iWH-140, AnsiString("ST Platforms: " + IntToStr(QGlobal::station[QGlobal::iSTATIONPOSINTAB].platforms)).c_str());
       freetype::print(our_font10, 20, iWH-160, AnsiString("ST edges    : " + IntToStr(QGlobal::station[QGlobal::iSTATIONPOSINTAB].platformedges)).c_str());
       freetype::print(our_font10, 20, iWH-180, AnsiString("ST tracks n : " + IntToStr(QGlobal::station[QGlobal::iSTATIONPOSINTAB].tracksnum)).c_str());

       int itempos = Global::findstationbyname(QGlobal::station[QGlobal::iSTATIONPOSINTAB].Name);
       int stracks = QGlobal::station[itempos].tracksnum;
       
     //  WriteLog(QGlobal::station[QGlobal::iSTATIONPOSINTAB].Name + ", " + IntToStr(id) + ", " + IntToStr(QGlobal::station[id].tracksnum) ) ;
     posy = 220;
     freetype::print(our_font10, 10, Global::iWindowHeight-posy, "numer   | dlugosc | elektr. | peron i dlugosc");
     posy = 240;
       for (int j = 0; j < stracks; j++)
         {
          AnsiString ELECTRIFIED = BoolToYN(QGlobal::station[itempos].trackinfo[j].electrified);
          AnsiString LENGTH =      IntToStr(QGlobal::station[itempos].trackinfo[j].len);
          AnsiString NUMBER =               QGlobal::station[itempos].trackinfo[j].number.c_str();
          AnsiString PERON =     StrToPERON(QGlobal::station[itempos].trackinfo[j].platformav.c_str());
          AnsiString PERONL =      IntToStr(QGlobal::station[itempos].trackinfo[j].platformlen) + "m";

          freetype::print(our_font10, 20, iWH-posy, AnsiString("  tor " + NUMBER + " : " + LENGTH + "m,   " + ELECTRIFIED  + ",     " + PERON + " " + PERONL).c_str());
          posy+=20;
         }
     posy+=20;
     freetype::print(our_font10, 12, Global::iWindowHeight-posy, "linie wychodzace i pierwsza stacja na nich");
     }
   }

  glEnable( GL_TEXTURE_2D );
  glEnable( GL_FOG );
  glEnable( GL_LIGHTING );
  true;
}


bool __fastcall TWorld::RenderMCURSOR(int type)
{
    getalphablendstate();

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.1, 1.1, 1.1, 0.6);
    glBindTexture(GL_TEXTURE_2D, QGlobal::mousepoint);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(QGlobal::iMPX+0,  QGlobal::iMPY+0,  0);  // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(QGlobal::iMPX+0,  QGlobal::iMPY+30, 0);  // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(QGlobal::iMPX+30, QGlobal::iMPY+30, 0);  // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(QGlobal::iMPX+30, QGlobal::iMPY+0,  0);  // GORNY PRAWY
    glEnd( );

    glEnable( GL_LIGHTING );
    glEnable(GL_FOG);
    setalphablendstate();
}

bool __fastcall renderpanel(int posx, int width, int posy, int height)
{
//
}

bool __fastcall TWorld::RenderMOUSE(double alpha)
{
    QGlobal::bTUTORIAL = false;
    QGlobal::bKEYBOARD = false;
    QGlobal::infotype = 0;
    float margin = 1;

    getalphablendstate();

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.7, 0.7, 0.7, 0.6);

    glBindTexture(GL_TEXTURE_2D, QGlobal::mousesymbol);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(Global::iWindowWidth-64,   margin,0);        // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(Global::iWindowWidth-64,   64,0);            // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin, 64,0);          // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin, margin,0);      // GORNY PRAWY
    glEnd( );

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_LIGHTING );
    glDisable(GL_BLEND);
    glEnable( GL_FOG );

    setalphablendstate();
}


bool __fastcall TWorld::RenderTUTORIAL(int type)
{
    float margin = 1;
    QGlobal::infotype = 0;
    getalphablendstate();

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity( );

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glEnable(GL_TEXTURE_2D);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1,0.1,0.1, QGlobal::GUITUTOPAC);
    glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(margin-0,   margin,0);                       // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(margin-0,   Global::iWindowHeight-margin,0); // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+0, Global::iWindowHeight-margin,0); // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+0, margin,0);    // GORNY PRAWY
    glEnd( );

    glColor4f(0.8f, 0.8f, 0.8f, 0.7f);

    if (type == 1) RenderInformation(7);
    if (type == 2) RenderInformation(8);

    //glColor4f(1.1, 1.1, 1.1, 1.0f);
    glEnable( GL_LIGHTING );
    glDisable(GL_BLEND);
    glEnable(GL_FOG);
    setalphablendstate();
}


bool __fastcall TWorld::RenderINFOX(int node)
{
 if (node > 0)
   {
    getalphablendstate();

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glEnable( GL_TEXTURE_2D);

    float trans = 0.35;
    float margin = 1;
    int i =1;

  //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1,0.1,0.1, QGlobal::GUITUTOPAC);
    glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-1024);  // dol lewy
    glVertex2i(GWW-1,GWH-1024);  //dol prawy
    glVertex2i(GWW-1,GWH); // gora
    glVertex2i(0,GWH);    // gora
    glEnd();

    glColor4f(0.9,0.6,0.0,0.7);
    BFONT->Begin();
    BFONT->Print_scale(2,0,AnsiString(IntToStr(node) + ": ").c_str(), 1,0.9,0.9);
    BFONT->End();


    glDisable(GL_LIGHTING);
    
    if (node ==  1) RenderInformation(node);
    if (node ==  2) RenderInformation(node);
    if (node ==  3) RenderInformation(node);
    if (node ==  4) RenderInformation(node);
    if (node ==  5) RenderInformation(node);
    if (node ==  6) RenderInformation(node);
    if (node ==  7) RenderInformation(node);
    if (node ==  8) RenderInformation(node);
    if (node ==  9) RenderInformation(node);
    if (node == 11) RenderInformation(node);
    if (node == 33) RenderInformation(node);
    if (node == 55) RenderInformation(node);
    if (node == 66) RenderInformation(node);
    if (node == 10) RenderInformation(node);

    glEnable( GL_LIGHTING );
    glDisable(GL_BLEND);
    glEnable(GL_FOG);
    setalphablendstate();
   }

  //  glLineWidth(0.2);
 return true;
}



bool __fastcall TWorld::RenderELEMENTINFO(int node, AnsiString KEY)
{
 if (node > 0)
   {
    float margin = 1;
    int i =1;
    int pm = 0;
    int eid = -1;

    getalphablendstate();
    glDisable( GL_FOG );
    glDisable( GL_LIGHTING );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.2,0.2,0.2, 0.2);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(margin-pm,   margin,0);   // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(margin-pm,   110-margin,0); // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+pm, 110-margin,0); // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+pm, margin,0);   // GORNY PRAWY
    glEnd( );

    //TMoverParameters *mvparam = Controlled->ControlledFind()->MoverParameters;;

    //mvControlled = Controlled->ControlledFind()->MoverParameters;

    BFONT->Begin();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    glColor4f(0.9, 0.8, 0.4, 0.6);
    BFONT->Print_scale(2, 0, AnsiString("10: ").c_str(), 1,0.9,0.9);
    BFONT->Print_scale(2, i++, AnsiString("INFORMACJE O PRZELACZNIKACH I WSKAZNIKACH").c_str(), 0, 1.1, 1.1);


         if (mvControlled != NULL)
         {
          QGlobal::cabelementid = 0;
         //BFONT->Print_scale(2,i++, AnsiString("> keycode         " + KEY ).c_str(), 1,0.7, 0.7);

         if (KEY == "0.70-0.70-0.71") eid = ce_ggCupboard3LButton;
         if (KEY == "0.70-0.70-0.72") eid = ce_ggCupboard3RButton;
         if (KEY == "0.70-0.70-0.73") eid = ce_ggAshTray1Button;
         if (KEY == "0.70-0.70-0.74") eid = ce_ggAshTray2Button;
         if (KEY == "0.70-0.70-0.75") eid = ce_ggHaslerBoltButton;
         if (KEY == "0.70-0.70-0.76") eid = ce_ggHaslerHullButton;

         if (KEY == "0.82-0.81-0.82") eid = ce_ggDirKey;
         if (KEY == "0.95-0.95-0.95") eid = ce_ggMainDistributorButton;
         if (KEY == "0.95-0.95-0.96") eid = ce_ggBatteryButton;
         if (KEY == "0.95-0.95-0.97") eid = ce_ggLZSButton;
         if (KEY == "0.98-0.99-0.91") eid = 1001;
         if (KEY == "0.98-0.99-0.92") eid = 1002;
         if (KEY == "0.98-0.99-0.93") eid = 1003;
         if (KEY == "0.98-0.99-0.94") eid = 1004;

         if (KEY == "0.99-1.00-0.91") eid = 1005;
         if (KEY == "0.99-1.00-0.92") eid = 1006;
         if (KEY == "0.99-1.00-0.93") eid = 1007;

         if (KEY == "0.90-0.90-0.80") eid = ce_iiMainDistributor;
         if (KEY == "0.90-0.90-0.90") eid = ce_iiSpeedometer;
         if (KEY == "0.91-0.90-0.90") eid = ce_iiCASHPSignal;

         if (KEY == "0.81-0.80-0.80") eid = ce_ggMainCtrl;
         if (KEY == "0.82-0.80-0.80") eid = ce_ggScndCtrl;
         if (KEY == "0.83-0.80-0.80") eid = ce_ggBrakeCtrl;
         if (KEY == "0.84-0.80-0.80") eid = ce_ggLocalBrake;
         if (KEY == "0.85-0.80-0.80") eid = ce_ggAckermansFlapButton;           // klapa ackermana  ) laczy PG z atmosfera

         if (KEY == "0.90-0.90-0.91") eid = ce_ggAntiSunRButton;   
         if (KEY == "0.90-0.90-0.92") eid = ce_ggAntiSunCButton;
         if (KEY == "0.90-0.90-0.93") eid = ce_ggAntiSunLButton;
         if (KEY == "0.90-0.90-0.88") eid = ce_ggArmChairRButton;
         if (KEY == "0.90-0.90-0.89") eid = ce_ggArmChairLButton;
         if (KEY == "0.90-0.90-0.87") eid = ce_ggVechDriveTypeButton;
         if (KEY == "0.90-0.90-0.86") eid = ce_ggHandBrakeButton;

         if (KEY == "0.90-0.90-0.98") eid = ce_ggHornButton;
         if (KEY == "0.90-0.90-0.94") eid = ce_iiPlug;
         if (KEY == "0.90-0.90-0.95") eid = ce_ggCupboard1Button;
         if (KEY == "0.90-0.90-0.96") eid = ce_ggCupboard2Button;
         if (KEY == "0.90-0.90-0.97") eid = ce_iiRadioTelefon;
         if (KEY == "0.90-0.90-0.99") eid = ce_ggCookerFlapButton;              // kuchenka pokrywa
         if (KEY == "0.91-0.91-0.90") eid = ce_iiCASHPCase1;
         if (KEY == "0.91-0.91-0.91") eid = ce_iiCASHPCase2;
         if (KEY == "0.91-0.91-0.92") eid = ce_iiTable;
         if (KEY == "0.91-0.91-0.93") eid = ce_iiHomologation;
         if (KEY == "0.91-0.91-0.94") eid = ce_iiCASHPSignal;

         if (KEY == "0.91-0.91-0.96") eid = ce_ggDoorPMButton;                  // drzwi przedzialu maszyn
         if (KEY == "0.91-0.91-0.97") eid = ce_ggDoorWNButton;                  // drzwi przedzialu wn
         if (KEY == "0.91-0.91-0.98") eid = ce_ggPantoAirSupplyButton;          // odciecie powietrza do pantografu
         if (KEY == "0.91-0.91-0.99") eid = ce_ggDoorWNBoltButton;              // rygiel drzwi wn
         if (KEY == "0.92-0.92-0.90") eid = ce_ggHangerButton;                  // wieszak
         if (KEY == "0.92-0.92-0.91") eid = ce_ggDoorCabLButton;                // drzwi kabiny lewe
         if (KEY == "0.92-0.92-0.92") eid = ce_ggDoorCabRButton;                // drzwi kabiny prawe
         if (KEY == "0.92-0.92-0.93") eid = ce_ggCabWinLButton;                 // okno lewe
         if (KEY == "0.92-0.92-0.94") eid = ce_ggCabWinRButton;                 // okno prawe

         //if (KEY == "0.80-0.95-0.95") eid = ce_ggTrainHeatingButton;            // ogrzewanie skladu
         if (KEY == "0.81-0.95-0.95") eid = 1035;
         if (KEY == "0.82-0.95-0.95") eid = 1036;
         if (KEY == "0.83-0.95-0.95") eid = 1037;
         if (KEY == "0.84-0.95-0.95") eid = 1038;
         if (KEY == "0.85-0.95-0.95") eid = 1039;
         if (KEY == "0.86-0.95-0.95") eid = 1040;
         if (KEY == "0.87-0.95-0.95") eid = 1041;
         if (KEY == "0.88-0.95-0.95") eid = 1042;
         if (KEY == "0.89-0.95-0.95") eid = 1043;
         if (KEY == "0.90-0.95-0.95") eid = 1044;

         if (KEY == "0.99-0.99-0.91") eid = 1045;
         if (KEY == "0.99-0.99-0.92") eid = 1046;

         if (KEY == "0.93-0.94-0.90") eid = ce_ggMainOffButton;
         if (KEY == "0.93-0.94-0.91") eid = ce_ggMainOnButton;
         if (KEY == "0.93-0.93-0.94") eid = 1049;
         if (KEY == "0.93-0.94-0.93") eid = ce_ggSecurityResetButton;
         if (KEY == "0.93-0.94-0.94") eid = ce_ggReleaserButton;
         if (KEY == "0.93-0.94-0.95") eid = ce_ggAntiSlipButton;
         if (KEY == "0.93-0.94-0.96") eid = ce_ggFuseButton;
         if (KEY == "0.93-0.94-0.97") eid = ce_ggConverterFuseButton;
         if (KEY == "0.93-0.94-0.98") eid = ce_ggStLinOffButton;
         if (KEY == "0.93-0.94-0.99") eid = ce_ggFootSandButton;

         if (KEY == "0.97-0.97-0.90") eid = ce_ggCompressorButton;
         if (KEY == "0.97-0.97-0.91") eid = ce_ggConverterButton;
         if (KEY == "0.97-0.97-0.92") eid = ce_ggPantFrontButton;
         if (KEY == "0.97-0.97-0.93") eid = ce_ggPantRearButton;
         if (KEY == "0.97-0.97-0.94") eid = ce_ggTrainHeatingButton;            //train heating +
         if (KEY == "0.97-0.97-0.95") eid = ce_ggResistanceFanButton;           //resistance fan +

         if (KEY == "0.95-0.96-0.80") eid = ce_ggPantoAirSupplyButton;
         if (KEY == "0.95-0.96-0.81") eid = ce_ggPantoAirSupplyButton;
         if (KEY == "0.95-0.96-0.82") eid = ce_ggCabLightButton;                //+
         if (KEY == "0.95-0.96-0.83") eid = ce_ggCabLightDimButton;             //+
         if (KEY == "0.95-0.96-0.84") eid = ce_ggHeadLightADimButton;           //+
         if (KEY == "0.95-0.96-0.85") eid = ce_ggHeadLightRDimButton;           //+
         if (KEY == "0.95-0.96-0.86") eid = ce_ggCabHeatingButton;              //+
         if (KEY == "0.95-0.96-0.87") eid = ce_ggVechDriveTypeButton;           //+
         if (KEY == "0.95-0.96-0.88") eid = ce_ggSHPDimButton;                  //+
         if (KEY == "0.95-0.96-0.89") eid = ce_ggRadioAlarmButton;              //+

         if (KEY == "0.96-0.96-0.92") eid = ce_ggWiperLButton;
         if (KEY == "0.96-0.96-0.93") eid = ce_ggWiperRButton;
         if (KEY == "0.96-0.96-0.94") eid = ce_ggCookerButton;                  //kuchenka elektryczna +

         if (KEY == "0.82-0.81-0.81") eid = ce_ggBrakeProfileCtrl;
         if (KEY == "0.83-0.81-0.81") eid = ce_ggMaxCurrentCtrl;
         if (KEY == "0.84-0.81-0.81") eid = 1069;

         if (KEY == "0.94-0.94-0.90") eid = 1070;
         if (KEY == "0.94-0.94-0.91") eid = 1071;

         if (KEY == "0.94-0.94-0.93") eid = 1072;
         if (KEY == "0.94-0.94-0.94") eid = 1073;
         if (KEY == "0.94-0.94-0.95") eid = 1074;

         
         if (eid > 0) QGlobal::cabelementid = eid;
         if (eid > 0) RenderELEMENTDESC(eid);
        }
    BFONT->End();
    
    glEnable( GL_LIGHTING );
    glDisable(GL_BLEND);
    glEnable(GL_FOG);
    setalphablendstate();
   }

 return true;
}


bool __fastcall TWorld::RenderELEMENTDESC(int elementid)
{
 AnsiString description;
 int i = 4;

 if (elementid == ce_ggDirKey) { description = "Klucz nawrotnika ";}
 if (elementid == ce_ggMainDistributorButton) { description = "wl rozrzadu glownego ";}
 if (elementid == ce_ggBatteryButton) { description = "przelacznik baterii ";}
 if (elementid == ce_ggLZSButton) { description = "lacznik zwierajacy sprezarki ";}
 if (elementid == ce_ggAckermansFlapButton) { description = "Klapa Ackermana ";}
 if (elementid == ce_ggDoorPMButton) { description = "Drzwi przedzialu maszynowego ";}
 if (elementid == ce_ggDoorWNButton) { description = "Drzwi przedzialu WN ";}
 if (elementid == ce_ggPantoAirSupplyButton) { description = "Odciecie powietrza do pantografu ";}
 if (elementid == ce_ggDoorWNBoltButton) { description = "Rygiel drzwi WN ";}
 if (elementid == ce_ggHangerButton) { description = "Na tym mozezsz sie powiesic po spowodowaniu wypadku ";}
 if (elementid == ce_ggDoorCabLButton) { description = "Drzwi wejsciowe lewe ";}
 if (elementid == ce_ggDoorCabRButton) { description = "Drzwi wejsciowe prawe ";}
 if (elementid == ce_ggCabWinLButton) { description = "okno ";}
 if (elementid == ce_ggCabWinRButton) { description = "okno ";}
 if (elementid == 1001) { description = "Amperomierz I ";}
 if (elementid == 1002) { description = "Amperomierz II";}
 if (elementid == 1003) { description = "Woltomierz WN";}
 if (elementid == 1004) { description = "Woltomierz NN";}
 if (elementid == 1005) { description = "Manometr Cylinder Hamulcowy";}
 if (elementid == 1006) { description = "Manometr Przewod glowny";}
 if (elementid == 1007) { description = "Manometr Zbiornik glowny";}
 if (elementid == ce_iiMainDistributor) { description = "Rozrzad glowny";}
 if (elementid == ce_iiSpeedometer) { description = "Predkosciomierz";}
 if (elementid == ce_iiCASHPSignal) { description = "Sygnalizator CA/SHP";}
 if (elementid == ce_ggMainCtrl) { description = "Nastawnik";}
 if (elementid == ce_ggScndCtrl) { description = "Bocznikowanie";}
 if (elementid == ce_ggBrakeCtrl) { description = "Hamulec zasadniczy";}
 if (elementid == ce_ggLocalBrake) { description = "Hamulec pomocniczy";}
 if (elementid == ce_ggAntiSunRButton) { description = "Przyslona przeciwsloneczna";}
 if (elementid == ce_ggAntiSunCButton) { description = "Przyslona przeciwsloneczna";}
 if (elementid == ce_ggAntiSunLButton) { description = "Przyslona przeciwsloneczna";}
 if (elementid == ce_ggArmChairRButton) { description = "Fotel maszynisty";}
 if (elementid == ce_ggArmChairLButton) { description = "Fotel pomocnika maszynisty";}
 if (elementid == ce_ggVechDriveTypeButton) { description = "Przelacznik rodzaju jazdy";}
 if (elementid == ce_ggHandBrakeButton) { description = "Hamulec reczny lokomotywy";}
 if (elementid == ce_ggHornButton) { description = "Syrena";}
 if (elementid == ce_iiPlug) { description = "Gniazdko 110V";}
 if (elementid == ce_ggCupboard1Button) { description = "szafka przelacznika baterii";}
 if (elementid == ce_ggCupboard2Button) { description = "szafka rozrzadu glownego i nadmiarowyvh";}
 if (elementid == ce_ggCupboard3LButton) { description = "szafka na pierdoly i piwo";}
 if (elementid == ce_ggCupboard3RButton) { description = "szafka na pierdoly i piwo";}
 if (elementid == ce_ggAshTray1Button) { description = "popielniczka";}
 if (elementid == ce_ggAshTray2Button) { description = "popielniczka";}
 if (elementid == ce_ggHaslerBoltButton) { description = "Pokretlo otwierania";}
 if (elementid == ce_ggHaslerHullButton) { description = "Hasler (pokrywa)";}
 if (elementid == ce_iiRadioTelefon) { description = "Radiotelefon";}
 if (elementid == ce_ggCookerFlapButton) { description = "Kuchenka";}
 if (elementid == ce_iiCASHPCase1) { description = "Szafa CA/SHP 1";}
 if (elementid == ce_iiCASHPCase2) { description = "Szafa CA/SHP 2";}
 if (elementid == ce_iiTable) { description = "Stolik";}
 if (elementid == ce_iiHomologation) { description = "Homologacja pojazdu";}
 if (elementid == ce_iiCASHPSignal) { description = "Sygnalizator dzwiekowy CA/SHP";}

 if (elementid == 1034) { description = "Sygnalizator ogrzewania skladu"; }
 if (elementid == 1035) { description = "Sygnalizator jazdy na oporach"; }
 if (elementid == 1036) { description = "Sygnalizator wysokiego rozruchu"; }
 if (elementid == 1037) { description = "Sygnalizator poslizgu"; }
 if (elementid == 1038) { description = "Sygnalizator stycznikow liniowych"; }
 if (elementid == 1039) { description = "Sygnalizator bezpiecznika nadm przetwornicy"; }
 if (elementid == 1040) { description = "Sygnalizator roznicowego"; }
 if (elementid == 1041) { description = "Sygnalizator bezpiecznika nadm silnikow"; }
 if (elementid == 1042) { description = "Sygnalizator stanu wyl szybkiego"; }
 if (elementid == 1043) { description = "Sygnalizator bezpiecznika nadm wentylatorow"; }
 if (elementid == 1044) { description = "Sygnalizator bezpiecznika nadm sprezarki"; }
 if (elementid == 1045) { description = "Przycisk nozny piasecznicy";  }
 if (elementid == 1046) { description = "Przycisk nozny CA/SHP"; }

 if (elementid == ce_ggMainOffButton) { description = "Obwod glowny wyl."; }
 if (elementid == ce_ggMainOnButton) { description = "Obwod glowny zal."; }
 if (elementid == ce_ggSecurityResetButton) { description = "przycisk skasowania CA/SHP"; }
 if (elementid == ce_ggReleaserButton) { description = "odluzniacz hamulca"; }
 if (elementid == ce_ggAntiSlipButton) { description = "antyposlizg"; }
 if (elementid == ce_ggFuseButton) { description = "Odblok. nadmiarowego silnikow"; }
 if (elementid == ce_ggConverterFuseButton) { description = "odblok przekaznika nadm przetwornic i ogrzewania"; }
 if (elementid == ce_ggStLinOffButton) { description = "odblok stycznikow liniowych"; }
 if (elementid == ce_ggFootSandButton) { description = "Pedal piasecznicy"; }
 if (elementid == ce_ggRadioAlarmButton) { description = "Przycisk alarmu radiowego (zaplombowany)"; }
 if (elementid == ce_ggCompressorButton) { description = "Wlacznik sprezarki"; }
 if (elementid == ce_ggConverterButton) { description = "Wlacznik przetwornicy"; }
 if (elementid == ce_ggPantFrontButton) { description = "Pantograf przedni"; }
 if (elementid == ce_ggPantRearButton) { description = "Pantograf tylny"; }
 if (elementid == ce_ggTrainHeatingButton) { description = "Ogrzewanie pociagu"; }
 if (elementid == ce_ggResistanceFanButton) { description = "Wentylatory oporow"; }
 if (elementid == ce_ggWiperRButton) { description = "Wycieraczka prawa"; }
 if (elementid == ce_ggWiperLButton) { description = "Wycieraczka lewa"; }
 if (elementid == ce_ggCookerButton) { description = "wl kuchenki"; }
 if (elementid == ce_ggBrakeProfileCtrl) { description = "Przelacznik typu hamulca"; }
 if (elementid == ce_ggMaxCurrentCtrl) { description = "Przelacznik pradu rozruchu"; }
 if (elementid == 1069) { description = "Przelacznik dost sil do niacisku kol"; }
 if (elementid == ce_ggSHPDimButton) { description = "Przyciemnienie sygnalizacji CA/SHP"; }
 if (elementid == ce_ggHeadLightADimButton) { description = "Przyciemnienie czola pociagu"; }
 if (elementid == ce_ggHeadLightRDimButton) { description = "Przyciemnienie prawego reflektora"; }
 if (elementid == ce_ggCabHeatingButton) { description = "Ogrzewanie kabiny"; }

/*
 {BFONT->Print_scale(2,i++, AnsiString("kamera poprzednia" ).c_str(), 1,0.9, 0.9); }
 {BFONT->Print_scale(2,i++, AnsiString("kamera nastepna" ).c_str(), 1,0.9, 0.9); }
 {BFONT->Print_scale(2,i++, AnsiString("kamera wylacz" ).c_str(), 1,0.9, 0.9); }
 */

 BFONT->Print_scale(2,3, AnsiString( elementid ).c_str(), 1,0.9, 0.9);
 BFONT->Print_scale(2,i++, AnsiString( description ).c_str(), 1,0.9, 0.9);
 glColor4f(0.9, 0.9, 0.9, 0.7);
 if (QGlobal::showelementdescatpointer) BFONT->Print_xy_scale(QGlobal::iMPX+20, (Global::iWindowHeight-QGlobal::iMPY)-10, description.c_str(), 1, 0.7, 0.7);
}


// *****************************************************************************
// LOADING LOADER CONFIG *******************************************************
// *****************************************************************************

bool __fastcall TWorld::LOADLOADERCONFIG()
{

 WriteLog("LOADING LOADER CONFIG...");
 AnsiString LINE, TEST, PAR;
 TStringList *LOADERCFG;
 LOADERCFG = new TStringList;
 LOADERCFG->LoadFromFile(QGlobal::asAPPDIR + "data\\loaderconf.txt");


 for (int l= 0; l < LOADERCFG->Count; l++)
      {
       LINE = LOADERCFG->Strings[l];
       TEST = LINE.SubString(1, LINE.Pos(":"));
       PAR =  LINE.SubString(LINE.Pos(":")+2, 255);
       //WriteLog(TEST + ", [" + PAR + "]");

       if (TEST == "COLOR_R:") LDR_COLOR_R = StrToFloat(PAR);
       if (TEST == "COLOR_G:") LDR_COLOR_G = StrToFloat(PAR);
       if (TEST == "COLOR_B:") LDR_COLOR_B = StrToFloat(PAR);
       if (TEST == "STR_1_T:") LDR_STR_LOAD = PAR;
       if (TEST == "STR_1_R:") LDR_STR_1_R = StrToFloat(PAR);
       if (TEST == "STR_1_G:") LDR_STR_1_G = StrToFloat(PAR);
       if (TEST == "STR_1_B:") LDR_STR_1_B = StrToFloat(PAR);
       if (TEST == "STR_1_A:") LDR_STR_1_A = StrToFloat(PAR);

       if (TEST == "STR_2_T:") LDR_STR_FRST = PAR;

       if (TEST == "TBACK_R:") LDR_TBACK_R = StrToFloat(PAR);
       if (TEST == "TBACK_G:") LDR_TBACK_G = StrToFloat(PAR);
       if (TEST == "TBACK_B:") LDR_TBACK_B = StrToFloat(PAR);
       if (TEST == "TBACK_A:") LDR_TBACK_A = StrToFloat(PAR);

       if (TEST == "PBARLEN:") LDR_PBARLEN = StrToFloat(PAR);
       if (TEST == "PBAR__R:") LDR_PBAR__R = StrToFloat(PAR);
       if (TEST == "PBAR__G:") LDR_PBAR__G = StrToFloat(PAR);
       if (TEST == "PBAR__B:") LDR_PBAR__B = StrToFloat(PAR);
       if (TEST == "PBAR__A:") LDR_PBAR__A = StrToFloat(PAR);

       if (TEST == "MLOGOON:") LDR_LOGOVIS = StrToFloat(PAR);
       if (TEST == "MLOGO_X:") LDR_MLOGO_X = StrToFloat(PAR);
       if (TEST == "MLOGO_Y:") LDR_MLOGO_Y = StrToFloat(PAR);
       if (TEST == "MLOGO_A:") LDR_MLOGO_A = StrToFloat(PAR);

       if (TEST == "BRIEFON:") LDR_DESCVIS = StrToFloat(PAR);
       if (TEST == "BRIEF_X:") LDR_BRIEF_X = StrToFloat(PAR);
       if (TEST == "BRIEF_Y:") LDR_BRIEF_Y = StrToFloat(PAR);
       if (TEST == "REFRESH:") QGlobal::LDRREFRESH = StrToFloat(PAR);

       //if (TEST == "SKYLO_MODEL:") SKYLO_MODEL = PAR;
       //if (TEST == "SKYHI_GLBLENDFUNC:") if (PAR == "GL_SRC_ALPHA,GL_ONE") SKYHI_BLENDFUNC = 1;
       //if (TEST == "SKYLO_GLBLENDFUNC:") if (PAR == "GL_SRC_ALPHA,GL_ONE") SKYLO_BLENDFUNC = 1;
       //if (TEST == "SKYHI_GLENABLE:") if (PAR == "1") SKYHI_BLEND = 1;
       //if (TEST == "SKYLO_GLENABLE:") if (PAR == "1") SKYLO_BLEND = 1;
      }

      delete LOADERCFG;
}


// *****************************************************************************
// LOADING LOADER FONTS ********************************************************
// *****************************************************************************

bool __fastcall TWorld::LOADLOADERFONTS()
{
 WriteLog("LOADING LOADER FONTS...");
 our_font10.init(AnsiString(QGlobal::asAPPDIR + "data\\fonts\\arial.ttf").c_str(), 10);
 our_font12.init(AnsiString(QGlobal::asAPPDIR + "data\\fonts\\arial.ttf").c_str(), 12);
 our_font14.init(AnsiString(QGlobal::asAPPDIR + "data\\fonts\\creditvz.ttf").c_str(), 14);
 our_font16.init(AnsiString(QGlobal::asAPPDIR + "data\\fonts\\creditvz.ttf").c_str(), 16);
 our_font18.init(AnsiString(QGlobal::asAPPDIR + "data\\fonts\\creditvz.ttf").c_str(), 18);			        //Build the freetype font

 if (!floaded) BFONT = new Font();
 if (!floaded) BFONT->init("none");
 floaded = true;
 return true;
}


// *****************************************************************************
// LOADING LOADER TEXTURES *****************************************************
// *****************************************************************************

bool __fastcall TWorld::LOADLOADERTEXTURES()
{
    WriteLog("LOADING LOADER TEXTURES...");

    AnsiString cscn = Global::szSceneryFile;
    AnsiString clok = Global::asHumanCtrlVehicle;
    AnsiString asBRIEFFILE = "data\\briefs\\briefbackg.tga";                        // Kartka z opisem
    AnsiString asSCNBACKG =  "data\\lbacks\\" + cscn + QGlobal::asLBACKEXT;         // tlo wczytywania
    AnsiString asBRIEFTEXT = "data\\briefs\\" + cscn + "-" + clok + ".txt";         // opis misji

    //WriteLog(asBRIEFTEXT);
    if (FEX(asBRIEFTEXT)) QGlobal::MBRIEF->LoadFromFile( asBRIEFTEXT);

    if (!FEX(asBRIEFTEXT)) QGlobal::bloaderbriefing = false;
    if ( FEX(asBRIEFTEXT)) QGlobal::bloaderbriefing = true;

    WriteLog("Loading -briefing: " + asBRIEFTEXT);

    Global::asCurrentTexturePath = QGlobal::asAPPDIR;

    loaderbrief = TTexturesManager::GetTextureID(szTexturePath, Global::asCurrentTexturePath.c_str(), AnsiString(asBRIEFFILE).c_str());
    QGlobal::bfonttex = TTexturesManager::GetTextureID(szTexturePath, Global::asCurrentTexturePath.c_str(), AnsiString("data\\menu\\menu_xfont.bmp").c_str());

    if (!FEX(asSCNBACKG)) loaderbackg = TTexturesManager::GetTextureID(szTexturePath, Global::asCurrentTexturePath.c_str(), AnsiString("data\\lbacks\\lbackgdef" + QGlobal::asLBACKEXT).c_str());
    if ( FEX(asSCNBACKG)) loaderbackg = TTexturesManager::GetTextureID(szTexturePath, Global::asCurrentTexturePath.c_str(), AnsiString(asSCNBACKG).c_str());

    Global::asCurrentTexturePath = AnsiString(szTexturePath);
 return true;
}


// *****************************************************************************
// RYSOWANIE OPCJONALNEGO SPLASH SCREENU POPRZEDZAJACEGO EKRAN WCZYTYWANIA
// *****************************************************************************
bool __fastcall TWorld::RenderSPLASHSCR(HDC hDC, int node, AnsiString text, double alpha)
{
 float et;
 float st = GetTickCount();
 float pt= 0;
 bool sndok = false;
 QGlobal::fscreenfade = 1;
 while (pt < 7000)      // Przez 7s renderujemy splasha az do zanikniecia dzwieku...
   {
    et = GetTickCount();
    pt = et-st;

    if ((pt > 150) && (!sndok) && FileExists("data\\sounds\\START.WAV")) PlaySound("data\\sounds\\START.WAV", NULL, SND_ASYNC);
    if ((pt > 150) && (!sndok)) sndok = true;

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
    glClearColor (1, 1, 1, 0.99);     // 09 07 04 07
    glDisable(GL_DEPTH_TEST);			// Disables depth testing

    glColor4f(1.0,1.0,1.0,1);
    int margin = 0;
    int pm = 0;

    //QGlobal::aspectratio = 43;     //
   // OBRAZEK LOADERA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   if (QGlobal::aspectratio == 169)
   {
    glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(margin-174,   margin,0);   // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(margin-174,   Global::iWindowHeight-margin,0); // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+174, Global::iWindowHeight-margin,0); // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+174, margin,0);   // GORNY PRAWY
    glEnd( );
   }
   if (QGlobal::aspectratio == 43)
   {
    float xp = -340;
    glBindTexture(GL_TEXTURE_2D, QGlobal::splashscreen);
    glBegin( GL_QUADS );
    glTexCoord2f(0, 1); glVertex3i(margin-pm+xp,   margin,0);   // GORNY LEWY
    glTexCoord2f(0, 0); glVertex3i(margin-pm+xp,   Global::iWindowHeight-margin,0); // DOLY LEWY
    glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+pm-xp, Global::iWindowHeight-margin,0); // DOLNY PRAWY
    glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+pm-xp, margin,0);   // GORNY PRAWY
    glEnd( );
   }

   int i =1;
   int l =0;
   int g =0;
   glEnable( GL_TEXTURE_2D);
   BFONT->Begin();
   glDisable(GL_TEXTURE_2D);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glColor4f(LDR_TBACK_R, LDR_TBACK_G, LDR_TBACK_B, LDR_TBACK_A);

   g=1;
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
   glEnable(GL_TEXTURE_2D);
   glColor4f(LDR_STR_1_R, LDR_STR_1_G, LDR_STR_1_B, LDR_STR_1_A);
   glColor4f(0.5, 0.5, 0.5, 0.7);  // 09 07 02
   BFONT->Print_scale(75,63,AnsiString(AnsiString(QGlobal::asAPPVERS)).c_str(), 1, 0.7, 0.7);       // WERSJA APLIKACJI, DATA KOMPILACJI
   BFONT->End();
   
   SwapBuffers(hDC);
  }


QGlobal::fscreenfade = 0;
while( QGlobal::fscreenfade < 1.0 ) // Proces plynnego zaciemniania splasha (tzw. fadeoff) ...
    {
    //if (!floaded) BFONT = new Font();
    //if (!floaded) BFONT->init("none");
    //floaded = true;

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    int i =1;
    int margin = 1;
    glEnable( GL_TEXTURE_2D);
    BFONT->Begin();

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,QGlobal::fscreenfade);

    // GORNY
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-1024);  // dol lewy
    glVertex2i(GWW-0,GWH-1024);  //dol prawy
    glVertex2i(GWW-0,GWH); // gora
    glVertex2i(0,GWH);    // gora
    glEnd();

    BFONT->End();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_FOG);
    if (QGlobal::fscreenfade < 1.0) QGlobal::fscreenfade += 0.01;
    Sleep(10);
    SwapBuffers(hDC);
    }


   QGlobal::fscreenfade = 1;
   QGlobal::fscreenfade2 = 1;

   glEnable(GL_TEXTURE_2D);
//   glEnable(GL_FOG);

 return true;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RenderLoaderU() - Zaraz po splashu, stopniowe rozjasnianie ekranu wczytywania
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
bool __fastcall TWorld::RenderLoaderU(HDC hDC, int node, AnsiString text)
{
 QGlobal::fscreenfade = 1;
 while ( QGlobal::fscreenfade > 0.01 )
    {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
    glClearColor (LDR_COLOR_R, LDR_COLOR_G, LDR_COLOR_B, 0.7);     // 09 07 04 07

    glDisable(GL_DEPTH_TEST);			// Disables depth testing
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);


    //if (!floaded) BFONT = new Font();
    //if (!floaded) BFONT->loadf("none");
    //floaded = true;

    glEnable(GL_TEXTURE_2D);

//    if (node != 77) nn = Global::iPARSERBYTESPASSED;

//    Global::postep = (Global::iPARSERBYTESPASSED * 100 / Global::iNODES ); // PROCENT
//    currloading = AnsiString(Global::postep) + "%";
//    currloading_bytes = IntToStr(Global::iPARSERBYTESPASSED);

   // else { currloading = IntToStr(nn) + "N, PIERWSZE WCZYTYWANIE"; }
    currloading_b = text;

   //glColor4f(1.0,1.0,1.0,1);
   glColor4f(LDR_COLOR_R, LDR_COLOR_G, LDR_COLOR_B, 1.0);
   int margin = 1;

   //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   //glEnable(GL_BLEND);
   // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   //glColor4f(0.5,0.45,0.4, 0.5);


    QGlobal::aspectratio = 169;
   // OBRAZEK LOADERA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   if (QGlobal::aspectratio == 43)
   {
   glBindTexture(GL_TEXTURE_2D, loaderbackg);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i(margin-174,   margin,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i(margin-174,   Global::iWindowHeight-margin,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+174, Global::iWindowHeight-margin,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+174, margin,0);   // GORNY PRAWY
   glEnd( );
   }

   if (QGlobal::aspectratio == 169)
   {
   int pm = 0;
   glBindTexture(GL_TEXTURE_2D, loaderbackg);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i(margin-pm,   margin,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i(margin-pm,   Global::iWindowHeight-margin,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+pm, Global::iWindowHeight-margin,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+pm, margin,0);   // GORNY PRAWY
   glEnd( );
   }


   // LOGO PROGRAMU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   /*
   if (LDR_LOGOVIS !=0)
   {
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glColor4f(0.8,0.8,0.8,LDR_MLOGO_A);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, Global::loaderlogo);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i( LDR_MLOGO_X,   LDR_MLOGO_Y,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i( LDR_MLOGO_X,   LDR_MLOGO_Y+100,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i( LDR_MLOGO_X+300, LDR_MLOGO_Y+100,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i( LDR_MLOGO_X+300, LDR_MLOGO_Y,0);   // GORNY PRAWY
   glEnd( );
   }
    */

   // BRIEFING - OPIS MISJI ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   if (LDR_DESCVIS != 0 && QGlobal::bloaderbriefing)
   {
   glDisable(GL_BLEND);
 //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
 //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);  // PRAWIE OK

   glColor4f(1.9,1.9,1.9,1.9);
 //glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, loaderbrief);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i( LDR_BRIEF_X,   LDR_BRIEF_Y,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i( LDR_BRIEF_X,   LDR_BRIEF_Y+1000,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i( LDR_BRIEF_X+500, LDR_BRIEF_Y+1000,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i( LDR_BRIEF_X+500, LDR_BRIEF_Y,0);   // GORNY PRAWY
   glEnd( );
   }
   glDisable(GL_BLEND);


   //PROGRESSBAR ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   int PBARY = PBY+68;
   float PBARLEN = Global::iWindowWidth / 100; //LDR_PBARLEN;

   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glEnable(GL_BLEND);
   glBegin(GL_QUADS);
   glColor4f(1,1,1,1); glVertex2f(20, PBARY-2);                                   // gorny lewy
   glColor4f(1,1,1,1); glVertex2f(20, PBARY-1);                                   // dolny lewy
   glColor4f(1,1,1,1); glVertex2f(100*PBARLEN, PBARY-1);                          // dolny prawy
   glColor4f(1,1,1,1); glVertex2f(100*PBARLEN, PBARY-2);                          // gorny prawy
   glEnd();

//  if (Global::bfirstloadingscn) Global::postep = 0;
//  if (Global::bfirstloadingscn) PBARLEN = 3;

  if (!QGlobal::bfirstloadingscn) QGlobal::postep = (QGlobal::iNODES * 100 / QGlobal::iPARSERBYTESPASSED );

   if (QGlobal::bfirstloadingscn)   // PRZY PIERWSZYM WCZYTYWANIU PASEK POSTEPU JEST USTAWIONY NA 100%
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        //glEnable(GL_BLEND);
        glColor4f(LDR_PBAR__R, LDR_PBAR__G, LDR_PBAR__B, LDR_PBAR__A);
        glBegin(GL_QUADS);
        glVertex2f(20, PBARY+2);    // gorny lewy
        glVertex2f(20, PBARY+10);   // dolny lewy
        glVertex2f(100*PBARLEN, PBARY+10);   // dolny prawy
        glVertex2f(100*PBARLEN, PBARY+2);   // gorny prawy
        glEnd();
       }

    else
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glColor4f(LDR_PBAR__R, LDR_PBAR__G, LDR_PBAR__B, LDR_PBAR__A);
        glBegin(GL_QUADS);
        glVertex2f(20, PBARY+2);    // gorny lewy
        glVertex2f(20, PBARY+10);   // dolny lewy
        glVertex2f(QGlobal::postep*PBARLEN, PBARY+10);   // dolny prawy
        glVertex2f(QGlobal::postep*PBARLEN, PBARY+2);   // gorny prawy
        glEnd();
       }

    
   //glEnable(GL_BLEND);
   //glBlendFunc(GL_SRC_ALPHA,GL_ONE);

   RenderInformation(99);


// ******************

 glEnable(GL_BLEND);

    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    int i =1;

    glEnable( GL_TEXTURE_2D);
    BFONT->Begin();

    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,QGlobal::fscreenfade);

    // GORNY
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-1024);  // dol lewy
    glVertex2i(GWW-0,GWH-1024);  //dol prawy
    glVertex2i(GWW-0,GWH); // gora
    glVertex2i(0,GWH);    // gora
    glEnd();
    BFONT->End();

    glEnable(GL_TEXTURE_2D);
    if (QGlobal::fscreenfade > 0.01)
    QGlobal::fscreenfade -= 0.020;
    Sleep(1);


    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
    glShadeModel(GL_SMOOTH); // Enable Smooth Shading
    glEnable(GL_DEPTH_TEST);


        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.04);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LEQUAL);

   if (!QGlobal::bSCNLOADED) SwapBuffers(hDC);
  }
 return true;
}



// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RenderLoader() - SCREEN WCZYTYWANIA SCENERII ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool __fastcall TWorld::RenderLoader(HDC hDC, int node, AnsiString text)
{
    if (!floaded) BFONT = new Font();
    if (!floaded) BFONT->init("none");
    floaded = true;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
    glClearColor (LDR_COLOR_R, LDR_COLOR_G, LDR_COLOR_B, 0.7);     // 09 07 04 07

    glDisable(GL_DEPTH_TEST);			// Disables depth testing
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glEnable(GL_TEXTURE_2D);

//    if (node != 77) nn = Global::iPARSERBYTESPASSED;

//    Global::postep = (Global::iPARSERBYTESPASSED * 100 / Global::iNODES ); // PROCENT
//    currloading = AnsiString(Global::postep) + "%";
//    currloading_bytes = IntToStr(Global::iPARSERBYTESPASSED);

   // else { currloading = IntToStr(nn) + "N, PIERWSZE WCZYTYWANIE"; }
    currloading_b = text;

   //glColor4f(1.0,1.0,1.0,1);
   glColor4f(LDR_COLOR_R, LDR_COLOR_G, LDR_COLOR_B, 1.0);
   int margin = 1;

   //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   //glEnable(GL_BLEND);
   // glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   //glColor4f(0.5,0.45,0.4, 0.5);


    QGlobal::aspectratio = 169;
   // OBRAZEK LOADERA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   if (QGlobal::aspectratio == 43)
   {
   glBindTexture(GL_TEXTURE_2D, loaderbackg);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i(margin-174,   margin,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i(margin-174,   Global::iWindowHeight-margin,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+174, Global::iWindowHeight-margin,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+174, margin,0);   // GORNY PRAWY
   glEnd( );
   }

   if (QGlobal::aspectratio == 169)
   {
   int pm = 0;
   glBindTexture(GL_TEXTURE_2D, loaderbackg);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i(margin-pm,   margin,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i(margin-pm,   Global::iWindowHeight-margin,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i(Global::iWindowWidth-margin+pm, Global::iWindowHeight-margin,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i(Global::iWindowWidth-margin+pm, margin,0);   // GORNY PRAWY
   glEnd( );
   }


   // LOGO PROGRAMU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   /*
   if (LDR_LOGOVIS !=0)
   {
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glColor4f(0.8,0.8,0.8,LDR_MLOGO_A);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, Global::loaderlogo);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i( LDR_MLOGO_X,   LDR_MLOGO_Y,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i( LDR_MLOGO_X,   LDR_MLOGO_Y+100,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i( LDR_MLOGO_X+300, LDR_MLOGO_Y+100,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i( LDR_MLOGO_X+300, LDR_MLOGO_Y,0);   // GORNY PRAWY
   glEnd( );
   }
    */

   // BRIEFING - OPIS MISJI ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   if (LDR_DESCVIS != 0 && QGlobal::bloaderbriefing)
   {
   glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA,GL_ONE);

// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);  // PRAWIE OK

   glColor4f(1.9,1.9,1.9,0.6);
 //glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, loaderbrief);
   glBegin( GL_QUADS );
   glTexCoord2f(0, 1); glVertex3i( LDR_BRIEF_X,   LDR_BRIEF_Y,0);   // GORNY LEWY
   glTexCoord2f(0, 0); glVertex3i( LDR_BRIEF_X,   LDR_BRIEF_Y+1000,0); // DOLY LEWY
   glTexCoord2f(1, 0); glVertex3i( LDR_BRIEF_X+500, LDR_BRIEF_Y+1000,0); // DOLNY PRAWY
   glTexCoord2f(1, 1); glVertex3i( LDR_BRIEF_X+500, LDR_BRIEF_Y,0);   // GORNY PRAWY
   glEnd( );
   }
   glEnable(GL_BLEND);



   //PROGRESSBAR ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   int PBARY = PBY+68;
   float PBARLEN = Global::iWindowWidth / 100; //LDR_PBARLEN;

   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glEnable(GL_BLEND);
   glBegin(GL_QUADS);
   glColor4f(1,1,1,1); glVertex2f(20, PBARY-2);                                   // gorny lewy
   glColor4f(1,1,1,1); glVertex2f(20, PBARY-1);                                   // dolny lewy
   glColor4f(1,1,1,1); glVertex2f(100*PBARLEN, PBARY-1);                          // dolny prawy
   glColor4f(1,1,1,1); glVertex2f(100*PBARLEN, PBARY-2);                          // gorny prawy
   glEnd();

//  if (Global::bfirstloadingscn) Global::postep = 0;
//  if (Global::bfirstloadingscn) PBARLEN = 3;

  if (!QGlobal::bfirstloadingscn) QGlobal::postep = (QGlobal::iNODES * 100 / QGlobal::iPARSERBYTESPASSED );

   if (QGlobal::bfirstloadingscn)   // PRZY PIERWSZYM WCZYTYWANIU PASEK POSTEPU JEST USTAWIONY NA 100%
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        //glEnable(GL_BLEND);
        glColor4f(LDR_PBAR__R, LDR_PBAR__G, LDR_PBAR__B, LDR_PBAR__A);
        glBegin(GL_QUADS);
        glVertex2f(20, PBARY+2);    // gorny lewy
        glVertex2f(20, PBARY+10);   // dolny lewy
        glVertex2f(100*PBARLEN, PBARY+10);   // dolny prawy
        glVertex2f(100*PBARLEN, PBARY+2);   // gorny prawy
        glEnd();
       }

    else
       {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        //glEnable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glColor4f(LDR_PBAR__R, LDR_PBAR__G, LDR_PBAR__B, LDR_PBAR__A);
        glBegin(GL_QUADS);
        glVertex2f(20, PBARY+2);    // gorny lewy
        glVertex2f(20, PBARY+10);   // dolny lewy
        glVertex2f(QGlobal::postep*PBARLEN, PBARY+10);   // dolny prawy
        glVertex2f(QGlobal::postep*PBARLEN, PBARY+2);   // gorny prawy
        glEnd();
       }

    
   //glEnable(GL_BLEND);
   //glBlendFunc(GL_SRC_ALPHA,GL_ONE);

   RenderInformation(99);


// ******************

 glEnable(GL_BLEND);
// QGlobal::fscreenfade = 1;
if ( QGlobal::fscreenfade > 0.01 )
    {
    GWW = Global::iWindowWidth;
    GWH = Global::iWindowHeight;

    int i =1;
    int margin = 1;
    glEnable( GL_TEXTURE_2D);
    BFONT->Begin();

    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0,0,0,QGlobal::fscreenfade);

    // GORNY
    glBegin(GL_QUADS);
    glVertex2i(0,GWH-1024);  // dol lewy
    glVertex2i(GWW-0,GWH-1024);  //dol prawy
    glVertex2i(GWW-0,GWH); // gora
    glVertex2i(0,GWH);    // gora
    glEnd();

    BFONT->End();

    glEnable(GL_TEXTURE_2D);
   // glEnable(GL_FOG);
   // if (QGlobal::fscreenfade > 0.01)
    QGlobal::fscreenfade -= 0.25;

   // SwapBuffers(hDC);

    }

   glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
   glShadeModel(GL_SMOOTH); // Enable Smooth Shading
   glEnable(GL_DEPTH_TEST);


        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.04);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LEQUAL);

   if (!QGlobal::bSCNLOADED) SwapBuffers(hDC);

 return true;
}

bool __fastcall TWorld::RenderFadeOff(int percent)
{

   return true;
}


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// RENDEROWANIE WSKAZNIKA ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

bool __fastcall TWorld::renderpointerx(double sizepx, int sw, int sh)
{
/*
   int posx1 = (sw/2)-(sizepx/2);
   int posy1 = (sh/2)-(sizepx/2);
   int posx2 = (sw/2)+(sizepx/2);
   int posy2 = (sh/2)+(sizepx/2);

   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glEnable(GL_BLEND);
   glBindTexture(GL_TEXTURE_2D, Global::texpointer);
   glBegin( GL_QUADS );
   glTexCoord2f(1, 1); glVertex3i(posx1,posy1,0);  // GORNY LEWY
   glTexCoord2f(1, 0); glVertex3i(posx1,posy2,0);  // DOLY LEWY
   glTexCoord2f(0, 0); glVertex3i(posx2,posy2,0);  // DOLNY PRAWY
   glTexCoord2f(0, 1); glVertex3i(posx2,posy1,0);  // GORNY PRAWY
   glEnd( );
   glDisable(GL_BLEND);
  */
}




//---------------------------------------------------------------------------
#pragma package(smart_init)










