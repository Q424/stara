/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef TrackH
#define TrackH

#include "Segment.h"
#include "ResourceManager.h"
#include "opengl/glew.h"
#include <system.hpp>
#include "Classes.h"
#include "model3d.h"
class TEvent;

typedef enum { rt_S49L, rt_RI60N , rt_LP, rt_SQR} TRailType;   // Q 250116 - podzial szyn na typy

typedef enum
{
    tt_Unknown,
    tt_Normal,
    tt_Switch,
    tt_Table,
    tt_Cross,
    tt_Tributary
} TTrackType;
// McZapkie-100502
typedef enum
{
    e_unknown = -1,
    e_flat = 0,
    e_mountains,
    e_canyon,
    e_tunnel,
    e_bridge,
    e_bank
} TEnvironmentType;
// Ra: opracowaæ alternatywny system cieni/œwiate³ z definiowaniem koloru oœwietlenia w halach

class TTrack;
class TGroundNode;
class TSubRect;
class TTraction;


class TSwitchExtension
{ // dodatkowe dane do toru, który jest zwrotnic¹
  public:
    TSwitchExtension(TTrack *owner, int what);
    ~TSwitchExtension();
    TSegment *Segments[6]; // dwa tory od punktu 1, pozosta³e dwa od 2? Ra 140101: 6 po³¹czeñ dla
    // skrzy¿owañ
    // TTrack *trNear[4]; //tory do³¹czone do punktów 1, 2, 3 i 4
    // dotychczasowe [2]+[2] wskaŸniki zamieniæ na nowe [4]
    TTrack *pNexts[2]; // tory do³¹czone do punktów 2 i 4
    TTrack *pPrevs[2]; // tory do³¹czone do punktów 1 i 3
    int iNextDirection[2]; // to te¿ z [2]+[2] przerobiæ na [4]
    int iPrevDirection[2];
    int CurrentIndex; // dla zwrotnicy
    double fOffset, fDesiredOffset; // aktualne i docelowe po³o¿enie napêdu iglic
    double fOffsetSpeed; // prêdkoœæ liniowa ruchu iglic
    double fOffsetDelay; // opóŸnienie ruchu drugiej iglicy wzglêdem pierwszej
    union
    {
        struct
        { // zmienne potrzebne tylko dla zwrotnicy
            double fOffset1, fOffset2; // przesuniêcia iglic - 0=na wprost
            bool RightSwitch; // czy zwrotnica w prawo
        };
        struct
        { // zmienne potrzebne tylko dla obrotnicy/przesuwnicy
            TGroundNode *pMyNode; // dla obrotnicy do wtórnego pod³¹czania torów
            // TAnimContainer *pAnim; //animator modelu dla obrotnicy
            TAnimModel *pModel; // na razie model
        };
        struct
        { // zmienne dla skrzy¿owania
            vector3 *vPoints; // tablica wierzcho³ków nawierzchni, generowana przez pobocze
            int iPoints; // liczba faktycznie u¿ytych wierzcho³ków nawierzchni
            bool bPoints; // czy utworzone?
            int iRoads; // ile dróg siê spotyka?
        };
    };
    bool bMovement; // czy w trakcie animacji
    int iLeftVBO, iRightVBO; // indeksy iglic w VBO
    TSubRect *pOwner; // sektor, któremu trzeba zg³osiæ animacjê
    TTrack *pNextAnim; // nastêpny tor do animowania
    TEvent *evPlus, *evMinus; // zdarzenia sygnalizacji rozprucia
    float fVelocity; // maksymalne ograniczenie prêdkoœci (ustawianej eventem)
    vector3 vTrans; // docelowa translacja przesuwnicy
  private:
};

const int iMaxNumDynamics = 40; // McZapkie-100303

