/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/
/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak, Maciej Czapkiewicz and others

*/

#include "system.hpp"
#include "classes.hpp"
#include<iostream>
#include<vector.h>
#include<algorithm>
#include "assert.h"
#include <string>
#include <fstream>
#include <sstream>
#include <istream>

#pragma hdrstop

#include "Globals.h"
#include "QueryParserComp.hpp"
#include "usefull.h"
#include "Mover.h"
#include "Driver.h"
#include "Console.h"
#include <Controls.hpp> //do odczytu daty
#include "World.h"
#include "model3d.h"
#include "dumb3d.h"
#include "dynobj.h"
#include "qutils.h"
#include "world.h"
#include "timer.h"
#include "qutils.h"
// namespace Global {

//using namespace Math3d

// ***********************************************************************************************************
// Q Globals
// ***********************************************************************************************************

AnsiString QModelInfo::snearestobj       = "";
AnsiString QModelInfo::sNI_file          = "";
AnsiString QModelInfo::sNI_name          = "";
AnsiString QModelInfo::sNI_type          = "";
int QModelInfo::iNI_submodels      = 0;
int QModelInfo::iNI_numtri         = 0;
int QModelInfo::iNI_numverts       = 0;
int QModelInfo::iNI_state          = 0;
int QModelInfo::iNI_type           = 0;
int QModelInfo::iNI_textureid      = 0;
float QModelInfo::fNI_angle          = 0.0;
float QModelInfo::fNI_posx           = 0.0;
float QModelInfo::fNI_posy           = 0.0;
float QModelInfo::fNI_posz           = 0.0;
bool QModelInfo::bnearestobjengaged = true;

HINSTANCE QGlobal::hDLL = NULL;
cLib *QGlobal::DLL;
HDC QGlobal::glHDC=NULL;
HGLRC QGlobal::glHGLRC=NULL;
HWND QGlobal::glHWND=NULL;
HINSTANCE QGlobal::hINST=NULL;

AnsiString QGlobal::USERPID="unknown";
AnsiString QGlobal::asCurrentSceneryPath="scenery/";
AnsiString QGlobal::asCurrentTexturePath=AnsiString(szTexturePath);
AnsiString QGlobal::asCurrentModelsPath="models/";
AnsiString QGlobal::asCurrentDynamicPath="";
AnsiString QGlobal::asCurrentSoundPath="sounds/";
AnsiString QGlobal::asAPPDIR="";
AnsiString QGlobal::asCWD="";
AnsiString QGlobal::logfilenm1= "\\runlog.txt";
AnsiString QGlobal::logwinname= "runlog - Notatnik";
AnsiString QGlobal::asSCREENSHOTFILE= "";
AnsiString QGlobal::asSSHOTDIR= "SCR\\";
AnsiString QGlobal::asSSHOTSUB= "2014\\";
AnsiString QGlobal::asSSHOTEXT= ".jpg";
AnsiString QGlobal::asSSHOTQLT= "99";
AnsiString QGlobal::asLBACKEXT= ".jpg";
AnsiString QGlobal::asAPPVERS= "0.0.0.0";
AnsiString QGlobal::asAPPDATE= "00.00.00";
AnsiString QGlobal::asAPPSIZE= "0";
AnsiString QGlobal::asAPPDAT2= "00:00:00";
AnsiString QGlobal::asAPPCOMP= "00:00:00";
AnsiString QGlobal::asAPPRLSE= "00:00:00";
AnsiString QGlobal::asGTIME= "00:00:00";
AnsiString QGlobal::asLOKTUTFILE = "";
AnsiString QGlobal::asLOKKBDFILE = "";
AnsiString QGlobal::asEXIFAUTHOR = "undefined";
AnsiString QGlobal::asEXIFCOPYRIGHT = "undefined";
AnsiString QGlobal::asEXIFCAMERASET = "";
AnsiString QGlobal::asNODERENDERED = "";
AnsiString QGlobal::asNODENAME = "";
AnsiString QGlobal::asRAILTYPE = "LOW";
AnsiString QGlobal::objectidinfo = "none";
AnsiString QGlobal::globalstr = "?";
AnsiString QGlobal::asINCLUDETYPE = "?";
AnsiString QGlobal::asINCLUDEFILE = "?";
AnsiString QGlobal::asDynamicTexturePath = "";
AnsiString QGlobal::asPASSTRAINNUMBER = "";
AnsiString QGlobal::asPASSDESTINATION = "";
AnsiString QGlobal::asKBDLAYOUT = "";
AnsiString QGlobal::asKBDLAYOUTSTR = "";
AnsiString QGlobal::astempx = "";
AnsiString QGlobal::font10file = "";
AnsiString QGlobal::font11file = "";
AnsiString QGlobal::font12file = "";
AnsiString QGlobal::font14file = "";
AnsiString QGlobal::font16file = "";
AnsiString QGlobal::font18file = "";
AnsiString QGlobal::asDEFAULTSLEEPER = "podklad-wood-2";
TStringList *QGlobal::SLTEMP;
TStringList *QGlobal::CONFIG;
TStringList *QGlobal::LOKTUT;
TStringList *QGlobal::LOKKBD;
TStringList *QGlobal::MBRIEF;
TStringList *QGlobal::MISSIO;
TStringList *QGlobal::CONSISTF;
TStringList *QGlobal::CONSISTB;
TStringList *QGlobal::CONSISTA;

GLblendstate QGlobal::GLBLENDSTATE;
GLlightstate QGlobal::GLLIGHTSTATE;

bool QGlobal::isshift = false;
bool QGlobal::camerasaved = false;
bool QGlobal::mousemode = false;
bool QGlobal::showelementdescatpointer = true;
bool QGlobal::bmodelpreview = false;
bool QGlobal::bGRIDPROAAA = true;
bool QGlobal::bzfightfix = false;
bool QGlobal::bloaderbriefing = false;
bool QGlobal::SCNLOADED = false;
bool QGlobal::bQueuedAdvLog = false;
bool QGlobal::bscrfilter = false;
bool QGlobal::bscrnoise = false;
bool QGlobal::bfirstloadingscn = false;
bool QGlobal::bpanview = false;
bool QGlobal::bTUTORIAL = false;
bool QGlobal::bKEYBOARD = false;
bool QGlobal::bSHOWBRIEFING = true;
bool QGlobal::bSCNLOADED = false;
bool QGlobal::bAPPDONE = false;
bool QGlobal::bFADEOFFP = false;
bool QGlobal::bRENDERSKY1 = true;
bool QGlobal::bRENDERSKY2 = true;
bool QGlobal::bRENDERRAIN = true;
bool QGlobal::bRENDERSNOW = false;
bool QGlobal::bRENDERCLDS = true;
bool QGlobal::bRENDERSUN = true;
bool QGlobal::bRENDERMOON = true;
bool QGlobal::bGRAYSCALE = false;
bool QGlobal::bSPLASHSCR = false;
bool QGlobal::bWRITEEXIF = false;
bool QGlobal::MIRROR_R_ACTIVE = false;
bool QGlobal::MIRROR_L_ACTIVE = false;
bool QGlobal::bISLAMP = false;
bool QGlobal::bADVDEBUG1 = false;
bool QGlobal::bRENDERGUI = false;
bool QGlobal::bSENDLOGFTP = false;
bool QGlobal::breplacescn = false;
bool QGlobal::bISDYNAMIC = false;
bool QGlobal::bRAINSTED = false;
bool QGlobal::bOPENLOGONX = false;
bool QGlobal::bEXITQUERY = false;
bool QGlobal::bWATERMARK= false;
bool QGlobal::bKBDREVERSED = false;
bool QGlobal::bSIMSTARTED = false;
bool QGlobal::bchangingfoga = false;
bool QGlobal::bchangingfogb = false;
bool QGlobal::bchangingfogsa = false;
bool QGlobal::bchangingfogsb = false;
bool QGlobal::brendermenu = false;
bool QGlobal::bfirstinitok = false;
bool QGlobal::bCABLOADING = false;
bool QGlobal::bRTIES = false;
bool QGlobal::bAUTOSWITCHBALLAST = false;
bool QGlobal::bFIRSTFRAME = true;
bool QGlobal::bCALCNORMALS = false;
bool QGlobal::bWIREFRAMETRACK = false;
bool QGlobal::bTRKISSWITCH = false;
bool QGlobal::bISINTERNET = false;
double QGlobal::fdestfogend = 0;
double QGlobal::fdestfogstart = 0;
double QGlobal::fogchangef = 0;
double QGlobal::SUNROTX = 0.0f;
double QGlobal::SUNROTY = 0.0f;

int QGlobal::objectid = 0;
int QGlobal::cabelementid = 0;
int QGlobal::iMPX = 0;
int QGlobal::iMPY = 0;
int QGlobal::iHLBLENDTYPE = 0;
int QGlobal::iMIRRORSUPDBY = 2;
int QGlobal::iRANDTABPOS = 0;
int QGlobal::iNODES = 100000;
int QGlobal::iNODESPASSED = 100000;
int QGlobal::iNODESFIRSTINIT = 0;
int QGlobal::iPARSERBYTESPASSED = 0;
int QGlobal::iPARSERBYTESTOTAL = 0;
int QGlobal::postep = 1;
int QGlobal::infotype=0;
int QGlobal::aspectratio = 43;
int QGlobal::loaderrefresh = 5;
int QGlobal::iINCLUDETYPE = 999;
int QGlobal::iSTATIONPOSINTAB = 0;
int QGlobal::iWH = 0;
int QGlobal::iWW = 0;
int QGlobal::iRENDEREDTIES = 0;
int QGlobal::iRENDEREDTRIS = 0;
int QGlobal::iRENDEREDSUBS = 0;
int QGlobal::iSWITCHDIRECT = 0;
int QGlobal::iSPLASHTIME = 0;
int QGlobal::iSNOWFLAKES = 40000;
int QGlobal::iSNOWSQUARE = 300;

double QGlobal::fMoveLightS = -1.0f;
double QGlobal::fps = 1.0f;
float QGlobal::gtc1 = 0;
float QGlobal::gtc2 = 0;
float QGlobal::lsec = 0;
int QGlobal::rtim = 0;
float QGlobal::ffov = 45.0f;
int QGlobal::LDRREFRESH = 100;
int   QGlobal::LDRBORDER = 1;
float QGlobal::GUITUTOPAC = 0.1f;
double QGlobal::fscreenfade = 1.0;
double QGlobal::fscreenfade2 = 1.0;
float QGlobal::ffovblocktime = 0.0;
float QGlobal::ftrwiresize = 1.2f;
float QGlobal::consistlen = 0.0;
float QGlobal::fnoisealpha = 0.06f;
float QGlobal::fTIEMAXDIST = 100;
float QGlobal::emm1[] = {1, 1, 1, 0};
float QGlobal::emm2[] = {0, 0, 0, 1};

GLuint QGlobal::reflecttex;
GLuint QGlobal::mousesymbol;
GLuint QGlobal::mousepoint;
GLuint QGlobal::splashscreen;
GLuint QGlobal::loaderbackg;
GLuint QGlobal::loaderbrief;
GLuint QGlobal::loaderlogo;
GLuint QGlobal::bfonttex;
GLuint QGlobal::consolebackg;
GLuint QGlobal::SCRFILTER;
GLuint QGlobal::sky1tex;
GLuint QGlobal::sky2tex;
GLuint QGlobal::texturetab[16];
GLuint QGlobal::texsun1;
GLuint QGlobal::texsun2;
GLuint QGlobal::texsun3;
GLfloat QGlobal::selcolor[4];
GLuint QGlobal::semlight;
GLuint QGlobal::semlense;
Color4 QGlobal::gCOLOR4F;
GLint QGlobal::blendSrc;
GLint QGlobal::blendDst;

