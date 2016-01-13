/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak and others

*/

#include <iostream>
#include <fstream>
#include "opengl/glew.h"
#include <ddraw>
#include <jpeg.hpp>          // TJPEGImage
#include <math.h>	// Header File For The Math Library
#include <olectl.h>	// Header File For The OLE Controls Library

#include "system.hpp"
#include "classes.hpp"
#include "stdio.h"
#pragma hdrstop

#include "Usefull.h"
#include "Texture.h"
#include "TextureDDS.h"

#include "logs.h"
#include "Globals.h"
#include "io.h"

TTexturesManager::Alphas TTexturesManager::_alphas;
TTexturesManager::Names TTexturesManager::_names;

void TTexturesManager::Init(){};


// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// PCX TEXTURES ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


struct gl_texture_t
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint internalFormat;
  GLuint id;

  GLubyte *texels;
  GLint numMipmaps;
};

#pragma pack(1)
/* PCX header */
struct pcx_header_t
{
  GLubyte manufacturer;
  GLubyte version;
  GLubyte encoding;
  GLubyte bitsPerPixel;

  GLushort xmin, ymin;
  GLushort xmax, ymax;
  GLushort horzRes, vertRes;

  GLubyte palette[48];
  GLubyte reserved;
  GLubyte numColorPlanes;

  GLushort bytesPerScanLine;
  GLushort paletteType;
  GLushort horzSize, vertSize;

  GLubyte padding[54];
};
#pragma pack(4)


// PCX 1 BIT ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
static void ReadPCX1bit (FILE *fp, const struct pcx_header_t *hdr, struct gl_texture_t *texinfo)
{
  int y, i, bytes;
  int colorIndex;
  int rle_count = 0, rle_value = 0;
  GLubyte *ptr = texinfo->texels;

  for (y = 0; y < texinfo->height; ++y)
    {
      ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];
      bytes = hdr->bytesPerScanLine;

      /* Decode line number y */
      while (bytes--)
	{
	  if (rle_count == 0)
	    {
	      if ( (rle_value = fgetc (fp)) < 0xc0)
		{
		  rle_count = 1;
		}
	      else
		{
		  rle_count = rle_value - 0xc0;
		  rle_value = fgetc (fp);
		}
	    }

	  rle_count--;

	  for (i = 7; i >= 0; --i, ptr += 3)
	    {
	      colorIndex = ((rle_value & (1 << i)) > 0);

	      ptr[0] = hdr->palette[colorIndex * 3 + 0];
	      ptr[1] = hdr->palette[colorIndex * 3 + 1];
	      ptr[2] = hdr->palette[colorIndex * 3 + 2];
	    }
	}
    }
}

// PCX 4 BIT ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
static void ReadPCX4bits (FILE *fp, const struct pcx_header_t *hdr, struct gl_texture_t *texinfo)
{
  GLubyte *colorIndex, *line;
  GLubyte *pLine, *ptr;
  int rle_count = 0, rle_value = 0;
  int x, y, c;
  int bytes;

  colorIndex = (GLubyte *)malloc (sizeof (GLubyte) * texinfo->width);
  line = (GLubyte *)malloc (sizeof (GLubyte) * hdr->bytesPerScanLine);

  for (y = 0; y < texinfo->height; ++y)
    {
      ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];

      memset (colorIndex, 0, texinfo->width * sizeof (GLubyte));

      for (c = 0; c < 4; ++c)
	{
	  pLine = line;
	  bytes = hdr->bytesPerScanLine;

	  /* Decode line number y */
	  while (bytes--)
	    {
	      if (rle_count == 0)
		{
		  if ( (rle_value = fgetc (fp)) < 0xc0)
		    {
		      rle_count = 1;
		    }
		  else
		    {
		      rle_count = rle_value - 0xc0;
		      rle_value = fgetc (fp);
		    }
		}

	      rle_count--;
	      *(pLine++) = rle_value;
	    }

	  /* Compute line's color indexes */
	  for (x = 0; x < texinfo->width; ++x)
	    {
	      if (line[x / 8] & (128 >> (x % 8)))
		colorIndex[x] += (1 << c);
	    }
	}

      /* Decode scan line.  color index => rgb  */
      for (x = 0; x < texinfo->width; ++x, ptr += 3)
	{
	  ptr[0] = hdr->palette[colorIndex[x] * 3 + 0];
	  ptr[1] = hdr->palette[colorIndex[x] * 3 + 1];
	  ptr[2] = hdr->palette[colorIndex[x] * 3 + 2];
	}
    }

  /* Release memory */
  free (colorIndex);
  free (line);
}


// PCX 8 BIT ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
static void ReadPCX8bits (FILE *fp, const struct pcx_header_t *hdr, struct gl_texture_t *texinfo)
{
  int rle_count = 0, rle_value = 0;
  GLubyte palette[768];
  GLubyte magic;
  GLubyte *ptr;
  fpos_t curpos;
  int y, bytes;

  /* The palette is contained in the last 769 bytes of the file */
  fgetpos (fp, &curpos);
  fseek (fp, -769, SEEK_END);
  magic = fgetc (fp);

  /* First byte must be equal to 0x0c (12) */
  if (magic != 0x0c)
    {
      fprintf (stderr, "error: colormap's first byte must be 0x0c! "
	       "(%#x)\n", magic);

      free (texinfo->texels);
      texinfo->texels = NULL;
      return;
    }

  /* Read palette */
  fread (palette, sizeof (GLubyte), 768, fp);
  fsetpos (fp, &curpos);

  /* Read pixel data */
  for (y = 0; y < texinfo->height; ++y)
    {
      ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];
      bytes = hdr->bytesPerScanLine;

      /* Decode line number y */
      while (bytes--)
	{
	  if (rle_count == 0)
	    {
	      if( (rle_value = fgetc (fp)) < 0xc0)
		{
		  rle_count = 1;
		}
	      else
		{
		  rle_count = rle_value - 0xc0;
		  rle_value = fgetc (fp);
		}
	    }

	  rle_count--;

	  ptr[0] = palette[rle_value * 3 + 0];
	  ptr[1] = palette[rle_value * 3 + 1];
	  ptr[2] = palette[rle_value * 3 + 2];
	  ptr += 3;
	}
    }
}

