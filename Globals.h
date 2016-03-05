/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef GlobalsH
#define GlobalsH

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include <Classes.hpp> // TStringList
#include <string>
#include <fstream>
#include <sstream>
#include <istream>
#include <iostream>

#include "system.hpp"
#include "opengl/glew.h"
#include "dumb3d.h"
#include "Logs.h"
#include "model3d.h"
#include "ground.h"
#include "particlesys1.h"


#define PI 3.1415926535897f
#define DTOR (PI/180.0f)
#define SQR(x) (x*x)
#define MAXSTATIONS 64
#define MAXPASSENGERENTYPOINTS 512

using namespace Math3D;


typedef struct {
	GLboolean blendEnabled;
	GLint blendSrc;
	GLint blendDst;
} GLblendstate;

typedef struct
{
  GLboolean lightEnabled;
} GLlightstate;

struct trackinfocontainer
{
  std::string platformav;
  std::string number;
  int len;
  int platformlen;
  int electrified;
};

struct stationscontainer
{
  AnsiString Name;
  AnsiString Info;
  AnsiString Type;
  AnsiString SubType;
  int platforms;
  int platformedges;
  int tracksnum;
  trackinfocontainer trackinfo[50];
};

struct pentrypointscontainer
{
 vector3 point;
 AnsiString dynname;
 AnsiString dyntrainnumber;
 AnsiString dyndestination;
};

struct semlightcontainer
{
  bool state;
  bool blink;
};


struct a
{
    static int sort_by;// determines which member to sort by
    float num1;
    vector3 num2;
    bool operator<(const a& rhs) const
    {
        if( sort_by == 1) return num1 < rhs.num1;// sort by num1
        if( sort_by == 2) return num2 < rhs.num2;// sort by num1
      //  return num2 < rhs.num2;// sort by num2
    }
};

int a::sort_by = 1;// default will be to sort by num1



// DEFINICJE ELEMENTOW KABINY

const int ce_ggMainCtrlAct = 10001;
const int ce_ggMainCtrl = 10002;
const int ce_ggScndCtrl = 10003;
const int ce_ggDirKey = 10004;
const int ce_ggBrakeCtrl = 10005;
const int ce_ggLocalBrake = 10006;
const int ce_ggManualBrake = 10007;
const int ce_ggBrakeProfileCtrl = 10008;
const int ce_ggBrakeProfileG = 10009;
const int ce_ggBrakeProfileR = 10010;
const int ce_ggMaxCurrentCtrl = 10011;
const int ce_ggMainOffButton = 10012;
const int ce_ggMainOnButton = 10013;
const int ce_ggSecurityResetButton = 10014;
const int ce_ggReleaserButton = 10015;
const int ce_ggAntiSlipButton = 10016;
const int ce_ggHornButton = 10017;
const int ce_ggFuseButton = 10018;
const int ce_ggConverterFuseButton = 10019;
const int ce_ggStLinOffButton = 10020;
const int ce_ggDoorLeftButton = 10021;
const int ce_ggDoorRightButton = 10022;
const int ce_ggDepartureSignalButton = 10023;
const int ce_ggUpperLightButton = 10024;
const int ce_ggLeftLightButton = 10025;
const int ce_ggRightLightButton = 10026;
const int ce_ggLeftEndLightButton = 10027;
const int ce_ggRightEndLightButton = 10028;
const int ce_ggRearUpperLightButton = 10029;
const int ce_ggRearLeftLightButton = 10030;
const int ce_ggRearRightLightButton = 10031;
const int ce_ggRearLeftEndLightButton = 10032;
const int ce_ggRearRightEndLightButton = 10033;
const int ce_ggCompressorButton = 10034;
const int ce_ggConverterButton = 10035;
const int ce_ggConverterOffButton = 10036;
const int ce_ggMainButton = 10037;
const int ce_ggRadioButton = 10038;
const int ce_ggPantFrontButton = 10039;
const int ce_ggPantRearButton = 10040;
const int ce_ggPantFrontButtonOff = 10041;
const int ce_ggPantAllDownButton = 10042;
const int ce_ggTrainHeatingButton = 10043;
const int ce_ggSignallingButton = 10044;
const int ce_ggDoorSignallingButton = 10045;
const int ce_ggNextCurrentButton = 10046;
const int ce_ggCabLightButton = 10047;
const int ce_ggCabLightDimButton = 10048;
const int ce_ggMainDistributorButton = 10049;
const int ce_ggBatteryButton = 10050;
const int ce_ggLZSButton = 10051;
const int ce_ggCupboard1Button = 10052;
const int ce_ggCupboard2Button = 10053;
const int ce_ggAntiSunRButton = 10054;
const int ce_ggAntiSunLButton = 10055;
const int ce_ggAntiSunCButton = 10056;
const int ce_ggCookerFlapButton = 10057;
const int ce_ggAckermansFlapButton = 10058;
const int ce_ggDoorPMButton = 10059;
const int ce_ggDoorWNButton = 10060;
const int ce_ggDoorCabLButton = 10061;
const int ce_ggDoorCabRButton = 10062;
const int ce_ggArmChairLButton = 10063;
const int ce_ggArmChairRButton = 10064;
const int ce_ggCabWinLButton = 10065;
const int ce_ggCabWinRButton = 10066;
const int ce_ggAshTray1Button = 10067;
const int ce_ggAshTray2Button = 10068;
const int ce_ggCupboard3LButton = 10069;
const int ce_ggCupboard3RButton = 10070;
const int ce_ggHandBrakeButton = 10071;
const int ce_ggHaslerBoltButton = 10072;
const int ce_ggHaslerHullButton = 10073;
const int ce_ggHandBrakeIndButton = 10074;
const int ce_ggRadioAlarmButton = 10075;
const int ce_ggFootSandButton = 10076;
const int ce_ggPantoAirSupplyButton = 10077;
const int ce_ggDoorWNBoltButton = 10078;
const int ce_ggHangerButton = 10079;
const int ce_ggVechDriveTypeButton = 10083;
const int ce_ggCabHeatingButton = 10084;
const int ce_ggHeadLightRDimButton = 10085;
const int ce_ggHeadLightADimButton = 10086;
const int ce_ggSHPDimButton = 10087;
const int ce_ggResistanceFanButton = 10088;
const int ce_ggCookerButton = 10089;
const int ce_ggWiperLButton = 10090;
const int ce_ggWiperRButton = 10091;
const int ce_ggAdjAxlePressureFButton = 10092;