class TIsolated
{ // obiekt zbieraj¹cy zajêtoœci z kilku odcinków
    int iAxles; // iloœæ osi na odcinkach obs³ugiwanych przez obiekt
    TIsolated *pNext; // odcinki izolowane s¹ trzymane w postaci listy jednikierunkowej
    static TIsolated *pRoot; // pocz¹tek listy
  public:
    AnsiString asName; // nazwa obiektu, baza do nazw eventów
    TEvent *evBusy; // zdarzenie wyzwalane po zajêciu grupy
    TEvent *evFree; // zdarzenie wyzwalane po ca³kowitym zwolnieniu zajêtoœci grupy
    TMemCell *pMemCell; // automatyczna komórka pamiêci, która wspó³pracuje z odcinkiem izolowanym
    TIsolated();
    TIsolated(const AnsiString &n, TIsolated *i);
    ~TIsolated();
    static TIsolated *__fastcall Find(
        const AnsiString &n); // znalezienie obiektu albo utworzenie nowego
    void Modify(int i, TDynamicObject *o); // dodanie lub odjêcie osi
    bool Busy()
    {
        return (iAxles > 0);
    };
    static TIsolated *__fastcall Root()
    {
        return (pRoot);
    };
    TIsolated *__fastcall Next()
    {
        return (pNext);
    };
};

class TTrack : public Resource
{ // trajektoria ruchu - opakowanie
  private:
    TSwitchExtension *SwitchExtension; // dodatkowe dane do toru, który jest zwrotnic¹
    TSegment *Segment;
    TTrack *trNext; // odcinek od strony punktu 2 - to powinno byæ w segmencie
    TTrack *trPrev; // odcinek od strony punktu 1
    // McZapkie-070402: dodalem zmienne opisujace rozmiary tekstur
    GLuint TextureID1; // tekstura szyn albo nawierzchni
    GLuint TextureID2; // tekstura automatycznej podsypki albo pobocza
    GLuint TextureID3; // tekstura podsypki dla rozjazdu