char **QGlobal::argv = NULL;

a QGlobal::array[MAXPASSENGERENTYPOINTS];
stationscontainer QGlobal::station[MAXSTATIONS];
pentrypointscontainer QGlobal::PEP[MAXPASSENGERENTYPOINTS];
semlightcontainer QGlobal::slc[128];
int QGlobal::currententrypoint = 0;

float QGlobal::rekrot3_rot = 0.0;
float QGlobal::rekrot3_time = 0.0;
int QGlobal::rekrot_step = 1;
bool QGlobal::rekrot_timepause = false;
Color4 QGlobal::lepc;
Color4 QGlobal::lfipc;
TModel3d *QGlobal::mdTIEh = NULL;
TModel3d *QGlobal::mdTIEl = NULL;
TTrack *QGlobal::pTrack = NULL;

// SNOW GLOBAL VARS
  long QGlobal::snow_objt = 1;
  long QGlobal::snow_type = 1;
  long QGlobal::snow_flakes = 35000;
 float QGlobal::snow_area = 150;
 float QGlobal::snow_base = 150.0;
 float QGlobal::snow_size = 0.08f;
 float QGlobal::snow_srcf = 0.1f;
 float QGlobal::snow_srct = 0.7f;
 float QGlobal::snow_sraf = 0.2f;
 float QGlobal::snow_srat = 0.7f;
  bool QGlobal::snow_color = true;
  bool QGlobal::snow_tex = false;
  bool QGlobal::snow_light = true;
  long QGlobal::snow_blend = 1;

// MWD VARS
std::string QGlobal::COM_port = "none";
 bool  QGlobal::bLOGIT = false;
 bool  QGlobal::bOPCOM = false;
 float QGlobal::fRXSPD = 0.2;
 float QGlobal::fmaxCH = 1.0;
 float QGlobal::fmaxPG = 1.0;
 float QGlobal::fmaxZG = 1.6;
 float QGlobal::fmaxV1 = 3600.0;
 float QGlobal::fmaxA1 = 800.0;
 float QGlobal::fanalog1min = 0;
 float QGlobal::fanalog1max = 255;
 float QGlobal::fanalog2min = 0;
 float QGlobal::fanalog2max = 255;
 float QGlobal::fMWDInEnable = 0;	//zablokowane prze³¹czniki i NEFy na pulpicie - nie blokuj¹ klawiatury

 bool QGlobal::DEV_P01[9];
 bool QGlobal::DEV_P02[9];
 bool QGlobal::DEV_P03[9];
 bool QGlobal::DEV_P04[9];
 bool QGlobal::DEV_P05[9];
 bool QGlobal::DEV_P06[9];	   // wejœcia: NEFy, hebelki
 bool QGlobal::DEV_P07[9];
 byte QGlobal::DEV_P08;		   // nastawnik
 byte QGlobal::DEV_P09;		   // bocznik
 byte QGlobal::DEV_P10;		   // analog1
 byte QGlobal::DEV_P11;		   // analog2
 byte QGlobal::DEV_P12;	       	   // analog3
 byte QGlobal::DEV_P13;		   // analog4
 long QGlobal::iobindsnum = 0;     // iobinds
 iosets QGlobal::IOSET[KEYBINDINGS];
 std::string QGlobal::IOCOMMAND = "";
 bool QGlobal::portstate[5][64];

// parametry do u¿ytku wewnêtrznego
// double Global::tSinceStart=0;
TGround *Global::pGround = NULL;
// char Global::CreatorName1[30]="2001-2004 Maciej Czapkiewicz <McZapkie>";
// char Global::CreatorName2[30]="2001-2003 Marcin WoŸniak <Marcin_EU>";
// char Global::CreatorName3[20]="2004-2005 Adam Bugiel <ABu>";
// char Global::CreatorName4[30]="2004 Arkadiusz Œlusarczyk <Winger>";
// char Global::CreatorName5[30]="2003-2009 £ukasz Kirchner <Nbmx>";
AnsiString Global::asCurrentSceneryPath = "scenery/";
AnsiString Global::asCurrentTexturePath = AnsiString(szTexturePath);
AnsiString Global::asCurrentDynamicPath = "";
AnsiString Global::asCurrentSoundPath = "sounds/";
int Global::iSlowMotion =
    0; // info o malym FPS: 0-OK, 1-wy³¹czyæ multisampling, 3-promieñ 1.5km, 7-1km
TDynamicObject *Global::changeDynObj = NULL; // info o zmianie pojazdu
bool Global::detonatoryOK; // info o nowych detonatorach
double Global::ABuDebug = 0;
AnsiString Global::asSky = "1";
double Global::fOpenGL = 0.0; // wersja OpenGL - do sprawdzania obecnoœci rozszerzeñ
bool Global::bOpenGL_1_5 = false; // czy s¹ dostêpne funkcje OpenGL 1.5
double Global::fLuminance = 1.0; // jasnoœæ œwiat³a do automatycznego zapalania
int Global::iReCompile = 0; // zwiêkszany, gdy trzeba odœwie¿yæ siatki
HWND Global::hWnd = NULL; // uchwyt okna
int Global::iCameraLast = -1;
AnsiString Global::asRelease = "15.4.1171.474";
AnsiString Global::asVersion =
    "Compilation 2015-11-15, release " + Global::asRelease + "."; // tutaj, bo wysy³any
int Global::iViewMode = 0; // co aktualnie widaæ: 0-kabina, 1-latanie, 2-sprzêgi, 3-dokumenty
int Global::iTextMode = 0; // tryb pracy wyœwietlacza tekstowego
int Global::iScreenMode[12] = {0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0}; // numer ekranu wyœwietlacza tekstowego
double Global::fSunDeclination = 0.0; // deklinacja S³oñca
double Global::fTimeAngleDeg = 0.0; // godzina w postaci k¹ta
float Global::fClockAngleDeg[6]; // k¹ty obrotu cylindrów dla zegara cyfrowego
char *Global::szTexturesTGA[4] = {"tga", "dds", "tex", "bmp"}; // lista tekstur od TGA
char *Global::szTexturesDDS[4] = {"dds", "tga", "tex", "bmp"}; // lista tekstur od DDS
int Global::iKeyLast = 0; // ostatnio naciœniêty klawisz w celu logowania
GLuint Global::iTextureId = 0; // ostatnio u¿yta tekstura 2D
int Global::iPause = 0x10; // globalna pauza ruchu
bool Global::bActive = true; // czy jest aktywnym oknem
int Global::iErorrCounter = 0; // licznik sprawdzañ do œledzenia b³êdów OpenGL
int Global::iTextures = 0; // licznik u¿ytych tekstur
TWorld *Global::pWorld = NULL;
Queryparsercomp::TQueryParserComp *Global::qParser = NULL;
cParser *Global::pParser = NULL;
int Global::iSegmentsRendered = 90; // iloœæ segmentów do regulacji wydajnoœci
TCamera *Global::pCamera = NULL; // parametry kamery
TDynamicObject *Global::pUserDynamic = NULL; // pojazd u¿ytkownika, renderowany bez trzêsienia
bool Global::bSmudge = false; // czy wyœwietlaæ smugê, a pojazd u¿ytkownika na koñcu
AnsiString Global::asTranscript[5]; // napisy na ekranie (widoczne)
TTranscripts Global::tranTexts; // obiekt obs³uguj¹cy stenogramy dŸwiêków na ekranie

// parametry scenerii
vector3 Global::pCameraPosition;
double Global::pCameraRotation;
double Global::pCameraRotationDeg;
vector3 Global::pFreeCameraInit[10];
vector3 Global::pFreeCameraInitAngle[10];
double Global::fFogStart = 1700;
double Global::fFogEnd = 2000;
GLfloat Global::AtmoColor[] = {0.423f, 0.702f, 1.0f};
GLfloat Global::FogColor[] = {0.6f, 0.7f, 0.8f};
GLfloat Global::ambientDayLight[] = {0.40f, 0.40f, 0.45f, 1.0f}; // robocze
GLfloat Global::diffuseDayLight[] = {0.55f, 0.54f, 0.50f, 1.0f};
GLfloat Global::specularDayLight[] = {0.95f, 0.94f, 0.90f, 1.0f};
GLfloat Global::ambientLight[] = {0.80f, 0.80f, 0.85f, 1.0f}; // sta³e
GLfloat Global::diffuseLight[] = {0.85f, 0.85f, 0.80f, 1.0f};
GLfloat Global::specularLight[] = {0.95f, 0.94f, 0.90f, 1.0f};
GLfloat Global::whiteLight[] = {1.00f, 1.00f, 1.00f, 1.0f};
GLfloat Global::noLight[] = {0.00f, 0.00f, 0.00f, 1.0f};
GLfloat Global::darkLight[] = {0.03f, 0.03f, 0.03f, 1.0f}; //œladowe
GLfloat Global::lightPos[4];
bool Global::bRollFix = true; // czy wykonaæ przeliczanie przechy³ki
bool Global::bJoinEvents = false; // czy grupowaæ eventy o tych samych nazwach
int Global::iHiddenEvents = 1; // czy ³¹czyæ eventy z torami poprzez nazwê toru

// parametry u¿ytkowe (jak komu pasuje)
int Global::Keys[MaxKeys];
int Global::iWindowWidth = 800;
int Global::iWindowHeight = 600;
float Global::fDistanceFactor = 768.0; // baza do przeliczania odleg³oœci dla LoD
int Global::iFeedbackMode = 1; // tryb pracy informacji zwrotnej
int Global::iFeedbackPort = 0; // dodatkowy adres dla informacji zwrotnych
bool Global::bFreeFly = false;
bool Global::bFullScreen = false;
bool Global::bInactivePause = true; // automatyczna pauza, gdy okno nieaktywne
float Global::fMouseXScale = 1.5;
float Global::fMouseYScale = 0.2;
char Global::szSceneryFile[256] = "td2.scn";
AnsiString Global::asHumanCtrlVehicle = "EU07-424";
int Global::iMultiplayer = 0; // blokada dzia³ania niektórych funkcji na rzecz komunikacji
double Global::fMoveLight = -1; // ruchome œwiat³o
double Global::fLatitudeDeg = 52.0; // szerokoœæ geograficzna
float Global::fFriction = 1.0; // mno¿nik tarcia - KURS90
double Global::fBrakeStep = 1.0; // krok zmiany hamulca dla klawiszy [Num3] i [Num9]
AnsiString Global::asLang = "pl"; // domyœlny jêzyk - http://tools.ietf.org/html/bcp47