// PCX 24 BIT ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
static void ReadPCX24bits (FILE *fp, const struct pcx_header_t *hdr, struct gl_texture_t *texinfo)
{
  GLubyte *ptr = texinfo->texels;
  int rle_count = 0, rle_value = 0;
  int y, c;
  int bytes;

  for (y = 0; y < texinfo->height; ++y)
    {
      /* For each color plane */
      for (c = 0; c < 3; ++c)
	{
	  ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];
	  bytes = hdr->bytesPerScanLine;

	  /* Decode line number y */
	  while (bytes--)
	    {
	      if (rle_count == 0)
		{
		  if( (rle_value = fgetc (fp)) < 0xc0)
		    {
		      rle_count = 1;
		    }
		  else
		    {
		      rle_count = rle_value - 0xc0;
		      rle_value = fgetc (fp);
		    }
		}

	      rle_count--;
	      ptr[c] = (GLubyte)rle_value;
	      ptr += 3;
	    }
	}
    }
}


static struct gl_texture_t * ReadPCXFile (const char *filename)
{
  struct gl_texture_t *texinfo;
  struct pcx_header_t header;
  FILE *fp = NULL;
  int bitcount;

  /* Open image file */
  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return NULL;
    }

  /* Read header file */
  fread (&header, sizeof (struct pcx_header_t), 1, fp);
  if (header.manufacturer != 0x0a)
    {
      fprintf (stderr, "error: bad version number! (%i)\n", header.manufacturer);
      return NULL;
    }

  /* Initialize texture parameters */
  texinfo = (struct gl_texture_t *)
  malloc (sizeof (struct gl_texture_t));
  texinfo->width = header.xmax - header.xmin + 1;
  texinfo->height = header.ymax - header.ymin + 1;
  texinfo->format = GL_RGB;
  texinfo->internalFormat = 3;
  texinfo->texels = (GLubyte *)
  malloc (sizeof (GLubyte) * texinfo->width * texinfo->height * texinfo->internalFormat);

  bitcount = header.bitsPerPixel * header.numColorPlanes;

  /* Read image data */
  switch (bitcount)
    {
    case  1: ReadPCX1bit (fp, &header, texinfo); break; /* 1 bit color index */
    case  4: ReadPCX4bits (fp, &header, texinfo); break; /* 4 bits color index */
    case  8: ReadPCX8bits (fp, &header, texinfo); break;  /* 8 bits color index */
    case 24: ReadPCX24bits (fp, &header, texinfo); break; /* 24 bits */

    default:
      /* Unsupported */
      fprintf (stderr, "error: unknown %i bitcount pcx files\n", bitcount);
      free (texinfo->texels);
      free (texinfo);
      texinfo = NULL;
      break;
    }

  fclose (fp);
  return texinfo;
}


TTexturesManager::AlphaValue   TTexturesManager::ReadPCX (char *szFileName)
{
  struct gl_texture_t *pcx_tex = NULL;
  GLuint tex_id = 0;
  GLuint ID = 0;
  AlphaValue fail(0, false);

  GLfloat maxaniso;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxaniso);

  pcx_tex = ReadPCXFile (szFileName);

  if (pcx_tex && pcx_tex->texels)
    {
      /* Generate texture */
      glGenTextures (1, &ID);
      glBindTexture (GL_TEXTURE_2D, ID);

      /* Setup some parameters for texture filters and mipmapping */
      //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxaniso);

//#if 0
      glTexImage2D (GL_TEXTURE_2D, 0, pcx_tex->internalFormat, pcx_tex->width, pcx_tex->height, 0, pcx_tex->format, GL_UNSIGNED_BYTE, pcx_tex->texels);
//#else
      gluBuild2DMipmaps (GL_TEXTURE_2D, pcx_tex->internalFormat, pcx_tex->width, pcx_tex->height, pcx_tex->format, GL_UNSIGNED_BYTE, pcx_tex->texels);
//#endif
 

      free (pcx_tex->texels);
      free (pcx_tex);
    }
  return std::make_pair(ID, false);                                       // Return True (All Good)
}



// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// LOADING JPEG TEXTURES

