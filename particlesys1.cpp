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
#include "camera.h"
#include "timer.h"
#include "qutils.h"
#include "addons.h"

psmokeemitercontainer PSYS::sec[64];
pfireemitercontainer PSYS::fec[32];
pfountainemitercontainer PSYS::fountainec[32];
pobstructlightscontainer PSYS::obstructlightsc[32];
pparteffectscontainer PSYS::parteffects[32];
std::string currentline;

void removeSpaces(std::string& str)
{
    /* remove multiple spaces */
    int k=0;
    for (int j=0; j<str.size(); ++j)
    {
            if ( (str[j] != ' ') || (str[j] == ' ' && str[j+1] != ' ' ))
            {
                    str [k] = str [j];
                    ++k;
            }

    }
    str.resize(k);

    /* remove space at the end */   
    if (str [k-1] == ' ')
            str.erase(str.end()-1);
    /* remove space at the begin */
    if (str [0] == ' ')
            str.erase(str.begin());
}

void pobstructlightscontainer::circleXYZ(vector3 center, vector3 cp, pobstructlightscontainer *CHL, int dots, int clevel)
{


if ( level[clevel].isOn && level[clevel].isBlink)
 {
  if (!level[clevel].cd)
   {
    if (level[clevel].a > level[clevel].fadeoff) level[clevel].a -= level[clevel].fadeoffspd;
    if (level[clevel].a < level[clevel].fadeoff)
     {
      double dt = Timer::GetDeltaTime();
      level[clevel].dpt += (dt);
      if (level[clevel].dpt > level[clevel].pauseOff) {level[clevel].cd = true; level[clevel].dpt = 0.0;}     // trzymanie w zgaszeniu
     }
   }
   else
   {
    if (level[clevel].a < level[clevel].fadein) level[clevel].a += level[clevel].fadeinspd;
    if (level[clevel].a > level[clevel].fadein)
     {
      double dt = Timer::GetDeltaTime();
      level[clevel].upt += (dt);
      if (level[clevel].upt > level[clevel].pauseOn) {level[clevel].cd = false; level[clevel].upt = 0.0;}   // trzymanie w zapaleniu do 30.0
     }
   }
 }
  if ( level[clevel].isOn && level[clevel].isBlink)  level[clevel].a = level[clevel].a;
  if ( level[clevel].isOn && !level[clevel].isBlink) level[clevel].a = 1.0;
  if (!level[clevel].isOn) level[clevel].a = 0.1;

  glColor4f(level[clevel].colorr, level[clevel].colorg, level[clevel].colorb, level[clevel].a);
  float stepSize = ((2*PI)/level[clevel].lights);
  float pointsize = 353.0f;
  glDisable(GL_TEXTURE_2D);
  
  for (float d = 0; d <= (2*PI)-stepSize; d += stepSize)
   {
      float CamDistToEmitter = VECTORLEN2(CH_POSITION, Global::GetCameraPosition());
      if (CamDistToEmitter < 0.1f) //avoid too big particles
      CamDistToEmitter = 0.1f;
      glPointSize(pointsize / CamDistToEmitter);

      glBegin(GL_POINTS);
      glVertex3f((sin(d) * level[clevel].radius) + center.x, level[clevel].height, (cos(d) * level[clevel].radius) + center.z);
      glEnd();
    }

}