    float fTexLength; // d³ugoœæ powtarzania tekstury w metrach
    float fTexRatio1; // proporcja rozmiarów tekstury dla nawierzchni drogi
    float fTexRatio2; // proporcja rozmiarów tekstury dla chodnika
    float fTexHeight1; // wysokoœæ brzegu wzglêdem trajektorii
    float fTexWidth; // szerokoœæ boku
    float fTexSlope;
    double fRadiusTable[2]; // dwa promienie, drugi dla zwrotnicy
    int iTrapezoid; // 0-standard, 1-przechy³ka, 2-trapez, 3-oba
    GLuint DisplayListID;
    TIsolated *pIsolated; // obwód izolowany obs³uguj¹cy zajêcia/zwolnienia grupy torów
    TGroundNode *
        pMyNode; // Ra: proteza, ¿eby tor zna³ swoj¹ nazwê TODO: odziedziczyæ TTrack z TGroundNode
  public:
    GLuint TextureT01;
    GLuint TextureT02;
    GLuint TextureT03;
    GLuint TextureS01;
    int iNumDynamics;
    int iRAILTYPE;
    TDynamicObject *Dynamics[iMaxNumDynamics];
    int iEvents; // Ra: flaga informuj¹ca o obecnoœci eventów
    TEvent *evEventall0; // McZapkie-140302: wyzwalany gdy pojazd stoi
    TEvent *evEventall1;
    TEvent *evEventall2;
    TEvent *evEvent0; // McZapkie-280503: wyzwalany tylko gdy headdriver
    TEvent *evEvent1;
    TEvent *evEvent2;
    AnsiString asEventall0Name; // nazwy eventów
    AnsiString asEventall1Name;
    AnsiString asEventall2Name;
    AnsiString asEvent0Name;
    AnsiString asEvent1Name;
    AnsiString asEvent2Name;
    AnsiString asStationName;
    AnsiString asTrackNumber;
    AnsiString asTieTexture1;
    AnsiString asTieTexture2;
    AnsiString asTieTexture3;
    AnsiString asTieModelL;
    AnsiString asRailJointModel;
    AnsiString asSwitchModel;
    AnsiString asSwitchTexture;
    AnsiString asName;
    TModel3d *mdTie1;
    TModel3d *mdRailJoint;
    TModel3d *mdSwitch;
    int iNextDirection; // 0:Point1, 1:Point2, 3:do odchylonego na zwrotnicy
    int iPrevDirection;
    TTrackType eType;
    TRailType rType;   // Q 250116: typ szyny
    float fRailHeight;
    int iCategoryFlag; // 0x100 - usuwanie pojazów
    float fTrackWidth; // szerokoœæ w punkcie 1
    float fTrackWidth2; // szerokoœæ w punkcie 2 (g³ównie drogi i rzeki)
    float fFriction; // wspó³czynnik tarcia
    float fSoundDistance;
    int iQualityFlag;
    int iDamageFlag;
    TEnvironmentType eEnvironment; // dŸwiêk i oœwietlenie
    bool bVisible; // czy rysowany
    bool bTiesAdded;
    int iAction; // czy modyfikowany eventami (specjalna obs³uga przy skanowaniu)
    float fOverhead; // informacja o stanie sieci: 0-jazda bezpr¹dowa, >0-z opuszczonym i
    // ograniczeniem prêdkoœci
  private:
    double fVelocity; // prêdkoœæ dla AI (powy¿ej roœnie prawdopowobieñstwo wykolejenia)
  public:
    // McZapkie-100502:
    double fTrackLength; // d³ugoœæ z wpisu, nigdzie nie u¿ywana
    double fRadius; // promieñ, dla zwrotnicy kopiowany z tabeli
    bool ScannedFlag; // McZapkie: do zaznaczania kolorem torów skanowanych przez AI
    TTraction *hvOverhead; // drut zasilaj¹cy do szybkiego znalezienia (nie u¿ywany)
    TGroundNode *nFouling[2]; // wspó³rzêdne ukresu albo oporu koz³a
    TTrack *trColides; // tor kolizyjny, na którym trzeba sprawdzaæ pojazdy pod k¹tem zderzenia

    TTrack(TGroundNode *g);
    ~TTrack();
    void Init();
    static TTrack *__fastcall Create400m(int what, double dx);
    TTrack *__fastcall NullCreate(int dir);
    inline bool IsEmpty()
    {
        return (iNumDynamics <= 0);
    };
    void ConnectPrevPrev(TTrack *pNewPrev, int typ);
    void ConnectPrevNext(TTrack *pNewPrev, int typ);
    void ConnectNextPrev(TTrack *pNewNext, int typ);
    void ConnectNextNext(TTrack *pNewNext, int typ);
    inline double Length()
    {
        return Segment->GetLength();
    };
    inline TSegment *__fastcall CurrentSegment()
    {
        return Segment;
    };
    inline TTrack *__fastcall CurrentNext()
    {
        return (trNext);
    };
    inline TTrack *__fastcall CurrentPrev()
    {
        return (trPrev);
    };
    TTrack *__fastcall Neightbour(int s, double &d);
    bool SetConnections(int i);
    bool Switch(int i, double t = -1.0, double d = -1.0);
    bool SwitchForced(int i, TDynamicObject *o);
    int CrossSegment(int from, int into);
    inline int GetSwitchState()
    {
        return (SwitchExtension ? SwitchExtension->CurrentIndex : -1);
    };
    void Load(cParser *parser, vector3 pOrigin, AnsiString name);
    bool AssignEvents(TEvent *NewEvent0, TEvent *NewEvent1, TEvent *NewEvent2);
    bool AssignallEvents(TEvent *NewEvent0, TEvent *NewEvent1, TEvent *NewEvent2);
    bool AssignForcedEvents(TEvent *NewEventPlus, TEvent *NewEventMinus);
    bool CheckDynamicObject(TDynamicObject *Dynamic);
    bool AddDynamicObject(TDynamicObject *Dynamic);
    bool RemoveDynamicObject(TDynamicObject *Dynamic);
    void MoveMe(vector3 pPosition);
    void AddSleepers(GLuint tex = 0);

