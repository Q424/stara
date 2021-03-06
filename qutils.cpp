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
#include "model3d.h"
//float emm2[] = {0, 0, 0, 1};
char **argv = NULL;  // zmienna trzymajaca mocne argumenty

double VECTORLEN2(vector3 P1, vector3 P2)
{
  float xDist = P1.x - P2.x;
  float yDist = P1.y - P2.y;
  float zDist = P1.z - P2.z;
  float CamDistToEmitter = sqrt(SQR(zDist)+SQR(yDist)+SQR(xDist));
  return CamDistToEmitter;
}

double VECTORLEN1(vector3 P1, vector3 P2)
{
 double vlen;
 double vec1, vec2, vec3, paX, paY, paZ, pbX, pbY, pbZ;

 if ((P1.x !=0) && (P1.y !=0) && (P2.x !=0) && (P2.y !=0))
  {

   vec1 = (P2.x-P1.x)*(P2.x-P1.x);  //1
   vec2 = (P2.y-P1.y)*(P2.y-P1.y);  //3
   vec3 = (P2.z-P1.z)*(P2.z-P1.z);

   vlen  = sqrt(vec1 + vec2 + vec3);        // wynik powinien byc 3,16227766
   return vlen;
  }
  else
  {
  return  0;
  }
}

void drawcube(float sf, float r, float g, float b, bool translate, vector3 trans, GLuint tex)
{

//glDisable(GL_LIGHTING);
//glBindTexture(GL_TEXTURE_2D, 0);
//glDisable(GL_TEXTURE_2D);
//   glColorMaterial(GL_FRONT, GL_EMISSION);

glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, tex);

if (translate) glPushMatrix();
if (translate) glTranslatef(trans.x, trans.y, trans.z);
 // White side - BACK

glColor4f(   r,  g, b, 1.0 );
glBegin(GL_QUADS);
      /* Front Face */
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -sf, -sf, sf );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  sf, -sf, sf );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  sf,  sf, sf );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -sf,  sf, sf );

      /* Back Face */
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -sf, -sf, -sf );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -sf,  sf, -sf );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  sf,  sf, -sf );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  sf, -sf, -sf );

      /* Top Face */
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -sf,  sf, -sf );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -sf,  sf,  sf );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f(  sf,  sf,  sf );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f(  sf,  sf, -sf );

      /* Bottom Face */
      /* Top Right Of The Texture and Quad */
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -sf, -sf, -sf );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  sf, -sf, -sf );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  sf, -sf,  sf );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -sf, -sf,  sf );

      /* Right face */
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sf, -sf, -sf );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sf,  sf, -sf );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sf,  sf,  sf );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sf, -sf,  sf );

      /* Left Face */
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( -sf, -sf, -sf );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -sf, -sf,  sf );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -sf,  sf,  sf );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( -sf,  sf, -sf );
    glEnd( );

if (translate) glPopMatrix();

glEnable(GL_TEXTURE_2D);
glEnable(GL_LIGHTING);
//   glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
 //  glMaterialfv(GL_FRONT, GL_EMISSION, emm2);
}



/*********************************************************************************************************************
WIN32 command line parser function
**********************************************************************************************************************/

/*******************************************************************************
WIN32 command line parser function
*******************************************************************************/

 
int ParseCommandline1()
{
	int    argc, BuffSize, i;
	WCHAR  *wcCommandLine;
	LPWSTR *argw;

	// Get a WCHAR version of the parsed commande line
	wcCommandLine = GetCommandLineW();
	argw = CommandLineToArgvW( wcCommandLine, &argc);

	// Create the first dimension of the double array
	argv = (char **)GlobalAlloc( LPTR, argc + 1 );

	// convert eich line of wcCommandeLine to MultiByte and place them
	// to the argv[] array
	for( i=0; i < argc; i++)
	{
		BuffSize = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, argw[i], -1, NULL, 0, NULL, NULL );
		argv[i] = (char *)GlobalAlloc( LPTR, BuffSize );
		WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, argw[i], BuffSize * sizeof( WCHAR ) ,argv[i], BuffSize, NULL, NULL );
	}

	// return the number of argument
	return argc;
}


