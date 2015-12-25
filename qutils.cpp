#pragma hdrstop

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <istream>
#include <iostream>
#include <wtypes.h>
#include <shellapi.h>
#include <stdio.h>         // sprintf()
#include <windows.h>
#include <registry.hpp>
#include "system.hpp"
#include "classes.hpp"
#include "opengl/glew.h"
#include "opengl/glut.h"
#include <GL/glu.h>    /* OpenGL utilities. */
#include <GL/glaux.h>

#include "globals.h"
#include "logs.h"


// *****************************************************************************
// Funkcja pobierajaca numer seryjny dysku do uzytku jako user personal ID
// w przyszlosci moze sie przydac do banowania nieogarnietych na multi ]:-)
// *****************************************************************************
AnsiString GetMachineID(LPCTSTR HDD)
{
    LPCTSTR szHD = HDD;  // ERROR
    string ss;
    ss = "Err_StringIsNull";
    UCHAR szFileSys[255],
    szVolNameBuff[255];
    DWORD dwSerial;
    DWORD dwMFL;
    DWORD dwSysFlags;
    int error = 0;

    bool success = GetVolumeInformation(szHD, (LPTSTR)szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, (LPTSTR)szFileSys, 255);
    if (!success)
    {
        ss = "Err_Not_Elevated";
    }
    std::stringstream errorStream;
    errorStream << dwSerial;
  return AnsiString(errorStream.str().c_str());
}


// *****************************************************************************
// Rysowanie znacznika srodka scenerii
// *****************************************************************************
void Draw_SCENE000(double sx, double sy, double sz)
{
  GLboolean blendEnabled;
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

//glEnable(GL_BLEND);
glDisable(GL_LIGHTING);
;

//x
 glDisable( GL_LINE_SMOOTH );
 glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
              glLineWidth(0.005);
              glColor3ub(50,50,50);
              glBegin(GL_LINE_STRIP);
                  glVertex3f(1000,0,0);
                  glVertex3f(-1000,0,0);
                  glVertex3f(1000,0,0);
              glEnd();

              glEnable( GL_LINE_SMOOTH );
              glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
              glLineWidth(0.8);
              glColor3ub(255,0,0);
              glBegin(GL_LINE_STRIP);
                  glVertex3f(0,0,0);
                  glVertex3f(100,0,0);
                  glVertex3f(0,0,0);
              glEnd();

// y

              glColor3ub(0,255,0);
              glBegin(GL_LINE_STRIP);
                  glVertex3f(0,0,0);
                  glVertex3f(0,100,0);
                  glVertex3f(0,0,0);
              glEnd();
              
//z
 glDisable( GL_LINE_SMOOTH );
 glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
              glLineWidth(0.005);
              glColor3ub(50,50,50);
              glBegin(GL_LINE_STRIP);
                  glVertex3f(0,0,1000);
                  glVertex3f(0,0,-1000);
                  glVertex3f(0,0,1000);
              glEnd();

              glEnable( GL_LINE_SMOOTH );
              glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
              glLineWidth(0.8);
              glColor3ub(0,0,255);
              glBegin(GL_LINE_STRIP);
                  glVertex3f(0,0,0);
                  glVertex3f(0,0,-100);
                  glVertex3f(0,0,0);
              glEnd();


              glPushMatrix();
              glTranslatef(0, 0, 0);
              glutSolidSphere(0.2,12,12);
              glPopMatrix();

 glEnable(GL_LIGHTING);
 glBlendFunc(blendSrc, blendDst);
}


// *****************************************************************************
// SHORT FileExists()
// *****************************************************************************
bool FEX(AnsiString filename)
 {
  if (FileExists(filename)) return 1; else return 0;
 }


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// KONWERSJE
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
char* stdstrtocharc(std::string var)
{
	char* ReturnString = new char[100];
	char szBuffer[100];
	LPCSTR lpMyString = var.c_str();

        sprintf(szBuffer,"%s",lpMyString);

	strcpy( ReturnString, szBuffer );

	return ReturnString;
}

bool StrToBool(AnsiString str)
 {
  if (str == "0") return false;
  if (str == "1") return true;
 }

AnsiString BoolToStr(bool value)
 {
  if (value == true) return "1";
  if (value == false) return "0";
 }


// *****************************************************************************
// GETCWD() - POBIERA SCIEZKE DO KATALOGU EXE
// *****************************************************************************
AnsiString __fastcall GETCWD()
 {
   AnsiString cwdbuffer;
   char szBuffer[200];

   // Get the current working directory:

   cwdbuffer = ExtractFilePath(  ParamStr(0) );

   QGlobal::asCWD = cwdbuffer;
   sprintf(szBuffer,"CWD: [%s]", cwdbuffer);
 //WriteLog(szBuffer);
	  
 //free(cwdbuffer);

   return cwdbuffer;
}


