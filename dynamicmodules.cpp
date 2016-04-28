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
  AnsiString test, strl, mpos;
  TStringList *ADDFILE;
  AnsiString textur[] = { "0", "0", "0", "0", "0", "0", "0", "0"};
  AnsiString ReplacableSkin;
  AnsiString RELATION = "nowhere";

  DYNOBJBDIR = QGlobal::asAPPDIR + D->asBaseDir;

  if (FileExists( ADDFILENAME ))
    {
     WriteLog("Loading - dynsets " +  ADDFILENAME);
     int ADDFILELC;
     ADDFILE = new TStringList;
     ADDFILE->Clear();
     ADDFILE->LoadFromFile( ADDFILENAME );
     ADDFILELC = ADDFILE->Count;
     //WriteLog(AnsiString(DYNOBJBDIR + D->MoverParameters->Name + ".madd"));
     AnsiString WHAT, T3D, TEX, OBJ, TST, CTP;
     vector3 POS;
     bool good, hash, mdl, tex, hid;
     int llen;

     for (int i=0; i<ADDFILELC; i++)
      {
       llen = 0;
       good = hash = mdl = tex = false;
       strl = Trim(ADDFILE->Strings[i]);
       llen = strl.Length();

       if (llen > 0)
       {
        mpos = strl.SubString(1, 255);
        hash = strl.Pos("#");
        std::vector<std::string> P = split(mpos.c_str(), ' ');

        WriteLog(AnsiString("[" + AS(P[0]) + "][" + AS(P[1]) + "][" + AS(P[2]) + "][" + AS(P[3]) + "][" + AS(P[4]) + "][" + AS(P[5]) + "]" ));

        OBJ = P[0].c_str();
        TST = P[1].c_str();


     /*
       if (TST == "mdventil1:") mdventil1 = T3D;
       if (TST == "mdventil2:") mdventil2 = T3D;
       if (TST == "mdhaslerA:") mdhaslerA = T3D;
       if (TST == "mdhaslerB:") mdhaslerB = T3D;
       if (TST == "mdczuwakA:") mdczuwakA = T3D;
       if (TST == "mdczuwakB:") mdczuwakB = T3D;
       if (TST == "mdbogey_A:") mdbogey_A = T3D;
       if (TST == "mdbogey_B:") mdbogey_B = T3D;
       if (TST == "mdclock_1:") mdclock_1 = T3D;
       if (TST == "mdclock_2:") mdclock_2 = T3D;
       if (TST == "mdpanto1A:") mdpanto1A = T3D;
       if (TST == "mdpanto1B:") mdpanto1B = T3D;
       if (TST == "mdfotel1A:") mdfotel1A = T3D;
       if (TST == "mdfotel2A:") mdfotel2A = T3D;
       if (TST == "mdfotel1B:") mdfotel1B = T3D;
       if (TST == "mdfotel2B:") mdfotel2B = T3D;
       if (TST == "mddirtab1:") mddirtab1 = T3D;
       if (TST == "mddirtab2:") mddirtab2 = T3D;
       if (TST == "mdwindown:") mdwindowN = T3D;
       if (TST == "mdogszyb1:") mdogszyb1 = T3D;
       if (TST == "mdogszyb2:") mdogszyb2 = T3D;
       if (TST == "mdswsockA:") mdswsockA = T3D;
       if (TST == "mdswsockB:") mdswsockB = T3D;
       if (TST == "mdplog__A:") mdzgrnczA = T3D;
       if (TST == "mdplog__B:") mdzgrnczB = T3D;
       if (TST == "mdnastawA:") mdnastawA = T3D;
       if (TST == "mdnastawB:") mdnastawB = T3D;
       if (TST == "mdstolikA:") mdstolikA = T3D;
       if (TST == "mdstolikB:") mdstolikB = T3D;
       if (TST == "mdmirroAR:") mdmirrorAR = T3D;
       if (TST == "mdmirroAL:") mdmirrorAL = T3D;
       if (TST == "mdmirroBR:") mdmirrorBR = T3D;
       if (TST == "mdmirroBL:") mdmirrorBL = T3D;
       if (TST == "mdstatic1:") mdstatic1 = T3D;
       if (TST == "mdstatic2:") mdstatic2 = T3D;
       if (TST == "mdstatic3:") mdstatic3 = T3D;
       if (TST == "mdstatic4:") mdstatic4 = T3D;
       if (TST == "mdstatic5:") mdstatic5 = T3D;
       if (TST == "mdstatic6:") mdstatic6 = T3D;
       if (TST == "mdstatic7:") mdstatic7 = T3D;
       if (TST == "mdstatic8:") mdstatic8 = T3D;
       if (TST == "mdstatic9:") mdstatic9 = T3D;
       if (TST == "mdstatic0:") mdstatic0 = T3D;
       */
       if (TST == "txkabinaA:") txkabinaA = Trim(P[2].c_str()); //strl.SubString(12, 255);
       if (TST == "txkabinaB:") txkabinaB = Trim(P[2].c_str()); //strl.SubString(12, 255);
       if (TST == "texture01:") textur[1] = Trim(P[2].c_str()); //strl.SubString(12, 255);
       if (TST == "texture02:") textur[2] = Trim(P[2].c_str()); //strl.SubString(12, 255);
       if (TST == "texture03:") textur[3] = Trim(P[2].c_str()); //strl.SubString(12, 255);
       if (TST == "texture04:") textur[4] = Trim(P[2].c_str()); //strl.SubString(12, 255);

       mdl = (OBJ == "mdl");
       tex = (OBJ == "tex");
       hid = (OBJ == "hid");
       hash = strl.Pos("#") || strl.Pos("//");

       if (hash == 0 && mdl && iItemNo < 20)
         {
            CTP = Global::asCurrentTexturePath;
            Global::asCurrentTexturePath = asBaseDir;
            WriteLog("Cab item " + IntToStr(iItemNo) + " init..");
            T3D = Trim(P[2].c_str());
            POS.x = StrToFloat(P[3].c_str());
            POS.y = StrToFloat(P[5].c_str());
            POS.z = StrToFloat(P[4].c_str());
            CABITEM[iItemNo].POS = POS;
            CABITEM[iItemNo].mdItem = TModelsManager::GetModel(AnsiString(D->asBaseDir  + T3D + ".t3d").c_str(), true);
            CABITEM[iItemNo].mdItem->Init(0);
        if (CABITEM[iItemNo].mdItem != NULL) CABITEM[iItemNo].exist = true;
        if (CABITEM[iItemNo].mdItem != NULL) WriteLog("Ok.");
        if (CABITEM[iItemNo].mdItem != NULL) iItemNo++;
            Global::asCurrentTexturePath = CTP;

         }
       if (hash == 0 && hid)
         {
          bHIDEDSM = true;
          SMHIDE[iHideNo].SMName = stdstrtocharc(P[1]);
          SMHIDE[iHideNo].SMVis = false;
          SMHIDE[iHideNo].SMhided = true;
         //smHides[iHideNo] = mdModel->GetFromName(P[1].c_str());

         //smHides[iHideNo]->iAnimOwner = ((int)this);
         //WriteLog("AnimOwner: " + IntToStr(((int)this)));
         //smHides[iHideNo]->iInstance = ((int)this);
         //smHides[iHideNo]->b_Anim = at_Hide;
         //smHides[iHideNo]->WillBeAnimated();

         iHideNo++;
         }
        }
       }

       //mdModel->SMList

      //D->SRJ1token = D->directtabletoken;                                     // SLUZBOWY ROZKLAD JAZDY, RELACJA 'DO'
      //D->SRJ2token = D->directtabletoken;                                     // SLUZBOWY ROZKLAD JAZDY, RELACJA 'Z'
      //D->associatedtrain = D->directtabletoken;                               // PRZYPISANIE RELACJI DO WAGONU

      if (M != NULL) RELATION = M->Relation();
      //if (M != NULL) WriteLog("MECHANIK FOUND!");
       //else WriteLog("MECHANIK WHERE?");

      if (textur[4] == "-")                                                      // jezeli nie ma w pliku .madd...
       if (M != NULL) textur[4] = RELATION;                                     // to bierze z rozkladu
       if (!FEX(QGlobal::asAPPDIR + D->asBaseDir + textur[4] +".tga")) textur[4] = "nowhere";

       //WriteLog("PTEXTURE: [" + D->asBaseDir + textur[4] + "]");
       for (int i = 1; i<=4; i++)
        if (textur[i] != "-") D->ReplacableSkinID[i] = TTexturesManager::GetTextureID(NULL, NULL, AnsiString(D->asBaseDir + textur[i]).c_str(), Global::iDynamicFiltering);

      if (D->ReplacableSkinID[1]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[1])) D->iAlpha = 0x31310031; // tekstura -1
      if (D->ReplacableSkinID[2]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[2])) D->iAlpha |= 0x02020002; // tekstura -2
      if (D->ReplacableSkinID[3]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[3])) D->iAlpha |= 0x04040004; // tekstura -3
      if (D->ReplacableSkinID[4]) if (TTexturesManager::GetAlpha(D->ReplacableSkinID[4])) D->iAlpha |= 0x08080008; // tekstura -4


      // DODATKOWE MODELE
      /*
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
      */
      delete ADDFILE;
      }

}



//---------------------------------------------------------------------------
#pragma package(smart_init)




