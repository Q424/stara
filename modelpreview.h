
#ifndef modelpreviewH
#define modelpreviewH

#include "Usefull.h"
#include "Classes.h"
#include "Texture.h"
#include "Camera.h"
#include "Ground.h"
#include "MdlMngr.h"
#include "Globals.h"

class TModelViewer
{
 public:
 AnsiString asAPPDIR, testp1, testp2, fext;
 std::string tocutstr;
 int tocutlen;
 bool bDYN;
 
 TModelViewer();
~TModelViewer();

 bool Update(AnsiString appdir);
 AnsiString UnifyCmdLine(AnsiString &cmd);
 AnsiString PrepareCompare(AnsiString cmd);
 bool modelpreview(std::string par1, std::string par2, std::string par3, std::string par4);
 bool isdynamicmodel(std::string cmdl);
 bool copymodelfiletotemp(std::string ftocopy, bool &fok);
 bool ismodelfileincmdline(std::string cmdl);
};





//---------------------------------------------------------------------------
#endif
