//---------------------------------------------------------------------------

#include "system.hpp"
#include "classes.hpp"
#include "jpeg.hpp"          // TJPEGImage
#pragma hdrstop

#include "Logs.h"
#include "Globals.h"
#include "mtable.hpp"
#include "screen.h"
#include "Console.h"
#include "freetype.h"		// Header for our little font library.
#include "qutils.h"
#include "camera.h"
#include <stdio.h>
#include <cstdio.h>


GLuint TSCREEN::loaderbrief;
double TSCREEN::CFOV = 45.0;
bool TSCREEN::ADJFOV = false;
bool TSCREEN::ADJSCR = false;
int x1 = 0;
int y1 = 0;
int x2 = GetSystemMetrics(SM_CXSCREEN);
int y2 = GetSystemMetrics(SM_CYSCREEN);
int sw, sh;

AnsiString FDATE;

//std::string FDATE;

char SSHOTFILEBMP[256];
char SSHOTFILETGA[256];
char SSHOTFILEPNG[256];
char SSHOTFILEJPG[256];
char SSHOTFILEBKP[256];
AnsiString SSHOTFILEJPEG;
int SSHOTQUALITY = 97;
bool SSHOTSMOOTHING = true;


inline int GetFilePointer(HANDLE FileHandle){
return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}


// *****************************************************************************
//
// *****************************************************************************
bool TSCREEN::SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height)
{
 bool Success=0;
 HDC SurfDC=NULL;
 HBITMAP OffscrBmp=NULL;
 HDC OffscrDC=NULL;
 LPBITMAPINFO lpbi=NULL;
 LPVOID lpvBits=NULL;
 HANDLE BmpFile=INVALID_HANDLE_VALUE;
 BITMAPFILEHEADER bmfh;

 if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
  return 0;
 if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
  return 0;
 HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
 BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
 if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL)
  return 0;
 ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
 lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 SelectObject(OffscrDC, OldBmp);
 if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
  return 0;
 if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
  return 0;
 if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
  return 0;
 if ((BmpFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
  return 0;
 DWORD Written;
 bmfh.bfType = 19778;
 bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
 if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
  return 0;
 if (Written < sizeof(bmfh))
  return 0;
 if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL))
  return 0;
 if (Written < sizeof(BITMAPINFOHEADER))
  return 0;
 int PalEntries;
 if (lpbi->bmiHeader.biCompression == BI_BITFIELDS)
 PalEntries = 3;
 else PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
 (int)(1 << lpbi->bmiHeader.biBitCount) : 0;
 if(lpbi->bmiHeader.biClrUsed)
 PalEntries = lpbi->bmiHeader.biClrUsed;
 if(PalEntries)
  {
   if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL))
    return 0;
   if (Written < PalEntries * sizeof(RGBQUAD))
    return 0;
  }

 bmfh.bfOffBits = GetFilePointer(BmpFile);
 if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL))
   {
    CloseHandle(BmpFile);
    return 0;
   }
 if (Written < lpbi->bmiHeader.biSizeImage)
    {
     CloseHandle(BmpFile);
     return 0;
    }
 bmfh.bfSize = GetFilePointer(BmpFile);
 SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
 if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
    {
     CloseHandle(BmpFile);
     return 0;
    }
 if (Written < sizeof(bmfh))
 return 0;
 CloseHandle(BmpFile);
 return 1;
}


// *****************************************************************************
// ZMIANA OGNISKOWEJ - PRZYBLIZANIE
// *****************************************************************************
void __fastcall TSCREEN::FOVADD()
{
       ADJSCR = true;
       ADJFOV = true;
       if (QGlobal::ffov < 55.0) QGlobal::ffov += 0.10;
       TSCREEN::CFOV = QGlobal::ffov;
       ReSizeGLSceneEx(QGlobal::ffov, Global::iWindowWidth, Global::iWindowHeight);
}