void SelectComPort() //added function to find the present serial
{
    char comport[100];
    TCHAR lpTargetPath[5000]; // buffer to store the path of the COMPORTS
    DWORD test;
    bool gotPort=0; // in case the port is not found

    for(int i=0; i<255; i++) // checking ports from COM0 to COM255
    {
        std::string str;
        //str.Format(_T("%d"),i);
        sprintf(comport, "%d", i);

        //WriteLog(comport);

        std::string ComName= std::string("COM") + std::string(comport); // converting to COM0, COM1, COM2

        //WriteLog(stdstrtocharc(ComName));

        test = QueryDosDevice(stdstrtocharc(ComName), stdstrtocharc(LPCSTR(lpTargetPath)), 5000);

            // Test the return value and error if any
        if(test!=0) //QueryDosDevice returns zero if it didn't find an object
        {
         WriteLog(ComName.c_str());
            //m_MyPort.AddString((CString)ComName); // add to the ComboBox
            gotPort=1; // found port
        }

        if(::GetLastError()==ERROR_INSUFFICIENT_BUFFER)
        {
            lpTargetPath[10000]; // in case the buffer got filled, increase size of the buffer.
            continue;
        }

    }

//    if(!gotPort) // if not port
//    m_MyPort.AddString((CString)"No Active Ports Found"); // to display error message incase no ports found

}


// *****************************************************************************
// POBIERANIE INFORMACJI O WERSJI APLIKACJI - RELEASE, BUILD, COMPILE
// *****************************************************************************
BOOL GetAppVersion( char *LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber )
{
 DWORD dwHandle, dwLen;
 UINT BufLen;
 LPTSTR lpData;
 VS_FIXEDFILEINFO *pFileInfo;
 dwLen = GetFileVersionInfoSize( LibName, &dwHandle );
 if (!dwLen) 
  return FALSE;

 lpData = (LPTSTR) malloc (dwLen);
 if (!lpData) 
  return FALSE;

 if( !GetFileVersionInfo( LibName, dwHandle, dwLen, lpData ) )
 {
  free (lpData);
  return FALSE;
 }
 if( VerQueryValue( lpData, "\\", (LPVOID *) &pFileInfo, (PUINT)&BufLen ) ) 
 {
  *MajorVersion = HIWORD(pFileInfo->dwFileVersionMS);
  *MinorVersion = LOWORD(pFileInfo->dwFileVersionMS);
  *BuildNumber = HIWORD(pFileInfo->dwFileVersionLS);
  *RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
  free (lpData);
  return TRUE;
 }
 free (lpData);
 return FALSE;
}


// *****************************************************************************
// POBIERANIE AKTUALNEJ ROZDZIELCZOSCI W SYSTEMIE
// *****************************************************************************
void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}


// *****************************************************************************
// REJESTROWANIE ROZSZERZEN E3D I T3D
// *****************************************************************************
void RegisterFileExtansion(AnsiString ext, AnsiString key, AnsiString desc, AnsiString iconfile)
{
   TRegistry* Reg = new TRegistry();
   Reg->RootKey = HKEY_CLASSES_ROOT;
   Reg->OpenKey(ext, true);
   Reg->WriteString("", key);
   Reg->CloseKey();

   //otwarcie pliku po jego dwukrotnym klikniêciu w Eksploratorze
   Reg->OpenKey(key, true);
   Reg->WriteString("", desc);
   Reg->OpenKey("shell", true);
   Reg->WriteString("", "open");
   Reg->OpenKey("open", true);
   Reg->WriteString("", "&Open");
   Reg->OpenKey("command", true);
   Reg->WriteString("", ParamStr(0) + " %1");
   Reg->CloseKey();

   //kojarzenie ikony z rozszerzeniem (taka sama jak ikona programu)
   Reg->OpenKey(key, true);
   Reg->OpenKey("DefaultIcon", true);
   Reg->WriteString("", AnsiString(ExtractFileDir(ParamStr(0)).c_str()) + iconfile);
}


// *****************************************************************************
// ZAPAMIETYWANIE/PRZYWRACANIE PARAMETROW ALPHABLENDINGU
// *****************************************************************************
bool getalphablendstate()
{
 glGetIntegerv(GL_BLEND_SRC_ALPHA, &QGlobal::GLBLENDSTATE.blendSrc);
 glGetIntegerv(GL_BLEND_DST_ALPHA, &QGlobal::GLBLENDSTATE.blendDst);
 glGetBooleanv(GL_BLEND, &QGlobal::GLBLENDSTATE.blendEnabled);
}