// parametry wydajnoœciowe (np. regulacja FPS, szybkoœæ wczytywania)
bool Global::bAdjustScreenFreq = true;
bool Global::bEnableTraction = true;
bool Global::bLoadTraction = true;
bool Global::bLiveTraction = true;
int Global::iDefaultFiltering = 9; // domyœlne rozmywanie tekstur TGA bez alfa
int Global::iBallastFiltering = 9; // domyœlne rozmywanie tekstur podsypki
int Global::iRailProFiltering = 5; // domyœlne rozmywanie tekstur szyn
int Global::iDynamicFiltering = 5; // domyœlne rozmywanie tekstur pojazdów
bool Global::bUseVBO = false; // czy jest VBO w karcie graficznej (czy u¿yæ)
GLint Global::iMaxTextureSize = 16384; // maksymalny rozmiar tekstury
bool Global::bSmoothTraction = false; // wyg³adzanie drutów starym sposobem
char **Global::szDefaultExt = Global::szTexturesDDS; // domyœlnie od DDS
int Global::iMultisampling = 2; // tryb antyaliasingu: 0=brak,1=2px,2=4px,3=8px,4=16px
bool Global::bGlutFont = false; // czy tekst generowany przez GLUT32.DLL
int Global::iConvertModels = 7; // tworzenie plików binarnych, +2-optymalizacja transformów
int Global::iSlowMotionMask = -1; // maska wy³¹czanych w³aœciwoœci dla zwiêkszenia FPS
int Global::iModifyTGA = 7; // czy korygowaæ pliki TGA dla szybszego wczytywania
// bool Global::bTerrainCompact=true; //czy zapisaæ teren w pliku
TAnimModel *Global::pTerrainCompact = NULL; // do zapisania terenu w pliku
AnsiString Global::asTerrainModel = ""; // nazwa obiektu terenu do zapisania w pliku
double Global::fFpsAverage = 20.0; // oczekiwana wartosæ FPS
double Global::fFpsDeviation = 5.0; // odchylenie standardowe FPS
double Global::fFpsMin = 0.0; // dolna granica FPS, przy której promieñ scenerii bêdzie zmniejszany
double Global::fFpsMax = 0.0; // górna granica FPS, przy której promieñ scenerii bêdzie zwiêkszany
double Global::fFpsRadiusMax = 3000.0; // maksymalny promieñ renderowania
int Global::iFpsRadiusMax = 225; // maksymalny promieñ renderowania
double Global::fRadiusFactor = 1.1; // wspó³czynnik jednorazowej zmiany promienia scenerii

// parametry testowe (do testowania scenerii i obiektów)
bool Global::bWireFrame = false;
bool Global::bSoundEnabled = true;
int Global::iWriteLogEnabled = 3; // maska bitowa: 1-zapis do pliku, 2-okienko, 4-nazwy torów
bool Global::bManageNodes = true;
bool Global::bDecompressDDS = false; // czy programowa dekompresja DDS

// parametry do kalibracji
// kolejno wspó³czynniki dla potêg 0, 1, 2, 3 wartoœci odczytanej z urz¹dzenia
double Global::fCalibrateIn[6][4] = {
    {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}};
double Global::fCalibrateOut[7][4] = {{0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0},
                                      {0, 1, 0, 0}};

// parametry przejœciowe (do usuniêcia)
// bool Global::bTimeChange=false; //Ra: ZiomalCl wy³¹czy³ star¹ wersjê nocy
// bool Global::bRenderAlpha=true; //Ra: wywali³am tê flagê
bool Global::bnewAirCouplers = true;
bool Global::bDoubleAmbient = false; // podwójna jasnoœæ ambient
double Global::fTimeSpeed = 1.0; // przyspieszenie czasu, zmienna do testów
bool Global::bHideConsole = false; // hunter-271211: ukrywanie konsoli
int Global::iBpp = 32; // chyba ju¿ nie u¿ywa siê kart, na których 16bpp coœ poprawi

 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

AnsiString Global::GetNextSymbol()
{ // pobranie tokenu z aktualnego parsera
    if (qParser)
        return qParser->EndOfFile ? AnsiString("endconfig") : qParser->GetNextSymbol();
    if (pParser)
    {
        std::string token;
        pParser->getTokens();
        *pParser >> token;
        return AnsiString(token.c_str());
    };
    return "";
};


// ************************************************************************************************
// Wczytywanie konfiguracji z pliku EU07.INI
// Q 20.01.16: Wprowadzam nowy parser do tego celu, u niektorych czasem sie sypalo tutaj
// ************************************************************************************************
void Global::LoadIniFile(AnsiString asFileName)
{
 //WriteLog("LoadIniFile() - 1");

    int i;
    for (i = 0; i < 10; ++i)
    { // zerowanie pozycji kamer
        pFreeCameraInit[i] = vector3(0, 0, 0); // wspó³rzêdne w scenerii
        pFreeCameraInitAngle[i] = vector3(0, 0, 0); // k¹ty obrotu w radianach
    }

    TFileStream *fs;

    fs = new TFileStream(Trim(QGlobal::asAPPDIR + asFileName), fmOpenRead | fmShareCompat);
    if (!fs)
        return;

    AnsiString str = "";
    int size = fs->Size;
    str.SetLength(size);
    fs->Read(str.c_str(), size);
    // str+="";
    delete fs;

    TQueryParserComp *Parser;
    Parser = new TQueryParserComp(NULL);
    Parser->TextToParse = str;
    // Parser->LoadStringToParse(asFile);
    Parser->First();
ConfigParse(Parser);

    delete Parser; // Ra: tego jak zwykle nie by³o wczeœniej :]



 // ConfigParseQ(asFileName);
};


AnsiString LCT(AnsiString lowercasetrim)
{
 return LowerCase(Trim(lowercasetrim));
}

// ************************************************************************************************
// Alternatywny parser plikow konfiguracyjnych
// ************************************************************************************************
void Global::ConfigParseQ(AnsiString filename)
{
  TStringList *slINI;
  slINI = new TStringList();
//char szINIFILE[200];
  AnsiString line;
  AnsiString test, key, p01, p02, p03, p04, p05, p06, debuglog, skyen, defaultext;
  int calprtI, caliP1, caliP2, caliP3, caliP4, maxtexsize;
  int calprtO, caloP1, caloP2, caloP3, caloP4;
  int pos1, pos2, pos3;
  bool calprtOok, calprtIok, i;
// sprintf(szINIFILE,"%s%s", QGlobal::asAPPDIR , "eu07.ini");
// WriteLog("READING CONFIG FILE... (" + AnsiString(szINIFILE) + ")");

  char *strings[30];
  char Policystr[4096] = "the|string|to|split";
  char delims[] = " ";

  slINI->LoadFromFile(QGlobal::asAPPDIR + "eu07.ini");
// std::ifstream FileINI("eu07.ini");
  WriteLog("Config parsing...");

//if(FileINI)
// {
//  while(std::getline(FileINI,line))
//   {
      for (int l = 0; l<slINI->Count-1; l++)
       {
        line = slINI->Strings[l];

        pos1 = pos2 = pos3 = 0;

        pos1 = line.Pos("//");
        pos2 = line.Pos(":");                 
        pos3 = line.Pos("//");
        test = line.SubString(1, pos1-1);

        test = StringReplace( test, "   ", " ", TReplaceFlags() << rfReplaceAll );
        test = StringReplace( test, "  ", " ", TReplaceFlags() << rfReplaceAll );

        strcpy(Policystr, test.c_str());

        i = 0;
        strings[i] = strtok( Policystr, delims );
        while( strings[i] != NULL )
          {
           printf("%d '%s'\n", i, strings[i]);
           strings[++i] = strtok( NULL, delims );
          }

          if (pos1 > 4) // gdy nie ma komentarza przed kluczem, troche prowizorycznie 
            {
             WriteLog(test + ": " + strings[0] + "-> [" + Trim(strings[1]) + "][" + Trim(strings[2]) + "]");

             key = LCT(strings[0]);
             p01 = LCT(strings[1]);
             p02 = LCT(strings[2]);
             p03 = LCT(strings[3]);
             p04 = LCT(strings[4]);
             p05 = LCT(strings[5]);
             p06 = LCT(strings[6]);

             if (key == "sceneryfile") strcpy(szSceneryFile, p01.c_str());
             if (key == "humanctrlvehicle") asHumanCtrlVehicle = p01;
             if (key == "width") iWindowWidth = p01.ToIntDef(800);
             if (key == "height") iWindowHeight = p01.ToIntDef(600);
             if (key == "fullscreen") Global::bFullScreen = YNToBool(p01);
             if (key == "bpp") iBpp = p01.ToIntDef(32);
             if (key == "debugmode") DebugModeFlag = YNToBool(p01);
             if (key == "soundenabled") bSoundEnabled = YNToBool(p01);
             if (key == "physicslog") WriteLogFlag = YNToBool(p01);
             if (key == "debuglog") debuglog = p01;
             if (key == "wireframe") bWireFrame = YNToBool(p01);
             if (key == "physicsdeactivation") PhysicActivationFlag = YNToBool(p01);
             if (key == "adjustscreenfreq") bAdjustScreenFreq = YNToBool(p01);
             if (key == "enabletraction") bEnableTraction = YNToBool(p01);
             if (key == "loadtraction") bLoadTraction = YNToBool(p01);
             if (key == "livetraction") bLiveTraction = YNToBool(p01);
             if (key == "managenodes") bManageNodes = YNToBool(p01);
             if (key == "friction") fFriction = p01.ToDouble();
             if (key == "skyenabled") skyen = p01;
             if (key == "brakestep") fBrakeStep = p01.ToDouble();
             if (key == "mousescale") {fMouseXScale = p01.ToDouble(); fMouseYScale = p02.ToDouble();}
             if (key == "defaultext") defaultext = p01;
             if (key == "maxtexturesize") maxtexsize = p01.ToIntDef(16384);
             if (key == "defaultfiltering") iDefaultFiltering = p01.ToIntDef(-1);
             if (key == "ballastfiltering") iBallastFiltering = p01.ToIntDef(-1);
             if (key == "railprofiltering") iRailProFiltering = p01.ToIntDef(-1);
             if (key == "dynamicfiltering") iDynamicFiltering = YNToBool(p01);
             if (key == "newaircouplers") bnewAirCouplers = YNToBool(p01);
             if (key == "usevbo") bUseVBO = YNToBool(p01);
             if (key == "feedbackmode") iFeedbackMode = p01.ToIntDef(1);
             if (key == "feedbackport") iFeedbackPort = p01.ToIntDef(0);
             if (key == "smoothtraction") bSmoothTraction = YNToBool(p01);
             if (key == "timespeed") fTimeSpeed = p01.ToIntDef(1);
             if (key == "multisampling") iMultisampling = p01.ToIntDef(2);   // domyœlnie 2
             if (key == "glufont") bGlutFont = YNToBool(p01);
             if (key == "lang") asLang = p01;
             if (key == "decompressdds") bDecompressDDS = YNToBool(p01);
             if (key == "freefly") { bFreeFly = YNToBool(p01);pFreeCameraInit[0].x = p02.ToDouble(); pFreeCameraInit[0].y = p03.ToDouble(); pFreeCameraInit[0].z = p04.ToDouble();}
             if (key == "convertmodels") iConvertModels = p01.ToIntDef(7);
             if (key == "inactivepause") bInactivePause = YNToBool(p01);
             if (key == "modifytga") iModifyTGA = p01.ToIntDef(0);
             if (key == "hideconsole") Global::bHideConsole = YNToBool(p01);
             if (key == "rollfix") Global::bRollFix = YNToBool(p01);
             if (key == "fpsaverage") fFpsAverage = p01.ToDouble();
             if (key == "fpsdeviation") fFpsDeviation = p01.ToDouble();
             if (key == "fpsradiusmax") fFpsRadiusMax = p01.ToDouble();
             if (key == "joinduplicatedevents") bJoinEvents = YNToBool(p01);
             if (key == "hiddenevents") iHiddenEvents = p01.ToIntDef(0);
             if (key == "opengl") fOpenGL = p01.ToDouble();
             if (key == "latitude") fLatitudeDeg = p01.ToDouble();
             if (key == "heightbase") fDistanceFactor = p01.ToInt();
             if (key == "doubleambient") bDoubleAmbient = YNToBool(p01);
             if (key == "slowmotion") iSlowMotionMask = p01.ToIntDef(-1);
             if (key == "calibratein") { calprtI = p01.ToIntDef(-1); caliP1 = p02.ToDouble(); caliP2 = p03.ToDouble(); caliP3 = p04.ToDouble(); caliP4 = p05.ToDouble(); calprtIok = true;}
             if (key == "calibrateout"){ calprtO = p01.ToIntDef(-1); caloP1 = p02.ToDouble(); caloP2 = p03.ToDouble(); caloP3 = p04.ToDouble(); caloP4 = p05.ToDouble(); calprtOok = true;}
            }
       }
// }

// na koniec trochê zale¿noœci

     if (defaultext == "tga") szDefaultExt = szTexturesTGA; // domyœlnie od TGA
     if (debuglog == "yes") iWriteLogEnabled = 3;
      if (debuglog == "no") iWriteLogEnabled = 0;
       else iWriteLogEnabled = 3;

     if (skyen == "yes") asSky = "1";
       else asSky = "0";

     if (maxtexsize <=   64) iMaxTextureSize = 64;
      else
     if (maxtexsize <=  128) iMaxTextureSize = 128;
      else
     if (maxtexsize <=  256) iMaxTextureSize = 256;
      else
     if (maxtexsize <=  512) iMaxTextureSize = 512;
      else
     if (maxtexsize <= 1024) iMaxTextureSize = 1024;
      else
     if (maxtexsize <= 2048) iMaxTextureSize = 2048;
      else
     if (maxtexsize <= 4096) iMaxTextureSize = 4096;
      else
     if (maxtexsize <= 8192) iMaxTextureSize = 8192;
      else
                             iMaxTextureSize = 16384;

    if (!bLoadTraction) // wczytywanie drutów i s³upów
    { // tutaj wy³¹czenie, bo mog¹ nie byæ zdefiniowane w INI
        bEnableTraction = false; // false = pantograf siê nie po³amie
        bLiveTraction = false; // false = pantografy zawsze zbieraj¹ 95% MaxVoltage
    }
    // if (fMoveLight>0) bDoubleAmbient=false; //wtedy tylko jedno œwiat³o ruchome
    // if (fOpenGL<1.3) iMultisampling=0; //mo¿na by z góry wy³¹czyæ, ale nie mamy jeszcze fOpenGL

    if (iMultisampling)
    { // antyaliasing ca³oekranowy wy³¹cza rozmywanie drutów
        bSmoothTraction = false;
    }

    if (iMultiplayer > 0)
    {
        bInactivePause = false; // okno "w tle" nie mo¿e pauzowaæ, jeœli w³¹czona komunikacja
        // pauzowanie jest zablokowane dla (iMultiplayer&2)>0, wiêc iMultiplayer=1 da siê zapauzowaæ
        // (tryb instruktora)
    }

    fFpsMin = fFpsAverage -
              fFpsDeviation; // dolna granica FPS, przy której promieñ scenerii bêdzie zmniejszany
    fFpsMax = fFpsAverage +
              fFpsDeviation; // górna granica FPS, przy której promieñ scenerii bêdzie zwiêkszany
    if (iPause)
        iTextMode = VK_F1; // jak pauza, to pokazaæ zegar

    if (calprtIok && calprtOok)
     {
      if ((calprtI < 0) || (calprtI > 5)) calprtI = 5; // na ostatni, bo i tak trzeba pomin¹æ wartoœci
      fCalibrateIn[calprtI][0] = caliP1;                                        // wyraz wolny
      fCalibrateIn[calprtI][1] = caliP2;                                        // mno¿nik
      fCalibrateIn[calprtI][2] = caliP3;                                        // mno¿nik dla kwadratu
      fCalibrateIn[calprtI][3] = caliP4;                                        // mno¿nik dla szeœcianu

      if ((calprtO < 0) || (calprtO > 6)) calprtO = 6; // na ostatni, bo i tak trzeba pomin¹æ wartoœci
      fCalibrateOut[calprtO][0] = caloP1;                                       // wyraz wolny
      fCalibrateOut[calprtO][1] = caloP2;                                       // mno¿nik liniowy
      fCalibrateOut[calprtO][2] = caloP3;                                       // mno¿nik dla kwadratu
      fCalibrateOut[calprtO][3] = caloP4;                                       // mno¿nik dla szeœcianu
     }

     { // to poni¿ej wykonywane tylko raz, jedynie po wczytaniu eu07.ini
        Console::ModeSet(iFeedbackMode, iFeedbackPort);                         // tryb pracy konsoli sterowniczej
        iFpsRadiusMax = 0.000025 * fFpsRadiusMax * fFpsRadiusMax;               // maksymalny promieñ renderowania 3000.0 -> 225
        if (iFpsRadiusMax > 400)
            iFpsRadiusMax = 400;
        if (fDistanceFactor > 1.0)
        { // dla 1.0 specjalny tryb bez przeliczania
            fDistanceFactor = iWindowHeight / fDistanceFactor;                  // fDistanceFactor>1.0 dla rozdzielczoœci wiêkszych ni¿ bazowa
            fDistanceFactor *= (iMultisampling + 1.0) * fDistanceFactor;        // do kwadratu, bo wiêkszoœæ odleg³oœci to ich kwadraty
        }
     }

     WriteLog("INI FILE OK.");
     WriteLog("");
  // FileINI.close();
}

