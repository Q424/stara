/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#pragma hdrstop

#include "system.hpp"
#include "classes.hpp"
#include "globals.h"
#include "qutils.h"

psmokeemitercontainer PSYS::sec[64];
pfireemitercontainer PSYS::fec[32];

// ***********************************************************************************************************
// SMOKE PARTICLE SYSTEM
// ***********************************************************************************************************

void psmokeemitercontainer::loadtexture(int ts)
{
 RGBApixmap SmokePixx;
 glGenTextures(1, &SmokeTex);
 SmokePixx.readBMPFile(".\\data\\gfxs\\smoke.bmp", ts, 0);
 SmokePixx.setTexture(SmokeTex);
}

void psmokeemitercontainer::setSmoke(vector3 pos, float rmaxdist, float sizemin, float sizemax, float speedmin, float speedmax, float spinspeedmin, float spinspeedmax, float maxdist, float rcolorf, float rcolort, float opacity, int seedingspeed, int ts)
{
 SMOKE_MAXDIST = rmaxdist;
 SMOKE_POSITION = pos;
 SMOKE_SIZE_MIN = sizemin;
 SMOKE_SIZE_MAX = sizemax;
 SMOKE_SPEED_MIN = speedmin;
 SMOKE_SPEED_MAX = speedmax;
 SMOKE_SEEDING_SPEED = seedingspeed;
 SMOKE_SPIN_SPEED_MAX = spinspeedmax;
 SMOKE_RCOLOR_F = rcolorf;
 SMOKE_RCOLOR_T = rcolort;
 SMOKE_MAX_DIST = maxdist;
 SMOKE_OPACITY = opacity;

 loadtexture(ts);
 PSYS::smoke_tid++;
}

void psmokeemitercontainer::createSmoke()
{


	for(int i =0; i<SMOKE_SEEDING_SPEED; i++){
		Particle par;
		par.pos.x = 0;
		par.pos.y = 0;
		par.pos.z = 0;
		par.size = random(SMOKE_SIZE_MIN, SMOKE_SIZE_MAX);
		par.spin = random(0,360);  
		par.spinspd = random(-SMOKE_SPIN_SPEED_MAX, SMOKE_SPIN_SPEED_MAX);
		par.color = random(SMOKE_RCOLOR_F, SMOKE_RCOLOR_T);

		par.para[ 0] = random(3.0, 3.0);
		par.para[ 1] = random(0.4, 0.7);
		par.para[ 2] = random(0.2, 0.2);
		par.para[ 3] = random(2.0, 2.0);
		par.para[ 4] = random(0.2, 0.6);
		par.para[ 5] = random(0.5, 0.8);

		par.para[ 6] = random(-0.5, 0.5);
		par.para[ 7] = random(0.1, 0.2);
		par.para[ 8] = random(0.1, 0.7);
		par.para[ 9] = random(0.2, 0.3);
		par.para[10] = random(0.2, 1.0);  

		Smoke.push(par);
	}

}

void psmokeemitercontainer::smokeMotion(Particle &p)
{
	p.pos.x += random(SMOKE_SPEED_MIN, SMOKE_SPEED_MAX);
	float x = p.pos.x;
	p.pos.y = p.para[0]*pow(x,p.para[1])+p.para[2]*sin(p.para[3]*x)+p.para[4]*sin(p.para[5]*x);
	p.pos.z = p.para[6]*x+p.para[7]*sin(p.para[8]*x)+p.para[9]*sin(p.para[10]*x);
	p.spin += p.spinspd;
}

void psmokeemitercontainer::updateSmoke()
{
	//int i = 0;
	ParNode** npp = &(Smoke.head);
	while(*npp) {

		smokeMotion((*npp)->par);
              //WriteLog(FloatToStr((*npp)->par.pos.y) + " =  " + FloatToStr(SMOKE_MAX_DIST));
	       //	if((*npp)->par.pos.y > SMOKE_MAX_DIST /**/)
                if((*npp)->par.opacity < 0.01 /**/)
                {
			Smoke.erase(npp);
                }
		else 
			npp = &((*npp)->next);
	} 

	//cout << i << endl;
	createSmoke();
}