int ParseCommandline2()
{
	int    argc, BuffSize, i;
	WCHAR  *wcCommandLine;
	LPWSTR *argw;

	// Get a WCHAR version of the parsed commande line
	wcCommandLine = GetCommandLineW();
	argw = CommandLineToArgvW(wcCommandLine, &argc);

	// Create the first dimension of the double array
	QGlobal::argv = (char **)GlobalAlloc(LPTR, argc + 1);

	// convert eich line of wcCommandeLine to MultiByte and place them
	// to the argv[] array
	for (i = 0; i < argc; i++)
	{
		BuffSize = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argw[i], -1, NULL, 0, NULL, NULL);
		QGlobal::argv[i] = (char *)GlobalAlloc(LPTR, BuffSize);
		WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, argw[i], BuffSize * sizeof(WCHAR), QGlobal::argv[i], BuffSize, NULL, NULL);
	}

	// return the number of argument
	return argc;
}


std::string encryptDecrypt(std::string toEncrypt) {
    char key[7] = {'K', 'C', 'Q', '4', '2', '4', 'i'}; //Any chars will work
    string output = toEncrypt;
    
    for (int i = 0; i < toEncrypt.size(); i++)
        output[i] = toEncrypt[i] ^ key[i % (sizeof(key) / sizeof(char))];
    
    return output;
}

std::string cryptdecrypt(std::string value, std::string key)
{
    string retval(value);
 
    short unsigned int klen=key.length();
    short unsigned int vlen=value.length();
    short unsigned int k=0;
    short unsigned int v=0;
     
    for(v;v<vlen;v++)
    {
        retval[v]=value[v]^key[k];
        k=(++k<klen?k:0);
    }
     
    return retval;
}

// *****************************************************************************
//
// *****************************************************************************
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
  std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


// *****************************************************************************
// Pobieranie numeru wersji systemu, nazwy i info o service packu
// *****************************************************************************
void GetOSVersionVCL(TStringList *lines)
{
  int buildNumber = Win32BuildNumber;
  if (Win32Platform == VER_PLATFORM_WIN32_WINDOWS)
    buildNumber = LOWORD(buildNumber);
    AnsiString v;
    String platform = "Unknown";
 // lines->Add("Build Number:   " +String(buildNumber));

   lines->Add("sysvers: " +
   String(Win32MajorVersion) + "." + String(Win32MinorVersion) + "." + String(buildNumber));

   v = String(Win32MajorVersion) + "." + String(Win32MinorVersion) + "." + String(buildNumber) ;


  if (Win32Platform == VER_PLATFORM_WIN32s)
    platform = "Win32s";
  else if (Win32Platform == 
             VER_PLATFORM_WIN32_WINDOWS)
  {
    if ((Win32MajorVersion > 4) ||((Win32MajorVersion == 4) &&(Win32MinorVersion > 0))) platform = "Windows 98";
    else platform = "windows 95";
  }
  //else if (Win32Platform == VER_PLATFORM_WIN32_NT) platform = "Windows NT";
   
   if (v == "5.0.2195") platform = "Windows 2000";
   if (v == "4.90.300") platform = "Windows ME";
   if (v == "5.1.2600") platform = "Windows XP";
   if (v == "5.2.3790") platform = "Windows XP Proffesional 64";
   if (v == "6.0.6001") platform = "Windows Vista";
   if (v == "6.1.7600") platform = "Windows 7";
   if (v == "6.1.7601") platform = "Windows 7";
   if (v == "6.2.9200") platform = "Windows 8";
   if (v == "6.2.9201") platform = "Windows 8";
   if (v == "6.3.9600") platform = "Windows 8.1";
   if (v == "10.0.10586") platform = "Windows 10";

  lines->Add("sysname: " + platform);

  if (Win32CSDVersion.IsEmpty() && Win32Platform == VER_PLATFORM_WIN32_NT)
    lines->Add("servpak: ""No Service Pack Installed");
  else
    lines->Add("servpak: " + Win32CSDVersion);

  lines->Add("klayout: " + QGlobal::asKBDLAYOUT + "(" + QGlobal::asKBDLAYOUTSTR + ")");
}


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

AnsiString str(int i)
{
  return IntToStr(i);
}