void Global::ConfigParse(TQueryParserComp *qp, cParser *cp)
{ // Ra: trzeba by przerobiæ na cParser, ¿eby to dzia³a³o w scenerii
 WriteLog("Config parsing...");
 pParser = cp;
 qParser = qp;
 AnsiString str;
 int i;
  do
    {
        str = GetNextSymbol().LowerCase();
        if (str == AnsiString("sceneryfile"))
        {
            str = GetNextSymbol().LowerCase();
            strcpy(szSceneryFile, str.c_str());
        }
        else if (str == AnsiString("humanctrlvehicle"))
        {
            str = GetNextSymbol().LowerCase();
            asHumanCtrlVehicle = str;
        }
        else if (str == AnsiString("width"))
            iWindowWidth = GetNextSymbol().ToInt();
        else if (str == AnsiString("height"))
            iWindowHeight = GetNextSymbol().ToInt();
        else if (str == AnsiString("heightbase"))
            fDistanceFactor = GetNextSymbol().ToInt();
        else if (str == AnsiString("bpp"))
            iBpp = ((GetNextSymbol().LowerCase() == AnsiString("32")) ? 32 : 16);
        else if (str == AnsiString("fullscreen"))
            Global::bFullScreen = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("freefly")) // Mczapkie-130302
        {
            bFreeFly = (GetNextSymbol().LowerCase() == AnsiString("yes"));
            pFreeCameraInit[0].x = GetNextSymbol().ToDouble();
            pFreeCameraInit[0].y = GetNextSymbol().ToDouble();
            pFreeCameraInit[0].z = GetNextSymbol().ToDouble();
        }
        else if (str == AnsiString("wireframe"))
            bWireFrame = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("debugmode")) // McZapkie! - DebugModeFlag uzywana w mover.pas, warto tez blokowac cheaty gdy false
            DebugModeFlag = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("soundenabled")) // McZapkie-040302 - blokada dzwieku - przyda sie do debugowania oraz na komp. bez karty dzw.
            bSoundEnabled = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        
        else if (str == AnsiString("physicslog")) // McZapkie-030402 - logowanie parametrow fizycznych dla kazdego pojazdu z maszynista
            WriteLogFlag = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("physicsdeactivation")) // McZapkie-291103 - usypianie fizyki
            PhysicActivationFlag = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("debuglog"))
        { // McZapkie-300402 - wylaczanie log.txt
            str = GetNextSymbol().LowerCase();
            if (str == "yes")
                iWriteLogEnabled = 3;
            else if (str == "no")
                iWriteLogEnabled = 0;
           else
               iWriteLogEnabled = str.ToIntDef(3);
        }
        else if (str == AnsiString("adjustscreenfreq"))
        { // McZapkie-240403 - czestotliwosc odswiezania ekranu
            str = GetNextSymbol();
            bAdjustScreenFreq = (str.LowerCase() == AnsiString("yes"));
        }
        else if (str == AnsiString("mousescale"))
        { // McZapkie-060503 - czulosc ruchu myszy (krecenia glowa)
            str = GetNextSymbol();
            fMouseXScale = str.ToDouble();
            str = GetNextSymbol();
            fMouseYScale = str.ToDouble();
        }
        else if (str == AnsiString("enabletraction"))
        { // Winger 040204 - 'zywe' patyki dostosowujace sie do trakcji; Ra 2014-03: teraz ³amanie
            bEnableTraction = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        }
        else if (str == AnsiString("loadtraction"))
        { // Winger 140404 - ladowanie sie trakcji
            bLoadTraction = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        }
        else if (str == AnsiString("friction")) // mno¿nik tarcia - KURS90
            fFriction = GetNextSymbol().ToDouble();
        else if (str == AnsiString("livetraction"))
        { // Winger 160404 - zaleznosc napiecia loka od trakcji; Ra 2014-03: teraz pr¹d przy braku
            // sieci
            bLiveTraction = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        }
        else if (str == AnsiString("skyenabled"))
        { // youBy - niebo
            if (GetNextSymbol().LowerCase() == AnsiString("yes"))
                asSky = "1";
            else
                asSky = "0";
        }
        else if (str == AnsiString("autoswitchballast"))
        {
            QGlobal::bAUTOSWITCHBALLAST = (GetNextSymbol().LowerCase() == AnsiString("yes"));    // Q: czy renderowac automatycznie podsypke pod rozjazdami
        }
        else if (str == AnsiString("managenodes"))
        {
            bManageNodes = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        }
        else if (str == AnsiString("decompressdds"))
        {
            bDecompressDDS = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        }
        //ShaXbee - domyslne rozszerzenie tekstur
        else if (str == AnsiString("defaultext"))
        {
            str = GetNextSymbol().LowerCase(); // rozszerzenie
            if (str == "tga")
                szDefaultExt = szTexturesTGA; // domyœlnie od TGA
            // szDefaultExt=std::string(Parser->GetNextSymbol().LowerCase().c_str());
        }
        else if (str == AnsiString("newaircouplers"))
            bnewAirCouplers = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("defaultfiltering"))
            iDefaultFiltering = GetNextSymbol().ToIntDef(-1);
        else if (str == AnsiString("ballastfiltering"))
            iBallastFiltering = GetNextSymbol().ToIntDef(-1);
        else if (str == AnsiString("railprofiltering"))
            iRailProFiltering = GetNextSymbol().ToIntDef(-1);
        else if (str == AnsiString("dynamicfiltering"))
            iDynamicFiltering = GetNextSymbol().ToIntDef(-1);
        else if (str == AnsiString("usevbo"))
            bUseVBO = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("feedbackmode"))
            iFeedbackMode = GetNextSymbol().ToIntDef(1); // domyœlnie 1
        else if (str == AnsiString("feedbackport"))
            iFeedbackPort = GetNextSymbol().ToIntDef(0); // domyœlnie 0
        else if (str == AnsiString("multiplayer"))
            iMultiplayer = GetNextSymbol().ToIntDef(0); // domyœlnie 0
        else if (str == AnsiString("maxtexturesize"))
        { // wymuszenie przeskalowania tekstur
            i = GetNextSymbol().ToIntDef(16384); // domyœlnie du¿e
            if (i <= 64)
                iMaxTextureSize = 64;
            else if (i <= 128)
                iMaxTextureSize = 128;
            else if (i <= 256)
                iMaxTextureSize = 256;
            else if (i <= 512)
                iMaxTextureSize = 512;
            else if (i <= 1024)
                iMaxTextureSize = 1024;
            else if (i <= 2048)
                iMaxTextureSize = 2048;
            else if (i <= 4096)
                iMaxTextureSize = 4096;
            else if (i <= 8192)
                iMaxTextureSize = 8192;
            else
                iMaxTextureSize = 16384;
        }
        else if (str == AnsiString("doubleambient")) // podwójna jasnoœæ ambient
            bDoubleAmbient = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("movelight")) // numer dnia w roku albo -1
        {
            fMoveLight = GetNextSymbol().ToIntDef(-1); // numer dnia 1..365
            if (fMoveLight == 0.0)
            { // pobranie daty z systemu
                unsigned short y, m, d;
                TDate date = Now();
                date.DecodeDate(&y, &m, &d);
                fMoveLight =
                    (double)date - (double)TDate(y, 1, 1) + 1; // numer bie¿¹cego dnia w roku
            }
            if (fMoveLight > 0.0) // tu jest nadal zwiêkszone o 1
            { // obliczenie deklinacji wg:
                // http://naturalfrequency.com/Tregenza_Sharples/Daylight_Algorithms/algorithm_1_11.htm
                // Spencer J W Fourier series representation of the position of the sun Search 2 (5)
                // 172 (1971)
                fMoveLight = M_PI / 182.5 * (Global::fMoveLight - 1.0); // numer dnia w postaci k¹ta
                fSunDeclination = 0.006918 - 0.3999120 * cos(fMoveLight) +
                                  0.0702570 * sin(fMoveLight) - 0.0067580 * cos(2 * fMoveLight) +
                                  0.0009070 * sin(2 * fMoveLight) -
                                  0.0026970 * cos(3 * fMoveLight) + 0.0014800 * sin(3 * fMoveLight);
            }
        }
        else if (str == AnsiString("smoothtraction")) // podwójna jasnoœæ ambient
            bSmoothTraction = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("timespeed")) // przyspieszenie czasu, zmienna do testów
            fTimeSpeed = GetNextSymbol().ToIntDef(1);
        else if (str == AnsiString("multisampling")) // tryb antyaliasingu: 0=brak,1=2px,2=4px
            iMultisampling = GetNextSymbol().ToIntDef(2); // domyœlnie 2
        else if (str == AnsiString("glutfont")) // tekst generowany przez GLUT
            bGlutFont = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("latitude")) // szerokoœæ geograficzna
            fLatitudeDeg = GetNextSymbol().ToDouble();
        else if (str == AnsiString("convertmodels")) // tworzenie plików binarnych
            iConvertModels = GetNextSymbol().ToIntDef(7); // domyœlnie 7
        else if (str == AnsiString("inactivepause")) // automatyczna pauza, gdy okno nieaktywne
            bInactivePause = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("slowmotion")) // tworzenie plików binarnych
            iSlowMotionMask = GetNextSymbol().ToIntDef(-1); // domyœlnie -1
        else if (str == AnsiString("modifytga")) // czy korygowaæ pliki TGA dla szybszego wczytywania
            iModifyTGA = GetNextSymbol().ToIntDef(0); // domyœlnie 0
        else if (str == AnsiString("hideconsole")) // hunter-271211: ukrywanie konsoli
            Global::bHideConsole = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("rollfix")) // Ra: poprawianie przechy³ki, aby wewnêtrzna szyna by³a "pozioma"
            bRollFix = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("fpsaverage")) // oczekiwana wartosæ FPS
            fFpsAverage = GetNextSymbol().ToDouble();
        else if (str == AnsiString("fpsdeviation")) // odchylenie standardowe FPS
            fFpsDeviation = GetNextSymbol().ToDouble();
        else if (str == AnsiString("fpsradiusmax")) // maksymalny promieñ renderowania
            fFpsRadiusMax = GetNextSymbol().ToDouble();
        else if (str == AnsiString("calibratein")) // parametry kalibracji wejœæ
        { //
            i = GetNextSymbol().ToIntDef(-1); // numer wejœcia
            if ((i < 0) || (i > 5))
                i = 5; // na ostatni, bo i tak trzeba pomin¹æ wartoœci
            fCalibrateIn[i][0] = GetNextSymbol().ToDouble(); // wyraz wolny
            fCalibrateIn[i][1] = GetNextSymbol().ToDouble(); // mno¿nik
            fCalibrateIn[i][2] = GetNextSymbol().ToDouble(); // mno¿nik dla kwadratu
            fCalibrateIn[i][3] = GetNextSymbol().ToDouble(); // mno¿nik dla szeœcianu
        }
        else if (str == AnsiString("calibrateout")) // parametry kalibracji wyjœæ
        { //
            i = GetNextSymbol().ToIntDef(-1); // numer wejœcia
            if ((i < 0) || (i > 6))
                i = 6; // na ostatni, bo i tak trzeba pomin¹æ wartoœci
            fCalibrateOut[i][0] = GetNextSymbol().ToDouble(); // wyraz wolny
            fCalibrateOut[i][1] = GetNextSymbol().ToDouble(); // mno¿nik liniowy
            fCalibrateOut[i][2] = GetNextSymbol().ToDouble(); // mno¿nik dla kwadratu
            fCalibrateOut[i][3] = GetNextSymbol().ToDouble(); // mno¿nik dla szeœcianu
        }
        else if (str == AnsiString("brakestep")) // krok zmiany hamulca dla klawiszy [Num3] i [Num9]
            fBrakeStep = GetNextSymbol().ToDouble();
        else if (str == AnsiString("joinduplicatedevents")) // czy grupowaæ eventy o tych samych nazwach
            bJoinEvents = (GetNextSymbol().LowerCase() == AnsiString("yes"));
        else if (str == AnsiString("hiddenevents")) // czy ³¹czyæ eventy z torami poprzez nazwê toru
            iHiddenEvents = GetNextSymbol().ToIntDef(0);
        else if (str == AnsiString("pause")) // czy po wczytaniu ma byæ pauza?
            iPause |= (GetNextSymbol().LowerCase() == AnsiString("yes")) ? 1 : 0;
        else if (str == AnsiString("lang"))
            asLang = GetNextSymbol(); // domyœlny jêzyk - http://tools.ietf.org/html/bcp47
        else if (str == AnsiString("opengl")) // deklarowana wersja OpenGL, ¿eby powstrzymaæ b³êdy
            fOpenGL = GetNextSymbol().ToDouble(); // wymuszenie wersji OpenGL
    } while (str != "endconfig"); //(!Parser->EndOfFile)

    // na koniec trochê zale¿noœci
    if (!bLoadTraction) // wczytywanie drutów i s³upów
    { // tutaj wy³¹czenie, bo mog¹ nie byæ zdefiniowane w INI
        bEnableTraction = false; // false = pantograf siê nie po³amie
        bLiveTraction = false; // false = pantografy zawsze zbieraj¹ 95% MaxVoltage
    }
    // if (fMoveLight>0) bDoubleAmbient=false; //wtedy tylko jedno œwiat³o ruchome
    // if (fOpenGL<1.3) iMultisampling=0; //mo¿na by z góry wy³¹czyæ, ale nie mamy jeszcze fOpenGL
    if (iMultisampling)
    { // antyaliasing ca³oekranowy wy³¹cza rozmywanie drutów
        bSmoothTraction = false;
    }
    if (iMultiplayer > 0)
    {
        bInactivePause = false; // okno "w tle" nie mo¿e pauzowaæ, jeœli w³¹czona komunikacja
        // pauzowanie jest zablokowane dla (iMultiplayer&2)>0, wiêc iMultiplayer=1 da siê zapauzowaæ
        // (tryb instruktora)
    }
    fFpsMin = fFpsAverage -
              fFpsDeviation; // dolna granica FPS, przy której promieñ scenerii bêdzie zmniejszany
    fFpsMax = fFpsAverage +
              fFpsDeviation; // górna granica FPS, przy której promieñ scenerii bêdzie zwiêkszany
    if (iPause)
        iTextMode = VK_F1; // jak pauza, to pokazaæ zegar
    if (qp)
    { // to poni¿ej wykonywane tylko raz, jedynie po wczytaniu eu07.ini
        Console::ModeSet(iFeedbackMode, iFeedbackPort); // tryb pracy konsoli sterowniczej
        iFpsRadiusMax = 0.000025 * fFpsRadiusMax * fFpsRadiusMax; // maksymalny promieñ renderowania 3000.0 -> 225
        if (iFpsRadiusMax > 400)
            iFpsRadiusMax = 400;
        if (fDistanceFactor > 1.0)
        { // dla 1.0 specjalny tryb bez przeliczania
            fDistanceFactor = iWindowHeight / fDistanceFactor; // fDistanceFactor>1.0 dla rozdzielczoœci wiêkszych ni¿ bazowa
            fDistanceFactor *= (iMultisampling + 1.0) * fDistanceFactor; // do kwadratu, bo wiêkszoœæ odleg³oœci to ich kwadraty
        }
    }
   WriteLog("OK.");
}

