/*
*  Created on: Dec 02, 2004
*  Last modified on: Nov 18, 2009
*  Author: Changwei Xiong
*  
*  Copyright (C) 2009, Changwei Xiong, 
*  axcw@hotmail.com, <http://www.cs.utah.edu/~cxiong/>
*
*  Licence: Revised BSD License
*/
 
#include "pixmap.h"
#include <math.h>

typedef unsigned short ushort;
typedef unsigned long  ulong;
fstream inf;        // global in this file for convenience


/* *** Helper function getShort() 
 * BMP format uses little-endian integer types
 * get and construct inmemory a 2-byte integer stored
 * in little-endian form
 */

ushort getShort() {
  char ic;
  ushort ip;
  inf.get(ic);
  ip = ic;              // 1st byte is little one
  inf.get(ic);
  ip |= ((ushort)ic<<8);       // or in high order byte
  return ip;
}

/* *** Helper function getLong() 
 * BMP format uses little-endian integer types
 * get and construct inmemory a 4-byte integer stored
 * in little-endian form
 */
ulong getLong() {
  ulong ip = 0;
  char  ic = 0;
  unsigned char uc = ic;
  
  
  inf.get(ic); uc = ic; ip = uc;
  inf.get(ic); uc = ic; ip |= ((ulong)uc << 8);
  inf.get(ic); uc = ic; ip |= ((ulong)uc <<16);
  inf.get(ic); uc = ic; ip |= ((ulong)uc <<24);
  return ip;
};


inline float fx(float x, float a0, float a1) {
  float y, t;
  t = x/a1;
  y = a0 * exp(-(t*t));
  return y;
}


inline
int getAlpha(int row, int col, int nRows,int nCols, float (*p)(float, float, float), float a1) {
  int   rowCenter, colCenter, alpha;
  float dx, dy, dist=0.0;
 
  rowCenter = (float)nRows/2.0;
  colCenter = (float)nCols/2.0;
  dx        = row - rowCenter;
  dy        = col - colCenter;
  dist      = sqrt(dx * dx + dy * dy);
  alpha     = (int)(*p)(dist, 255.0, a1);
  return alpha;
};


// *** Read into memory an mRGB image from an uncompressed BMP file
// return 0 on fail, 1 for Ok
int RGBApixmap::readBMPFile(string fname, float para, int flag) {
  int k, row, col, numPadBytes, nBytesInRow;
  char ch1, ch2;
  
  
  inf.open(fname.c_str(), ios::in|ios::binary); // read binary chars
  if (!inf) {
    cout << "can't open file: "<<fname<<endl;
	//exit(0);
    return 0;
  }
  
  
  inf.get(ch1); inf.get(ch2); // type: always 'BM'
  ulong  fileSize     = getLong();
  ushort reserved1    = getShort(); // always 0
  ushort reserved2    = getShort(); // always 0
  ulong  offBits      = getLong();  // offset to image-unreliable
  
  
  ulong  headerSize   = getLong();  // always 40
  ulong  numCols      = getLong();  // number of columns in image
  ulong  numRows      = getLong();  // number of rows in image
  
  
  //cout <<ch1<<ch2<<numRows<<"x"<<numCols<<cout.flush()<<endl;
  
  ushort planes       = getShort(); // always 1
  ushort bitsPerPixel = getShort(); // 8 or 24; allow only 24 here
  ulong  compression  = getLong();  // must be 0 for uncompressed
  ulong  imageSize    = getLong();  // total bytes in image
  ulong  xPels        = getLong();  // always 0
  ulong  yPels        = getLong();  // always 0
  ulong  numLUTentries= getLong();  // 256 for 8 bit, otherwise 0
  ulong  impColors    = getLong();  // always 0  
  
  if (bitsPerPixel !=24) {
    cout << "not a 24-bit pixel image, or is compressed!\n";
    inf.close();
                                    return 0 ; // failed
  }
  
  
  // add bytes at end of each row so total# is a multiple of 4
  // round up 3*numCols to next mult. of 4
  nBytesInRow = ((3*numCols + 3)/4) * 4;
  numPadBytes = nBytesInRow - 3*numCols; // need this many
  nRows = numRows; // set class's data members
  nCols = numCols;
  pixel = new mRGBA[nRows*nCols]; // make space for array
  if (!pixel) return 0; // out of memory!
  long count = 0;
  char dum;
  for(row=0; row<nRows; row++) {
    for(col=0; col<nCols; col++) {
      char r,g,b;
      inf.get(b); inf.get(g); inf.get(r); // read bytes

	 // cout<<r<<"  "<<g<<"   "<<b<<"   "<<endl;
      unsigned ir = pixel[count].r = r; // place them in colors
      unsigned ig = pixel[count].g = g; // place them in colors
      unsigned ib = pixel[count].b = b; // place them in colors
	  //cout<<ir<<"  "<<ig<<"  "<<ib<<"  "<<endl;
	  if(flag == 0){//original function
		  pixel[count].a = (uchar)getAlpha(row,col,nRows,nCols,fx,para); // set Alpha value to opaque
	  }
	  else if(flag == 1){
		  if(ir<3 && ig<3 && ib<3)
			  pixel[count].a = 0;
		  else 
			  pixel[count].a = 255;
	  }
	  else if(flag == 2){
		  float luma = (r*0.3 + g*0.59+ b*0.11)/256.0;//convert grayscale
   		  pixel[count].a = (uchar)(luma*getAlpha(row,col,nRows,nCols,fx,para));
	  }
                          
      count++;
    }
    for(k=0; k<numPadBytes; k++) // skip pad bytes at row's end
      inf >> dum;
  }
  inf.close();
                        return 1;
} // end of readBMPFile


// *** make a procedure texture: checkerboard
void RGBApixmap::makeCheckerboard(int alpha) {
  nRows = nCols = 64;
  pixel = new mRGBA[3*nRows*nCols];
  if (!pixel) {
    cout << "out of memory!\n";
    return;
  }
  long count = 0;
  for (int i=0; i<nRows; i++)
    for (int j=0; j<nCols; j++) {
      int c = ( ( (i/8)+(j/8) ) % 2 ) * 255;
      pixel[count].r = c;
      pixel[count].g = c;
      pixel[count].b = 0;
      pixel[count++].a = (uchar)alpha;
    }
}

// *** create a texture object, bind ID and load the texture object
// from the image file
void RGBApixmap::setTexture(GLuint textureName) {
  glBindTexture(GL_TEXTURE_2D, textureName);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,nCols, nRows, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, nCols, nRows, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
  cout << "set texture " << textureName << endl;
} // setTexture