    void Release();
    void Compile(GLuint tex = 0);
    void Compile2(GLuint tex = 0);

    void Render(); // renderowanie z Display Lists
    void RenderA();
    int RaArrayPrepare(); // zliczanie rozmiaru dla VBO sektroa
    void RaArrayFill(CVertNormTex *Vert, const CVertNormTex *Start); // wype³nianie VBO
    void RaRenderVBO(int iPtr); // renderowanie z VBO sektora
    void RenderDyn(); // renderowanie nieprzezroczystych pojazdów (oba tryby)
    void RenderDynAlpha(); // renderowanie przezroczystych pojazdów (oba tryby)
    void RenderDynSounds(); // odtwarzanie dŸwiêków pojazdów jest niezale¿ne od ich
    // wyœwietlania

    void RaOwnerSet(TSubRect *o)
    {
        if (SwitchExtension)
            SwitchExtension->pOwner = o;
    };
    bool InMovement(); // czy w trakcie animacji?
    void RaAssign(TGroundNode *gn, TAnimContainer *ac);
    void RaAssign(TGroundNode *gn, TAnimModel *am, TEvent *done, TEvent *joined);
    void RaAnimListAdd(TTrack *t);
    TTrack *__fastcall RaAnimate();

    void RadioStop();
    void AxleCounter(int i, TDynamicObject *o)
    {
        if (pIsolated)
            pIsolated->Modify(i, o);
    }; // dodanie lub odjêcie osi
    AnsiString IsolatedName();
    bool IsolatedEventsAssign(TEvent *busy, TEvent *free);
    double WidthTotal();
    GLuint TextureGet(int i)
    {
        return i ? TextureID1 : TextureID2;
    };
    bool IsGroupable();
    int TestPoint(vector3 *Point);
    void MovedUp1(double dh);
    AnsiString NameGet();
    void VelocitySet(float v);
    float VelocityGet();
    void ConnectionsLog();

  private:
    void EnvironmentSet();
    void EnvironmentReset();
};


#define S49L 1
#define RI60 2


const int nnumPts=12;
/*
const vector6 szyna[nnumPts]= //szyna - vextor6(x,y,mapowanie tekstury,xn,yn,zn)
{pierwotna szyna, opracowa³ youBy, zmiany w celu uzyskania symetrii
 vector6( 0.111,-0.180,0.00, 1.000, 0.000,0.000),
 vector6( 0.045,-0.155,0.15, 0.707, 0.707,0.000),
 vector6( 0.045,-0.070,0.25, 0.707,-0.707,0.000),
 vector6( 0.071,-0.040,0.35, 0.707,-0.707,0.000), //albo tu 0.073
 vector6( 0.072,-0.010,0.40, 0.707, 0.707,0.000),
 vector6( 0.052,-0.000,0.45, 0.000, 1.000,0.000),
 vector6( 0.020,-0.000,0.55, 0.000, 1.000,0.000),
 vector6( 0.000,-0.010,0.60,-0.707, 0.707,0.000),
 vector6( 0.001,-0.040,0.65,-0.707,-0.707,0.000), //albo tu -0.001
 vector6( 0.027,-0.070,0.75,-0.707,-0.707,0.000), //albo zostanie asymetryczna
 vector6( 0.027,-0.155,0.85,-0.707, 0.707,0.000),
 vector6(-0.039,-0.180,1.00,-1.000, 0.000,0.000)
};
*/
const vector6 szyna[nnumPts]= //szyna - vextor6(x,y,mapowanie tekstury,xn,yn,zn)
{//tê wersjê opracowa³ Tolein (bez pochylenia)
 vector6( 0.111,-0.180,0.00, 1.000, 0.000,0.000),
 vector6( 0.046,-0.150,0.15, 0.707, 0.707,0.000),
 vector6( 0.044,-0.050,0.25, 0.707,-0.707,0.000),
 vector6( 0.073,-0.038,0.35, 0.707,-0.707,0.000),
 vector6( 0.072,-0.010,0.40, 0.707, 0.707,0.000),
 vector6( 0.052,-0.000,0.45, 0.000, 1.000,0.000),
 vector6( 0.020,-0.000,0.55, 0.000, 1.000,0.000),
 vector6( 0.000,-0.010,0.60,-0.707, 0.707,0.000),
 vector6( -0.001,-0.038,0.65,-0.707,-0.707,0.000),
 vector6( 0.028,-0.050,0.75,-0.707,-0.707,0.000),
 vector6( 0.026,-0.150,0.85,-0.707, 0.707,0.000),
 vector6(-0.039,-0.180,1.00,-1.000, 0.000,0.000)
};