TTexturesManager::AlphaValue   TTexturesManager::LOADJPG(char* szPathName)				// Load Image And Convert To A Texture
{
	HDC		hdcTemp;						// The DC To Hold Our Bitmap
	HBITMAP		hbmpTemp;						// Holds The Bitmap Temporarily
	IPicture	*pPicture;						// IPicture Interface
	OLECHAR		wszPath[MAX_PATH+1];					// Full Path To Picture (WCHAR)
	char		szPath[MAX_PATH+1];					// Full Path To Picture
	long		lWidth;							// Width In Logical Units
	long		lHeight;						// Height In Logical Units
	long		lWidthPixels;						// Width In Pixels
	long		lHeightPixels;						// Height In Pixels
	GLint		glMaxTexDim ;						// Holds Maximum Texture Size
        GLuint ID;

        ID = 0;

        AlphaValue fail(0, false);

        //WriteLog(szPathName);

        GLfloat maxaniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxaniso);
         /*
	if (strstr(szPathName, "http://"))					// If PathName Contains http:// Then...
	{
        strcpy(szPath, szPathName);						// Append The PathName To szPath
        //WriteLog("http tex:");
        //WriteLog(szPath);
	}
	else									// Otherwise... We Are Loading From A File
	{
		GetCurrentDirectory(MAX_PATH, szPath);				// Get Our Working Directory
		strcat(szPath, "\\");						// Append "\" After The Working Directory
		strcat(szPath, szPathName);					// Append The PathName
	}
          */
	if (strstr(szPathName, "qqq"))						// If PathName Contains http:// Then...
	{

		std::string tmp;
		tmp.append(szPathName);                                         // PRZEPISUJEMY Z char DO std::string
              //WriteLog("sss:" + AnsiString(tmp.c_str()));                     // sss: www.\annuncio.jpg
		tmp = tmp.substr(4, 144);                                       // USUWAMY tex/www.
              //WriteLog("TMP: " + AnsiString(tmp.c_str()));                    // TMP: \annuncio.jpg
		strcpy(szPath, "http://eu07.es");                               // NADPISUJEMY szPath prefixem protokolu
		strcat(szPath, tmp.c_str());                                    // DODAJEMY DO TEGO ADRES PLIKU
              //WriteLog( szPath);
		//strcpy(szPath, szPathName);					// Append The PathName To szPath
	}
	else									// Otherwise... We Are Loading From A File
	{
		GetCurrentDirectory(MAX_PATH, szPath);				// Get Our Working Directory
		strcat(szPath, "\\");						// Append "\" After The Working Directory
		strcat(szPath, szPathName);					// Append The PathName
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);		// Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))								// If Loading Failed
		return fail;							// Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));					// Create The Windows Compatible Device Context
	if(!hdcTemp)								// Did Creation Fail?
	{
		pPicture->Release();						// Decrements IPicture Reference Count
		return fail;							// Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);			// Get Maximum Texture Size Supported
	
	pPicture->get_Width(&lWidth);						// Get IPicture Width (Convert To Pixels)
	lWidthPixels	= MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);						// Get IPicture Height (Convert To Pixels)
	lHeightPixels	= MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim) // Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else  // Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;
 
	if (lHeightPixels <= glMaxTexDim) // Is Image Height Greater Than Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else  // Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;
	
	//	Create A Temporary Bitmap
	BITMAPINFO	bi = {0};						// The Type Of Bitmap We Request
	DWORD		*pBits = 0;						// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);		// Set Structure Size
	bi.bmiHeader.biBitCount		= 32;					// 32 Bit
	bi.bmiHeader.biWidth		= lWidthPixels;				// Power Of Two Width
	bi.bmiHeader.biHeight		= lHeightPixels;			// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression	= BI_RGB;				// RGB Encoding
	bi.bmiHeader.biPlanes		= 1;					// 1 Bitplane

	//	Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hbmpTemp)								// Did Creation Fail?
	{
		DeleteDC(hdcTemp);						// Delete The Device Context
		pPicture->Release();						// Decrements IPicture Reference Count
		return fail;							// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);					// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)			// Loop Through All Of The Pixels
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);				// Grab The Current Pixel
		BYTE  temp		= pPixel[0];				// Store 1st Color In Temp Variable (Blue)
		pPixel[0]		= pPixel[2];				// Move Red Value To Correct Position (1st)
		pPixel[2]		= temp;					// Move Temp Value To Correct Blue Position (3rd)
                pPixel[3]	        = 255;                                  // Set The Alpha Value To 255  ( NIEPRZEZROCZYSTE)

                //if (Global::bGrayScale)
                //  {
                //   // CONVERTIG TO GRAYSCALE
                //   pPixel[0] = 0.5*pPixel[0] + 0.39*pPixel[1] + 0.11*pPixel[2];
                //   pPixel[1] = 0.5*pPixel[0] + 0.39*pPixel[1] + 0.11*pPixel[2];
                //   pPixel[2] = 0.5*pPixel[0] + 0.39*pPixel[1] + 0.11*pPixel[2];
                //  }

		// This Will Make Any Black Pixels, Completely Transparent	(You Can Hardcode The Value If You Wish)
		//if ((pPixel[0]==0) && (pPixel[1]==0) && (pPixel[2]==0)) pPixel[3] =   0;	 // Is Pixel Completely Black  Set The Alpha Value To 0
	}

	glGenTextures(1, &ID);							// Create The Texture

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, ID);					// Bind To The Texture ID
  	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);		// (Modify This For The Type Of Filtering You Want)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);         // (Modify This For The Type Of Filtering You Want)

        // FILTROWANIE TEKSTURY COBY UZYSKAC NA TORZE TAKI EFEKT JAK PRZY TEKSTURZE Z PLIKU .TEX
        gluBuild2DMipmaps( GL_TEXTURE_2D, 4, lWidthPixels, lHeightPixels, GL_RGBA, GL_UNSIGNED_BYTE, pBits );

        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //--glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //--glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        //-glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxaniso);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);	// (Modify This If You Want Mipmaps)


	DeleteObject(hbmpTemp);							// Delete The Object
	DeleteDC(hdcTemp);							// Delete The Device Context

	pPicture->Release();							// Decrements IPicture Reference Count

        return std::make_pair(ID, false);                                       // Return True (All Good)
}


TTexturesManager::Names::iterator TTexturesManager::LoadFromFile(std::string fileName, int filter)
{

    std::string message("Loading - texture: ");
    AnsiString fnnoext, str, inetlink;
    std::string realFileName(fileName);
    std::ifstream file(fileName.c_str());
    // Ra: niby bez tego jest lepiej, ale dzia³a gorzej, wiêc przywrócone jest oryginalne
    if (!file.is_open())
        realFileName.insert(0, szTexturePath);
    else
        file.close();

    // char* cFileName = const_cast<char*>(fileName.c_str());

    message += realFileName;
    WriteLog(message.c_str()); // Ra: chybaa mia³o byæ z komunikatem z przodu, a nie tylko nazwa

    char* cFileName = const_cast<char*>(fileName.c_str());  // Q:

    size_t pos = fileName.rfind('.');
    std::string ext(fileName, pos + 1, std::string::npos);


    if (strstr(realFileName.c_str(), "http-"))
        {
         str = AnsiString(realFileName.c_str());                                // textures\http-xxxxxx.jpg
         inetlink = "http://eu07.es/textures\\" + str.SubString(15, 255);       // UCINAMY  "textures\http-"
         WriteLog("INETLINK: " + inetlink);                                     // INETLINK: http://q.matinf.pl/textures\ip\jpegtestd.jpg
         cFileName = inetlink.c_str();
        }

    AlphaValue texinfo;

    if (ext == "tga")
        texinfo = LoadTGA(realFileName, filter);
    else if (ext == "tex")
        texinfo = LoadTEX(realFileName);
    else if (ext == "bmp")
        texinfo = LoadBMP(realFileName);
    else if (ext == "dds")
        texinfo = LoadDDS(realFileName, filter);
    else if (ext == "jpg")
     texinfo = LOADJPG( cFileName );
    else if (ext == "gif")
     texinfo = LOADJPG( cFileName );
    else if (ext == "pcx")
     texinfo = ReadPCX( cFileName );
     
    _alphas.insert(texinfo); // zapamiêtanie stanu przezroczystoœci tekstury - mo¿na by tylko przezroczyste
    std::pair<Names::iterator, bool> ret = _names.insert(std::make_pair(fileName, texinfo.first));

    if (!texinfo.first)
    {
        WriteLog("Failed");
        ErrorLog("Missed texture: " + AnsiString(realFileName.c_str()));
        return _names.end();
    };

    _alphas.insert(texinfo);
    ret = _names.insert(std::make_pair(fileName, texinfo.first)); // dodanie tekstury do magazynu (spisu nazw)

    // WriteLog("OK"); //Ra: "OK" nie potrzeba, samo "Failed" wystarczy
    
    return ret.first;
};