const int ce_btLampkaPoslizg = 20001;
const int ce_btLampkaStyczn = 20002;
const int ce_btLampkaNadmPrzetw = 20003;
const int ce_btLampkaPrzetw = 20004;
const int ce_btLampkaPrzekRozn = 20005;
const int ce_btLampkaPrzekRoznPom = 20006;
const int ce_btLampkaNadmSil = 20007;
const int ce_btLampkaWylSzybki = 20008;
const int ce_btLampkaNadmWent = 20009;
const int ce_btLampkaNadmSpr = 20010;
const int ce_btLampkaOporyB = 20011;
const int ce_btLampkaStycznB = 20012;
const int ce_btLampkaWylSzybkiB = 20013;
const int ce_btLampkaNadmPrzetwB = 20014;
const int ce_btLampkaBezoporowaB = 20015;
const int ce_btLampkaBezoporowa = 20016;
const int ce_btLampkaUkrotnienie = 20017;
const int ce_btLampkaHamPosp = 20018;
const int ce_btLampkaRadio = 20019;
const int ce_btLampkaHamowanie1zes = 20020;
const int ce_btLampkaHamowanie2zes = 20021;
const int ce_btLampkaOpory = 20022;
const int ce_btLampkaWysRozrs = 20023;
const int ce_btLampkaUniversal3 = 20024;
const int ce_btLampkaWentZaluzje = 20025;
const int ce_btLampkaOgrzewanieSkladu = 20026;
const int ce_btLampkaSHP = 20027;
const int ce_btLampkaCzuwaka = 20028;
const int ce_btLampkaRezerwa = 20029;
const int ce_btLampkaNapNastHam = 20030;
const int ce_btLampkaSprezarka = 20031;
const int ce_btLampkaSprezarkaB = 20032;
const int ce_btLampkaBocznik1 = 20033;
const int ce_btLampkaBocznik2 = 20034;
const int ce_btLampkaBocznik3 = 20035;
const int ce_btLampkaBocznik4 = 20036;
const int ce_btLampkaRadiotelefon = 20037;
const int ce_btLampkaHamienie = 20038;
const int ce_btLampkaJazda = 20039;
const int ce_btLampkaBoczniki = 20040;
const int ce_btLampkaMaxSila = 20041;
const int ce_btLampkaPrzekrMaxSila = 20042;
const int ce_btLampkaDoorLeft = 20043;
const int ce_btLampkaDoorRight = 20044;
const int ce_btLampkaDepartureSignal = 20045;
const int ce_btLampkaBlokadaDrzwi = 20046;
const int ce_btLampkaHamulecReczny = 20047;
const int ce_btLampkaForward = 20048;
const int ce_btLampkaBackward = 20049;
const int ce_btCabLight = 20050;
const int ce_btHaslerBrakes = 20051;
const int ce_btHaslerCurrent = 20052;