const vector6 iglica[nnumPts]= //iglica - vextor3(x,y,mapowanie tekstury)
{vector6( 0.010,-0.152,0.00, 1.000, 0.000,0.000),
 vector6( 0.010,-0.150,0.15, 1.000, 0.000,0.000),
 vector6( 0.010,-0.070,0.25, 1.000, 0.000,0.000),
 vector6( 0.010,-0.040,0.35, 1.000, 0.000,0.000),
 vector6( 0.010,-0.010,0.40, 1.000, 0.000,0.000),
 vector6( 0.010,-0.000,0.45, 0.707, 0.707,0.000),
 vector6( 0.000,-0.000,0.55, 0.707, 0.707,0.000),
 vector6( 0.000,-0.010,0.60,-1.000, 0.000,0.000),
 vector6( 0.000,-0.040,0.65,-1.000, 0.000,0.000),
 vector6( 0.000,-0.070,0.75,-1.000, 0.000,0.000),
 vector6( 0.000,-0.150,0.85,-0.707, 0.707,0.000),
 vector6(-0.040,-0.152,1.00,-1.000, 0.000,0.000) //1mm wiêcej, ¿eby nie nachodzi³y tekstury?
};

float szp = 0.035;
float szh = 0.13;
const int numPts = 4;
const vector6 szynaSQR[numPts]= //szyna - vextor6(x,y,mapowanie tekstury,xn,yn,zn)
{//tê wersjê opracowa³ Tolein (bez pochylenia)
 vector6( 0.05+szp, 0.000-szh, 0.05, 1.000, 0.000,0.000),
 vector6( 0.05+szp, 0.149-szh, 0.35, 0.707, 0.707,0.000),
 vector6(-0.05+szp, 0.149-szh, 0.65, 0.707,-0.707,0.000),
 vector6(-0.05+szp, 0.000-szh, 0.95, 0.707,-0.707,0.000),
};

float szpsqri = -0.02;
const vector6 iglicaSQR[numPts]= //iglica - vextor3(x,y,mapowanie tekstury)
{
 vector6( 0.010+szpsqri, 0.010-szh, 0.40, 1.000, 0.000,0.000),
 vector6( 0.010+szpsqri, 0.149-szh, 0.45, 0.707, 0.707,0.000),
 vector6( 0.000+szpsqri, 0.149-szh, 0.55, 0.707, 0.707,0.000),
 vector6( 0.000+szpsqri, 0.010-szh, 0.60,-1.000, 0.000,0.000),
};


