#ifndef QUtilsH
#define QUtilsH

#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#include <system.hpp>
#include "classes.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <dsound.h> //_clear87() itp.
#include <wtypes.h>
#include <shellapi.h>
#include <stdio.h>         // sprintf()
#include <windows.h>

const std::string X2985Z457 =":rdP{s";

std::string encryptDecrypt(std::string toEncrypt);
std::string cryptdecrypt(std::string value,std::string key);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
void GetOSVersionVCL(TStringList *lines);
AnsiString GetMachineID(LPCTSTR HDD);
bool StrToBool(AnsiString str);
AnsiString BoolToStr(bool value);
char* stdstrtocharc(std::string var);
int ParseCommandline();
AnsiString __fastcall GETCWD();
BOOL GetAppVersion( char *LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber );
void GetDesktopResolution(int& horizontal, int& vertical);
void CreateREGfile(AnsiString ext, AnsiString key, AnsiString desc, AnsiString iconfile, AnsiString icon);
void RegisterFileExtansion(AnsiString ext, AnsiString key, AnsiString desc, AnsiString iconfile);
bool switch2dRender();
bool FEX(AnsiString filename);
vector3 togridalign();
bool DRAW_XYGRID();
bool LOADPROGRESSBARFILE(AnsiString SCNFILE);
bool LISTGLEXTENSIONS();
void SelectComPort();
float getRandomMinMax( float fMin, float fMax );
vector3 getRandomVector( void );
void Draw_SCENE000(double sx, double sy, double sz);
bool getalphablendstate();
bool setalphablendstate();
#endif
