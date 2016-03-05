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

class PSYS
{
 public:

 static long smoke_tid;
 static long fire_tid;
 static psmokeemitercontainer sec[64];
 static pfireemitercontainer fec[32];
};

long PSYS::smoke_tid = 0;
long PSYS::fire_tid = 0;

//---------------------------------------------------------------------------
#endif
