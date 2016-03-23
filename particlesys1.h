/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef particlesys1H
#define particlesys1H
#include <system.hpp>
#include "vector.h"
#include "pixmap.h"
#include "addons.h"

//Particle variables:

//CCCParticleSystem g_ParticleSystem2;
//CCCParticleSystem g_ParticleSystem3;
//CCCParticleSystem g_ParticleSystem4;
//CCCParticleSystem g_ParticleSystem5;
//CCCParticleSystem g_ParticleSystem6;

//void InitParticles();


// ***********************************************************************************************************
// System czasteczek 1 - dla efektu ognia i dymu (orginal sources at http://www.cs.utah.edu/~cxiong/)
// ***********************************************************************************************************

class Particle{
public:
	xvector pos;
	xvector vel;
        float cposz;
	float para[11];
	float size;
	float spin;
	float spinspd;
	float spinmag;
	float speed;
	float color;
	bool live;
        float opacity;
        float opacityfactor;


	Particle(){}
	
	Particle(Particle &p) {
		pos = p.pos;
		vel = p.vel;
		for(int i =0; i<11; i++)
			para[i] = p.para[i];
		size = p.size;
		spin = p.spin;
		spinspd = p.spinspd;
		spinmag = p.spinmag;
		speed = p.speed;
		color = p.color;
		live = p.live;		
	}

	Particle& operator= (Particle& p){ 
		if(this != &p){ 
			pos = p.pos;
			vel = p.vel;
			for(int i =0; i<11; i++)
				para[i] = p.para[i];
			size = p.size;
			spin = p.spin;
			spinspd = p.spinspd;
			spinmag = p.spinmag;
			speed = p.speed;
			color = p.color;
			live = p.live;	
		}          
		return *this;
	}

};

struct ParNode{
	int num;
	Particle par;
	ParNode *next;
	
};

class ParList{
public:
	ParNode* head;

	ParList(){ head = 0;}
	ParList(ParList &l){head = l.head;}

	void push(Particle &p){ 
		ParNode* node = new ParNode;
		node->par = p;
		node->next = head;
		head = node;
	}

	void erase(ParNode** p){
		ParNode *tmp = *p;
		*p = (*p)->next;
		delete tmp;
	}

};


// ***********************************************************************************************************
// SMOKE EMITTER CONTAINER
// ***********************************************************************************************************
struct psmokeemitercontainer
{
 xvector vWorldUp;
 GLuint SmokeTex0;
 GLuint SmokeTex;
 ParList Smoke;//smoke particle list
 vector3 SMOKE_POSITION;
 float eye[3];
 float SMOKE_MAXDIST;
 float SMOKE_SIZE_MIN;
 float SMOKE_SIZE_MAX;
 float SMOKE_SPEED_MIN;
 float SMOKE_SPEED_MAX;
 float SMOKE_SPIN_SPEED_MAX ;
 float SMOKE_MAX_DIST;
 float SMOKE_RCOLOR_F;
 float SMOKE_RCOLOR_T;
 float SMOKE_OPACITY;
 int   SMOKE_SEEDING_SPEED;
 void setSmoke(vector3 pos, float rmaxdist, float sizemin, float sizemax, float speedmin, float speedmax, float spinspeedmin, float spinspeedmax, float maxdist, float rcolorf, float rcolort, float opacity, int seedingspeed, int ts);
 void drawSmoke(vector3 cp);
 void createSmoke();
 void updateSmoke();
 void smokeMotion(Particle &p);
 void loadtexture(int ts);
 bool exist;
};


// ***********************************************************************************************************
// FIRE EMITTER CONTAINER
// ***********************************************************************************************************
struct pfireemitercontainer
{
 GLuint FireTex;
 RGBApixmap FirePix;
 ParList Fire;//smoke particle list
 xvector vWorldUp;
 vector3 FIRE_POSITION;
 float eye[3];
 float FIRE_MAXDIST;
 float FIRE_SIZE_MIN;
 float FIRE_SIZE_MAX;
 float FIRE_SPEED_MIN;       // 0.04
 float FIRE_SPEED_MAX;       // 0.08
 float FIRE_SPIN_SPEED_MAX;   // 0.3
 float FIRE_MAX_DIST;        // 6
 int   FIRE_SEEDING_SPEED;  // 5
 void setsFire(vector3 pos, float rmaxdist, float sizemin, float sizemax, float speedmin, float speedmax, float spinspeedmin, float spinspeedmax, float maxdist, float rcolorf, float rcolort, float opacity, int seedingspeed, int ts);
 void drawFire(vector3 cp);
 void createFire();
 void updateFire();
 void motionFire(Particle &p);
 void loadtexture(int ts);
};


// ***********************************************************************************************************
// PARTICLE EFFECTS EMITTER CONTAINER
// ***********************************************************************************************************
struct pparteffectscontainer
{
 CCCParticleSystem g_ParticleSystem1;
 GLuint EffectTex;
 vector3 PE_POSITION;
 float PE_MAXDIST;
 int   PE_SEEDING_SPEED;
 long  PE_TYPE;
 void setsPE(long  type,
             float rmaxdist,
             int  initialnum,
             int  createpersec,
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
       std::string texture);

 void drawPE(vector3 cp);
 void createPE();
 void updatePE();
 void setposPE(float x, float y, float z);
 void loadtexture(int ts);
};


// ***********************************************************************************************************
// OBSTRUCT LIGHTS SYSTEM
// ***********************************************************************************************************
struct pchimneylevel
{
   long lights;
  float radius;
  float height;
  float rotate;
   bool isOn;
   bool isBlink;
  float sdelay; 
  float fadeoff;
  float fadeoffspd;
  float fadein;
  float fadeinspd;
  float pauseOn;
  float pauseOff;
  float a, dpt, upt;
   bool cd;
  float colorr, colorg, colorb, colora; 
 color4 color;
};

struct pobstructlightscontainer
{
 pchimneylevel level[15];
 vector3 CH_POSITION;
 float CH_MAXDIST;
  long CH_LEVELS;
 float CH_STARTY;
 bool setsObstructLights(char* scriptfile);
 void drawObstructLights(vector3 CP, double dt);
 void circleXYZ(vector3 center, vector3 cp, pobstructlightscontainer *CHL, int dots, int clevel);
 bool exist;
};


// KLASA KONTENEROW ******************************************************************************************
class PSYS
{
 public:
 static long smoke_tid;
 static long fire_tid;
 static long fountain_tid;
 static long obstructl_tid;
 static long parteffects_tid;
 static psmokeemitercontainer sec[64];
 static pfireemitercontainer fec[32];
 static pfountainemitercontainer fountainec[32];
 static pobstructlightscontainer obstructlightsc[32];
 static pparteffectscontainer parteffects[32];
};

long PSYS::smoke_tid = 0;
long PSYS::fire_tid = 0;
long PSYS::fountain_tid = 0;
long PSYS::obstructl_tid = 0;
long PSYS::parteffects_tid = 0;
//------------------------------------------------------------------------------------------------------------
#endif
