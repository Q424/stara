#ifndef POOL_INCLUDED
#define POOL_INCLUDED

#include <gl\glut.h>
#include "poolvectors.h"
#include "../../dumb3d.h"

using namespace Math3D;

struct SOscillator
{
	GLfloat x,y,z;     //position
	GLfloat nx,ny,nz;  //normal vector
	GLfloat u,v;	   //texture coordinates

	GLfloat UpSpeed;
	GLfloat newY;
};

class CPool
{
protected:
	//vertex data for the waves:
	SOscillator * m_Oscillators;
	GLuint * m_Indices;

	
	int m_NumOscillators;  //x*z
	int m_xSize,m_zSize;

	int m_NumIndices;

	float m_OscillatorDistance;
	float m_OscillatorWeight;
	float m_Damping;


public:
        color4 color;
	float GetOscillatorDistance();
	void Initialize(int xSize, int zSize, float OscillatorDistance, float OscillatorWeight, float Damping, float TextureStretchX, float TextureStretchZ);
	void Reset();
	void AffectOscillator(int xPos, int zPos, float deltaY);
	void Update(float deltaTime);
	void Render(vector3 cp, SF3dVector ep, GLuint tex);
};

#endif