//---------------------------------------------------------------------------

#ifndef env_snowH
#define env_snowH

#include "MdlMngr.h"



typedef struct{
   bool  active;
   float life;
   float fade;
   float r, g, b;
   float x, y, z;
   float xi,yi,zi;
   float xg,yg,zg;
   float alfa;
   float alfa_inc;
} particles;

int sqrsize;
int nofSnow;
typedef struct{
   bool  active;
   float x, y, z;
   float xg,yg,zg;
   float alfa;
   float alfa_inc;
} snows;



class TSnow
{
private:



public:
 bool __fastcall Init(int snowflakesnum, float size);
 bool __fastcall Render();
};

//---------------------------------------------------------------------------
#endif