const int ce_iiRadioTelefon = 30001;
const int ce_iiPlug = 30002;
const int ce_iiCASHPCase1 = 30003;
const int ce_iiCASHPCase2 = 30004;
const int ce_iiTable = 30005;
const int ce_iiHomologation = 30006;
const int ce_iiCASHPSignal = 30007;
const int ce_iiSpeedometer = 30008;
const int ce_iiMainDistributor = 30009;
const int ce_iiVMETERH = 30010;
const int ce_iiVMETERL = 30011;
const int ce_iiVMETER1 = 30012;
const int ce_iiAMETERL = 30013;
const int ce_iiAMETER1 = 30014;
const int ce_iiAMETER2 = 30015;
const int ce_iiAMETER3 = 30016;


// definicje klawiszy
const int k_IncMainCtrl = 0; //[Num+]
const int k_IncMainCtrlFAST = 1; //[Num+] [Shift]
const int k_DecMainCtrl = 2; //[Num-]
const int k_DecMainCtrlFAST = 3; //[Num-] [Shift]
const int k_IncScndCtrl = 4; //[Num/]
const int k_IncScndCtrlFAST = 5;
const int k_DecScndCtrl = 6;
const int k_DecScndCtrlFAST = 7;
const int k_IncLocalBrakeLevel = 8;
const int k_IncLocalBrakeLevelFAST = 9;
const int k_DecLocalBrakeLevel = 10;
const int k_DecLocalBrakeLevelFAST = 11;
const int k_IncBrakeLevel = 12;
const int k_DecBrakeLevel = 13;
const int k_Releaser = 14;
const int k_EmergencyBrake = 15;
const int k_Brake3 = 16;
const int k_Brake2 = 17;
const int k_Brake1 = 18;
const int k_Brake0 = 19;
const int k_WaveBrake = 20;
const int k_AntiSlipping = 21;
const int k_Sand = 22;

const int k_Main = 23;
const int k_DirectionForward = 24;
const int k_DirectionBackward = 25;

const int k_Fuse = 26;
const int k_Compressor = 27;
const int k_Converter = 28;
const int k_MaxCurrent = 29;
const int k_CurrentAutoRelay = 30;
const int k_BrakeProfile = 31;

const int k_Czuwak = 32;
const int k_Horn = 33;
const int k_Horn2 = 34;

const int k_FailedEngineCutOff = 35;

const int k_MechUp = 36;
const int k_MechDown = 37;
const int k_MechLeft = 38;
const int k_MechRight = 39;
const int k_MechForward = 40;
const int k_MechBackward = 41;

const int k_CabForward = 42;
const int k_CabBackward = 43;

const int k_Couple = 44;
const int k_DeCouple = 45;

const int k_ProgramQuit = 46;
// const int k_ProgramPause= 47;
const int k_ProgramHelp = 48;
// NBMX
const int k_OpenLeft = 49;
const int k_OpenRight = 50;
const int k_CloseLeft = 51;
const int k_CloseRight = 52;
const int k_DepartureSignal = 53;
// NBMX
const int k_PantFrontUp = 54;
const int k_PantRearUp = 55;
const int k_PantFrontDown = 56;
const int k_PantRearDown = 57;

const int k_Heating = 58;

// const int k_FreeFlyMode= 59;

const int k_LeftSign = 60;
const int k_UpperSign = 61;
const int k_RightSign = 62;