void Global::InitKeys(AnsiString asFileName)
{
 WriteLog("Keys init...");
    //    if (FileExists(asFileName))
    //    {
    //       Error("Chwilowo plik keys.ini nie jest obs³ugiwany. £adujê standardowe
    //       ustawienia.\nKeys.ini file is temporarily not functional, loading default keymap...");
    /*        TQueryParserComp *Parser;
            Parser=new TQueryParserComp(NULL);
            Parser->LoadStringToParse(asFileName);

            for (int keycount=0; keycount<MaxKeys; keycount++)
             {
              Keys[keycount]=Parser->GetNextSymbol().ToInt();
             }

            delete Parser;
    */
    //    }
    //    else
    {
        Keys[k_IncMainCtrl] = VK_ADD;
        Keys[k_IncMainCtrlFAST] = VK_ADD;
        Keys[k_DecMainCtrl] = VK_SUBTRACT;
        Keys[k_DecMainCtrlFAST] = VK_SUBTRACT;
        Keys[k_IncScndCtrl] = VK_DIVIDE;
        Keys[k_IncScndCtrlFAST] = VK_DIVIDE;
        Keys[k_DecScndCtrl] = VK_MULTIPLY;
        Keys[k_DecScndCtrlFAST] = VK_MULTIPLY;
        ///*NORMALNE
        Keys[k_IncLocalBrakeLevel] = VK_NUMPAD1; // VK_NUMPAD7;
        // Keys[k_IncLocalBrakeLevelFAST]=VK_END;  //VK_HOME;
        Keys[k_DecLocalBrakeLevel] = VK_NUMPAD7; // VK_NUMPAD1;
        // Keys[k_DecLocalBrakeLevelFAST]=VK_HOME; //VK_END;
        Keys[k_IncBrakeLevel] = VK_NUMPAD3; // VK_NUMPAD9;
        Keys[k_DecBrakeLevel] = VK_NUMPAD9; // VK_NUMPAD3;
        Keys[k_Releaser] = VK_NUMPAD6;
        Keys[k_EmergencyBrake] = VK_NUMPAD0;
        Keys[k_Brake3] = VK_NUMPAD8;
        Keys[k_Brake2] = VK_NUMPAD5;
        Keys[k_Brake1] = VK_NUMPAD2;
        Keys[k_Brake0] = VK_NUMPAD4;
        Keys[k_WaveBrake] = VK_DECIMAL;
        //*/
        /*MOJE
                Keys[k_IncLocalBrakeLevel]=VK_NUMPAD3;  //VK_NUMPAD7;
                Keys[k_IncLocalBrakeLevelFAST]=VK_NUMPAD3;  //VK_HOME;
                Keys[k_DecLocalBrakeLevel]=VK_DECIMAL;  //VK_NUMPAD1;
                Keys[k_DecLocalBrakeLevelFAST]=VK_DECIMAL; //VK_END;
                Keys[k_IncBrakeLevel]=VK_NUMPAD6;  //VK_NUMPAD9;
                Keys[k_DecBrakeLevel]=VK_NUMPAD9;   //VK_NUMPAD3;
                Keys[k_Releaser]=VK_NUMPAD5;
                Keys[k_EmergencyBrake]=VK_NUMPAD0;
                Keys[k_Brake3]=VK_NUMPAD2;
                Keys[k_Brake2]=VK_NUMPAD1;
                Keys[k_Brake1]=VK_NUMPAD4;
                Keys[k_Brake0]=VK_NUMPAD7;
                Keys[k_WaveBrake]=VK_NUMPAD8;
        */
        Keys[k_AntiSlipping] = VK_RETURN;
        Keys[k_Sand] = VkKeyScan('s');
        Keys[k_Main] = VkKeyScan('m');
        Keys[k_Active] = VkKeyScan('w');
        Keys[k_Battery] = VkKeyScan('j');
        Keys[k_DirectionForward] = VkKeyScan('d');
        Keys[k_DirectionBackward] = VkKeyScan('r');
        Keys[k_Fuse] = VkKeyScan('n');
        Keys[k_Compressor] = VkKeyScan('c');
        Keys[k_Converter] = VkKeyScan('x');
        Keys[k_MaxCurrent] = VkKeyScan('f');
        Keys[k_CurrentAutoRelay] = VkKeyScan('g');
        Keys[k_BrakeProfile] = VkKeyScan('b');
        Keys[k_CurrentNext] = VkKeyScan('z');

        Keys[k_Czuwak] = VkKeyScan(' ');
        Keys[k_Horn] = VkKeyScan('a');
        Keys[k_Horn2] = VkKeyScan('a');

        Keys[k_FailedEngineCutOff] = VkKeyScan('e');

        Keys[k_MechUp] = VK_PRIOR;
        Keys[k_MechDown] = VK_NEXT;
        Keys[k_MechLeft] = VK_LEFT;
        Keys[k_MechRight] = VK_RIGHT;
        Keys[k_MechForward] = VK_UP;
        Keys[k_MechBackward] = VK_DOWN;

        Keys[k_CabForward] = VK_HOME;
        Keys[k_CabBackward] = VK_END;

        Keys[k_Couple] = VK_INSERT;
        Keys[k_DeCouple] = VK_DELETE;

        Keys[k_ProgramQuit] = VK_F10;
        // Keys[k_ProgramPause]=VK_F3;
        Keys[k_ProgramHelp] = VK_F1;
        // Keys[k_FreeFlyMode]=VK_F4;
        Keys[k_WalkMode] = VK_F5;

        Keys[k_OpenLeft] = VkKeyScan(',');
        Keys[k_OpenRight] = VkKeyScan('.');
        Keys[k_CloseLeft] = VkKeyScan(',');
        Keys[k_CloseRight] = VkKeyScan('.');
        Keys[k_DepartureSignal] = VkKeyScan('/');

        // Winger 160204 - obsluga pantografow
        Keys[k_PantFrontUp] = VkKeyScan('p'); // Ra: zamieniony przedni z tylnym
        Keys[k_PantFrontDown] = VkKeyScan('p');
        Keys[k_PantRearUp] = VkKeyScan('o');
        Keys[k_PantRearDown] = VkKeyScan('o');
        // Winger 020304 - ogrzewanie
        Keys[k_Heating] = VkKeyScan('h');
        Keys[k_LeftSign] = VkKeyScan('y');
        Keys[k_UpperSign] = VkKeyScan('u');
        Keys[k_RightSign] = VkKeyScan('i');
        Keys[k_EndSign] = VkKeyScan('t');

        Keys[k_SmallCompressor] = VkKeyScan('v');
        Keys[k_StLinOff] = VkKeyScan('l');
        // ABu 090305 - przyciski uniwersalne, do roznych bajerow :)
        Keys[k_Univ1] = VkKeyScan('[');
        Keys[k_Univ2] = VkKeyScan(']');
        Keys[k_Univ3] = VkKeyScan(';');
        Keys[k_Univ4] = VkKeyScan('\'');
    }
   WriteLog("Keys init OK."); 
}
/*
vector3 Global::GetCameraPosition()
{
    return pCameraPosition;
}
*/
void Global::SetCameraPosition(vector3 pNewCameraPosition)
{
    pCameraPosition = pNewCameraPosition;
}

