/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef SegmentH
#define SegmentH
#include "VBO.h"
#include "dumb3d.h"
#include "Classes.h"

using namespace Math3D;

// 110405 Ra: klasa punkt�w przekroju z normalnymi

class vector6 : public vector3
{ // punkt przekroju wraz z wektorem normalnym
  public:
    vector3 n;
    vector6()
    {
        x = y = z = n.x = n.z = 0.0;
        n.y = 1.0;
    };
    vector6(double a, double b, double c, double d, double e, double f)
    //{x=a; y=b; z=c; n.x=d; n.y=e; n.z=f;};
    {
        x = a;
        y = b;
        z = c;
        n.x = 0.0;
        n.y = 1.0;
        n.z = 0.0;
    }; // Ra: bo na razie s� z tym problemy
    vector6(double a, double b, double c)
    {
        x = a;
        y = b;
        z = c;
        n.x = 0.0;
        n.y = 1.0;
        n.z = 0.0;
    };
};

class TSegment
{ // aproksymacja toru (zwrotnica ma dwa takie, jeden z nich jest aktywny)
  private:
    vector3 Point1, CPointOut, CPointIn, Point2;
    double fRoll1, fRoll2; // przechy�ka na ko�cach
    double fLength; // d�ugo�� policzona
    double *fTsBuffer; // warto�ci parametru krzywej dla r�wnych odcink�w
    double fStep;
    int iSegCount; // ilo�� odcink�w do rysowania krzywej
    double fDirection; // Ra: k�t prostego w planie; dla �uku k�t od Point1
    double fStoop; // Ra: k�t wzniesienia; dla �uku od Point1
    vector3 vA, vB, vC; // wsp�czynniki wielomian�w trzeciego stopnia vD==Point1
    // TSegment *pPrev; //odcinek od strony punktu 1 - w segmencie, �eby nie skaka� na zwrotnicach
    // TSegment *pNext; //odcinek od strony punktu 2
    TTrack *pOwner; // wska�nik na w�a�ciciela
    double fAngle[2]; // k�ty zako�czenia drogi na przejazdach
    vector3 GetFirstDerivative(double fTime);
    double RombergIntegral(double fA, double fB);
    double GetTFromS(double s);
    vector3 RaInterpolate(double t);
    vector3 RaInterpolate0(double t);

    // TSegment *segNeightbour[2]; //s�siednie odcinki - musi by� przeniesione z Track
    // int iNeightbour[2]; //do kt�rego ko�ca doczepiony
  public:
    bool bCurve;
    bool bTieAdded;
    // int iShape; //Ra: flagi kszta�tu dadz� wi�cej mo�liwo�ci optymalizacji
    // (0-Bezier,1-prosty,2/3-�uk w lewo/prawo,6/7-przej�ciowa w lewo/prawo)
    TSegment(TTrack *owner);
    ~TSegment();
    bool Init(vector3 NewPoint1, vector3 NewPoint2, double fNewStep, double fNewRoll1 = 0,
              double fNewRoll2 = 0);
    bool Init(vector3 &NewPoint1, vector3 NewCPointOut, vector3 NewCPointIn, vector3 &NewPoint2,
              double fNewStep, double fNewRoll1 = 0, double fNewRoll2 = 0, bool bIsCurve = true);
    inline double ComputeLength(); // McZapkie-150503
    inline vector3 GetDirection1()
    {
        return bCurve ? CPointOut - Point1 : CPointOut;
    };
    inline vector3 GetDirection2()
    {
        return bCurve ? CPointIn - Point2 : CPointIn;
    };
    vector3 GetDirection(double fDistance);
    vector3 GetDirection()
    {
        return CPointOut;
    };
    vector3 FastGetDirection(double fDistance, double fOffset);
    vector3 GetPoint(double fDistance);
    void RaPositionGet(double fDistance, vector3 &p, vector3 &a);
    vector3 FastGetPoint(double t);
    inline vector3 FastGetPoint_0()
    {
        return Point1;
    };
    inline vector3 FastGetPoint_1()
    {
        return Point2;
    };
    inline double GetRoll(double s)
    {
        s /= fLength;
        return ((1.0 - s) * fRoll1 + s * fRoll2);
    }
    void GetRolls(double &r1, double &r2)
    { // pobranie przechy�ek (do generowania tr�jk�t�w)
        r1 = fRoll1;
        r2 = fRoll2;
    }
    void RenderLoft(const vector6 *ShapePoints, int iNumShapePoints, double fTextureLength, int iSkip = 0, int iQualityFactor = 1, vector3 **p = NULL, bool bRender = true);
    void RenderRTie(const vector6 *ShapePoints, int iNumShapePoints, double fTextureLength, int iSkip, int iQualityFactor, TTrack *TRK);
    void RenderSwitch(const vector6 *ShapePoints, int iNumShapePoints, double fTextureLength, int iSkip, int iQualityFactor, TTrack *TRK);
    void RenderSwitchRail(const vector6 *ShapePoints1, const vector6 *ShapePoints2, int iNumShapePoints, double fTextureLength, int iSkip = 0,
                          double fOffsetX = 0.0f);
    void Render();
    inline double GetLength()
    {
        return fLength;
    };
    void MoveMe(vector3 pPosition)
    {
        Point1 += pPosition;
        Point2 += pPosition;
        if (bCurve)
        {
            CPointIn += pPosition;
            CPointOut += pPosition;
        }
    }
    int RaSegCount()
    {
        return fTsBuffer ? iSegCount : 1;
    };
    void RaRenderLoft(CVertNormTex *&Vert, const vector6 *ShapePoints, int iNumShapePoints,
                      double fTextureLength, int iSkip = 0, int iEnd = 0, double fOffsetX = 0.0);
    void RaAnimate(CVertNormTex *&Vert, const vector6 *ShapePoints, int iNumShapePoints,
                   double fTextureLength, int iSkip = 0, int iEnd = 0, double fOffsetX = 0.0);
    void AngleSet(int i, double a)
    {
        fAngle[i] = a;
    };
    void Rollment(double w1, double w2); // poprawianie przechy�ki
};

//---------------------------------------------------------------------------
#endif