const int k_SmallCompressor = 63;

const int k_StLinOff = 64;

const int k_CurrentNext = 65;

const int k_Univ1 = 66;
const int k_Univ2 = 67;
const int k_Univ3 = 68;
const int k_Univ4 = 69;
const int k_EndSign = 70;

const int k_Active = 71;
// Winger 020304
const int k_Battery = 72;
const int k_WalkMode = 73;
const int MaxKeys = 74;

// klasy dla wskaŸników globalnych
class TGround;
class TWorld;
class TCamera;
class TDynamicObject;
class TAnimModel; // obiekt terenu
namespace Queryparsercomp
{
class TQueryParserComp; // stary(?) parser
}
class cParser; // nowy (powolny!) parser
class TEvent;
class TTextSound;

class TTranscript
{ // klasa obs³uguj¹ca linijkê napisu do dŸwiêku
  public:
    float fShow; // czas pokazania
    float fHide; // czas ukrycia/usuniêcia
    AnsiString asText; // tekst gotowy do wyœwietlenia (usuniête znaczniki czasu)
    bool bItalic; // czy kursywa (dŸwiêk nieistotny dla prowadz¹cego)
    int iNext; // nastêpna u¿ywana linijka, ¿eby nie przestawiaæ fizycznie tabeli
};

#define MAX_TRANSCRIPTS 30
class TTranscripts
{ // klasa obs³uguj¹ca napisy do dŸwiêków
    TTranscript aLines[MAX_TRANSCRIPTS]; // pozycje na napisy do wyœwietlenia
    int iCount; // liczba zajêtych pozycji
    int iStart; // pierwsza istotna pozycja w tabeli, ¿eby sortowaæ przestawiaj¹c numerki
    float fRefreshTime;

  public:
    TTranscripts();
    ~TTranscripts();
    void AddLine(char *txt, float show, float hide, bool it);
    void Add(char *txt, float len,
             bool backgorund = false); // dodanie tekstów, d³ugoœæ dŸwiêku, czy istotne
    void Update(); // usuwanie niepotrzebnych (ok. 10 razy na sekundê)
};


// ********************************************************************************************************************
// 24.12.15 Q: Swoje zmienne globalne daje do osobnej klasy, bo z doswiadczenia wiem, ze gdy uzbiera sie multum
//             zmiennych w jednej klasie to zaczynaja sie dziac cyrki. Docelowo zmienne konfiguracyjne z Global
//             mozna by przeniesc do oddzielnej klasy CFG
// ********************************************************************************************************************
class QModelInfo
{
 public:
 static AnsiString snearestobj;
 static AnsiString sNI_file;
 static AnsiString sNI_name;
 static AnsiString sNI_type;
 static int iNI_submodels;
 static int iNI_numtri;
 static int iNI_numverts;
 static int iNI_state;
 static int iNI_type;
 static int iNI_textureid;
 static float fNI_angle;
 static float fNI_posx;
 static float fNI_posy;
 static float fNI_posz;
 static bool bnearestobjengaged;
};

class QGlobal
{
 public:
 static HDC glHDC;
 static HGLRC glHGLRC;
 static HWND glHWND;
 static HINSTANCE hINST;
 static char **argv;
// TFtpClient *FTP;
 
 static TStringList *SLTEMP;
 static TStringList *CONFIG;
 static TStringList *LOKTUT;
 static TStringList *LOKKBD;
 static TStringList *MBRIEF;
 static TStringList *MISSIO;
 static TStringList *CONSISTF;
 static TStringList *CONSISTB;
 static TStringList *CONSISTA;