struct ReplaceSlash
{
    const char operator()(const char input)
    {
        return input == '/' ? '\\' : input;
    }
};

GLuint TTexturesManager::GetTextureID(char *dir, char *where, std::string fileName, int filter)
{ // ustalenie numeru tekstury, wczytanie jeœli nie jeszcze takiej nie by³o
    /*
    // Ra: niby tak jest lepiej, ale dzia³a gorzej, wiêc przywrócone jest oryginalne
     //najpierw szukamy w katalogu, z którego wywo³ywana jest tekstura, potem z wy¿szego
     //Ra: przerobiæ na wyszukiwanie w drzewie nazw, do którego zapisywaæ np. rozmiary,
    przezroczystoœæ
     //Ra: ustalaæ, które tekstury mo¿na wczytaæ ju¿ w trakcie symulacji
     size_t pos=fileName.find(':'); //szukamy dwukropka
     if (pos!=std::string::npos) //po dwukropku mog¹ byæ podane dodatkowe informacje
      fileName=fileName.substr(0,pos); //niebêd¹ce nazw¹ tekstury
     std::transform(fileName.begin(),fileName.end(),fileName.begin(),ReplaceSlash()); //zamiana "/"
    na "\"
     //jeœli bie¿aca œcie¿ka do tekstur nie zosta³a dodana to dodajemy domyœln¹
     //if (fileName.find('\\')==std::string::npos) //bz sensu
     // fileName.insert(0,szDefaultTexturePath);
     //najpierw szukamy w podanym katalogu, potem w domyœlnym
     Names::iterator iter;
     std::ifstream file;
     if ((fileName.find('.')==fileName.npos)?true:(fileName.rfind('.')<fileName.rfind('\\')))
    //pierwsza kropka od prawej jest wczeœniej ni¿ "\"
     {//Ra: jeœli nie ma kropki w nazwie pliku, wypróbowanie rozszerzeñ po kolei, zaczynaj¹c od
    domyœlnego
      fileName.append("."); //kropkê trzeba dodaæ na pewno, resztê trzeba próbowaæ
      std::string test; //zmienna robocza
      for (int i=0;i<4;++i)
      {//najpierw szukamy w magazynie
       test=fileName;
       if (where) test.insert(0,where); //œcie¿ka obiektu wywo³uj¹cego
       test.append(Global::szDefaultExt[i]); //dodanie jednego z kilku rozszerzeñ
       iter=_names.find(test); //czy mamy ju¿ w magazynie?
       if (iter!=_names.end())
        return iter->second; //znalezione!
       if (dir)
       {//mo¿e we wskazanym katalogu?
        test=fileName;
        test.insert(0,dir); //jeszcze próba z dodatkow¹ œcie¿k¹
        test.append(Global::szDefaultExt[i]); //dodanie jednego z kilku rozszerzeñ
        iter=_names.find(test); //czy mamy ju¿ w magazynie?
        if (iter!=_names.end())
         return iter->second; //znalezione!
       }
      //}
      //for (int i=0;i<4;++i)
      //{//w magazynie nie ma, to sprawdzamy na dysku
       test=fileName;
       if (where) test.insert(0,where); //œcie¿ka obiektu wywo³uj¹cego
       test.append(Global::szDefaultExt[i]); //dodanie jednego z kilku rozszerzeñ
       file.open(test.c_str());
       if (!file.is_open())
       {test=fileName;
        if (dir) test.insert(0,dir); //próba z dodatkow¹ œcie¿k¹
        test.append(Global::szDefaultExt[i]); //dodanie jednego z kilku rozszerzeñ
        file.open(test.c_str());
       }
       if (file.is_open())
       {//jak znaleziony, to plik zostaje otwarty
        fileName=test; //zapamiêtanie znalezionego rozszerzenia
        break; //wyjœcie z pêtli na etapie danego rozszerzenia
       }
      }
     }
     else
     {//gdy jest kropka, to rozszerzenie jest jawne
      std::string test; //zmienna robocza
      //najpierw szukamy w magazynie
      test=fileName;
      if (where) test.insert(0,where); //œcie¿ka obiektu wywo³uj¹cego
      iter=_names.find(test); //czy mamy ju¿ w magazynie?
      if (iter!=_names.end())
       return iter->second; //znalezione!
      test=fileName;
      if (dir) test.insert(0,dir); //jeszcze próba z dodatkow¹ œcie¿k¹
      iter=_names.find(test); //czy mamy ju¿ w magazynie?
      if (iter!=_names.end())
       return iter->second; //znalezione!
      //w magazynie nie ma, to sprawdzamy na dysku
      test=fileName;
      if (where) test.insert(0,where); //œcie¿ka obiektu wywo³uj¹cego
      file.open(test.c_str());
      if (!file.is_open())
      {//jak znaleziony, to plik zostaje otwarty
       test=fileName;
       if (dir) test.insert(0,dir); //próba z dodatkow¹ œcie¿k¹
       file.open(test.c_str());
       if (file.is_open())
        fileName=test; //ustalenie nowej nazwy
      }
     }
     if (file.is_open())
     {//plik pozostaje otwarty, gdy znaleziono na dysku
      file.close(); //mo¿na ju¿ zamkn¹æ
      iter=LoadFromFile(fileName,filter); //doda siê do magazynu i zwróci swoj¹ pozycjê
     }
    */
    size_t pos = fileName.find(':'); // szukamy dwukropka
    if (pos != std::string::npos) // po dwukropku mog¹ byæ podane dodatkowe informacje
        fileName = fileName.substr(0, pos); // niebêd¹ce nazw¹ tekstury
    pos = fileName.find('|'); // szukamy separatora tekstur
    if (pos != std::string::npos) // po | mo¿e byæ nazwa kolejnej tekstury
        fileName = fileName.substr(0, pos); // i trzeba to obci¹æ
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), ReplaceSlash());
    // jeœli bie¿aca œcie¿ka do tekstur nie zosta³a dodana to dodajemy domyœln¹
    if (fileName.find('\\') == std::string::npos)
        fileName.insert(0, szTexturePath);
    Names::iterator iter;
    if (fileName.find('.') == std::string::npos)
    { // Ra: wypróbowanie rozszerzeñ po kolei, zaczynaj¹c od domyœlnego
        fileName.append("."); // kropka bêdze na pewno, resztê trzeba próbowaæ
        std::string test; // zmienna robocza
        for (int i = 0; i < 4; ++i)
        { // najpierw szukamy w magazynie
            test = fileName;
            test.append(Global::szDefaultExt[i]);
            iter = _names.find(fileName); // czy mamy ju¿ w magazynie?
            if (iter != _names.end())
                return iter->second; // znalezione!
            test.insert(0, szTexturePath); // jeszcze próba z dodatkow¹ œcie¿k¹
            iter = _names.find(fileName); // czy mamy ju¿ w magazynie?
            if (iter != _names.end())
                return iter->second; // znalezione!
        }
        for (int i = 0; i < 4; ++i)
        { // w magazynie nie ma, to sprawdzamy na dysku
            test = fileName;
            test.append(Global::szDefaultExt[i]);
            std::ifstream file(test.c_str());
            if (!file.is_open())
            {
                test.insert(0, szTexturePath);
                file.open(test.c_str());
            }
            if (file.is_open())
            {
                fileName.append(Global::szDefaultExt[i]); // dopisanie znalezionego
                file.close();
                break; // wyjœcie z pêtli na etapie danego rozszerzenia
            }
        }
    }
    iter = _names.find(fileName); // czy mamy ju¿ w magazynie
    if (iter == _names.end())
        iter = LoadFromFile(fileName, filter);
    return (iter != _names.end() ? iter->second : 0);
};

