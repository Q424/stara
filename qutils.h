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
#include "Model3d.h"

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
            ( std::ostringstream() << std::dec << x ) ).str()


const std::string X2985Z457 =":rdP{s";

double VECTORLEN1(vector3 P1, vector3 P2);
double VECTORLEN2(vector3 P1, vector3 P2);
void drawcube(float size, float r, float g, float b, bool translate, vector3 trans, GLuint tex);
int ParseCommandline1();
int ParseCommandline2();
std::string encryptDecrypt(std::string toEncrypt);
std::string cryptdecrypt(std::string value,std::string key);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
void GetOSVersionVCL(TStringList *lines);
AnsiString GetMachineID(LPCTSTR HDD);
bool StrToBool(AnsiString str);
AnsiString BoolToStr(bool value);
AnsiString BoolToYN(bool value);
bool YNToBool(AnsiString value);
AnsiString StrToPERON(AnsiString value);

AnsiString str(int i);
string trim(string& str);
char* stdstrtocharc(std::string var);
char* stdstrtochar(std::string var);
std::string chartostdstr(char *var);
std::string ToLowerCase(std::string text);
char* trim_and_reduce_spaces( char* s );
std::string trimextras(const std::string & sStr);
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
float random(float a, float b);
float random();
float length(float v[3]);

void Draw_SCENE000(double sx, double sy, double sz);
bool getalphablendstate();
bool setalphablendstate();
bool getlightstate(int n);
bool setlightstate(int n);
bool draw_sphere_q(double x, double y, double z, double r, int q);
bool draw_sphere(double x, double y, double z, double r, Color4 color);
bool draw_sphere_nt(double x, double y, double z, double r, Color4 color);
bool draw_railtie(double x, double y, double z, double a, double roll, bool hd);
#endif