 static AnsiString USERPID;
 static AnsiString asCurrentSceneryPath;
 static AnsiString asCurrentTexturePath;
 static AnsiString asCurrentDynamicPath;
 static AnsiString asCurrentModelsPath;
 static AnsiString asCurrentSoundPath;
 static AnsiString asAPPDIR;
 static AnsiString asCWD;
 static AnsiString logfilenm1;
 static AnsiString logwinname;
 static AnsiString asSCREENSHOTFILE;
 static AnsiString asSSHOTDIR;
 static AnsiString asSSHOTSUB;
 static AnsiString asSSHOTEXT;
 static AnsiString asSSHOTQLT;
 static AnsiString asLBACKEXT;
 static AnsiString asAPPVERS;
 static AnsiString asAPPDATE;
 static AnsiString asAPPSIZE;
 static AnsiString asAPPDAT2;
 static AnsiString asAPPCOMP;
 static AnsiString asAPPRLSE;
 static AnsiString asGTIME;
 static AnsiString asLOKTUTFILE;
 static AnsiString asLOKKBDFILE;
 static AnsiString asEXIFAUTHOR;
 static AnsiString asEXIFCOPYRIGHT;
 static AnsiString asEXIFCAMERASET;
 static AnsiString asNODERENDERED;
 static AnsiString asNODENAME;
 static AnsiString asRAILTYPE;
 static AnsiString objectidinfo;
 static AnsiString globalstr;
 static AnsiString asINCLUDETYPE;
 static AnsiString asINCLUDEFILE;
 static AnsiString asPASSTRAINNUMBER;
 static AnsiString asPASSDESTINATION;
 static AnsiString asDynamicTexturePath;
 static AnsiString asKBDLAYOUT;
 static AnsiString asKBDLAYOUTSTR;
 static AnsiString astempx;
 static AnsiString font10file;
 static AnsiString font11file;
 static AnsiString font12file;
 static AnsiString font14file;
 static AnsiString font16file;
 static AnsiString font18file;
 static AnsiString asDEFAULTSLEEPER;
 
 static GLblendstate GLBLENDSTATE;
 static GLlightstate GLLIGHTSTATE;

 static bool isshift;
 static bool camerasaved;
 static bool mousemode;
 static bool showelementdescatpointer;
 static bool bmodelpreview;
 static bool bGRIDPROAAA;
 static bool bzfightfix;
 static bool bloaderbriefing;
 static bool SCNLOADED;
 static bool bQueuedAdvLog;
 static bool bscrfilter;
 static bool bscrnoise;
 static bool bfirstloadingscn;
 static bool bpanview;
 static bool bTUTORIAL;
 static bool bKEYBOARD;
 static bool bSHOWBRIEFING;
 static bool bSCNLOADED;
 static bool bAPPDONE;
 static bool bFADEOFFP;
 static bool bRENDERSKY1;
 static bool bRENDERSKY2;
 static bool bRENDERRAIN;
 static bool bRENDERSNOW;
 static bool bRENDERCLDS;
 static bool bRENDERSUN;
 static bool bRENDERMOON;
 static bool bGRAYSCALE;
 static bool bSPLASHSCR;
 static bool bWRITEEXIF;
 static bool MIRROR_R_ACTIVE;
 static bool MIRROR_L_ACTIVE;
 static bool bISLAMP;
 static bool bADVDEBUG1;
 static bool bRENDERGUI;
 static bool bSENDLOGFTP;
 static bool breplacescn;
 static bool bISDYNAMIC;  // Q 040116: dla zmiany sciezki tekstur dla modelviewera
 static bool bRAINSTED;
 static bool bOPENLOGONX;
 static bool bEXITQUERY;
 static bool bWATERMARK;
 static bool bKBDREVERSED;
 static bool bSIMSTARTED;
 static bool bchangingfoga;
 static bool bchangingfogb;
 static bool bchangingfogsa;
 static bool bchangingfogsb;
 static bool brendermenu;
 static bool bfirstinitok;
 static bool bCABLOADING;
 static bool bRTIES;
 static bool bFIRSTFRAME;
 static bool bAUTOSWITCHBALLAST;
 static bool bCALCNORMALS;
 static bool bWIREFRAMETRACK;
 static bool bTRKISSWITCH;
 static bool bISINTERNET;
 static double fdestfogend;
 static double fdestfogstart;
 static double fogchangef;

 static int objectid;
 static int cabelementid;
 static int iMPX;
 static int iMPY;
 static int iHLBLENDTYPE;
 static int iMIRRORSUPDBY;
 static int iRANDTABPOS;
 static int iNODESPASSED;
 static int iNODESFIRSTINIT;
 static int iNODES;
 static int iPARSERBYTESPASSED;
 static int iPARSERBYTESTOTAL;
 static int postep;
 static int infotype;
 static int aspectratio;
 static int loaderrefresh;
 static int iINCLUDETYPE;
 static int iSTATIONPOSINTAB;
 static int iWH;
 static int iWW;
 static int iRENDEREDTIES;
 static int iRENDEREDTRIS;
 static int iRENDEREDSUBS;
 static int iSWITCHDIRECT;
 static int iSPLASHTIME;
 static int iSNOWFLAKES;
 static int iSNOWSQUARE;

