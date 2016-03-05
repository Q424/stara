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
 
#ifndef _RGB_PIXMAP
#define _RGBPIXMAP
//#include <windows.h> // strange!! without this, you'll get enormous error!! 
//#include <gl\gl.h>
//#include <gl\glu.h>
#include <gl\glut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

using namespace std;
typedef unsigned char uchar;
// ********** mRGBA class **********
class mRGBA { // the name RGBA is already used by Windows
public: uchar r, g, b, a;
		mRGBA() { r=g=b=0; a=255; }
		mRGBA(mRGBA &p) { r=p.r; g=p.g; b=p.b; a=p.a; }
		mRGBA(uchar rr, uchar gg, uchar bb, uchar aa)
		{ r=rr; g=gg; b=bb; a=aa; }
		void set(uchar rr, uchar gg, uchar bb, uchar aa)
		{ r=rr; g=gg; b=bb; a=aa; }
};

// *** for 2D points with integer coordinates
class IntPoint {
public:
	int x, y;
	IntPoint() { x = y = 0; }
	IntPoint(int xx, int yy) { x = xx; y = yy; }
	void set(int xx, int yy) { x = xx; y = yy; }
	void set(IntPoint &p) { x = p.x; y = p.y; }
};

class IntRect {
public:
	int left, top, right, bott;
	IntRect() { left = top = right = bott = 0; }
	IntRect(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bott = b; }
	void set(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bott = b; }
	void set(IntRect &r)
	{ left = r.left; top = r.top; right = r.right; bott = r.bott; }
};

// ********** RGBPixmap class ************
class RGBApixmap {
private:
	mRGBA *pixel;  // array of pixels
public:
	int nRows, nCols;                       // dimensions of pixel map
	RGBApixmap() { nRows = nCols = 0; pixel = 0; }
	RGBApixmap(int rows, int cols) {
		nRows = rows;
		nCols = cols;
		pixel = new mRGBA[rows * cols];
	};

	// *** read BMP file into this pixel map (see RGBpixmap.cpp)
	int readBMPFile(string fname, float para, int flag);

	void freeIt() {
		delete []pixel;
		nRows = nCols = 0;
	};


	// *** copy a region of the dispaly back onto the display
	void mCopy(IntPoint from, IntPoint to, int x, int y, int width, int height) {
		if (nRows == 0 || nCols == 0) 
			return;
		glCopyPixels(x, y, width, height, GL_COLOR);
	};


	// *** draw this pixel map at current position
	void mDraw() {
		if (nRows == 0 || nCols == 0) 
			return;
		// tell OpenGL: don't align pixels with 4-byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glDrawPixels(nCols, nRows, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	};


	// *** read a rectangule of pixels into this pixmap
	int mRead(int x, int y, int wid, int ht) {
		nRows = ht;
		nCols = wid;
		pixel = new mRGBA[nRows*nCols];
		if ( !pixel ) return -1;
		// tell OpenGL: don't align pixels with 4-byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glReadPixels(x, y, nCols, nRows, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		return 0;
	};


	// *** read a rectangule of pixels into this pixmap
	int mRead(IntRect r) {
		nRows = r.top - r.bott;
		nCols = r.right - r.left;
		pixel = new mRGBA[nRows*nCols];
		if ( !pixel ) return -1;
		// tell OpenGL: don't align pixels with 4-byte boundaries in memory
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glReadPixels(r.left, r.bott, nCols, nRows, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		return 0;
	};


	// *** setPixel 
	void setPixel(int x, int y, mRGBA color) {
		if ( x>=0 && x<nCols && y>=0 && y<nRows)
			pixel[nCols*y + x] = color;
	};


	// *** get pixel
	mRGBA getPixel(int x, int y) {
		mRGBA bad(255, 255, 255, 255);
		assert(x>=0 && x<nCols);
		assert(y>=0 && y<nRows);
		return pixel[nCols*y+x];
	};


	// these 2 are used in texture example
	void makeCheckerboard(int);
	void setTexture(GLuint textureName);


}; // class RGBApixmap

#endif
