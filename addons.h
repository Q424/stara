/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef addonsH
#define addonsH
#include <system.hpp>
#include "addons/fountain1/poolvectors.h"
#include "addons/fountain1/pool.h"
#include "addons/fountain1/AirFountain.h"

//Constants:
//#define NUM_X_OSCILLATORS	150
//#define NUM_Z_OSCILLATORS	150
//#define OSCILLATOR_DISTANCE	0.015
//#define OSCILLATOR_WEIGHT       0.0003
//#define MAXX		        (NUM_X_OSCILLATORS*OSCILLATOR_DISTANCE)
//#define MAXZ			(NUM_Z_OSCILLATORS*OSCILLATOR_DISTANCE)

//#define POOL_HEIGHT		0.3

//The "pool" which represents the water within the fountain bowl
//CPool AirFountainP;
//water outside the bowl is in the air:
//CAirFountain AirFountainW;


struct pfountainemitercontainer
{
  CPool AirFountainP;
  CAirFountain AirFountainW;
  GLuint WaterTex;
  vector3 EM_POSITION;
  float EM_MAXDIST;
  float EM_OSCILLATORS;
  float EM_OSCILLATOR_DISTANCE;
  float EM_OSCILLATOR_WEIGHT;
  float EM_OSCILLATOR_DAMPING;
  float EM_MAXX;
  float EM_MAXZ;
  float EM_POOL_HEIGHT;
  float EM_SEEDING_SPEED;
  // water: steps, rays per step, drops per ray, AngleOfDeepestStep, AngleOfHighestStep, RandomAngleAddition, AccFactor, PointSize
  void setsFountain(vector3 pos, float rmaxdist, int oscillators, float oscdist, float oscweight, float damp, float seedingspeed, char *sep, color4 color,
                    int steps, int raysperstep, int dropsperray, float angleofDstep, float angleofHststep, float rangleadd, float accf, float psize);
  void drawFountain(vector3 cp);
  void makeFountain();
  void updateFountain();
  void loadtexture(int ts);
  bool exist;
};


//---------------------------------------------------------------------------
#endif