 static double fscreenfade;
 static double fscreenfade2;
 static double fMoveLightS;
 static double fps;
 static float gtc1;
 static float gtc2;
 static float lsec;
 static int rtim;
 static float ffov;
 static int LDRREFRESH;
 static int   LDRBORDER;
 static float GUITUTOPAC;
 static float ffovblocktime;
 static float ftrwiresize;
 static float consistlen;
 static float fnoisealpha;
 static float fTIEMAXDIST;

 static GLuint reflecttex;
 static GLuint mousesymbol;
 static GLuint mousepoint;
 static GLuint splashscreen;
 static GLuint loaderbackg;
 static GLuint loaderbrief;
 static GLuint loaderlogo;
 static GLuint bfonttex;
 static GLuint consolebackg;
 static GLuint SCRFILTER;
 static GLuint sky1tex;
 static GLuint sky2tex;
 static GLuint texturetab[16];
 static GLuint texsun1;
 static GLuint texsun2;
 static GLuint texsun3;
 static GLfloat selcolor[4];
 static GLuint semlight;
 static GLuint semlense;
 static Color4 gCOLOR4F;

 static a array[MAXPASSENGERENTYPOINTS];
 static stationscontainer station[MAXSTATIONS];     // POWODUJE KRZACZENIE PODCZAS WYCHODZENIA???
 static pentrypointscontainer PEP[MAXPASSENGERENTYPOINTS];
 static semlightcontainer slc[128];
 static int currententrypoint;

 static float rekrot3_rot;
 static float rekrot3_time;
 static int rekrot_step;
 static bool rekrot_timepause;
 static Color4 lepc;
 static Color4 lfipc;
 static TModel3d *mdTIEh;
 static TModel3d *mdTIEl;
 static TTrack *pTrack;

 // SNOW GLOBAL VARS
 static  long snow_type;
 static  long snow_flakes;
 static float snow_area;
 static float snow_size;
 static float snow_srcf;
 static float snow_srct;
 static float snow_sraf;
 static float snow_srat;
 static  bool snow_color;
 static  bool snow_tex;
 static  bool snow_light;
 static  long snow_blend;

};