// ***********************************************************************************************************
// OBSTRUCT LIGHTS SYSTEM
// ***********************************************************************************************************
bool pobstructlightscontainer::setsObstructLights(char* scriptfile)
{
 float x, y, z, md, lradius, lheight, lrotate, lsdelay, lfadeoff, lfadeoffspd, lfadein, lfadeinspd, lpauseOn, lpauseOff, lcolorr, lcolorg, lcolorb, lcolora;
 int i1, i2, i3, i4, i5, i6, i7, i8, i9, lev, clev, lights, lison, lisblink;
 color4 lcolor;
 bool commented, headerok, levelok, delayok;
 char line[256], fn[256];
 std::string xline, comment = "//";
 strcpy(fn, QGlobal::asAPPDIR.c_str()); strcat(fn, "data\\"); strcat(fn, scriptfile);

 if (!FileExists(fn)) {WriteLog("script cfile don't exist!"); return false;}

 headerok = false;
 clev = -1;
 
 FILE *file = fopen(fn, "r");

 while (fgets(line, sizeof(line), file))// note that fgets don't strip the terminating \n,
  {
   commented = false;
   levelok = false;
   delayok = false;
   
   line[strcspn(line, "\n")] = '\0';  // USUWANIE ZNAKU KONCA LINII
   currentline = line;
   currentline = chartostdstr(line);
   removeSpaces(currentline);

   if(sscanf(currentline.c_str(), "MAIN %f %f %f %f %i", &md, &x, &y, &z, &lev) == 5) headerok = true;
   if(sscanf(currentline.c_str(), "LEVEL %i %f %f %f %f %f %f %f %f %f %f %i %i %f %f %f %f", &lights,
                                                                                              &lradius,
                                                                                              &lheight,
                                                                                              &lrotate,
                                                                                              &lsdelay,
                                                                                              &lfadeoff,
                                                                                              &lfadeoffspd,
                                                                                              &lfadein,
                                                                                              &lfadeinspd,
                                                                                              &lpauseOn,
                                                                                              &lpauseOff,
                                                                                              &lison,
                                                                                              &lisblink,
                                                                                              &lcolorr,
                                                                                              &lcolorg,
                                                                                              &lcolorb,
                                                                                              &lcolora) == 17) levelok = true;

   if (headerok)
    {
     CH_POSITION = vector3(x, y, z);
     CH_MAXDIST = md;
     CH_LEVELS = lev;
    }

   if (levelok)
    {
     clev++;
     level[clev].a = 1.0f;
     level[clev].dpt = 0.0f;
     level[clev].upt = 0.0f;
     level[clev].cd = false;
     level[clev].lights = lights;
     level[clev].radius = lradius;
     level[clev].rotate = lrotate;
     level[clev].sdelay = lsdelay;
     level[clev].height = lheight;
     level[clev].isOn = lison;
     level[clev].isBlink= lisblink;
     level[clev].fadeoff= lfadeoff;
     level[clev].fadeoffspd= lfadeoffspd;
     level[clev].fadein= lfadein;
     level[clev].fadeinspd= lfadeinspd;
     level[clev].pauseOn= lpauseOn;
     level[clev].pauseOff= lpauseOff;
     level[clev].colorr = lcolorr;
     level[clev].colorg = lcolorg;
     level[clev].colorb = lcolorb;
     level[clev].colora = lcolora;
     levelok = false;
    }
  }

  PSYS::obstructl_tid++;
}

void pobstructlightscontainer::drawObstructLights(vector3 CP, double dt)
{
  float CamDistToEmitter = VECTORLEN2(CH_POSITION, Global::GetCameraPosition());
  if (CamDistToEmitter < CH_MAXDIST)
  {

  glGetIntegerv(GL_BLEND_SRC_ALPHA, &QGlobal::blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &QGlobal::blendDst);
  glDepthMask(0);
//glDisable(GL_LIGHTING);
//glEnable(GL_BLEND);
  glEnable(GL_ARB_point_sprite);
//glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_POINT_SMOOTH);
//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glColorMaterial(GL_FRONT, GL_EMISSION);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  for (int i = 0; i < CH_LEVELS; i++)
   {
    circleXYZ(CH_POSITION, CP, this, level[i].lights, i);
   }

//glDisable(GL_BLEND);
  glBlendFunc(QGlobal::blendSrc, QGlobal::blendDst);
  glEnable(GL_TEXTURE_2D);
//glEnable(GL_LIGHTING);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glMaterialfv(GL_FRONT, GL_EMISSION, QGlobal::emm2);
  }
}


void Global::renderobstructlights(vector3 camera, double dt)
{
 for (int i = 0; i < PSYS::obstructl_tid; i++)
  {
   PSYS::obstructlightsc[i].drawObstructLights(camera, dt);
  }
}


// ***********************************************************************************************************
// FOUNTAIN PARTICLE SYSTEM
// ***********************************************************************************************************
void pfountainemitercontainer::setsFountain(vector3 pos, float rmaxdist, int oscillators, float oscdist, float oscweight, float damp, float seedingspeed, char *sep, color4 color,
                                            int steps, int raysperstep, int dropsperray, float angleofDstep, float angleofHststep, float rangleadd, float accf, float psize)
{
 EM_MAXDIST = rmaxdist;
 EM_POSITION = pos;
 EM_OSCILLATORS = oscillators;
 EM_OSCILLATOR_DISTANCE = oscdist;
 EM_OSCILLATOR_WEIGHT = oscweight;
 EM_OSCILLATOR_DAMPING = damp;
 EM_MAXX = EM_OSCILLATORS * EM_OSCILLATOR_DISTANCE;
 EM_MAXZ = EM_OSCILLATORS * EM_OSCILLATOR_DISTANCE;
 EM_SEEDING_SPEED = seedingspeed;
 EM_POOL_HEIGHT = 0.3;

 AirFountainP.Initialize(EM_OSCILLATORS, EM_OSCILLATORS, EM_OSCILLATOR_DISTANCE, EM_OSCILLATOR_WEIGHT, EM_OSCILLATOR_DAMPING, 1.0, 1.0);
 AirFountainP.color = color;
 AirFountainW.Initialize(steps, raysperstep, dropsperray, angleofDstep, angleofHststep, rangleadd, accf, psize);
 AirFountainW.EmitterPos = F3dVector(pos.x, pos.y, pos.z);
 AirFountainW.color = color;
 AirFountainW.Position = F3dVector(EM_OSCILLATORS*EM_OSCILLATOR_DISTANCE/2.0f, EM_POOL_HEIGHT, EM_OSCILLATORS*EM_OSCILLATOR_DISTANCE/2.0f);

//loadtexture(ts);
 PSYS::fountain_tid++;
}

