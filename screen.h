//---------------------------------------------------------------------------

#ifndef screenH
#define screenH

#include "classes.hpp"
#include "Logs.h"
#include "Globals.h"
#include "camera.h"
#include "Usefull.h"
#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

class TSCREEN
{
public:
void __fastcall FOVADD(float incrf);
void __fastcall FOVREM(float incrf);
void __fastcall FOVDEL();
void __fastcall FOVADDF();
void __fastcall FOVREMF();
void __fastcall RESADD(int mode);
void __fastcall RESREM();
bool __fastcall SaveScreen_BMP();
void __fastcall SaveScreen_TGA();
bool __fastcall SaveScreen_JPG();  //HWND GL_HWND, HDC GL_HDC, HGLRC GL_hRC
bool __fastcall SaveScreen_xxx();
bool SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height);

GLvoid __fastcall ReSizeGLSceneEx(double fov, GLsizei width, GLsizei height);
BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel);



static double CFOV;
static bool ADJSCR;
static bool ADJFOV;
static bool FOVBLOCKED; 

static GLuint loaderbrief;
private:


};
//---------------------------------------------------------------------------
#endif