void psmokeemitercontainer::drawSmoke(vector3 cp)
{
  xvector vWorldUp(0,1,0);
  xvector SmokeOrigin(SMOKE_POSITION.x, SMOKE_POSITION.y, SMOKE_POSITION.z);//smoke origin

  GLboolean blendEnabled;
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glGetBooleanv(GL_BLEND, &blendEnabled);
  GLint value = 0;
  glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &value);

  xvector eyepos(SMOKE_POSITION.x, SMOKE_POSITION.y, SMOKE_POSITION.z);

  glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glDepthMask(0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindTexture(GL_TEXTURE_2D, SmokeTex);
  
	ParNode* np = Smoke.head;
	while(np) {
                glPushMatrix();
           	Particle &par = np->par;



			xvector z =SmokeOrigin + par.pos - eyepos;
			z.normalize();
			xvector x = vWorldUp ^ z;
			x.normalize();
			xvector y = z ^ x;
			float rotate[16] = {x.x, x.y, x.z, 0, y.x, y.y, y.z, 0,	z.x, z.y, z.z,   0, 0, 0,   0, 1};
			glTranslatef(par.pos.x + SmokeOrigin.x, par.pos.y + SmokeOrigin.y, par.pos.z + SmokeOrigin.z);

			float a = (SMOKE_MAX_DIST-par.pos.length())/ SMOKE_MAX_DIST;
			float aaa = a*a*a;
                        par.opacity = aaa*SMOKE_OPACITY;

		       //	glMultMatrixf(rotate);            // + duzoFPS
		       //	glRotatef(par.spin, 0,0,1);       // + duzoFPS


                      //BILLBOARDING - cos nie tak to dziala jak powinno, ale po wlaczeinu renderowania dwustronnego raczej juz nie potrzebne
                      //matrix4x4 mat; //potrzebujemy wspó³rzêdne przesuniêcia œrodka uk³adu wspó³rzêdnych submodelu
                      //glGetDoublev(GL_MODELVIEW_MATRIX,mat.getArray()); //pobranie aktualnej matrycy
                      //float3 gdzie = float3(mat[3][0],mat[3][1],mat[3][2]); //pocz¹tek uk³adu wspó³rzêdnych submodelu wzglêdem kamery
                      //glRotated(atan2(gdzie.x,gdzie.z)*180.0/M_PI,0.0,1.0,0.0); //jedynie obracamy w pionie o k¹t

			glBegin(GL_QUADS);
				glColor4f(par.color, par.color, par.color, par.opacity);
				glTexCoord2f( 0.0,  0.0); glVertex3f( par.size,  par.size, 0);
				glTexCoord2f( 1.0,  0.0); glVertex3f(-par.size,  par.size, 0);
				glTexCoord2f( 1.0,  1.0); glVertex3f(-par.size, -par.size, 0);
				glTexCoord2f( 0.0,  1.0); glVertex3f( par.size, -par.size, 0);
			glEnd();

		glPopMatrix();
		np = np->next;
	}
        glDepthMask(1);
        glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
        glBlendFunc(blendSrc, blendDst);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, value);
        glPolygonMode(GL_FRONT, GL_FILL);

}

void Global::rendersmokeem()
{
 for (int i = 0; i < PSYS::smoke_tid; i++)
  {
   PSYS::sec[i].updateSmoke();
   PSYS::sec[i].drawSmoke(vector3(0, 0, 0));
  }
}


// ***********************************************************************************************************
// FIRE PARTICLE SYSTEM
// ***********************************************************************************************************

void pfireemitercontainer::loadtexture(int ts)
{
 RGBApixmap FirePixx;
 glGenTextures(1, &FireTex);
 FirePixx.readBMPFile("./data/gfxs/fire.bmp", ts, 0);
 FirePixx.setTexture(FireTex);
}

void pfireemitercontainer::setsFire(vector3 pos, float rmaxdist, float sizemin, float sizemax, float speedmin, float speedmax, float spinspeedmin, float spinspeedmax, float maxdist, float rcolorf, float rcolort, float opacity, int seedingspeed, int ts)
{
 FIRE_MAXDIST = rmaxdist;
 FIRE_POSITION = pos;
 FIRE_SIZE_MIN = sizemin;
 FIRE_SIZE_MAX = sizemax;
 FIRE_SPEED_MIN = speedmin;
 FIRE_SPEED_MAX = speedmax;
 FIRE_SPIN_SPEED_MAX = spinspeedmax;
 FIRE_MAX_DIST = maxdist;
 FIRE_SEEDING_SPEED = seedingspeed;

 loadtexture(ts);
 PSYS::fire_tid++;
}

