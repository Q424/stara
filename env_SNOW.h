//---------------------------------------------------------------------------

#ifndef env_snowH
#define env_snowH

#include "MdlMngr.h"

enum TSnowType // rodzaj animacji
{
    sf_none,
    sf_tri,
    sf_poly,
    sf_line,
    sf_point
};

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
   float r,g,b,a;
} snows;



class TSnow
{
private:

int type, blendf;
bool color, text, light;
float size;
float rcf, rct, raf, rat;

public:
 bool Init(int stype, int sflakesnum, float sarea, float ssize, float srcf, float srct, float sraf, float srat, bool scolor, bool stex, bool slight, int sbf);
 bool Render();
};

//---------------------------------------------------------------------------
#endif
