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
#include <vcl.h>
#pragma hdrstop

#include "sky.h"
#include "Globals.h"
#include "Console.h"
#include "Logs.h"
#include "qutils.h"
#include "modelpreview.h"

char    buff[BUFSIZ];
FILE    *in, *out;
size_t  n;


TModelViewer::TModelViewer()
{
 bDYN = false;
}

TModelViewer::~TModelViewer()
{
 //
}


bool TModelViewer::CreateREGfile(AnsiString ext, AnsiString key, AnsiString desc, AnsiString iconfile, AnsiString icon)
{
/*
Windows Registry Editor Version 5.00

[HKEY_CLASSES_ROOT\maszynamodelbin]
@="Binarny plik modelu MaSZyna"

[HKEY_CLASSES_ROOT\maszynamodelbin\DefaultIcon]
@="F:\\MaSzyna_15_04\\data\\icons\\e3d.ico,0"

[HKEY_CLASSES_ROOT\maszynamodelbin\shell]
@="open"

[HKEY_CLASSES_ROOT\maszynamodelbin\shell\open]
@="&Open"

[HKEY_CLASSES_ROOT\maszynamodelbin\shell\open\command]
@="F:\\MaSzyna_15_04\\EU07-271215-2.exe %1"

*/
 AnsiString pathx;

 QGlobal::SLTEMP->Clear();
 QGlobal::SLTEMP->Add("Windows Registry Editor Version 5.00");
 QGlobal::SLTEMP->Add("");
 QGlobal::SLTEMP->Add("[HKEY_CLASSES_ROOT\\" + key + "]");
 QGlobal::SLTEMP->Add("@=\""+desc+"\"");
 QGlobal::SLTEMP->Add("");

 pathx = AnsiString(StringReplace( QGlobal::asAPPDIR, "\\", "\\\\", TReplaceFlags() << rfReplaceAll )).c_str();
 QGlobal::SLTEMP->Add("[HKEY_CLASSES_ROOT\\" + key + "\\DefaultIcon]");
 QGlobal::SLTEMP->Add("@=\"" + pathx + "\data\\\\icons\\\\" + icon + ".ico,0\"");
 QGlobal::SLTEMP->Add("");
 QGlobal::SLTEMP->Add("[HKEY_CLASSES_ROOT\\" + key + "\\shell\\open]");
 QGlobal::SLTEMP->Add("@=\"&Open\"");
 QGlobal::SLTEMP->Add("");
 QGlobal::SLTEMP->Add("[HKEY_CLASSES_ROOT\\" + key + "\\shell\\open]");
 QGlobal::SLTEMP->Add("@=\"&Open\"");
 QGlobal::SLTEMP->Add("");

 pathx = AnsiString(StringReplace( ParamStr(0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll )).c_str();
 QGlobal::SLTEMP->Add("[HKEY_CLASSES_ROOT\\" + key + "\\shell\\open\\command]");
 QGlobal::SLTEMP->Add("@=\"" + pathx + " %1\"");
 QGlobal::SLTEMP->Add("");

 QGlobal::SLTEMP->Add("[HKEY_CLASSES_ROOT\\." + UpperCase(icon) + "]");
 QGlobal::SLTEMP->Add("@=\"" + key + "\"");
 QGlobal::SLTEMP->Add("");

 QGlobal::SLTEMP->SaveToFile(key + ".reg");

 return FileExists(key + ".reg");
}


// *****************************************************************************
// Oblicza ile znakow do obciecia coby usunac sciezke bezwzgledna
// *****************************************************************************
bool TModelViewer::Update(AnsiString appdir)
 {
  asAPPDIR = appdir;
  tocutstr = AnsiString(asAPPDIR + "models\\").c_str();
  tocutlen = tocutstr.length();
 }

AnsiString TModelViewer::UnifyCmdLine(AnsiString &cmd)
{
 cmd = StringReplace( cmd, "e3d", "t3d", TReplaceFlags() << rfReplaceAll ); /* ZAMIENIA 'e3d' na 't3d'    */
 cmd = StringReplace( cmd, "E3D", "t3d", TReplaceFlags() << rfReplaceAll );
 cmd = StringReplace( cmd, "\\", "/", TReplaceFlags() << rfReplaceAll );    // np: C:/MaSzyna_15_04/models/ip/wloclawek/wwek_przychodniak.t3d

 return cmd;
}

AnsiString TModelViewer::PrepareCompare(AnsiString cmd)
{
 testp1 = cmd.SubString(1, cmd.Pos("models")-1);
 testp2 = StringReplace( asAPPDIR, "\\", "/", TReplaceFlags() << rfReplaceAll ); // katalog tej maszyny do porownania z katalogiem otwieranego modelu
}


// *****************************************************************************
// Sprawdza czy w lini polecen jest plik modelu
// *****************************************************************************
bool TModelViewer::ismodelfileincmdline(std::string cmdl)
{
 AnsiString line = cmdl.c_str();

 if ((line.Pos("t3d") > 0) || (line.Pos("e3d") > 0))
 {
  WriteLog("MODEL VIEWER MODE...");
  WriteLog("FILE: " + AnsiString(cmdl.c_str()));
  return true;
 }
 else
return false;
}

// *****************************************************************************
// Sprawdza czy czasem model nie odpalony z katalogu dynamic
// *****************************************************************************
bool TModelViewer::isdynamicmodel(std::string cmdl)
{
 AnsiString line = cmdl.c_str();

 if ((line.Pos("dynamic") > 0))
 {
  bDYN = true;
  QGlobal::bISDYNAMIC = true;
  return true;
 }
 else
return false;
}


// *****************************************************************************
// Tworzenie pliku tymczasowego do podgladu
// *****************************************************************************
bool TModelViewer::copymodelfiletotemp(std::string ftocopy, bool &fok)
{
 AnsiString fext = ExtractFileExt(LowerCase(ftocopy.c_str()));
 AnsiString line = ftocopy.c_str();

 if ((line.Pos("t3d") > 0) || (line.Pos("e3d") > 0))
  {
  if (fext == ".e3d")
   {
    in = fopen( ftocopy.c_str(), "rb" );
    out = fopen( "models\\temp.e3d", "wb" );
    while ( (n=fread(buff,1,BUFSIZ,in)) != 0 ) { fwrite( buff, 1, n, out ); }
    fclose (in);
    fclose (out);
    Application->ProcessMessages();
    if (FEX(AnsiString(QGlobal::asAPPDIR + "models\\temp.e3d").c_str())) WriteLog("MODEL TEMP FILE CREATED!");
    if (FEX(AnsiString(QGlobal::asAPPDIR + "models\\temp.e3d").c_str())) fok = true;
   }

  if (fext == ".t3d")
   {
    in = fopen( ftocopy.c_str(), "rb" );
    out = fopen( AnsiString(QGlobal::asAPPDIR +"models\\temp.t3d").c_str(), "wb" );
    while ( (n=fread(buff,1,BUFSIZ,in)) != 0 ) { fwrite( buff, 1, n, out ); }
    fclose (in);
    fclose (out);
    Application->ProcessMessages();
    if (FEX(AnsiString(QGlobal::asAPPDIR + "models\\temp.t3d").c_str())) WriteLog("MODEL TEMP FILE CREATED!");
    if (FEX(AnsiString(QGlobal::asAPPDIR + "models\\temp.t3d").c_str())) fok = true;
   }
 
  }
 else fok = false;

 return fok;
}

bool TModelViewer::modelpreview(std::string par1, std::string par2, std::string par3, std::string par4)
{
         //MessageBox(0, "MODEL VIEWING.", "INFO",MB_OK|MB_ICONEXCLAMATION);
         FILE *stream = NULL;
         QGlobal::bmodelpreview = true;
         QGlobal::bGRIDPROAAA = true;
         char addline[100];
         std::string atmoline;
         std::string lightline;
         std::string skyline;
         std::string cam1line;

         stream = fopen("scenery\\modelpreview.scn", "w"); // Global::logfilenm1.c_str()
         fclose(stream);

         atmoline = "atmo 0.41 0.41 0.44 150 383 0.41 0.41 0.44 endatmo";
         lightline = "light 331 336 -337 0.13 0.14 0.19 0.14 0.12 0.12 0.4 0.4 0.4 endlight";
         skyline = "sky cgskj_blueclear008.t3d sky/modelviewer.bmp endsky";
         cam1line = "camera 30.0 30.0 30.0 0.0 21.0 1.0 1 endcamera";

         stream = fopen("scenery\\modelpreview.scn", "a+");
         sprintf(addline, "node;-1;0;modelpreview;model;0.0;0.2;0.0;0.0;%s;%s;;endmodel", par1.c_str(), "none");
         fprintf(stream, atmoline.c_str());
         fprintf(stream, "\n");
         fprintf(stream, lightline.c_str());
         fprintf(stream, "\n");
         fprintf(stream, skyline.c_str());
         fprintf(stream, "\n");
         fprintf(stream, cam1line.c_str());
         fprintf(stream, "\n");

         fprintf(stream, addline);
         fprintf(stream, "\n");
         fprintf(stream, "\n");
         fprintf(stream, "firstinit");
         fprintf(stream, "\n");
         fclose(stream);

         par2 = "modelpreview.scn";
         strcpy(Global::szSceneryFile, par2.c_str());
         WriteLog("SCENERY= " + AnsiString(Global::szSceneryFile));

}

//---------------------------------------------------------------------------

#pragma package(smart_init)