void pfireemitercontainer::createFire(){
	for(int i =0; i<FIRE_SEEDING_SPEED; i++){
		Particle par;
		par.pos.x = 0;
		par.pos.y = 0;
		par.pos.z = 0;
		par.size = random(FIRE_SIZE_MIN, FIRE_SIZE_MAX);
		par.spin = random(0,360);  
		par.spinspd = random(-FIRE_SPIN_SPEED_MAX, FIRE_SPIN_SPEED_MAX);

		par.para[ 0] = random(0.5, 0.1); //
		par.para[ 1] = random(0.3, 0.1);
		par.para[ 2] = random(0.2, 0.2);
		par.para[ 3] = random(2.0, 2.0);
		par.para[ 4] = random(0.2, 0.6);
		par.para[ 5] = random(0.5, 0.8);
		float angle = random(0, PI*2);
		par.para[ 6] = cos(angle);
		par.para[ 7] = sin(angle);

		Fire.push(par);
	}

}

void pfireemitercontainer::motionFire(Particle &p){
	p.pos.y += random(FIRE_SPEED_MIN, FIRE_SPEED_MAX);
	float y = p.pos.y;
	float r = (p.para[0]*pow(y,p.para[1]));//+p.para[2]*sin(p.para[3]*y)+p.para[4]*sin(p.para[5]*y));
	//cout << y<<"    "<<r<<endl;
	p.pos.x = r*p.para[6];
	p.pos.z = r*p.para[7];
	p.spin += p.spinspd;
}

void pfireemitercontainer::updateFire(){
	//int i = 0;	
	ParNode** npp = &(Fire.head);
	while(*npp) {
		//i++;
		motionFire((*npp)->par);
		if((*npp)->par.pos.length() > FIRE_MAX_DIST)
			Fire.erase(npp);
		else 
			npp = &((*npp)->next);
	}  
	//cout << i << endl;
	createFire();   
}

void pfireemitercontainer::drawFire(vector3 cp)
{
xvector vWorldUp(0,1,0);
xvector FLAME(0,0,0.1);
xvector FireOrigin(FIRE_POSITION.x, FIRE_POSITION.y, FIRE_POSITION.z);//fire origin

  GLboolean blendEnabled;
  GLint blendSrc;
  GLint blendDst;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);
  glGetBooleanv(GL_BLEND, &blendEnabled);
  GLint value = 0;
  glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &value);
  xvector eyepos(cp.x, cp.y, cp.z);

  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_CULL_FACE);
  glDepthMask(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D, FireTex);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ParNode* np = Fire.head;
	while(np) {
		glPushMatrix();
           	Particle &par = np->par; 

			xvector z =FireOrigin + par.pos - eyepos;
			z.normalize();
			xvector x = vWorldUp ^ z;
			x.normalize(); 
			xvector y = z ^ x;
			float rotate[16] = {x.x, x.y, x.z, 0,
								y.x, y.y, y.z, 0,
								z.x, z.y, z.z, 0,
								  0,   0,   0, 1};
			glTranslatef(par.pos.x + FireOrigin.x, par.pos.y + FireOrigin.y, par.pos.z + FireOrigin.z);
			
			float dist = (par.pos - FLAME).length();
			float a = (FIRE_MAX_DIST - dist)/ FIRE_MAX_DIST;
			float aaa = a*a*a;


			glMultMatrixf(rotate);
			glRotatef(par.spin, 0,0,1);


                      //BILLBOARDING
                        matrix4x4 mat; //potrzebujemy wspó³rzêdne przesuniêcia œrodka uk³adu wspó³rzêdnych submodelu
                      //  glGetDoublev(GL_MODELVIEW_MATRIX,mat.getArray()); //pobranie aktualnej matrycy
                      //  float3 gdzie = float3(mat[3][0],mat[3][1],mat[3][2]); //pocz¹tek uk³adu wspó³rzêdnych submodelu wzglêdem kamery
                      //  glRotated(atan2(gdzie.x,gdzie.z)*180.0/M_PI,0.0,1.0,0.0); //jedynie obracamy w pionie o k¹t

			glBegin(GL_QUADS);
				glColor4f(a, a ,a, aaa);
				glTexCoord2f( 0.0,  0.0); glVertex3f( par.size,  par.size, 0);
				glTexCoord2f( 1.0,  0.0); glVertex3f(-par.size,  par.size, 0);
				glTexCoord2f( 1.0,  1.0); glVertex3f(-par.size, -par.size, 0);
				glTexCoord2f( 0.0,  1.0); glVertex3f( par.size, -par.size, 0);
			glEnd();
		glPopMatrix();
		np = np->next;
	}

        glDepthMask(1);
        glEnable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
      //glEnable(GL_DEPTH_TEST);
        glBlendFunc(blendSrc, blendDst);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, value);
}


void Global::renderfireem()
{
 for (int i = 0; i < PSYS::fire_tid; i++)
  {
   PSYS::fec[i].updateFire();
   PSYS::fec[i].drawFire(vector3(0, 0, 0));
  }
}


//---------------------------------------------------------------------------

#pragma package(smart_init)