void Global::SetCameraRotation(double Yaw)
{ // ustawienie bezwzglêdnego kierunku kamery z korekcj¹ do przedzia³u <-M_PI,M_PI>
    pCameraRotation = Yaw;
    while (pCameraRotation < -M_PI)
        pCameraRotation += 2 * M_PI;
    while (pCameraRotation > M_PI)
        pCameraRotation -= 2 * M_PI;
    pCameraRotationDeg = pCameraRotation * 180.0 / M_PI;
}

void Global::BindTexture(GLuint t)
{ // ustawienie aktualnej tekstury, tylko gdy siê zmienia
    if (t != iTextureId)
    {
        iTextureId = t;
    }
};

void Global::TrainDelete(TDynamicObject *d)
{ // usuniêcie pojazdu prowadzonego przez u¿ytkownika
    if (pWorld)
        pWorld->TrainDelete(d);
};

TDynamicObject *__fastcall Global::DynamicNearest()
{ // ustalenie pojazdu najbli¿szego kamerze
    return pGround->DynamicNearest(pCamera->Pos);
};

TDynamicObject *__fastcall Global::CouplerNearest()
{ // ustalenie pojazdu najbli¿szego kamerze
    return pGround->CouplerNearest(pCamera->Pos);
};

bool Global::AddToQuery(TEvent *event, TDynamicObject *who)
{
    return pGround->AddToQuery(event, who);
};
//---------------------------------------------------------------------------

bool Global::DoEvents()
{ // wywo³ywaæ czasem, ¿eby nie robi³ wra¿enia zawieszonego
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return FALSE;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return TRUE;
}
//---------------------------------------------------------------------------

TTranscripts::TTranscripts()
{
    iCount = 0; // brak linijek do wyœwietlenia
    iStart = 0; // wype³niaæ od linijki 0
    for (int i = 0; i < MAX_TRANSCRIPTS; ++i)
    { // to do konstruktora mo¿na by daæ
        aLines[i].fHide = -1.0; // wolna pozycja (czas symulacji, 360.0 to doba)
        aLines[i].iNext = -1; // nie ma kolejnej
    }
    fRefreshTime = 360.0; // wartoœc zaporowa
};
TTranscripts::~TTranscripts(){};
void TTranscripts::AddLine(char *txt, float show, float hide, bool it)
{ // dodanie linii do tabeli, (show) i (hide) w [s] od aktualnego czasu
    if (show == hide)
        return; // komentarz jest ignorowany
    show = Global::fTimeAngleDeg + show / 240.0; // jeœli doba to 360, to 1s bêdzie równe 1/240
    hide = Global::fTimeAngleDeg + hide / 240.0;
    int i = iStart, j, k; // od czegoœ trzeba zacz¹æ
    while ((aLines[i].iNext >= 0) ? (aLines[aLines[i].iNext].fShow <= show) :
                                    false) // póki nie koniec i wczeœniej puszczane
        i = aLines[i].iNext; // przejœcie do kolejnej linijki
    //(i) wskazuje na liniê, po której nale¿y wstawiæ dany tekst, chyba ¿e
    while (txt ? *txt : false)
        for (j = 0; j < MAX_TRANSCRIPTS; ++j)
            if (aLines[j].fHide < 0.0)
            { // znaleziony pierwszy wolny
                aLines[j].iNext = aLines[i].iNext; // dotychczasowy nastêpny bêdzie za nowym
                if (aLines[iStart].fHide < 0.0) // jeœli tablica jest pusta
                    iStart = j; // fHide trzeba sprawdziæ przed ewentualnym nadpisaniem, gdy i=j=0
                else
                    aLines[i].iNext = j; // a nowy bêdzie za tamtym wczeœniejszym
                aLines[j].fShow = show; // wyœwietlaæ od
                aLines[j].fHide = hide; // wyœwietlaæ do
                aLines[j].bItalic = it;
                aLines[j].asText = AnsiString(txt); // bez sensu, wystarczy³by wskaŸnik
                if ((k = aLines[j].asText.Pos("|")) > 0)
                { // jak jest podzia³ linijki na wiersze
                    aLines[j].asText = aLines[j].asText.SubString(1, k - 1);
                    txt += k;
                    i = j; // kolejna linijka dopisywana bêdzie na koniec w³aœnie dodanej
                }
                else
                    txt = NULL; // koniec dodawania
                if (fRefreshTime > show) // jeœli odœwie¿acz ustawiony jest na póŸniej
                    fRefreshTime = show; // to odœwie¿yæ wczeœniej
                break; // wiêcej ju¿ nic
            }
};
void TTranscripts::Add(char *txt, float len, bool backgorund)
{ // dodanie tekstów, d³ugoœæ dŸwiêku, czy istotne
    if (!txt)
        return; // pusty tekst
    int i = 0, j = int(0.5 + 10.0 * len); //[0.1s]
    if (*txt == '[')
    { // powinny byæ dwa nawiasy
        while (*++txt ? *txt != ']' : false)
            if ((*txt >= '0') && (*txt <= '9'))
                i = 10 * i + int(*txt - '0'); // pierwsza liczba a¿ do ]
        if (*txt ? *++txt == '[' : false)
        {
            j = 0; // drugi nawias okreœla czas zakoñczenia wyœwietlania
            while (*++txt ? *txt != ']' : false)
                if ((*txt >= '0') && (*txt <= '9'))
                    j = 10 * j + int(*txt - '0'); // druga liczba a¿ do ]
            if (*txt)
                ++txt; // pominiêcie drugiego ]
        }
    }
    AddLine(txt, 0.1 * i, 0.1 * j, false);
};
void TTranscripts::Update()
{ // usuwanie niepotrzebnych (nie czêœciej ni¿ 10 razy na sekundê)
    if (fRefreshTime > Global::fTimeAngleDeg)
        return; // nie czas jeszcze na zmiany
    // czas odœwie¿enia mo¿na ustaliæ wg tabelki, kiedy coœ siê w niej zmienia
    fRefreshTime = Global::fTimeAngleDeg + 360.0; // wartoœæ zaporowa
    int i = iStart, j = -1; // od czegoœ trzeba zacz¹æ
    bool change = false; // czy zmieniaæ napisy?
    do
    {
        if (aLines[i].fHide >= 0.0) // o ile aktywne
            if (aLines[i].fHide < Global::fTimeAngleDeg)
            { // gdy czas wyœwietlania up³yn¹³
                aLines[i].fHide = -1.0; // teraz bêdzie woln¹ pozycj¹
                if (i == iStart)
                    iStart = aLines[i].iNext >= 0 ? aLines[i].iNext : 0; // przestawienie pierwszego
                else if (j >= 0)
                    aLines[j].iNext = aLines[i].iNext; // usuniêcie ze œrodka
                change = true;
            }
            else
            { // gdy ma byæ pokazane
                if (aLines[i].fShow > Global::fTimeAngleDeg) // bêdzie pokazane w przysz³oœci
                    if (fRefreshTime > aLines[i].fShow) // a nie ma nic wczeœniej
                        fRefreshTime = aLines[i].fShow;
                if (fRefreshTime > aLines[i].fHide)
                    fRefreshTime = aLines[i].fHide;
            }
        // mo¿na by jeszcze wykrywaæ, które nowe maj¹ byæ pokazane
        j = i;
        i = aLines[i].iNext; // kolejna linijka
    } while (i >= 0); // póki po tablicy
    change = true; // bo na razie nie ma warunku, ¿e coœ siê doda³o
    if (change)
    { // aktualizacja linijek ekranowych
        i = iStart;
        j = -1;
        do
        {
            if (aLines[i].fHide > 0.0) // jeœli nie ukryte
                if (aLines[i].fShow < Global::fTimeAngleDeg) // to dodanie linijki do wyœwietlania
                    if (j < 5 - 1) // ograniczona liczba linijek
                        Global::asTranscript[++j] = aLines[i].asText; // skopiowanie tekstu
            i = aLines[i].iNext; // kolejna linijka
        } while (i >= 0); // póki po tablicy
        for (++j; j < 5; ++j)
            Global::asTranscript[j] = ""; // i czyszczenie nieu¿ywanych linijek
    }
};