string trim(string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

char* stdstrtocharc(std::string var)
{
	char* ReturnString = new char[150];
	char szBuffer[150];
	LPCSTR lpMyString = var.c_str();

        sprintf(szBuffer,"%s",lpMyString);

	strcpy( ReturnString, szBuffer );

	return ReturnString;
}


char* stdstrtochar(std::string var)
{
	char* ReturnString = new char[150];
	char szBuffer[150];
	LPCSTR lpMyString = var.c_str();

    sprintf(szBuffer,"%s",lpMyString);

	strcpy( ReturnString, szBuffer );

	return ReturnString;
}


std::string chartostdstr(char *var)
{
std::string strFromChar;
strFromChar.append(var);

return strFromChar;
}

AnsiString AS(std::string str)
{
  return str.c_str();
}

std::string ToLowerCase(std::string text)
{

   for(int i = 0; i<text.length(); i++){
      char c = text[i]; 
      if((c>=65)&&(c<=90)){
	 text[i]|=0x20; 
      }
   }
   return text; 
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

AnsiString BoolToYN(bool value)
 {
  if (value == true) return "tak";
  if (value == false) return "nie";
 }

bool YNToBool(AnsiString value)
 {
  if (value == "yes") return true;
  if (value == "no") return false;
 }

AnsiString StrToPERON(AnsiString value)
 {
  if (value == "l") return "P-l";
  if (value == "r") return "P-r";
  if (value == "lr") return "P-b";
  if (value == "0") return "";
 }

char* trim_and_reduce_spaces( char* s )
  {
  if (s)
    {
    char* from = s + strspn( s, " " );
    char* to   = s;

    do if ((*to = *from++) == ' ') 
      from += strspn( from, " " );
    while (*to++);
    
    while (*--to == ' ') 
      *to = '\0';
    }
  return s;
  }
// trim_and_reduce_spaces( s );



std::string trimextras(const string & sStr)
{
    int nSize = sStr.size();
    int nSPos = 0, nEPos = 1, i;
    for(i = 0; i< nSize; ++i) {
        if( !isspace( sStr[i] ) ) {
            nSPos = i ;
            break;
        }
    }
    for(i = nSize -1 ; i >= 0 ; --i) {
        if( !isspace( sStr[i] ) ) {
            nEPos = i;
            break;
        }
    }
    return string(sStr, nSPos, nEPos - nSPos + 1);
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
   QGlobal::iWW = desktop.right;
   QGlobal::iWH = desktop.bottom;
   horizontal = desktop.right;
   vertical = desktop.bottom;


}

void CreateREGfile(AnsiString ext, AnsiString key, AnsiString desc, AnsiString iconfile, AnsiString icon)
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
 AnsiString pathx ;

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

   //otwarcie pliku po jego dwukrotnym klikni�ciu w Eksploratorze
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
// ZAPAMIETYWANIE/PRZYWRACANIE PARAMETROW SWIATLA
// *****************************************************************************
bool getlightstate(int n)
{
 glGetBooleanv(GL_LIGHTING,&QGlobal::GLLIGHTSTATE.lightEnabled);
}

bool setlightstate(int n)
{
 glGetBooleanv(GL_LIGHTING,&QGlobal::GLLIGHTSTATE.lightEnabled);
 if (QGlobal::GLLIGHTSTATE.lightEnabled) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);
}


// *****************************************************************************
// RENDEROWANIE SFERY W ZADANYM MIEJSCU
// *****************************************************************************
bool draw_sphere(double x, double y, double z, double r, Color4 color)
{
    glDisable(GL_BLEND);
    glColor4f(color.r, color.g, color.b, color.o);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(r, 16, 16);
    glPopMatrix();
}

// *****************************************************************************
// RENDEROWANIE SFERY W ZADANYM MIEJSCU
// *****************************************************************************
bool draw_sphere_q(double x, double y, double z, double r, int q)
{
    glDisable(GL_BLEND);
    glColor4f(0.3, 0.3, 0.3, 0.9);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(r, q, q);
    glPopMatrix();
}

// *****************************************************************************
// RENDEROWANIE SFERY W ZADANYM MIEJSCU BEZ TRANSLACJI
// *****************************************************************************
bool draw_sphere_nt(double x, double y, double z, double r, Color4 color)
{
    glDisable(GL_BLEND);
    glColor4f(color.r, color.g, color.b, color.o);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(r, 8, 8);
    glPopMatrix();
}

// *****************************************************************************
// PRZELACZNIE NA RENDEROWANIE 2D
// *****************************************************************************
bool switch2dRender()
{
  int GWW = Global::iWindowWidth;
  int GWH = Global::iWindowHeight;
//glViewport(0, 0, Global::iWindowWidth, Global::iWindowHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Global::iWindowWidth, Global::iWindowHeight, 0, -1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );
  //glClear(GL_COLOR_BUFFER_BIT);
}

float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

float random(float a, float b)
{
	return a + rand()/(float)RAND_MAX * (b-a);
}

float random(){
	return rand()/(float)RAND_MAX;
}

float length(float v[3])
{
	return (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
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


// *****************************************************************************
// Listowanie rozszerzen OpenGL w logu
// *****************************************************************************
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