void __fastcall TSCREEN::FOVADDF()
{
       ADJSCR = true;
       ADJFOV = true;
       if (QGlobal::ffov < 46.0) QGlobal::ffov += 2.0;
       TSCREEN::CFOV = QGlobal::ffov;
       ReSizeGLSceneEx(QGlobal::ffov, Global::iWindowWidth, Global::iWindowHeight);
}


// *****************************************************************************
// ZMIANA OGNISKOWEJ - ODDALANIE
// *****************************************************************************
void __fastcall TSCREEN::FOVREM()
{
       ADJSCR = true;
       ADJFOV = true;
       if (QGlobal::ffov > 11.0) QGlobal::ffov -= 0.10;
       TSCREEN::CFOV = QGlobal::ffov;
       ReSizeGLSceneEx(QGlobal::ffov, Global::iWindowWidth, Global::iWindowHeight);
}

void __fastcall TSCREEN::FOVDEL()
{
       ADJSCR = true;
       ADJFOV = true;
       QGlobal::ffov = 45.0f;
       TSCREEN::CFOV = QGlobal::ffov;
       ReSizeGLSceneEx(QGlobal::ffov, Global::iWindowWidth, Global::iWindowHeight);
}

void __fastcall TSCREEN::FOVREMF()
{
       ADJSCR = true;
       ADJFOV = true;
       if (QGlobal::ffov > 3.0)  QGlobal::ffov -= 2.0;
       TSCREEN::CFOV = QGlobal::ffov;
       ReSizeGLSceneEx(QGlobal::ffov, Global::iWindowWidth, Global::iWindowHeight);
}

void __fastcall TSCREEN::RESADD(int mode)
{
       ADJSCR = true;
       ADJFOV = false;

       if (mode == 2) ReSizeGLSceneEx(QGlobal::ffov,  640,  480);
       if (mode == 3) ReSizeGLSceneEx(QGlobal::ffov,  800,  600);
       if (mode == 4) ReSizeGLSceneEx(QGlobal::ffov, 1024,  768);
       if (mode == 5) ReSizeGLSceneEx(QGlobal::ffov, 1152,  864);
       if (mode == 6) ReSizeGLSceneEx(QGlobal::ffov, 1280, 1024);
}

void __fastcall TSCREEN::RESREM()
{

}


// *****************************************************************************
//
// *****************************************************************************
BOOL TSCREEN::ChangeScreenResolution (int width, int height, int bitsPerPixel)	// Change the screen resolution
{
	DEVMODE dmScreenSettings;					        // Device mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));			// Make sure memory is cleared
	dmScreenSettings.dmSize	      = sizeof (DEVMODE);     	                // Size of the devmode structure
	dmScreenSettings.dmPelsWidth  = width;			                // Select screen width
	dmScreenSettings.dmPelsHeight = height;			                // Select screen height
	dmScreenSettings.dmBitsPerPel = bitsPerPixel;			        // Select bits per pixel
	dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
         return FALSE;				                                // Display change failed, return false
	}
	return TRUE;                                                            // Display change was successful, return true
}


// *****************************************************************************
//
// *****************************************************************************
GLvoid __fastcall TSCREEN::ReSizeGLSceneEx(double fov, GLsizei width, GLsizei height)
{
    //WindowWidth= width;
    //WindowHeight= height;

    Global::iWindowWidth = width;
    Global::iWindowHeight = height;

/*
	if (height==0) { height=1; }	// Prevent A Divide By Zero By, Making Height Equal One

        RECT windowRect = {0, 0, Global::iWindowWidth, Global::iWindowHeight};	// Define our window coordinates

        DWORD windowStyle = WS_OVERLAPPEDWINDOW;	                        // Define our window style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;	                        // Define the window's extended style


if (!adjfov)
    {
		if (ChangeScreenResolution (Global::iWindowWidth, Global::iWindowHeight, Global::iBpp) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);

		}
		else						                // Otherwise (If fullscreen mode was successful)
		{
                  windowStyle = WS_POPUP;			                // Set the windowStyle to WS_POPUP (Popup window)
                  windowExtendedStyle |= WS_EX_TOPMOST;	                        // Set the extended window style to WS_EX_TOPMOST
		}

                AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
    }
*/

        glViewport(0,0,Global::iWindowWidth, Global::iWindowHeight);		// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix

	gluPerspective(fov,(GLdouble)Global::iWindowWidth/(GLdouble)Global::iWindowHeight, 0.1f, 1999600.0f);       // Calculate The Aspect Ratio Of The Window

        TSCREEN::CFOV = fov;

	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix


}