bool TTexturesManager::GetAlpha(GLuint id)
{ // atrybut przezroczystoœci dla tekstury o podanym numerze (id)
    Alphas::iterator iter = _alphas.find(id);
    return (iter != _alphas.end() ? iter->second : false);
}

TTexturesManager::AlphaValue TTexturesManager::LoadBMP(std::string fileName)
{

    AlphaValue fail(0, false);
    std::ifstream file(fileName.c_str(), std::ios::binary);

    if (!file.is_open())
    {
        // file.close();
        return fail;
    };

    BITMAPFILEHEADER header;
    size_t bytes;

    file.read((char *)&header, sizeof(BITMAPFILEHEADER));
    if (file.eof())
    {
        file.close();
        return fail;
    }

    // Read in bitmap information structure
    BITMAPINFO info;
    long infoSize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
    file.read((char *)&info, infoSize);

    if (file.eof())
    {
        file.close();
        return fail;
    };

    GLuint width = info.bmiHeader.biWidth;
    GLuint height = info.bmiHeader.biHeight;

    unsigned long bitSize = info.bmiHeader.biSizeImage;
    if (!bitSize)
        bitSize = (width * info.bmiHeader.biBitCount + 7) / 8 * height;

    GLubyte *data = new GLubyte[bitSize];
    file.read((char *)data, bitSize);

    if (file.eof())
    {
        delete[] data;
        file.close();
        return fail;
    };

    file.close();

    GLuint id;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // This is specific to the binary format of the data read in.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return std::make_pair(id, false);
};