const int S49LnnumPts = 34;
const vector6 szynaS49L[S49LnnumPts]= //szyna - vextor3(x,y,mapowanie tekstury)
{
 // JEDNA POLOWA SZYNY
 vector6( 0.062+szp, 0.000-szh, 0.00, 0.000, 0.000, 0.000),
 vector6( 0.062+szp, 0.008-szh, 0.15, 0.000, 0.000, 0.000),
 vector6( 0.061+szp, 0.010-szh, 0.25, 0.000, 0.000, 0.000),
 vector6( 0.059+szp, 0.011-szh, 0.35, 0.000, 0.000, 0.000),
 vector6( 0.050+szp, 0.012-szh, 0.40, 0.000, 0.000, 0.000),
 vector6( 0.018+szp, 0.020-szh, 0.45, 0.000, 0.000, 0.000),
 vector6( 0.013+szp, 0.023-szh, 0.10, 0.000, 0.000, 0.000),
 vector6( 0.009+szp, 0.026-szh, 0.15, 0.000, 0.000, 0.000),
 vector6( 0.007+szp, 0.053-szh, 0.20, 0.000, 0.000, 0.000),
 vector6( 0.008+szp, 0.096-szh, 0.25, 0.000, 0.000, 0.000),
 vector6( 0.011+szp, 0.102-szh, 0.30, 0.000, 0.000, 0.000),
 vector6( 0.036+szp, 0.110-szh, 0.32, 0.000, 0.000, 0.000),
 vector6( 0.034+szp, 0.140-szh, 0.35, 0.000, 0.000, 0.000),
 vector6( 0.031+szp, 0.144-szh, 0.36, 0.000, 0.000, 0.000),
 vector6( 0.025+szp, 0.148-szh, 0.37, 0.000, 0.000, 0.000),
 vector6( 0.019+szp, 0.149-szh, 0.40, 0.000, 0.000, 0.000),
 vector6( 0.010+szp, 0.149-szh, 0.45, 0.000, 0.000, 0.000),

 // DRUGA POLOWA
 vector6(-0.010+szp, 0.149-szh, 0.55, 0.000, 0.000, 0.000),
 vector6(-0.019+szp, 0.149-szh, 0.60, 0.000, 0.000, 0.000),
 vector6(-0.025+szp, 0.148-szh, 0.63, 0.000, 0.000, 0.000),
 vector6(-0.031+szp, 0.144-szh, 0.64, 0.000, 0.000, 0.000),
 vector6(-0.034+szp, 0.140-szh, 0.66, 0.000, 0.000, 0.000),
 vector6(-0.036+szp, 0.110-szh, 0.68, 0.000, 0.000, 0.000),
 vector6(-0.011+szp, 0.102-szh, 0.70, 0.000, 0.000, 0.000),
 vector6(-0.008+szp, 0.096-szh, 0.75, 0.000, 0.000, 0.000),
 vector6(-0.007+szp, 0.053-szh, 0.80, 0.000, 0.000, 0.000),
 vector6(-0.009+szp, 0.026-szh, 0.85, 0.000, 0.000, 0.000),
 vector6(-0.013+szp, 0.023-szh, 0.87, 0.000, 0.000, 0.000),
 vector6(-0.018+szp, 0.020-szh, 0.90, 0.000, 0.000, 0.000),
 vector6(-0.050+szp, 0.012-szh, 0.92, 0.000, 0.000, 0.000),
 vector6(-0.059+szp, 0.011-szh, 0.95, 0.000, 0.000, 0.000),
 vector6(-0.061+szp, 0.010-szh, 0.97, 0.000, 0.000, 0.000),
 vector6(-0.062+szp, 0.008-szh, 0.98, 0.000, 0.000, 0.000), 
 vector6(-0.062+szp, 0.000-szh, 1.00, 0.000, 0.000, 0.000),

};