// *****************************************************************************
// SaveScreen_BMP()
// *****************************************************************************
bool __fastcall TSCREEN::SaveScreen_BMP()
{
    char FN[80];
    std::time_t rawtime;
    std::tm* timeinfo;
  
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(FN, 80, "%Y%m%d-%H%M%S.bmp", timeinfo);

    WriteLog("SAVING SCREEN TO BMP");

    strcpy(SSHOTFILEBMP, "SCR/");
    strcat(SSHOTFILEBMP, FN); // =  FDATE + ".bmp";
    WriteLog(SSHOTFILEBMP);


        int x0, y0;
	int scr_size_x = Global::iWindowWidth;
	int scr_size_y = Global::iWindowHeight;

	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;

	FILE *sub;
	int line_size = 3*scr_size_x;
	if(line_size%4)line_size += 4 - line_size%4;

	unsigned char* buf = new unsigned char[3*line_size*scr_size_y];
	char file[30];

	if(buf==NULL )return false;

	glPixelStorei( GL_PACK_ALIGNMENT, 4);
	//if(ext.EXT_bgra)
		glReadPixels(0, 0, scr_size_x, scr_size_y, GL_BGR_EXT, GL_UNSIGNED_BYTE, buf);
	//else
	//	glReadPixels(x0, y0, scr_size_x, scr_size_y, GL_RGB, GL_UNSIGNED_BYTE, buf);


	fileheader.bfType = 0x4D42; // Magic identifier   - "BM"	| identifikacia BMP suboru musi byt "BM"
	fileheader.bfSize = 3*scr_size_x*scr_size_y+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// File size in bytes			| velkos suboru v byte
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// Offset to image data, bytes	| posun na zaciatok dat
	
	infoheader.biSize = sizeof(BITMAPINFOHEADER);	// Header size in bytes			| velkost hlavicky BITMAPINFOHEADER
	infoheader.biWidth = scr_size_x;	// Width of image			| sirka obrazka - sizeX
	infoheader.biHeight = scr_size_y;	// Height of image			| vyska obrazka - sizeY
	infoheader.biPlanes = 1;		// Number of colour planes	| pocet farebnych rovin musi bit 1
	infoheader.biBitCount = 24;		// Bits per pixel			| bitov na pixel moze bit 1,4,8,24
	infoheader.biCompression = 0;	// Compression type			| typ compresie , 0 - bez kompresie
	infoheader.biSizeImage = line_size*infoheader.biHeight ;	// Image size in bytes		| velkost obrazka v byte
	infoheader.biXPelsPerMeter = 0;	// Pixels per meter X		| pixelov na meter v smere x
	infoheader.biYPelsPerMeter = 0;	// Pixels per meter Y		| pixelov na meter v smere y
	infoheader.biClrUsed = 0;		// Number of colours		| pocet  farieb v palete, ak 0 vsetky su pouzivane
	infoheader.biClrImportant = 0;	// Important colours		| dolezite farby v palete, ak 0 vsetky su dolezite
	
	sub = fopen(SSHOTFILEBMP, "wb");
	fwrite( &fileheader, sizeof(BITMAPFILEHEADER), 1, sub);
	fwrite( &infoheader, sizeof(BITMAPINFOHEADER), 1, sub);
/*	
	if(!ext.EXT_bgra)
	{
		for(int y=0; y<scr_size_y; y++)
		{
			for(int x=0; x<scr_size_x; x++)
			{
				unsigned char temp = buf[y*line_size+x*3+0];
				buf[y*line_size+x*3+0] = buf[y*line_size+x*3+2];
				buf[y*line_size+x*3+2] = temp;
			}
		}
	}
	*/
	fwrite( buf, line_size*scr_size_y, 1, sub);

	delete[] buf;
	fclose(sub);

        PlaySound("data\\sounds\\shutter.wav", NULL, SND_ASYNC);
 return true;
}