TTexturesManager::AlphaValue TTexturesManager::LoadTGA(std::string fileName, int filter)
{
    AlphaValue fail(0, false);
    int writeback = -1; //-1 plik jest OK, >=0 - od którego bajtu zapisaæ poprawiony plik
    GLubyte TGACompheader[] = {0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // uncompressed TGA header
    GLubyte TGAcompare[12]; // used to compare TGA header
    GLubyte header[6]; // first 6 useful bytes from the header
    std::fstream file(fileName.c_str(), std::ios::binary | std::ios::in);
    file.read((char *)TGAcompare, sizeof(TGAcompare));
    file.read((char *)header, sizeof(header));
    // std::cout << file.tellg() << std::endl;
    if (file.eof())
    {
        file.close();
        return fail;
    };
    bool compressed = (memcmp(TGACompheader, TGAcompare, sizeof(TGACompheader)) == 0);
    GLint width = header[1] * 256 + header[0]; // determine the TGA width (highbyte*256+lowbyte)
    GLint height = header[3] * 256 + header[2]; // determine the TGA height (highbyte*256+lowbyte)
    // check if width, height and bpp is correct
    if (!width || !height || (header[4] != 24 && header[4] != 32))
    {
        WriteLog("Bad texture: " + AnsiString(fileName.c_str()) +
                 " has wrong header or bits per pixel");
        file.close();
        return fail;
    };
    { // sprawdzenie prawid³owoœci rozmiarów
        int i, j;
        for (i = width, j = 0; i; i >>= 1)
            if (i & 1)
                ++j;
        if (j == 1)
            for (i = height, j = 0; i; i >>= 1)
                if (i & 1)
                    ++j;
        if (j != 1)
            WriteLog("Bad texture: " + AnsiString(fileName.c_str()) + " is " + AnsiString(width) +
                     "×" + AnsiString(height));
    }
    GLuint bpp = header[4]; // grab the TGA's bits per pixel (24 or 32)
    GLuint bytesPerPixel = bpp / 8; // divide by 8 to get the bytes per pixel
    GLuint imageSize =
        width * height * bytesPerPixel; // calculate the memory required for the TGA data
    GLubyte *imageData = new GLubyte[imageSize]; // reserve memory to hold the TGA data
    if (!compressed)
    { // WriteLog("Not compressed.");
        file.read(imageData, imageSize);
        if (file.eof())
        {
            delete[] imageData;
            file.close();
            return fail;
        };
    }
    else
    { // skompresowany plik TGA
        GLuint filesize; // current byte
        GLuint colorbuffer[1]; // Storage for 1 pixel
        file.seekg(0, ios::end); // na koniec
        filesize = (int)file.tellg() - 18; // rozmiar bez nag³ówka
        file.seekg(18, ios::beg); // ponownie za nag³ówkiem
        GLubyte *copyto = imageData; // gdzie wstawiaæ w buforze
        GLubyte *copyend = imageData + imageSize; // za ostatnim bajtem bufora
        GLubyte *copyfrom = imageData + imageSize - filesize; // gdzie jest pocz¹tek
        int chunkheader = 0; // Ra: bêdziemy wczytywaæ najm³odszy bajt
        if (filesize < imageSize) // jeœli po kompresji jest mniejszy ni¿ przed
        { // Ra: nowe wczytywanie skompresowanych: czytamy ca³e od razu, dekompresja w pamiêci
            GLuint copybytes;
            file.read(copyfrom, filesize); // wczytanie reszty po nag³ówku
            // najpierw trzeba ustaliæ, ile skopiowanych pikseli jest na samym koñcu
            copyto = copyfrom; // roboczo przelatujemy wczytane dane
            copybytes = 0; // licznik bajtów obrazka
            while (copybytes < imageSize)
            {
                chunkheader = (unsigned char)*copyto; // jeden bajt, pozosta³e zawsze zerowe
                copyto += 1 + bytesPerPixel; // bajt licznika oraz jeden piksel jest zawsze
                copybytes += (1 + (chunkheader & 127)) * bytesPerPixel; // iloœæ pikseli
                if (chunkheader < 128) // jeœli kopiowanie, pikseli jest wiêcej
                    copyto += (chunkheader)*bytesPerPixel; // rozmiar kopiowanego obszaru (bez
                // jednego piksela)
            }
            if (copybytes > imageSize)
            { // nie ma prawa byæ wiêksze
                WriteLog("Compression error");
                delete[] imageData;
                file.close();
                return fail;
            }
            // na koñcu mog¹ byæ œmieci
            int extraend = copyend - copyto; // d³ugoœæ œmieci na koñcu
            if (extraend > 0)
            { // przesuwamy bufor do koñca obszaru dekompresji
                WriteLog("Extra bytes: " + AnsiString(extraend));
                memmove(copyfrom + extraend, copyfrom, filesize - extraend);
                copyfrom += extraend;
                file.close();
                filesize -= extraend; // to chyba nie ma znaczenia
                if (Global::iModifyTGA & 2) // flaga obcinania œmieci
                { // najlepiej by by³o obci¹æ plik, ale fstream tego nie potrafi
                    int handle;
                    for (unsigned int i = 0; i < fileName.length(); ++i)
                        if (fileName[i] == '/')
                            fileName[i] == '\\'; // bo to Windows
                    WriteLog("Truncating extra bytes");
                    handle = open(fileName.c_str(), O_RDWR | O_BINARY);
                    chsize(handle, 18 + filesize); // obciêcie œmieci
                    close(handle);
                    extraend = 0; // skoro obciêty, to siê ju¿ nie liczy
                }
                file.open(fileName.c_str(), std::ios::binary | std::ios::in);
            }
            if (chunkheader < 128) // jeœli ostatnie piksele s¹ kopiowane
                copyend -= (1 + chunkheader) *
                           bytesPerPixel; // bajty kopiowane na koñcu nie podlegaj¹ce dekompresji
            else
                copyend -= bytesPerPixel; // ostatni piksel i tak siê nie zmieni
            copyto = imageData; // teraz bêdzie wype³nianie od pocz¹tku obszaru
            while (copyto < copyend)
            {
                chunkheader = (unsigned char)*copyfrom; // jeden bajt, pozosta³e zawsze zerowe
                if (copyto > copyfrom)
                { // jeœli piksele maj¹ byæ kopiowane, to mo¿liwe jest przesuniêcie ich o 1 bajt, na
                    // miejsce licznika
                    filesize = (imageData + imageSize - copyto) /
                               bytesPerPixel; // ile pikseli pozosta³o do koñca
                    // WriteLog("Decompression buffer overflow at pixel
                    // "+AnsiString((copyto-imageData)/bytesPerPixel)+"+"+AnsiString(filesize));
                    // pozycjê w pliku trzeba by zapamietaæ i po wczytaniu reszty pikseli star¹
                    // metod¹
                    // zapisaæ od niej dane od (copyto), poprzedzone bajtem o wartoœci (filesize-1)
                    writeback = imageData + imageSize + extraend -
                                copyfrom; // ile bajtów skompresowanych zosta³o do koñca
                    copyfrom = copyto; // adres piksela do zapisania
                    file.seekg(-writeback, ios::end); // odleg³oœæ od koñca (ujemna)
                    if ((filesize > 128) ||
                        !(Global::iModifyTGA & 4)) // gdy za du¿o pikseli albo wy³¹czone
                        writeback = -1; // zapis mo¿liwe jeœli iloœæ problematycznych pikseli nie
                    // przekaracza 128
                    break; // bufor siê zatka³, dalej w ten sposób siê nie da
                }
                if (chunkheader < 128)
                { // dla nag³ówka < 128 mamy podane ile pikseli przekopiowaæ minus 1
                    copybytes = (++chunkheader) * bytesPerPixel; // rozmiar kopiowanego obszaru
                    memcpy(copyto, ++copyfrom, copybytes); // skopiowanie tylu bajtów
                    copyto += copybytes;
                    copyfrom += copybytes;
                }
                else
                { // chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
                    chunkheader -= 127;
                    // copy the color into the image data as many times as dictated
                    if (bytesPerPixel == 4)
                    { // przy czterech bajtach powinno byæ szybsze u¿ywanie int
                        __int32 *ptr = (__int32 *)(copyto); // wskaŸnik na int
                        __int32 bgra = *((__int32 *)++copyfrom); // kolor wype³niaj¹cy (4 bajty)
                        for (int counter = 0; counter < chunkheader; counter++)
                            *ptr++ = bgra;
                        copyto = (char *)(ptr); // rzutowanie, ¿eby nie dodawaæ
                        copyfrom += 4;
                    }
                    else
                    {
                        colorbuffer[0] = *((int *)(++copyfrom)); // pobranie koloru (3 bajty)
                        for (int counter = 0; counter < chunkheader; counter++)
                        { // by the header
                            memcpy(copyto, colorbuffer, 3);
                            copyto += 3;
                        }
                        copyfrom += 3;
                    }
                }
            } // while (copyto<copyend)
        }
        else
        {
            WriteLog("Compressed file is larger than uncompressed!");
            if (Global::iModifyTGA & 1)
                writeback = 0; // no zapisaæ ten krótszy zaczynajac od pocz¹tku...
        }
        // if (copyto<copyend) WriteLog("Slow loader...");
        while (copyto < copyend)
        { // Ra: stare wczytywanie skompresowanych, z nadu¿ywaniem file.read()
            // równie¿ wykonywane, jeœli dekompresja w buforze przekroczy jego rozmiar
            file.read((char *)&chunkheader, 1); // jeden bajt, pozosta³e zawsze zerowe
            if (file.eof())
            {
                MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK); // display error
                delete[] imageData;
                file.close();
                return fail;
            };
            if (chunkheader < 128)
            { // if the header is < 128, it means the that is the number of RAW color packets minus
                // 1
                chunkheader++; // add 1 to get number of following color values
                file.read(copyto, chunkheader * bytesPerPixel);
                copyto += chunkheader * bytesPerPixel;
            }
            else
            { // chunkheader>128 RLE data, next color reapeated (chunkheader-127) times
                chunkheader -= 127;
                file.read((char *)colorbuffer, bytesPerPixel);
                // copy the color into the image data as many times as dictated
                if (bytesPerPixel == 4)
                { // przy czterech bajtach powinno byæ szybsze u¿ywanie int
                    __int32 *ptr = (__int32 *)(copyto), bgra = *((__int32 *)colorbuffer);
                    for (int counter = 0; counter < chunkheader; counter++)
                        *ptr++ = bgra;
                    copyto = (char *)(ptr); // rzutowanie, ¿eby nie dodawaæ
                }
                else
                    for (int counter = 0; counter < chunkheader; counter++)
                    { // by the header
                        memcpy(copyto, colorbuffer, bytesPerPixel);
                        copyto += bytesPerPixel;
                    }
            }
        } // while (copyto<copyend)
        if (writeback >= 0)
        { // zapisanie pliku
            file.close(); // tamten zamykamy, bo by³ tylko do odczytu
            if (writeback)
            { // zapisanie samej koñcówki pliku, która utrudnia dekompresjê w buforze
                WriteLog("Rewriting end of file...");
                chunkheader = filesize - 1; // licznik jest o 1 mniejszy
                file.open(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::in);
                file.seekg(-writeback, ios::end); // odleg³oœæ od koñca (ujemna)
                file.write((char *)&chunkheader, 1); // zapisanie licznika
                file.write(copyfrom, filesize * bytesPerPixel); // piksele bez kompresji
            }
            else
            { // zapisywanie ca³oœci pliku, bêdzie krótszy, wiêc trzeba usun¹æ go w ca³oœci
                WriteLog("Writing uncompressed file...");
                TGAcompare[2] = 2; // bez kompresji
                file.open(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
                file.write((char *)TGAcompare, sizeof(TGAcompare));
                file.write((char *)header, sizeof(header));
                file.write(imageData, imageSize);
            }
        }
    };
    file.close(); // plik zamykamy dopiero na samym koñcu
    bool alpha = (bpp == 32);
    bool hash = (fileName.find('#') != std::string::npos); // true gdy w nazwie jest "#"
    bool dollar = (fileName.find('$') == std::string::npos); // true gdy w nazwie nie ma "$"
    size_t pos = fileName.rfind('%'); // ostatni % w nazwie
    if (pos != std::string::npos)
        if (pos < fileName.size())
        {
            filter = (int)fileName[pos + 1] - '0'; // zamiana cyfry za % na liczbê
            if ((filter < 0) || (filter > 10))
                filter = -1; // jeœli nie jest cyfr¹
        }
    if (!alpha && !hash && dollar && (filter < 0))
        filter = Global::iDefaultFiltering; // dotyczy tekstur TGA bez kana³u alfa
    // ewentualne przeskalowanie tekstury do dopuszczalnego rozumiaru
    GLint w = width, h = height;
    if (width > Global::iMaxTextureSize)
        width = Global::iMaxTextureSize; // ogranizczenie wielkoœci
    if (height > Global::iMaxTextureSize)
        height = Global::iMaxTextureSize;
    if ((w != width) || (h != height))
    { // przeskalowanie tekstury, ¿eby siê nie wyœwietla³a jako bia³a
        GLubyte *imgData = new GLubyte[width * height * bytesPerPixel]; // nowy rozmiar
        gluScaleImage(bytesPerPixel == 3 ? GL_RGB : GL_RGBA, w, h, GL_UNSIGNED_BYTE, imageData,
                      width, height, GL_UNSIGNED_BYTE, imgData);
        delete imageData; // usuniêcie starego
        imageData = imgData;
    }
    GLuint id = CreateTexture(imageData, (alpha ? GL_BGRA : GL_BGR), width, height, alpha, hash,
                              dollar, filter);
    delete[] imageData;
    ++Global::iTextures;
    return std::make_pair(id, alpha);
};

TTexturesManager::AlphaValue TTexturesManager::LoadTEX(std::string fileName)
{

    AlphaValue fail(0, false);

    std::ifstream file(fileName.c_str(), ios::binary);

    char head[5];
    file.read(head, 4);
    head[4] = 0;

    bool alpha;

    if (std::string("RGB ") == head)
    {
        alpha = false;
    }
    else if (std::string("RGBA") == head)
    {
        alpha = true;
    }
    else
    {
        std::string message("Unrecognized texture format: ");
        message += head;
        Error(message.c_str());
        return fail;
    };

    GLuint width;
    GLuint height;

    file.read((char *)&width, sizeof(int));
    file.read((char *)&height, sizeof(int));

    GLuint bpp = alpha ? 4 : 3;
    GLuint size = width * height * bpp;

    GLubyte *data = new GLubyte[size];
    file.read(data, size);

    bool hash = (fileName.find('#') != std::string::npos);

    GLuint id = CreateTexture(data, (alpha ? GL_RGBA : GL_RGB), width, height, alpha, hash);
    delete[] data;

    return std::make_pair(id, alpha);
};

TTexturesManager::AlphaValue TTexturesManager::LoadDDS(std::string fileName, int filter)
{

    AlphaValue fail(0, false);

    std::ifstream file(fileName.c_str(), ios::binary);

    char filecode[5];
    file.read(filecode, 4);
    filecode[4] = 0;

    if (std::string("DDS ") != filecode)
    {
        file.close();
        return fail;
    };

    DDSURFACEDESC2 ddsd;
    file.read((char *)&ddsd, sizeof(ddsd));

    DDS_IMAGE_DATA data;

    //
    // This .dds loader supports the loading of compressed formats DXT1, DXT3
    // and DXT5.
    //

    GLuint factor;

    switch (ddsd.ddpfPixelFormat.dwFourCC)
    {
    case FOURCC_DXT1:
        // DXT1's compression ratio is 8:1
        data.format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        factor = 2;
        break;

    case FOURCC_DXT3:
        // DXT3's compression ratio is 4:1
        data.format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        factor = 4;
        break;

    case FOURCC_DXT5:
        // DXT5's compression ratio is 4:1
        data.format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        factor = 4;
        break;

    default:
        file.close();
        return fail;
    }

    GLuint bufferSize = (ddsd.dwMipMapCount > 1 ? ddsd.dwLinearSize * factor : ddsd.dwLinearSize);

    data.pixels = new GLubyte[bufferSize];
    file.read((char *)data.pixels, bufferSize);

    file.close();

    data.width = ddsd.dwWidth;
    data.height = ddsd.dwHeight;
    data.numMipMaps = ddsd.dwMipMapCount;
    { // sprawdzenie prawid³owoœci rozmiarów
        int i, j;
        for (i = data.width, j = 0; i; i >>= 1)
            if (i & 1)
                ++j;
        if (j == 1)
            for (i = data.height, j = 0; i; i >>= 1)
                if (i & 1)
                    ++j;
        if (j != 1)
            WriteLog("Bad texture: " + AnsiString(fileName.c_str()) + " is " +
                     AnsiString(data.width) + "×" + AnsiString(data.height));
    }

    if (ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DXT1)
        data.components = 3;
    else
        data.components = 4;

    data.blockSize = (data.format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16);

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    if (filter >= 0)
        SetFiltering(filter); // cyfra po % w nazwie
    else
        // SetFiltering(bHasAlpha&&bDollar,bHash); //znaki #, $ i kana³ alfa w nazwie
        SetFiltering(data.components == 4, fileName.find('#') != std::string::npos);

    GLuint offset = 0;
    int firstMipMap = 0;

    while ((data.width > Global::iMaxTextureSize) || (data.height > Global::iMaxTextureSize))
    { // pomijanie zbyt du¿ych mipmap, jeœli wymagane jest ograniczenie rozmiaru
        offset += ((data.width + 3) / 4) * ((data.height + 3) / 4) * data.blockSize;
        data.width /= 2;
        data.height /= 2;
        firstMipMap++;
    };

    for (int i = 0; i < data.numMipMaps - firstMipMap; i++)
    { // wczytanie kolejnych poziomów mipmap
        if (!data.width)
            data.width = 1;
        if (!data.height)
            data.height = 1;
        GLuint size = ((data.width + 3) / 4) * ((data.height + 3) / 4) * data.blockSize;
        if (Global::bDecompressDDS)
        { // programowa dekompresja DDS
            // if (i==1) //should be i==0 but then problem with "glBindTexture()"
            {
                GLuint decomp_size = data.width * data.height * 4;
                GLubyte *output = new GLubyte[decomp_size];
                DecompressDXT(data, data.pixels + offset, output);
                glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, data.width, data.height, 0, GL_RGBA,
                             GL_UNSIGNED_BYTE, output);
                delete[] output;
            }
        }
        else // przetwarzanie DDS przez OpenGL (istnieje odpowiednie rozszerzenie)
            glCompressedTexImage2D(GL_TEXTURE_2D, i, data.format, data.width, data.height, 0, size,
                                   data.pixels + offset);
        offset += size;
        // Half the image size for the next mip-map level...
        data.width /= 2;
        data.height /= 2;
    };
    delete[] data.pixels;
    return std::make_pair(id, data.components == 4);
};

