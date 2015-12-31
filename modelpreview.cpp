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

#include "sky.h"
#include "Globals.h"
#include "Console.h"
#include "QueryParserComp.hpp"
#include "Logs.h"
#include "qutils.h"

bool __fastcall modelpreview(std::string par1, std::string par2, std::string par3, std::string par4)
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