// *****************************************************************************
// SaveScreen_TGA()
// *****************************************************************************
void __fastcall TSCREEN::SaveScreen_TGA()
{
int screen_x = Global::iWindowWidth;
int screen_y = Global::iWindowHeight;
char FN [80];

FILE *sub;

    std::time_t rawtime;
    std::tm* timeinfo;
  
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(FN, 80, "%Y%m%d-%H%M%S.tga", timeinfo);

    WriteLog("SAVING SCREEN TO TGA");

    strcpy(SSHOTFILETGA, "SCR/");

    strcat(SSHOTFILETGA, FN); // =  FDATE + ".bmp";

    WriteLog(SSHOTFILETGA);

	unsigned char* buf = new unsigned char[3*screen_x*screen_y];
	
	unsigned char ctmp = 0, type, mode, rb;
	short int width = screen_x, height = screen_y, itmp = 0;
	unsigned char pixelDepth = 24;

	if(buf==NULL )return;

	glPixelStorei( GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, screen_x, screen_y, GL_RGB, GL_UNSIGNED_BYTE, buf);

	sub = fopen(SSHOTFILETGA, "wb");
	
	mode = pixelDepth / 8;
	if ((pixelDepth == 24) || (pixelDepth == 32))type = 2;
	else type = 3;

	// tu sa musi prehodit R->B - pozri specifikaciu TGA formatu.
	if (mode >= 3) {
		for (int i = 0; i < width * height * mode; i += mode)
		{
			rb = buf[i];
			buf[i] = buf[i+2];
			buf[i+2] = rb;
		}
	}

	// tu sa zapisuje hlavicka
	fwrite(&ctmp, sizeof(unsigned char), 1, sub);
	fwrite(&ctmp, sizeof(unsigned char), 1, sub);

	fwrite(&type, sizeof(unsigned char), 1, sub);

	fwrite(&itmp, sizeof(short int), 1, sub);
	fwrite(&itmp, sizeof(short int), 1, sub);
	fwrite(&ctmp, sizeof(unsigned char), 1, sub);
	fwrite(&itmp, sizeof(short int), 1, sub);
	fwrite(&itmp, sizeof(short int), 1, sub);

	fwrite(&width, sizeof(short int), 1, sub);
	fwrite(&height, sizeof(short int), 1, sub);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, sub);

	fwrite(&ctmp, sizeof(unsigned char), 1, sub);
	
	fwrite(buf, 3*screen_x*screen_y, 1, sub);

	delete[] buf;
	fclose(sub);

        PlaySound("data\\sounds\\shutter.wav", NULL, SND_ASYNC);
}



// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// SaveScreen_JPG() ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
bool __fastcall TSCREEN::SaveScreen_JPG()
{
   char FN[80];
   char FNBKP[80];
   char stime[32];

    std::time_t rawtime;
    std::tm* timeinfo;

    AnsiString exiftool = QGlobal::asAPPDIR + "data\\exiftool\\exiftool.exe";;
    AnsiString jpgcomment, artist, copyright, datetimeorg, make, model, focallenght, gpsinfo, NL, sc, ec, comment, par, dir;
    sc = "\"";
    ec = "\"";
    NL = '\n';


    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(FN, 80, "%Y%m%d-%H%M%S.bmp", timeinfo);
    
    sprintf(stime, "%02d:%02d", GlobalTime->hh, GlobalTime->mm);

    AnsiString atmocolor = FloatToStrF(Global::AtmoColor[0], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::AtmoColor[1], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::AtmoColor[2], ffFixed, 3, 2) + " fogs foge " +
                           FloatToStrF(Global::FogColor[0], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::FogColor[1], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::FogColor[2], ffFixed, 3, 2);
                           
    AnsiString lightdata = "0 0 0 " +
                           FloatToStrF(Global::ambientDayLight[0], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::ambientDayLight[1], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::ambientDayLight[2], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::diffuseDayLight[0], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::diffuseDayLight[1], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::diffuseDayLight[2], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::specularDayLight[0], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::specularDayLight[1], ffFixed, 3, 2) + " " +
                           FloatToStrF(Global::specularDayLight[2], ffFixed, 3, 2);

    comment = sc + "scenery: " + Global::szSceneryFile + NL +
                   "scrdate: " + FormatDateTime("yyy.mm.dd hh:mm:ss", Now()) + NL +
                   "position: " + QGlobal::asEXIFCAMERASET + NL +
                   "movelight: " + FloatToStr(QGlobal::fMoveLightS) + NL +
                   "time: " + stime + NL +
                   "atmo: " + atmocolor + NL +
                   "light: " + lightdata + NL +
                   "fps: " + FloatToStrF(QGlobal::fps, ffFixed, 3, 2) + NL +
                   ec;

    artist = sc + QGlobal::asEXIFAUTHOR + ec;
    copyright = sc + QGlobal::asEXIFCOPYRIGHT + ec;
    datetimeorg = sc + FormatDateTime("yyyy:mm:dd hh:mm:ss", Now()) + ec;    
    make = sc + "MaSZyna" + ec;
    model = sc + "474++" + ec;
    focallenght = sc + AnsiString(CFOV) + ec;
    gpsinfo = sc + "15002900" + ec;

    strcpy(SSHOTFILEBMP, AnsiString(QGlobal::asAPPDIR + QGlobal::asSSHOTDIR + QGlobal::asSSHOTSUB).c_str());
    strcat(SSHOTFILEBMP, FN); // =  FDATE + ".bmp";


    int x0, y0;
    int scr_size_x = Global::iWindowWidth;
    int scr_size_y = Global::iWindowHeight;

    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;

    FILE *sub;
    int line_size = 3*scr_size_x;
    if(line_size%4)line_size += 4 - line_size%4;

    unsigned char* buf = new unsigned char[3*line_size*scr_size_y];
    char file[30];

	if(buf==NULL ) return false;

	glPixelStorei( GL_PACK_ALIGNMENT, 4);

        glReadPixels(0, 0, scr_size_x, scr_size_y, GL_BGR_EXT, GL_UNSIGNED_BYTE, buf);   //GL_BGR_EXT

      //glReadPixels(x0, y0, scr_size_x, scr_size_y, GL_RGB, GL_UNSIGNED_BYTE, buf);


	fileheader.bfType = 0x4D42;                                             // Magic identifier   - "BM"	| identifikacia BMP suboru musi byt "BM"
	fileheader.bfSize = 3*scr_size_x*scr_size_y+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// File size in bytes | velkos suboru v byte
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);	// Offset to image data, bytes	| posun na zaciatok dat
	
	infoheader.biSize = sizeof(BITMAPINFOHEADER);	                        // Header size in bytes	| velkost hlavicky BITMAPINFOHEADER
	infoheader.biWidth = scr_size_x;	                                // Width of image | sirka obrazka - sizeX
	infoheader.biHeight = scr_size_y;	                                // Height of image | vyska obrazka - sizeY
	infoheader.biPlanes = 1;	                                     	// Number of colour planes | pocet farebnych rovin musi bit 1
	infoheader.biBitCount = 24;	                                 	// Bits per pixel | bitov na pixel moze bit 1,4,8,24
	infoheader.biCompression = 0;	                                        // Compression type | typ compresie , 0 - bez kompresie
	infoheader.biSizeImage = line_size*infoheader.biHeight ;	        // Image size in bytes		| velkost obrazka v byte
	infoheader.biXPelsPerMeter = 0;	                                        // Pixels per meter X		| pixelov na meter v smere x
	infoheader.biYPelsPerMeter = 0;	                                        // Pixels per meter Y		| pixelov na meter v smere y
	infoheader.biClrUsed = 0;	                                      	// Number of colours		| pocet  farieb v palete, ak 0 vsetky su pouzivane
	infoheader.biClrImportant = 0;	                                        // Important colours		| dolezite farby v palete, ak 0 vsetky su dolezite
	
	sub = fopen(SSHOTFILEBMP, "wb");
	fwrite( &fileheader, sizeof(BITMAPFILEHEADER), 1, sub);
	fwrite( &infoheader, sizeof(BITMAPINFOHEADER), 1, sub);


	fwrite( buf, line_size*scr_size_y, 1, sub);

	delete[] buf;
	fclose(sub);

        std::strftime(FN, 80, "%Y%m%d-%H%M%S.jpg", timeinfo);
        std::strftime(FNBKP, 80, "%Y%m%d-%H%M%S.jpg_original", timeinfo);

        WriteLog("SAVING SCREEN TO JPG");

        strcpy(SSHOTFILEJPG, AnsiString(QGlobal::asAPPDIR + QGlobal::asSSHOTDIR + QGlobal::asSSHOTSUB).c_str());
        strcat(SSHOTFILEJPG, FN); // =  FDATE + ".bmp";
        WriteLog(SSHOTFILEJPG);

        strcpy(SSHOTFILEBKP, AnsiString(QGlobal::asAPPDIR + QGlobal::asSSHOTDIR + QGlobal::asSSHOTSUB).c_str());
        strcat(SSHOTFILEBKP, FNBKP); // PLIK BACKUPU DO USUNIECIA

        Graphics::TBitmap *FormBitmap = new Graphics::TBitmap();
        FormBitmap->LoadFromFile(SSHOTFILEBMP);

        TJPEGImage *JPG = new TJPEGImage();

        JPG->Assign(FormBitmap);        // (BMP)
        JPG->DIBNeeded();
        JPG->ProgressiveEncoding = false;
        JPG->Smoothing = true;
        JPG->CompressionQuality = 99;
        JPG->SaveToFile(SSHOTFILEJPG);

        DeleteFile( SSHOTFILEBMP );

      //par = "-overwrite_original -Comment=" + comment + " " + SSHOTFILEJPG;

        par = "-overwrite_original -Comment=" + comment +
                                  " -artist=" + artist +
                                  " -copyright=" + copyright +
                                  " -dateTimeOriginal=" + datetimeorg +
                                  " -make=" + make +
                                  " -model=" + model +
                                  " -EXIF:FocalLength=" + focallenght +
                                  " -EXIF:GPSInfo=" + gpsinfo +
                                  " " + SSHOTFILEJPG;

        //if (QGlobal::bWRITEEXIF) WriteLog("exiftool param: " + par);

        if (QGlobal::bWRITEEXIF) ShellExecute(0, "open", exiftool.c_str(), par.c_str(), dir.c_str(), SW_HIDE);

        if (!FileExists(SSHOTFILEBMP))  PlaySound("data\\sounds\\shutter.wav", NULL, SND_ASYNC);
 return true;
}


// *****************************************************************************
// ROBI ZRZUT EKRANU DO WYBRANEGO W KONFIGU FORMATU
// *****************************************************************************
bool __fastcall TSCREEN::SaveScreen_xxx()
{
 QGlobal::asEXIFCAMERASET = (FloatToStrF(Global::pCamera->Pos.x, ffFixed,7,3) +" "
                           +FloatToStrF(Global::pCamera->Pos.y, ffFixed,7,3) +" "
                           +FloatToStrF(Global::pCamera->Pos.z, ffFixed,7,3) +" "
                           +FloatToStrF(RadToDeg(Global::pCamera->Pitch), ffFixed,7,3) +" "
                           +FloatToStrF(RadToDeg(Global::pCamera->Yaw), ffFixed,7,3) +" "
                           +FloatToStrF(RadToDeg(Global::pCamera->Roll), ffFixed,7,3) +" " +AnsiString(0));

                                  
 if (QGlobal::asSSHOTEXT == ".jpg") SaveScreen_JPG();
 if (QGlobal::asSSHOTEXT == ".bmp") SaveScreen_BMP();
 if (QGlobal::asSSHOTEXT == ".tga") SaveScreen_TGA();
 return true;
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