class Global
{
  private:
    static GLuint iTextureId; // ostatnio u¿yta tekstura 2D
  public:
    // double Global::tSinceStart;
    static int Keys[MaxKeys];
    static vector3 pCameraPosition; // pozycja kamery w œwiecie
    static double
        pCameraRotation; // kierunek bezwzglêdny kamery w œwiecie: 0=pó³noc, 90°=zachód (-azymut)
    static double pCameraRotationDeg; // w stopniach, dla animacji billboard
    static vector3 pFreeCameraInit[10]; // pozycje kamery
    static vector3 pFreeCameraInitAngle[10];
    static int iWindowWidth;
    static int iWindowHeight;
    static float fDistanceFactor;
    static int iBpp;
    static bool bFullScreen;
    static bool bFreeFly;
    // float RunningTime;
    static bool bWireFrame;
    static bool bSoundEnabled;
    // McZapkie-131202
    // static bool bRenderAlpha;
    static bool bAdjustScreenFreq;
    static bool bEnableTraction;
    static bool bLoadTraction;
    static float fFriction;
    static bool bLiveTraction;
    static bool bManageNodes;
    static bool bDecompressDDS;
    //    bool WFreeFly;
    static float Global::fMouseXScale;
    static float Global::fMouseYScale;
    static double fFogStart;
    static double fFogEnd;
    static TGround *pGround;
    static char **szDefaultExt;
    static char szSceneryFile[256];
    static char CreatorName1[20];
    static char CreatorName2[20];
    static char CreatorName3[20];
    static char CreatorName4[30];
    static char CreatorName5[30];
    static AnsiString asCurrentSceneryPath;
    static AnsiString asCurrentTexturePath;
    static AnsiString asCurrentDynamicPath;
    static AnsiString asCurrentSoundPath;
    // McZapkie-170602: zewnetrzna definicja pojazdu uzytkownika
    static AnsiString asHumanCtrlVehicle;
    static void LoadIniFile(AnsiString asFileName);
    static void InitKeys(AnsiString asFileName);
    inline static vector3 GetCameraPosition()
    {
        return pCameraPosition;
    };
    static void SetCameraPosition(vector3 pNewCameraPosition);
    static void SetCameraRotation(double Yaw);
    static int iWriteLogEnabled; // maska bitowa: 1-zapis do pliku, 2-okienko
    // McZapkie-221002: definicja swiatla dziennego
    static GLfloat AtmoColor[];
    static GLfloat FogColor[];
    // static bool bTimeChange;
    static GLfloat ambientDayLight[];
    static GLfloat diffuseDayLight[];
    static GLfloat specularDayLight[];
    static GLfloat ambientLight[];
    static GLfloat diffuseLight[];
    static GLfloat specularLight[];
    static GLfloat whiteLight[];
    static GLfloat noLight[];
    static GLfloat darkLight[];
    static GLfloat lightPos[4];
    static int iSlowMotion;
    static TDynamicObject *changeDynObj;
    static double ABuDebug;
    static bool detonatoryOK;
    static AnsiString asSky;
    static bool bnewAirCouplers;
    // Ra: nowe zmienne globalne
    static int iDefaultFiltering; // domyœlne rozmywanie tekstur TGA
    static int iBallastFiltering; // domyœlne rozmywanie tekstury podsypki
    static int iRailProFiltering; // domyœlne rozmywanie tekstury szyn
    static int iDynamicFiltering; // domyœlne rozmywanie tekstur pojazdów
    static int iReCompile; // zwiêkszany, gdy trzeba odœwie¿yæ siatki
    static bool bUseVBO; // czy jest VBO w karcie graficznej
    static int iFeedbackMode; // tryb pracy informacji zwrotnej
    static int iFeedbackPort; // dodatkowy adres dla informacji zwrotnych
    static double fOpenGL; // wersja OpenGL - przyda siê
    static bool bOpenGL_1_5; // czy s¹ dostêpne funkcje OpenGL 1.5
    static double fLuminance; // jasnoœæ œwiat³a do automatycznego zapalania
    static int iMultiplayer; // blokada dzia³ania niektórych eventów na rzecz kominikacji
    static HWND hWnd; // uchwyt okna
    static int iCameraLast;
    static AnsiString asRelease; // numer
    static AnsiString asVersion; // z opisem
    static int
        iViewMode; // co aktualnie widaæ: 0-kabina, 1-latanie, 2-sprzêgi, 3-dokumenty, 4-obwody
    static GLint iMaxTextureSize; // maksymalny rozmiar tekstury
    static int iTextMode; // tryb pracy wyœwietlacza tekstowego
    static int iScreenMode[12]; // numer ekranu wyœwietlacza tekstowego
    static bool bDoubleAmbient; // podwójna jasnoœæ ambient
    static double fMoveLight; // numer dnia w roku albo -1
    static bool bSmoothTraction; // wyg³adzanie drutów
    static double fSunDeclination; // deklinacja S³oñca
    static double fTimeSpeed; // przyspieszenie czasu, zmienna do testów
    static double fTimeAngleDeg; // godzina w postaci k¹ta
    static float fClockAngleDeg[6]; // k¹ty obrotu cylindrów dla zegara cyfrowego
    static double fLatitudeDeg; // szerokoœæ geograficzna
    static char *szTexturesTGA[4]; // lista tekstur od TGA
    static char *szTexturesDDS[4]; // lista tekstur od DDS
    static int iMultisampling; // tryb antyaliasingu: 0=brak,1=2px,2=4px,3=8px,4=16px
    static bool bGlutFont; // tekst generowany przez GLUT
    static int iKeyLast; // ostatnio naciœniêty klawisz w celu logowania
    static int iPause; // globalna pauza ruchu: b0=start,b1=klawisz,b2=t³o,b3=lagi,b4=wczytywanie
    static bool bActive; // czy jest aktywnym oknem
    static void BindTexture(GLuint t);
    static int iConvertModels; // tworzenie plików binarnych
    static int iErorrCounter; // licznik sprawdzañ do œledzenia b³êdów OpenGL
    static bool bInactivePause; // automatyczna pauza, gdy okno nieaktywne
    static int iTextures; // licznik u¿ytych tekstur
    static int iSlowMotionMask; // maska wy³¹czanych w³aœciwoœci
    static int iModifyTGA; // czy korygowaæ pliki TGA dla szybszego wczytywania
    static bool bHideConsole; // hunter-271211: ukrywanie konsoli
    static TWorld *pWorld; // wskaŸnik na œwiat do usuwania pojazdów
    static TAnimModel *pTerrainCompact; // obiekt terenu do ewentualnego zapisania w pliku
    static AnsiString asTerrainModel; // nazwa obiektu terenu do zapisania w pliku
    static bool bRollFix; // czy wykonaæ przeliczanie przechy³ki
    static Queryparsercomp::TQueryParserComp *qParser;
    static cParser *pParser;
    static int iSegmentsRendered; // iloœæ segmentów do regulacji wydajnoœci
    static double fFpsAverage; // oczekiwana wartosæ FPS
    static double fFpsDeviation; // odchylenie standardowe FPS
    static double fFpsMin; // dolna granica FPS, przy której promieñ scenerii bêdzie zmniejszany
    static double fFpsMax; // górna granica FPS, przy której promieñ scenerii bêdzie zwiêkszany
    static double fFpsRadiusMax; // maksymalny promieñ renderowania
    static int iFpsRadiusMax; // maksymalny promieñ renderowania w rozmiarze tabeli sektorów
    static double fRadiusFactor; // wspó³czynnik zmiany promienia
    static TCamera *pCamera; // parametry kamery
    static TDynamicObject *pUserDynamic; // pojazd u¿ytkownika, renderowany bez trzêsienia
    static double fCalibrateIn[6][4]; // parametry kalibracyjne wejœæ z pulpitu
    static double fCalibrateOut[7][4]; // parametry kalibracyjne wyjœæ dla pulpitu
    static double fBrakeStep; // krok zmiany hamulca dla klawiszy [Num3] i [Num9]
    static bool bJoinEvents; // czy grupowaæ eventy o tych samych nazwach
    static bool bSmudge; // czy wyœwietlaæ smugê, a pojazd u¿ytkownika na koñcu
    static AnsiString asTranscript[5]; // napisy na ekranie (widoczne)
    static TTranscripts tranTexts; // obiekt obs³uguj¹cy stenogramy dŸwiêków na ekranie
    static AnsiString asLang; // domyœlny jêzyk - http://tools.ietf.org/html/bcp47
    static int iHiddenEvents; // czy ³¹czyæ eventy z torami poprzez nazwê toru
    static TTextSound *tsRadioBusy[10]; // zajêtoœæ kana³ów radiowych (wskaŸnik na odgrywany dŸwiêk)
    // metody
    static void TrainDelete(TDynamicObject *d);
    static void ConfigParse(Queryparsercomp::TQueryParserComp *qp, cParser *cp = NULL);
    static void ConfigParseQ(AnsiString filename);
    static AnsiString GetNextSymbol();
    static TDynamicObject *__fastcall DynamicNearest();
    static TDynamicObject *__fastcall CouplerNearest();
    static bool AddToQuery(TEvent *event, TDynamicObject *who);
    static bool DoEvents();
    static AnsiString Bezogonkow(AnsiString str, bool _ = false);
    static double Min0RSpeed(double vel1, double vel2);
    static AnsiString LoadStationsBase();
    static int findstationbyname(AnsiString name);
    static int findpassengerdynamic(vector3 PPos, AnsiString asName, AnsiString REL, AnsiString DST, TGroundNode *GN);
    static int listdir(const char *szDir, bool bCountHidden, AnsiString ext, TStringList &SL );
    static int setpassengerdest(AnsiString train, AnsiString station);
    static std::string GetKbdLayout();
    static void LOADMISSIONDESCRIPTION();
    static bool CHECKINTERNET();
    static void divideline(AnsiString line, TStringList *sl, int ll);
    static void SENDLOGTOFTP(AnsiString DATE);

    static void rendersmokeem();
    static void renderfireem();
};

//---------------------------------------------------------------------------
#endif
