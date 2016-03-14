#include <gl/glew.h>
#include <gl/glut.h>
#include <gl/glext.h>
#include <gl/glaux.h>
#include "poolvectors.h"
#include "pool.h"
#include "../../dumb3d.h"

/**

  "Air fountain" is the description for the fountain's water in the air.
  The rest, which is most of the water, resists in the bowl.

**/
using namespace Math3D;

class CAirFountain;

class CDrop
{
private:
	GLfloat time;  //How many steps the drop was "outside", when it falls into the water, time is set back to 0
	SF3dVector ConstantSpeed;  //See the fountain doc for explanation of the physics
	GLfloat AccFactor;
public:
	void SetConstantSpeed (SF3dVector NewSpeed);
	void SetAccFactor(GLfloat NewAccFactor);
	void SetTime(GLfloat NewTime);
	void GetNewPosition(SF3dVector * PositionVertex, float dtime, CPool * pPool, CAirFountain * pAirFountain);
};

class CAirFountain
{
protected:
	SF3dVector * FountainVertices;
	CDrop * FountainDrops;
public:
        GLuint tex;
        float pointsize;
        vector3 cp;
        color4 color;
	SF3dVector Position;
        SF3dVector EmitterPos;
	void Render(vector3 cp);
	void Update(float dtime, CPool * pPool);
	GLint m_NumDropsComplete;
	void Initialize(GLint Steps, GLint RaysPerStep, GLint DropsPerRay, 
					GLfloat AngleOfDeepestStep, 
					GLfloat AngleOfHighestStep,
					GLfloat RandomAngleAddition,
					GLfloat AccFactor, GLfloat PointSize);
	void Delete();
};