// Ra: tymczasowe rozwi¹zanie kwestii zagranicznych (czeskich) napisów
char bezogonkowo[128] = "E?,?\"_++?%S<STZZ?`'\"\".--??s>stzz"
                        " ^^L$A|S^CS<--RZo±,l'uP.,as>L\"lz"
                        "RAAAALCCCEEEEIIDDNNOOOOxRUUUUYTB"
                        "raaaalccceeeeiiddnnoooo-ruuuuyt?";

AnsiString Global::Bezogonkow(AnsiString str, bool _)
{ // wyciêcie liter z ogonkami, bo OpenGL nie umie wyœwietliæ
    for (int i = 1; i <= str.Length(); ++i)
        if (str[i] & 0x80)
            str[i] = bezogonkowo[str[i] & 0x7F];
        else if (str[i] < ' ') // znaki steruj¹ce nie s¹ obs³ugiwane
            str[i] = ' ';
        else if (_)
            if (str[i] == '_') // nazwy stacji nie mog¹ zawieraæ spacji
                str[i] = ' '; // wiêc trzeba wyœwietlaæ inaczej
    return str;
}

double Global::Min0RSpeed(double vel1, double vel2)
{ // rozszerzenie funkcji Min0R o wartoœci -1.0
	if (vel1 == -1.0)
		vel1 = std::numeric_limits<double>::max();
	if (vel2 == -1.0)
		vel2 = std::numeric_limits<double>::max();
	return Min0R(vel1, vel2);
}


// ***********************************************************************************************************
// TWORZENIE LISTY PLIKOW Z WYBRANEGO KATALOGU
// ***********************************************************************************************************
int Global::listdir(const char *szDir, bool bCountHidden, AnsiString ext, TStringList &SL)
{
	char path[MAX_PATH];
	WIN32_FIND_DATA fd;
	DWORD dwAttr = FILE_ATTRIBUTE_DIRECTORY;
	if( !bCountHidden) dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	sprintf( path, "%s\\*", szDir);
	HANDLE hFind = FindFirstFile( path, &fd);
        AnsiString noext, sinext;
        
	if(hFind != INVALID_HANDLE_VALUE)
	{
		int count = 0;
		do
		{
			if( !(fd.dwFileAttributes & dwAttr))
                          {
				puts( fd.cFileName);

                              // TWORZENIE LISTY SCENERII ^^^^^^^^^^^^^^^^^^^^^^
                              if (ext == "txt")
                              if (ExtractFileExt(fd.cFileName) == ".txt")
                                  {
                                   noext = fd.cFileName;
                                   sinext = noext.SubString(1, noext.Length()-4);
                                   SL.Add( fd.cFileName );
                                 //WriteLog( sinext );
                                  }

                           }
		}while( FindNextFile( hFind, &fd));
		FindClose( hFind);
		return count;
	}
	return -1;
}


// ***********************************************************************************************************
// Q 030116: Wczytywanie informacji o stacjach
// ***********************************************************************************************************
AnsiString Global::LoadStationsBase()
{
 WriteLog("");
 WriteLog("Loading station base...");
 int fn = 0;
 int tn = -1;
 int pos1 = 0;
 int pos2 = 0;
 bool trackinfo = false;
 TStringList *slFiles = new TStringList;
 TStringList *slFile = new TStringList;
 std::string line;
 AnsiString test, par1, par2;
 slFiles->Clear();

 listdir("stations\\", false, "txt", *slFiles);

 fn = slFiles->Count;

 for (int i = 0; i < fn; i++)   // LISTA PLIKOW ...
      {
       WriteLog("Retrieving station info from " + slFiles->Strings[i]);
       slFile->Clear();
       slFile->LoadFromFile(QGlobal::asAPPDIR + "stations\\" + slFiles->Strings[i]);
       tn = -1;
       
       for (int j = 0; j< slFile->Count-1; j++)  // PARSOWANIE PLIKU ...
          {
           line = slFile->Strings[j].c_str();
           pos1 = line.find("=");
           pos2 = line.find("=");
           test = Trim(line.substr(0, pos1).c_str());

           par1 = Trim(line.substr(pos2 + 1, 255).c_str());     

          // if (test != "") WriteLog("[" +test+ "][" + par1 + "]");

           if (test == "name") QGlobal::station[i].Name = Trim(par1.LowerCase());
           if (test == "info") QGlobal::station[i].Info = Trim(par1);
           if (test == "type") QGlobal::station[i].Type = Trim(par1);                     // junction JS, suburban SS,
           if (test == "subtype") QGlobal::station[i].SubType = Trim(par1);               // P - passenger, F - freight
           if (test == "platforms") QGlobal::station[i].platforms = par1.ToInt();         // ilosc peronow
           if (test == "platformedges") QGlobal::station[i].platformedges = par1.ToInt(); // ilosc krawedzi peronowych
           if (test == "tracksnum") QGlobal::station[i].tracksnum = par1.ToInt();         // ogolna liczba torow na stacji


           // czytanie wlasnosci torow
           if (QGlobal::station[i].tracksnum > 0)
               {
                if (test.Pos("{") > 0) tn++;
              //if (test.Pos("{") > 0) WriteLog("TAB=" + IntToStr(tn));
                if (test == "len") QGlobal::station[i].trackinfo[tn].len = StrToInt(par1);      // dlugosc pomiedzy semaforami wyjazdowymi
                if (test == "number") QGlobal::station[i].trackinfo[tn].number = par1.c_str();     // kolejowa numeracja torow - parzyste po lewej, nieparzyste po prawej  (w kierunku rosnacym kilometrarza)
                if (test == "platformav") QGlobal::station[i].trackinfo[tn].platformav = par1.c_str();   // lr, l, r, none
                if (test == "platformlen") QGlobal::station[i].trackinfo[tn].platformlen = StrToInt(par1);  // dlugosc peronu
                if (test == "electrified") QGlobal::station[i].trackinfo[tn].electrified = StrToInt(par1);    // 1 - zelektryfikowany, 0 - nie,

                trackinfo = false;
               }
          }
      }
}


// ***********************************************************************************************************
// Q 03.01.16: Po pobraniu nazwy stacji z toru wypadaloby wyszukac informacji o niej w bazie
// ***********************************************************************************************************
int Global::findstationbyname(AnsiString name)
{
 int ipos = 0;
 for (int i = 0; i<50; i++)   // LISTA STACJI ...
      {
       ipos = i;
       //WriteLog(LowerCase(QGlobal::station[i].Name) + " : " + LowerCase(name));
       if (LowerCase(QGlobal::station[ipos].Name) == LowerCase(name)) return ipos;
      }
 return -1;
}


// ***********************************************************************************************************
// USTAWIANIE PASAZEROWI NUMERU POCIAGU I MIEJSCA DOCELOWEGO
// ***********************************************************************************************************
int Global::setpassengerdest(AnsiString train, AnsiString station)   // Wywolywane z parser.cpp
{
 train = Trim(train);
 station = Trim(station);
 WriteLog("relation: " + train + ":" + station + " ");
 return 0;
}