void TTexturesManager::SetFiltering(int filter)
{
    if (filter < 4) // rozmycie przy powiêkszeniu
    { // brak rozmycia z bliska - tych jest 4: 0..3, aby nie by³o przeskoku
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        filter += 4;
    }
    else
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    switch (filter) // rozmycie przy oddaleniu
    {
    case 4: // najbli¿szy z tekstury
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    case 5: //œrednia z tekstury
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case 6: // najbli¿szy z mipmapy
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        break;
    case 7: //œrednia z mipmapy
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    case 8: // najbli¿szy z dwóch mipmap
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        break;
    case 9: //œrednia z dwóch mipmap
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
};

void TTexturesManager::SetFiltering(bool alpha, bool hash)
{

    if (alpha || hash)
    {
        if (alpha) // przezroczystosc: nie wlaczac mipmapingu
        {
            if (hash) // #: calkowity brak filtracji - pikseloza
            {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
            else
            {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
        }
        else // filtruj ale bez dalekich mipmap - robi artefakty
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
    }
    else // $: filtruj wszystko - brzydko siê zlewa
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
};

///////////////////////////////////////////////////////////////////////////////
GLuint TTexturesManager::CreateTexture(char *buff, GLint bpp, int width, int height, bool bHasAlpha,
                                       bool bHash, bool bDollar, int filter)
{ // Ra: u¿ywane tylko dla TGA i TEX
    // Ra: dodana obs³uga GL_BGR oraz GL_BGRA dla TGA - szybciej siê wczytuje
    GLuint ID;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (filter >= 0)
        SetFiltering(filter); // cyfra po % w nazwie
    else
        SetFiltering(bHasAlpha && bDollar, bHash); // znaki #, $ i kana³ alfa w nazwie
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    if (bHasAlpha || bHash || (filter == 0))
        glTexImage2D(GL_TEXTURE_2D, 0, (bHasAlpha ? GL_RGBA : GL_RGB), width, height, 0, bpp,
                     GL_UNSIGNED_BYTE, buff);
    else
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, bpp, GL_UNSIGNED_BYTE, buff);
    return ID;
}

void TTexturesManager::Free()
{ // usuniêcie wszyskich tekstur (bez usuwania struktury)
    for (Names::iterator iter = _names.begin(); iter != _names.end(); iter++)
        glDeleteTextures(1, &(iter->second));
}

std::string TTexturesManager::GetName(GLuint id)
{ // pobranie nazwy tekstury
    for (Names::iterator iter = _names.begin(); iter != _names.end(); iter++)
        if (iter->second == id)
            return iter->first;
    return "";
};
#pragma package(smart_init)