float szpi = 0.00;
const vector3 iglicaS49L[S49LnnumPts]= //szyna - vextor3(x,y,mapowanie tekstury)
{

 // JEDNA POLOWA SZYNY
 vector3( 0.010+szpi, 0.000-szh,0.00),
 vector3( 0.010+szpi, 0.008-szh,0.15),
 vector3( 0.010+szpi, 0.010-szh,0.25),
 vector3( 0.010+szpi, 0.011-szh,0.35), //albo tu 0.073
 vector3( 0.010+szpi, 0.012-szh,0.40),
 vector3( 0.010+szpi, 0.020-szh,0.45),
 vector3( 0.010+szpi, 0.023-szh,0.10),
 vector3( 0.010+szpi, 0.026-szh,0.15),
 vector3( 0.010+szpi, 0.053-szh,0.20), //albo tu -0.001
 vector3( 0.010+szpi, 0.096-szh,0.25), //albo zostanie asymetryczna
 vector3( 0.010+szpi, 0.102-szh,0.30),
 vector3( 0.010+szpi, 0.110-szh,0.32),
 vector3( 0.010+szpi, 0.140-szh,0.35),
 vector3( 0.010+szpi, 0.144-szh,0.36),
 vector3( 0.010+szpi, 0.148-szh,0.37),
 vector3( 0.010+szpi, 0.149-szh,0.40), //albo tu 0.073
 vector3( 0.010+szpi, 0.149-szh,0.45),

 // DRUGA POLOWA
 vector3(-0.000+szpi, 0.149-szh, 0.55),
 vector3(-0.000+szpi, 0.149-szh, 0.60),
 vector3(-0.000+szpi, 0.148-szh, 0.63),
 vector3(-0.000+szpi, 0.144-szh, 0.64), //albo tu 0.073
 vector3(-0.000+szpi, 0.140-szh, 0.66),
 vector3(-0.000+szpi, 0.110-szh, 0.68),
 vector3(-0.000+szpi, 0.102-szh, 0.70),
 vector3(-0.000+szpi, 0.096-szh, 0.75),
 vector3(-0.000+szpi, 0.053-szh, 0.80), //albo tu -0.001
 vector3(-0.000+szpi, 0.026-szh, 0.85), //albo zostanie asymetryczna
 vector3(-0.000+szpi, 0.023-szh, 0.87),
 vector3(-0.000+szpi, 0.020-szh, 0.90),
 vector3(-0.000+szpi, 0.012-szh, 0.92),
 vector3(-0.000+szpi, 0.011-szh, 0.95),
 vector3(-0.000+szpi, 0.010-szh, 0.97),
 vector3(-0.000+szpi, 0.008-szh, 0.98), //albo tu 0.073
 vector3(-0.000+szpi, 0.000-szh, 1.00),
};


