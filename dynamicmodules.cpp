//---------------------------------------------------------------------------

/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak and others

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

#include "system.hpp"
#include "classes.hpp"

#include "opengl/glew.h"
#include "opengl/glut.h"

#pragma hdrstop

#include "Timer.h"
#include "Texture.h"
#include "Ground.h"
#include "Globals.h"
#include "Event.h"
#include "EvLaunch.h"
#include "TractionPower.h"
#include "Traction.h"
#include "Track.h"
#include "RealSound.h"
#include "AnimModel.h"
#include "MemCell.h"
#include "mtable.hpp"
#include "DynObj.h"
#include "Data.h"
#include "world.h"
#include "parser.h" //Tolaris-010603
#include "Driver.h"
#include "Train.h"
#include "logs.h"
#include "qutils.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

TWorld *W;
TTrain *T;

bool TDynamicObject::LoadAdditionals(AnsiString ADDFILENAME, TDynamicObject *D, TController *M)
{

  AnsiString DYNOBJBDIR =  QGlobal::asAPPDIR + D->asBaseDir;

  ADDFILENAME =  AnsiString(DYNOBJBDIR + D->MoverParameters->Name + ".madd");
  ADDFILENAME =  AnsiString(QGlobal::asAPPDIR + "dynstates\\" + D->MoverParameters->Name + ".madd");

  AnsiString mdventil1="";
  AnsiString mdventil2="";
  AnsiString mdhaslerA="";
  AnsiString mdhaslerB="";
  AnsiString mdczuwakA="";
  AnsiString mdczuwakB="";
  AnsiString mdclock_1="";
  AnsiString mdclock_2="";
  AnsiString mdbogey_A="";
  AnsiString mdbogey_B="";
  AnsiString mdpanto1A="";
  AnsiString mdpanto1B="";
  AnsiString mdpanto2A="";
  AnsiString mdpanto2B="";
  AnsiString mddirtab1="";
  AnsiString mddirtab2="";
  AnsiString mdfotel1A="";
  AnsiString mdfotel2A="";
  AnsiString mdfotel1B="";
  AnsiString mdfotel2B="";
  AnsiString mdogszyb1="";
  AnsiString mdogszyb2="";
  AnsiString mdswsockA="";
  AnsiString mdswsockB="";
  AnsiString mdwindowN="";
  AnsiString mdzgrnczA="";
  AnsiString mdzgrnczB="";
  AnsiString mdnastawA="";
  AnsiString mdnastawB="";
  AnsiString mdstolikA="";
  AnsiString mdstolikB="";
  AnsiString mdmirrorAR="";
  AnsiString mdmirrorAL="";
  AnsiString mdmirrorBR="";
  AnsiString mdmirrorBL="";
  AnsiString mdstatic1="";
  AnsiString mdstatic2="";
  AnsiString mdstatic3="";
  AnsiString mdstatic4="";
  AnsiString mdstatic5="";
  AnsiString mdstatic6="";
  AnsiString mdstatic7="";
  AnsiString mdstatic8="";
  AnsiString mdstatic9="";
  AnsiString mdstatic0="";

  AnsiString txkabinaA="";
  AnsiString txkabinaB="";
  AnsiString texture01="";
  AnsiString texture02="";
  AnsiString texture03="";
  AnsiString texture04="";
  AnsiString test, strl;
  TStringList *ADDFILE;
  AnsiString textur[] = { "0", "0", "0", "0", "0", "0", "0", "0"};
  AnsiString ReplacableSkin;
  AnsiString RELATION = "nowhere";

  DYNOBJBDIR = QGlobal::asAPPDIR + D->asBaseDir;
  WriteLog("Loading - dynsets " +  ADDFILENAME);
  if (FileExists( ADDFILENAME ))
    {

     int ADDFILELC;
     ADDFILE = new TStringList;
     ADDFILE->Clear();
     ADDFILE->LoadFromFile( ADDFILENAME );
     ADDFILELC = ADDFILE->Count;
     //WriteLog(AnsiString(DYNOBJBDIR + D->MoverParameters->Name + ".madd"));
     AnsiString WHAT, T3D, TEX;
     for (int i=0; i<ADDFILELC; i++)
      {
       strl = ADDFILE->Strings[i];
       test = strl.SubString(1, 10);

       if (test == "mdventil1:") mdventil1 = strl.SubString(12, 255);
       if (test == "mdventil2:") mdventil2 = strl.SubString(12, 255);
       if (test == "mdhaslerA:") mdhaslerA = strl.SubString(12, 255);
       if (test == "mdhaslerB:") mdhaslerB = strl.SubString(12, 255);
       if (test == "mdczuwakA:") mdczuwakA = strl.SubString(12, 255);
       if (test == "mdczuwakB:") mdczuwakB = strl.SubString(12, 255);
       if (test == "mdbogey_A:") mdbogey_A = strl.SubString(12, 255);
       if (test == "mdbogey_B:") mdbogey_B = strl.SubString(12, 255);
       if (test == "mdclock_1:") mdclock_1 = strl.SubString(12, 255);
       if (test == "mdclock_2:") mdclock_2 = strl.SubString(12, 255);
       if (test == "mdpanto1A:") mdpanto1A = strl.SubString(12, 255);
       if (test == "mdpanto1B:") mdpanto1B = strl.SubString(12, 255);
       if (test == "mdfotel1A:") mdfotel1A = strl.SubString(12, 255);
       if (test == "mdfotel2A:") mdfotel2A = strl.SubString(12, 255);
       if (test == "mdfotel1B:") mdfotel1B = strl.SubString(12, 255);
       if (test == "mdfotel2B:") mdfotel2B = strl.SubString(12, 255);
       if (test == "mddirtab1:") mddirtab1 = strl.SubString(12, 255);
       if (test == "mddirtab2:") mddirtab2 = strl.SubString(12, 255);
       if (test == "mdwindown:") mdwindowN = strl.SubString(12, 255);
       if (test == "mdogszyb1:") mdogszyb1 = strl.SubString(12, 255);
       if (test == "mdogszyb2:") mdogszyb2 = strl.SubString(12, 255);
       if (test == "mdswsockA:") mdswsockA = strl.SubString(12, 255);
       if (test == "mdswsockB:") mdswsockB = strl.SubString(12, 255);
       if (test == "mdplog__A:") mdzgrnczA = strl.SubString(12, 255);
       if (test == "mdplog__B:") mdzgrnczB = strl.SubString(12, 255);
       if (test == "mdnastawA:") mdnastawA = strl.SubString(12, 255);
       if (test == "mdnastawB:") mdnastawB = strl.SubString(12, 255);
       if (test == "mdstolikA:") mdstolikA = strl.SubString(12, 255);
       if (test == "mdstolikB:") mdstolikB = strl.SubString(12, 255);
       if (test == "mdmirroAR:") mdmirrorAR = strl.SubString(12, 255);
       if (test == "mdmirroAL:") mdmirrorAL = strl.SubString(12, 255);
       if (test == "mdmirroBR:") mdmirrorBR = strl.SubString(12, 255);
       if (test == "mdmirroBL:") mdmirrorBL = strl.SubString(12, 255);
       if (test == "mdstatic1:") mdstatic1 = strl.SubString(12, 255);
       if (test == "mdstatic2:") mdstatic2 = strl.SubString(12, 255);
       if (test == "mdstatic3:") mdstatic3 = strl.SubString(12, 255);
       if (test == "mdstatic4:") mdstatic4 = strl.SubString(12, 255);
       if (test == "mdstatic5:") mdstatic5 = strl.SubString(12, 255);
       if (test == "mdstatic6:") mdstatic6 = strl.SubString(12, 255);
       if (test == "mdstatic7:") mdstatic7 = strl.SubString(12, 255);
       if (test == "mdstatic8:") mdstatic8 = strl.SubString(12, 255);
       if (test == "mdstatic9:") mdstatic9 = strl.SubString(12, 255);
       if (test == "mdstatic0:") mdstatic0 = strl.SubString(12, 255);

       if (test == "txkabinaA:") txkabinaA = strl.SubString(12, 255);
       if (test == "txkabinaB:") txkabinaB = strl.SubString(12, 255);
       if (test == "texture01:") textur[1] = strl.SubString(12, 255);
       if (test == "texture02:") textur[2] = strl.SubString(12, 255);
       if (test == "texture03:") textur[3] = strl.SubString(12, 255);
       if (test == "texture04:") textur[4] = strl.SubString(12, 255);
       }

      //D->SRJ1token = D->directtabletoken;                                     // SLUZBOWY ROZKLAD JAZDY, RELACJA 'DO'
      //D->SRJ2token = D->directtabletoken;                                     // SLUZBOWY ROZKLAD JAZDY, RELACJA 'Z'
      //D->associatedtrain = D->directtabletoken;                               // PRZYPISANIE RELACJI DO WAGONU

      if (M != NULL) RELATION = M->Relation();
      //if (M != NULL) WriteLog("MECHANIK FOUND!");
       //else WriteLog("MECHANIK WHERE?");

      if (textur[4] == "")                                                      // jezeli nie ma w pliku .madd...
       if (M != NULL) textur[4] = RELATION;                                     // to bierze z rozkladu
       if (!FEX(QGlobal::asAPPDIR + D->asBaseDir + textur[4] +".tga")) textur[4] = "nowhere";

       //WriteLog("PTEXTURE: [" + D->asBaseDir + textur[4] + "]");
       for (int i = 1; i<=4; i++)
        if (textur[i] != "") D->ReplacableSkinID[i] = TTexturesManager::GetTextureID(NULL, NULL, AnsiString(D->asBaseDir + textur[i]).c_str(), Global::iDynamicFiltering);

      if (D->ReplacableSkinID[1]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[1])) D->iAlpha = 0x31310031; // tekstura -1
      if (D->ReplacableSkinID[2]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[2])) D->iAlpha |= 0x02020002; // tekstura -2
      if (D->ReplacableSkinID[3]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[3])) D->iAlpha |= 0x04040004; // tekstura -3
      if (D->ReplacableSkinID[4]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[4])) D->iAlpha |= 0x08080008; // tekstura -4


      // DODATKOWE MODELE
      if (mdventil1 != "") D->mdVentilator1 = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdventil1).c_str(),true);
      if (mdventil2 != "") D->mdVentilator2 = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdventil2).c_str(),true);
      if (mdfotel1A != "") D->mdFotel1      = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdfotel1A).c_str(),true);
      if (mdfotel2A != "") D->mdFotel2      = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdfotel2A).c_str(),true);
      if (mdclock_1 != "") D->mdClock1      = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdclock_1).c_str(),true);
      if (mdclock_2 != "") D->mdClock2      = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdclock_2).c_str(),true);
      if (mddirtab1 != "") D->mdDIRTABLE1   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mddirtab1).c_str(),true);
      if (mddirtab2 != "") D->mdDIRTABLE2   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mddirtab2).c_str(),true);
      if (mdmirrorAR != "") D->mdMirrorAR   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdmirrorAR).c_str(),true);
      if (mdmirrorAL != "") D->mdMirrorAL   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdmirrorAL).c_str(),true);

      if (mdstatic1 != "") D->mdSTATIC01   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic1).c_str(),true);
      if (mdstatic2 != "") D->mdSTATIC02   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic2).c_str(),true);
      if (mdstatic3 != "") D->mdSTATIC03   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic3).c_str(),true);
      if (mdstatic4 != "") D->mdSTATIC04   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic4).c_str(),true);
      if (mdstatic5 != "") D->mdSTATIC05   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic5).c_str(),true);
      if (mdstatic6 != "") D->mdSTATIC06   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic6).c_str(),true);
      if (mdstatic7 != "") D->mdSTATIC07   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic7).c_str(),true);
      if (mdstatic8 != "") D->mdSTATIC08   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic8).c_str(),true);
      if (mdstatic9 != "") D->mdSTATIC09   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic9).c_str(),true);
      if (mdstatic0 != "") D->mdSTATIC10   = TModelsManager::GetModel(AnsiString(D->asBaseDir  + mdstatic0).c_str(),true);

      if (mdclock_1 != "") D->mdClock1->Init(0);
      if (mdclock_2 != "") D->mdClock2->Init(0);
      if (mdventil1 != "") D->mdVentilator1->Init(0);
      if (mdventil2 != "") D->mdVentilator2->Init(0);
      if (mdfotel1A != "") D->mdFotel1->Init(0);
      if (mdfotel2A != "") D->mdFotel2->Init(0);
      if (mddirtab1 != "") D->mdDIRTABLE1->Init(0);
      if (mddirtab2 != "") D->mdDIRTABLE2->Init(0);
      if (mdmirrorAR != "") D->mdMirrorAR->Init(0);
      if (mdmirrorAL != "") D->mdMirrorAL->Init(0);
      if (mdstatic1 != "") D->mdSTATIC01->Init(0);
      if (mdstatic2 != "") D->mdSTATIC02->Init(0);
      if (mdstatic3 != "") D->mdSTATIC03->Init(0);
      if (mdstatic4 != "") D->mdSTATIC04->Init(0);
      if (mdstatic5 != "") D->mdSTATIC05->Init(0);
      if (mdstatic6 != "") D->mdSTATIC06->Init(0);
      if (mdstatic7 != "") D->mdSTATIC07->Init(0);
      if (mdstatic8 != "") D->mdSTATIC08->Init(0);
      if (mdstatic9 != "") D->mdSTATIC09->Init(0);
      if (mdstatic0 != "") D->mdSTATIC10->Init(0);
      
      delete ADDFILE;
      }

}



//---------------------------------------------------------------------------
#pragma package(smart_init)