bool setalphablendstate()
{
 glBlendFunc(QGlobal::GLBLENDSTATE.blendSrc, QGlobal::GLBLENDSTATE.blendDst);
 if (QGlobal::GLBLENDSTATE.blendEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
}


// *****************************************************************************
// PRZELACZNIE NA RENDEROWANIE 2D
// *****************************************************************************
bool switch2dRender()
{
  int GWW = Global::iWindowWidth;
  int GWH = Global::iWindowHeight;
//glViewport(0, 0, Global::iWindowWidth, Global::iWindowHeight);

//  glOrtho(0, GWW, GWH, 0, -1, 1);
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity( );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
}

float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}


//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
//-----------------------------------------------------------------------------
vector3 getRandomVector( void )
{
	vector3 vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax( -1.0f, 1.0f );

    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -PI, PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cos(t) * radius;
    vVector.y = (float)sin(t) * radius;

	return vVector;
}


// *****************************************************************************
// Cos w rodzaju przyciagania do siatki, przydatne w roznych zastosowaniach
// *****************************************************************************
vector3 togridalign()
{
 vector3 GA;
 float cxx = Global::pCameraPosition.x;
 float cyy = Global::pCameraPosition.y;
 float czz = Global::pCameraPosition.z;
 cxx = ceil(Global::pCameraPosition.x / 10)*10;
 cyy = ceil(Global::pCameraPosition.y / 10)*10;
 czz = ceil(Global::pCameraPosition.z / 10)*10;

 return vector3(cxx, cyy, czz);
}


// ****************************************************************************************
// Draw_Grid()
// ****************************************************************************************

bool DRAW_XYGRID()
{
 float cxx = Global::pCameraPosition.x;
 float czz = Global::pCameraPosition.z;
 cxx = ceil(Global::pCameraPosition.x/ 10)*10;
 czz = ceil(Global::pCameraPosition.z / 10)*10;


 if ( QGlobal::bGRIDPROAAA) glEnable( GL_LINE_SMOOTH );
 if (!QGlobal::bGRIDPROAAA) glDisable( GL_LINE_SMOOTH );
 glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );

//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); // PRO BLEND


 glDisable(GL_LIGHTING);
 //glDisable(GL_FOG);
 //glEnable(GL_FOG);
 glDisable(GL_DEPTH_TEST);

 if (QGlobal::bGRIDPROAAA) glEnable(GL_BLEND);
 glDisable(GL_COLOR_MATERIAL);

 
 glColor4f(0.7f, 0.7f, 0.7f, 0.55f);

 glLineWidth(0.02);
 glPushMatrix();
 glTranslatef(cxx, 0, czz);
 //glBindTexture(GL_TEXTURE_2D, texgrid);
	
 int bound = 500;
	for(float i = -bound; i <= bound; i += 10)
	{
		glBegin(GL_LINES);
			glVertex3f(-bound, 0, i);					
			glVertex3f(bound, 0, i);
			glVertex3f(i, 0,-bound);							
			glVertex3f(i, 0, bound);
		glEnd();
	}

glLineWidth(0.001); 
  bound = 400;
	for(float i = -bound; i <= bound; i += 1)
	{  
		glBegin(GL_LINES);
			glVertex3f(-bound, 0, i);					
			glVertex3f(bound, 0, i);
			glVertex3f(i, 0,-bound);							
			glVertex3f(i, 0, bound);
		glEnd();
	}

  
  glPopMatrix();
  
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  //if (Global::isFOGON) glEnable(GL_FOG);
  return true;
}

bool LOADPROGRESSBARFILE(AnsiString SCNFILE)
{
 if (FileExists("data\\pbars\\" + SCNFILE))
     {
      QGlobal::bfirstloadingscn = false;
      QGlobal::SLTEMP->LoadFromFile("data\\pbars\\" + SCNFILE);
      QGlobal::iPARSERBYTESPASSED = StrToInt(QGlobal::SLTEMP->Strings[0]);

      if (QGlobal::iPARSERBYTESPASSED == 0) QGlobal::iPARSERBYTESPASSED = 100;
      if (QGlobal::iPARSERBYTESPASSED == 0) QGlobal::bfirstloadingscn = true;
      return true;
     }
    else return false;

}

bool LISTGLEXTENSIONS()
{

 QGlobal::SLTEMP->Clear();
 QGlobal::SLTEMP->Add((char*) glGetString(GL_EXTENSIONS));

 AnsiString asext;
 AnsiString what1 = QGlobal::SLTEMP->Strings[0];
 int extnum = 0;
    for (int loop1=0; loop1<392; loop1++)
    {
     asext = what1.SubString(1, what1.Pos(" ")-1); what1.Delete(1,what1.Pos(" "));
     if (asext != "")
     {
      WriteLog(asext.c_str());
      extnum++;
     }
    }
    WriteLog("------------------------------" );
    WriteLog("EXTENSIONS: " + IntToStr(extnum));
    WriteLog(" ");

 return true;
}