void pfountainemitercontainer::drawFountain(vector3 camera)
{
 float CamDistToEmitter = VECTORLEN2(EM_POSITION, Global::GetCameraPosition());
  if (CamDistToEmitter < EM_MAXDIST)
  {
    GLfloat psize;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &QGlobal::blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &QGlobal::blendDst);
    glGetFloatv(GL_POINT_SIZE, &psize);           // zapamietanie aktualnego rozmiaru punktow

    AirFountainP.Update(0.007);
    AirFountainW.Update(0.007, &AirFountainP);

    AirFountainP.Render(camera, AirFountainW.EmitterPos, QGlobal::texturetab[3]);
    AirFountainW.Render(camera);
    glBlendFunc(QGlobal::blendSrc, QGlobal::blendDst);
    glPointSize(psize);               // aby freespoty semaforow sie nie powiekszaly
  }  
}

void Global::renderfountainem(vector3 camera)
{
 for (int i = 0; i < PSYS::fountain_tid; i++)
  {
 //PSYS::fountainec[i].updateFountain();
   PSYS::fountainec[i].drawFountain(camera);
  }
}

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
  float CamDistToEmitter = VECTORLEN2(SMOKE_POSITION, Global::GetCameraPosition());

  if (CamDistToEmitter < SMOKE_MAXDIST)
  {
  xvector vWorldUp(0,1,0);
  xvector SmokeOrigin(SMOKE_POSITION.x, SMOKE_POSITION.y, SMOKE_POSITION.z);//smoke origin
  GLboolean blendEnabled;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &QGlobal::blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &QGlobal::blendDst);
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
        glBlendFunc(QGlobal::blendSrc, QGlobal::blendDst);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, value);
        glPolygonMode(GL_FRONT, GL_FILL);
  }
}

void Global::rendersmokeem()
{
 glDisable(GL_LIGHTING);
 for (int i = 0; i < PSYS::smoke_tid; i++)
  {
   PSYS::sec[i].updateSmoke();
   PSYS::sec[i].drawSmoke(vector3(0, 0, 0));
  }
 glEnable(GL_LIGHTING);
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
  float CamDistToEmitter = VECTORLEN2(FIRE_POSITION, Global::GetCameraPosition());

  if (CamDistToEmitter < FIRE_MAXDIST)
  {
  xvector vWorldUp(0,1,0);
  xvector FLAME(0,0,0.1);
  xvector FireOrigin(FIRE_POSITION.x, FIRE_POSITION.y, FIRE_POSITION.z);//fire origin

  GLboolean blendEnabled;
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &QGlobal::blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &QGlobal::blendDst);
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
        glBlendFunc(QGlobal::blendSrc, QGlobal::blendDst);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, value);
  }     
}


void Global::renderfireem()
{
 glDisable(GL_LIGHTING);
 for (int i = 0; i < PSYS::fire_tid; i++)
  {
   PSYS::fec[i].updateFire();
   PSYS::fec[i].drawFire(vector3(0, 0, 0));
  }
 glEnable(GL_LIGHTING);
}


// ***********************************************************************************************************
//
// ***********************************************************************************************************