// ***********************************************************************************************************
// Wolane z ground.cpp w TGroundNode::RenderDL() 
// ***********************************************************************************************************
int Global::findpassengerdynamic(vector3 PPos, AnsiString asName, AnsiString Prel, AnsiString DST, TGroundNode *GN)
{
 AnsiString Drel, Ddst, Dnam;
 vector3 dpnt, vstart, vend, dnorm;
 TDynamicObject *DO;
 TGroundNode *pdyn;
 float elapsed = 0.003f;
 float distance = 0;
 float distance2 = 0;
 float walkdelay = 0;
 int PEPSIZE = MAXPASSENGERENTYPOINTS;

 if (Global::iPause == 0)        
 for (int l = 0; l < PEPSIZE - 1; l++)    // Jedziemy po tablicy entrypointow...
     {
      Drel = QGlobal::PEP[l].dyntrainnumber;
      Ddst = QGlobal::PEP[l].dyndestination;
      dpnt = QGlobal::PEP[l].point;
      Dnam = QGlobal::PEP[l].dynname;

      float fSquareDist = SquareMagnitude(dpnt - GN->pCenter);                  // porownywanie pozycji aktualnego posera z pozycjami wszystkich pojazdow

      if ((!GN->bINTRAIN) && (fSquareDist < 6000))                              // Jezeli odleglosc pasazera do pojadu mniejsza niz 60 metrow?
        {
          if (Prel == Drel)                                                     // Gdy relacja pasazera zgadza sie z numerem pociagu...
           {
           // Sortowanie punktow wejsciowych aby najblizszy pasazerowi byl na pierwszym miejscu listy
            for (int j = 0; j < PEPSIZE - 1; j++)
             {
              QGlobal::array[j].num1 = SquareMagnitude(QGlobal::PEP[j].point - GN->pCenter);
              QGlobal::array[j].num2 = QGlobal::PEP[j].point;
             }
            a::sort_by = 1;
            std::sort(QGlobal::array, QGlobal::array + PEPSIZE);                // sortowanie po odleglosci pomiedzy pasazerem a punktem wejscia

            pdyn = Global::pGround->DynamicFindAny(Dnam);                       // znajdz wskaznik na pojazd znajac nazwe z tablicy entrypointow
            if (!GN->bINTRAIN && pdyn != NULL) DO = pdyn->DynamicObject;

            if (DO->MoverParameters->Vel < 3.0) GN->fPassengerCDelay += 0.004;  // Gdy sklad hamuje, juz moga zaczyanc myslec o podchodzeniu, niektorym to ciezko idzie ;)

            if (DO->MoverParameters->Vel < 0.2)   
            if (GN->fPassengerCDelay >= GN->fPassengerDDelay)                   // Gdy juz pomyslal (czas myslenia jest randomowy), zaczyna isc do drzwi
            {
             dpnt = QGlobal::array[0].num2;                                     // pierwszy item zawsze jest najmniejsza wartoscia (najblizsze drzwi)
             distance = SquareMagnitude(dpnt - vector3(PPos));                  // dystans do przebycia
             vector3 direction = Normalize(dpnt - vector3(PPos));
             GN->pCenter += (direction * GN->fPassengerSpeed) * elapsed;        // aktualizacja pozycji

             distance2 = SquareMagnitude(dpnt - GN->pCenter);                   // dystans pomiedzy pasazerem a drzwiami
            }


            // Wejscie pasazera do wagonu
            if (DO->MoverParameters->Vel < 0.2)                                 // ...GDY PREDKOSC MNIEJSZA OD 0.1km/h
            if ((distance2 > 0) && (distance2 < 0.3))                           // jezeli dystans pomiedzy pasazerem a drzwiami wiekszy od 0 i mniejszy niz 30cm...
             {
              if (!GN->bINTRAIN && pdyn != NULL)                                // JEZELI JESCZE NIE W POJEZDZIE TO...
                 {
                    {
                     DO->MoverParameters->Mass += 80;                           // zwiekszenie masy wagonu o 80kg (random cos nie dziala) //getRandomMinMax( 60.0f, 100.0f );    // Zwiekszyc wage wagonu o wage pasazera
                     GN->bINTRAIN = true;                                       // Wlazi do pociagu
                    }
                  }
              Global::pWorld->Controlled->GetConsist_f(1, Global::pWorld->Controlled); // odswiezenie danych na liscie skladu
             }

             //glEnable(GL_LINE_SMOOTH);
             //glLineWidth(1.62);
             //glColor3ub(150,10,10);
             //glBegin(GL_LINES);
             //glVertex3f(dpnt.x, dpnt.y+0.2, dpnt.z);
             //glVertex3f(PPos.x, PPos.y+0.2, PPos.z);
             //glEnd();
             //WriteLog("POSER " + asName + ": " + REL + ", " + DST + ", train: " + drel + " wagon: " + dnam);
           }
          // Global::pWorld->Controlled->GetConsist_f(1, Global::pWorld->Controlled);
        }
     }
 return 0;
}


// ***********************************************************************************************************
//
// ***********************************************************************************************************
std::string Global::GetKbdLayout()
{
 char kbdlayout[100];
 std::string skbdlayout, kl;
 HWND hWnd = QGlobal::glHWND;
 DWORD procid = GetWindowThreadProcessId (hWnd, NULL);
 HKL kstate = GetKeyboardLayout (procid);

 sprintf(kbdlayout, "%02X", LOWORD (kstate));
 kl = kbdlayout;

 QGlobal::asKBDLAYOUT = kl.c_str();

 if (kl == "401") QGlobal::asKBDLAYOUTSTR = "Arabic";
 if (kl == "402") QGlobal::asKBDLAYOUTSTR = "Bulgarian";
 if (kl == "403") QGlobal::asKBDLAYOUTSTR = "Catalan";
 if (kl == "404") QGlobal::asKBDLAYOUTSTR = "Chinese (PRC)";
 if (kl == "405") QGlobal::asKBDLAYOUTSTR = "Czech";
 if (kl == "406") QGlobal::asKBDLAYOUTSTR = "Danish";
 if (kl == "413") QGlobal::asKBDLAYOUTSTR = "Dutch";
 if (kl == "409") QGlobal::asKBDLAYOUTSTR = "English";
 if (kl == "40B") QGlobal::asKBDLAYOUTSTR = "Finnish";
 if (kl == "40C") QGlobal::asKBDLAYOUTSTR = "French";
 if (kl == "407") QGlobal::asKBDLAYOUTSTR = "German";
 if (kl == "408") QGlobal::asKBDLAYOUTSTR = "Greek";
 if (kl == "40D") QGlobal::asKBDLAYOUTSTR = "Hebrew";
 if (kl == "40E") QGlobal::asKBDLAYOUTSTR = "Hungarian";
 if (kl == "410") QGlobal::asKBDLAYOUTSTR = "Italian";
 if (kl == "411") QGlobal::asKBDLAYOUTSTR = "Japanese";
 if (kl == "412") QGlobal::asKBDLAYOUTSTR = "Korean";
 if (kl == "427") QGlobal::asKBDLAYOUTSTR = "Lithuanian";
 if (kl == "414") QGlobal::asKBDLAYOUTSTR = "Norwegian";
 if (kl == "816") QGlobal::asKBDLAYOUTSTR = "Portugese";
 if (kl == "419") QGlobal::asKBDLAYOUTSTR = "Russian";
 if (kl == "41B") QGlobal::asKBDLAYOUTSTR = "Slovak";
 if (kl == "40A") QGlobal::asKBDLAYOUTSTR = "Spanish";
 if (kl == "41D") QGlobal::asKBDLAYOUTSTR = "Swedish";
 if (kl == "41E") QGlobal::asKBDLAYOUTSTR = "Thai";
 if (kl == "41F") QGlobal::asKBDLAYOUTSTR = "Turkish";
 if (kl == "422") QGlobal::asKBDLAYOUTSTR = "Ukrainian";
 if (kl == "415") QGlobal::asKBDLAYOUTSTR = "Polish";
 return skbdlayout;
}


// ***********************************************************************************************************
// WYSYLANIE PLIKU LOG.TXT NA SERVER FTP
// ***********************************************************************************************************
void Global::SENDLOGTOFTP(AnsiString DATE)
{
    CopyFile("log.txt", "templog.txt", false);
    Sleep(50);

    std::string ftppassword;

    ftppassword = encryptDecrypt(X2985Z457);

    char ftp[]      = "lisek.org.pl";

    char user[]     = "queued_q";

    char password[] = "********";

    char localFile[] = "templog.txt";

    char remoteFile[] = "/nazwaplikunaserwerze.txt";

    std::string rf = AnsiString("log-" + DATE + ".txt").c_str();

    sprintf(remoteFile, "%s", stdstrtocharc(rf));

    HINTERNET hInternet;

    HINTERNET hFtpSession;

    if(InternetAttemptConnect(0) == ERROR_SUCCESS) WriteLog("FTP: internet ok, sending log.txt...");
     else WriteLog("FTP: Internet blocked for this app");


    hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL,0);

    if(hInternet != NULL){

        hFtpSession = InternetConnect(hInternet, ftp, INTERNET_DEFAULT_FTP_PORT, user, ftppassword.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);

        if(hFtpSession != NULL)
        {

            if (!FtpCreateDirectory(hFtpSession, QGlobal::USERPID.c_str()))
               {
                WriteLog("FTP: creating directory error. Code:" );
               }

            if (!FtpSetCurrentDirectory(hFtpSession, QGlobal::USERPID.c_str()))
                {
                 WriteLog("FTP: irectory changing error. Code:" );
                }

            if(FtpPutFile(hFtpSession, localFile, remoteFile , FTP_TRANSFER_TYPE_BINARY,0)){

                InternetCloseHandle(hFtpSession);

                InternetCloseHandle(hInternet);

                }
            else {
                WriteLog("FTP: Error during log upload");
              //  return -1;
            }


        }

      //  else return -1;
    }

   // else  return -1;

    WriteLog("FTP: Wyslano Plik.");

  //  return 0;
};


// ***********************************************************************************************************
// SPRAWDZANIE POLACZENIA Z INTERNETEM
// ***********************************************************************************************************
bool Global::CHECKINTERNET()
{
/*
  bool ret = false;
  WSADATA wsaData;
  SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  SOCKADDR_IN SockAddr;
   try
    {
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
       // cout << "WSAStartup failed.\n";
    }

    struct hostent *host;
    host = gethostbyname("google.com");

    SockAddr.sin_port = htons(80);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    //cout << "Connecting...\n";
    }
    __finally
    {
     if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0)
      {
      //  cout << "Could not connect";
        return false;
      }
    }
  //  cout << "Connected.\n";

 return true;
 */

char url[128];
strcat(url, "http://www.google.com");
bool bConnect = InternetCheckConnection(url, FLAG_ICC_FORCE_CONNECTION, 0);

if (bConnect)
  {
    return true;
  }
 else
  {
   return false;
  }
}


// ***********************************************************************************************************
// DZIELENIE LANCUCHA ZNAKOW NA LINIE
// ***********************************************************************************************************
void Global::divideline(AnsiString line, TStringList *sl, int ll)
{
 AnsiString tmp;
 int linelen, linesout;

 linelen = line.Length();

 linesout = linelen % ll;

 for (int i = 1; i < linesout+1; i++)
  {
    tmp = line.SubString(1, ll);
     line.Delete(1, ll);

     sl->Add(tmp);
  }
}


// ***********************************************************************************************************
// WCZYTYWANIE OPISU MISJI Z PLIKU SCENERII
// ***********************************************************************************************************
void Global::LOADMISSIONDESCRIPTION()
{
    WriteLog("Loading mission description...");
    AnsiString asfile, line, xtest, isd;
    AnsiString cscn = Global::szSceneryFile;

    asfile = QGlobal::asAPPDIR + "scenery\\" + cscn;

    if (!FileExists(asfile)) return;

    QGlobal::SLTEMP->LoadFromFile(asfile);

    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "³", "l", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "ê", "e", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "¹", "a", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "³", "l", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "æ", "c", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "œ", "s", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "ó", "o", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "¿", "z", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "ñ", "n", TReplaceFlags() << rfReplaceAll );
    QGlobal::SLTEMP->Text= StringReplace( QGlobal::SLTEMP->Text, "Ÿ", "z", TReplaceFlags() << rfReplaceAll );
    
    bool descr = false;
    for (int i= 0; i < QGlobal::SLTEMP->Count-1; i++)
     {
      descr = false;
      xtest = QGlobal::SLTEMP->Strings[i];

      isd = xtest.SubString(1,5);

      if (isd.SubString(1,4) == "//$d") descr = true; //WriteLog("^isdescript");;
      if (isd.Pos("d") && isd.Pos("/") && descr)
       {
       xtest = QGlobal::SLTEMP->Strings[i];
        line = xtest.SubString(5,1024);

        if (line.Length() >= 110) Global::divideline(line, QGlobal::MISSIO, 110);
         else  QGlobal::MISSIO->Add(line);
       }
     }
 WriteLog("OK.");
}

#pragma package(smart_init)