float szpri = 0.02;
//szh = 0.18;
const int RI60NnnumPts= 74;
const vector3 szynaRI60N[RI60NnnumPts]= //szyna - vextor3(x,y,mapowanie tekstury)
{

// PIERWSZA POLOWA NA PLUSIE 32v
vector3(0.088+szpri, 0.0001-szh,1.00),

vector3(0.089+szpri, 0.001-szh,1.00),
vector3(0.090+szpri, 0.002-szh,1.00),
vector3(0.090+szpri, 0.007-szh,1.00),
vector3(0.089+szpri, 0.008-szh,1.00),
vector3(0.088+szpri, 0.009-szh,1.00),       // 6

vector3(0.014+szpri, 0.017-szh,1.00),
vector3(0.012+szpri, 0.017-szh,1.00),
vector3(0.009+szpri, 0.019-szh,1.00),
vector3(0.007+szpri, 0.021-szh,1.00),
vector3(0.007+szpri, 0.023-szh,1.00),
vector3(0.006+szpri, 0.026-szh,1.00),   //12

vector3(0.006+szpri, 0.133-szh,1.00),
vector3(0.007+szpri, 0.136-szh,1.00),
vector3(0.008+szpri, 0.139-szh,1.00),
vector3(0.011+szpri, 0.141-szh,0.27),
vector3(0.014+szpri, 0.142-szh,0.28),   //17


vector3(0.030+szpri, 0.145-szh,0.29),
vector3(0.032+szpri, 0.146-szh,0.30),
vector3(0.036+szpri, 0.149-szh,0.34),
vector3(0.038+szpri, 0.153-szh,0.35),
vector3(0.039+szpri, 0.155-szh,0.36),
vector3(0.040+szpri, 0.157-szh,0.37),   //23


vector3(0.040+szpri, 0.174-szh,0.38),
vector3(0.039+szpri, 0.176-szh,0.39),
vector3(0.038+szpri, 0.178-szh,0.40),
vector3(0.037+szpri, 0.178-szh,0.410),
vector3(0.032+szpri, 0.180-szh,0.420),
vector3(0.026+szpri, 0.181-szh,0.430),
vector3(0.022+szpri, 0.181-szh,0.440),
vector3(0.011+szpri, 0.181-szh,0.450),
vector3(0.001+szpri, 0.180-szh,0.460),  //32



// DRUGA POLOWA NA MINUSIE 32verts
vector3(-0.000+szpri, 0.180-szh,0.460),
vector3(-0.004+szpri, 0.179-szh,0.500),
vector3(-0.009+szpri, 0.177-szh,0.550),
vector3(-0.013+szpri, 0.174-szh,0.570),
vector3(-0.015+szpri, 0.169-szh,0.580),
vector3(-0.017+szpri, 0.161-szh,0.585), //38


vector3(-0.019+szpri, 0.145-szh,0.590),
vector3(-0.021+szpri, 0.140-szh,0.595),
vector3(-0.024+szpri, 0.136-szh,0.60),
vector3(-0.031+szpri, 0.133-szh,0.62),
vector3(-0.036+szpri, 0.134-szh,0.63),
vector3(-0.041+szpri, 0.136-szh,0.64),
vector3(-0.046+szpri, 0.142-szh,0.65),
vector3(-0.048+szpri, 0.148-szh,0.66),  //46


vector3(-0.051+szpri, 0.173-szh,0.67),
vector3(-0.052+szpri, 0.175-szh,0.68),
vector3(-0.053+szpri, 0.175-szh,0.69),
vector3(-0.054+szpri, 0.176-szh,0.695), //50


vector3(-0.067+szpri, 0.176-szh,0.70),
vector3(-0.070+szpri, 0.176-szh,0.705),
vector3(-0.071+szpri, 0.174-szh,0.88),
vector3(-0.072+szpri, 0.172-szh,0.89),  //54


vector3(-0.068+szpri, 0.143-szh,0.90),
vector3(-0.065+szpri, 0.137-szh,0.91),
vector3(-0.061+szpri, 0.132-szh,0.92),
vector3(-0.055+szpri, 0.126-szh,0.93),
vector3(-0.050+szpri, 0.123-szh,0.975),
vector3(-0.044+szpri, 0.121-szh,0.98),  //60


vector3(-0.012+szpri, 0.114-szh,0.985),
vector3(-0.008+szpri, 0.111-szh,0.99),
vector3(-0.006+szpri, 0.106-szh,0.995),
vector3(-0.006+szpri, 0.100-szh,1.00),  //64

vector3(-0.006+szpri, 0.026-szh,0.985),
vector3(-0.007+szpri, 0.023-szh,0.99),
vector3(-0.007+szpri, 0.021-szh,0.995),
vector3(-0.009+szpri, 0.019-szh,1.00),
vector3(-0.012+szpri, 0.017-szh,1.00),
vector3(-0.014+szpri, 0.017-szh,1.00),   //70

vector3(-0.088+szpri, 0.008-szh,1.00),
vector3(-0.089+szpri, 0.008-szh,1.00),
vector3(-0.009+szpri, 0.007-szh,1.00),

vector3(-0.09+szpri, 0.002-szh,1.00),

};

//---------------------------------------------------------------------------
#endif