void pparteffectscontainer::setsPE
            (long type,
             float rmaxdist,
             int   initialnum,
             int   createpersec,
             float mindieage,
             float maxdieage,
             float creationvar,
             bool  recreateondie,
             bool  particleleavesys,
             long  billboardtype,
        std::string  blendtype,
             vector3 empos,
             vector3 emarea,
             vector3 emdir,
             vector3 emdeviation,
             vector3 accdir,
             color4 borncolor1,
             color4 borncolor2,
             color4 diecolor1,
             color4 diecolor2,
             color4 alpha,
             float accmin,
             float accmax,
             float ssizemin,
             float ssizemax,
             float esizemin,
             float esizemax,
             float emspdmin,
             float emspdmax,
             float spinspdmin,
             float spinspdmax,
       std::string texture)
{
  PE_MAXDIST = rmaxdist;
  PE_POSITION = empos;
  PE_TYPE = type;

  texture = "data/particles/" + texture;

  bool nt = false;
  if (texture.find("none") != string::npos) nt = true;

//INIT SYSTEM 1 (FIRE1) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// if (PE_TYPE == 1)
//   {
	g_ParticleSystem1.Initialize(initialnum);
	g_ParticleSystem1.m_iParticlesCreatedPerSec = createpersec;
	g_ParticleSystem1.m_fMinDieAge = mindieage;
	g_ParticleSystem1.m_fMaxDieAge = maxdieage;
	g_ParticleSystem1.m_fCreationVariance = creationvar;
	g_ParticleSystem1.m_bRecreateWhenDied = recreateondie;
	g_ParticleSystem1.m_bParticlesLeaveSystem = particleleavesys;
        g_ParticleSystem1.m_iBillboarding = billboardtype;                                                    // 1=BILLBOARDING_PERPTOVIEWDIR, 2=;

	g_ParticleSystem1.SetEmitter(empos.x, empos.y, empos.z, emarea.x, emarea.y, emarea.z);
        g_ParticleSystem1.SetEmissionDirection(emdir.x, emdir.y, emdir.z, emdeviation.x, emdeviation.y, emdeviation.z);
        g_ParticleSystem1.SetAcceleration(F3dVector(accdir.x, accdir.y, accdir.z), accmin, accmax);
	g_ParticleSystem1.SetCreationColor(borncolor1.r, borncolor1.g, borncolor1.b, borncolor2.r, borncolor2.g, borncolor2.b);
	g_ParticleSystem1.SetDieColor(diecolor1.r, diecolor1.g, diecolor1.b, diecolor2.r, diecolor2.g, diecolor2.b);
	g_ParticleSystem1.SetAlphaValues(alpha.r, alpha.g, alpha.b, alpha.a);
	g_ParticleSystem1.SetSizeValues(ssizemin, ssizemax, esizemin, esizemax);
        g_ParticleSystem1.m_fMinEmitSpeed = emspdmin;
	g_ParticleSystem1.m_fMaxEmitSpeed = emspdmax;
	g_ParticleSystem1.SetSpinSpeed(spinspdmin*PI, spinspdmax*PI);
if(!nt)	g_ParticleSystem1.LoadTextureFromFile(stdstrtochar(texture));                                         // "data/particles/particle1.tga"
//   }

 PSYS::parteffects_tid++;
}

void pparteffectscontainer::drawPE(vector3 cp)
{
  float CamDistToEmitter = VECTORLEN2(PE_POSITION, Global::GetCameraPosition());

  if (CamDistToEmitter < PE_MAXDIST)
  {
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    if (!g_ParticleSystem1.m_bUseTexture) glPointSize(5.0);
    if (!g_ParticleSystem1.m_bUseTexture) glDisable(GL_TEXTURE_2D);

    //if (PE_TYPE == 1)
    g_ParticleSystem1.UpdateSystem(0.08f);
    //if (PE_TYPE == 1)
    g_ParticleSystem1.Render(0, 0);

    glEnable(GL_CULL_FACE);
  }  
}

void pparteffectscontainer::setposPE(float x, float y, float z)
{
  g_ParticleSystem1.m_EmitterPosition.x+=x;
  g_ParticleSystem1.m_EmitterPosition.y+=y;
  g_ParticleSystem1.m_EmitterPosition.z+=z;
}

void Global::renderparticleeffect(vector3 camera)
{
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &QGlobal::blendSrc);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &QGlobal::blendDst);
  glDepthMask(GL_FALSE);
//glEnable(GL_DEPTH_TEST);

  glDisable(GL_LIGHTING);
  for (int i = 0; i < PSYS::parteffects_tid; i++)
   {
    PSYS::parteffects[i].drawPE(vector3(0, 0, 0));
    PSYS::parteffects[i].setposPE(0.05, 0.0, 0.02);
   }
  glEnable(GL_LIGHTING);
  glBlendFunc(QGlobal::blendSrc, QGlobal::blendDst);
  glDepthMask(GL_TRUE);
}


//------------------------------------------------------------------------------------------------------------

#pragma package(smart_init)
