/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef WorldH
#define WorldH

#include "Usefull.h"
#include "Classes.h"
#include "Texture.h"
#include "Camera.h"
#include "Ground.h"
#include "MdlMngr.h"
#include "Globals.h"
#include "sky.h"
#include "screen.h"
#include "freetype.h"

//#include <winuser.h>

class TWorld
{
    void InOutKey();
    void FollowView(bool wycisz = true);
    void DistantView();

  public:
    bool Init(HWND NhWnd, HDC hDC);
    bool Load(HWND NhWnd, HDC hDC);
    HWND hWnd;
    GLvoid glPrint(const char *fmt);
    void OnKeyDown(int cKey);
    void OnKeyUp(int cKey);
    void OnSysKeyUp(int cKey);
    // void UpdateWindow();
    void OnMouseMove(double x, double y);
    void OnMouseWheel(int zDelta);
    void OnMouseLpush(double x, double y);
    void OnMouseRpush(double x, double y);
    void OnMouseMpush(double x, double y);
    void OnCommandGet(DaneRozkaz *pRozkaz);
    bool Update();
    void TrainDelete(TDynamicObject *d = NULL);
    TWorld();
    ~TWorld();
    TSCREEN *SCR;
    static GLuint loaderbackg;
    static GLuint loaderbrief;
    static GLuint loaderlogo;
    static GLuint bfonttex;
    static GLuint consolebackg;
    freetype::font_data font10;
    freetype::font_data font11;
    freetype::font_data font12;
    freetype::font_data font14;
    freetype::font_data font16;
    freetype::font_data font18;
    freetype::font_data font20;
    // double Aspect;

    TDynamicObject *Controlled; // pojazd, kt�ry prowadzimy

    bool __fastcall menuinitctrls();
    bool __fastcall STARTSIMULATION();
    bool __fastcall LOADLOADERFONTS();
    bool __fastcall LOADLOADERCONFIG();
    bool __fastcall LOADLOADERTEXTURES();
    bool __fastcall LOADQCONFIG();
    bool __fastcall RenderFPS(double alpha);
    bool __fastcall RenderFILTER(double alpha);
    bool __fastcall RenderMOUSE(double alpha);
    void __fastcall RenderSPLASHSCR(HDC hDC, int node, AnsiString text, double alpha);
    bool __fastcall RenderEXITQUERY(double alpha);
    bool __fastcall RenderWATERMARK(double alpha);
    bool __fastcall RenderINFOPANEL(int num, double alpha);
    bool __fastcall RenderINFOPANELB(int x, int y, int w, int h, float r, float a, AnsiString title);
    bool __fastcall RenderConsole(double speed, double dt);
    bool __fastcall RenderConsoleText();
    bool __fastcall RenderIRCEU07Text();
    bool __fastcall RenderDOC();
    bool __fastcall RenderSplash(HDC hDC);
    bool __fastcall RenderMenu(HDC hDC);
    bool __fastcall RenderMenuCheckBox(int w, int h, int x, int y, int ident, bool check, bool selected, AnsiString label);
    bool __fastcall RenderMenuInputBox(int w, int h, int x, int y, int ident, bool selected, AnsiString label);
    bool __fastcall RenderMenuButton(int w, int h, int x, int y, int ident, bool selected, AnsiString label);
    bool __fastcall RenderMenuPanel1(int w, int h, int x, int y, int ident, bool selected, AnsiString label, AnsiString backg);
    bool __fastcall RenderMenuListBox(int w, int h, int x, int y, int ident, bool selected, int items, AnsiString label);
    bool __fastcall RenderMenuListBoxItem(int w, int h, int x, int y, int ident, int selid, bool selected, int item, AnsiString label);
    void __fastcall RenderLoader(HDC hDC, int node, AnsiString text);
    void __fastcall RenderLoaderU(HDC hDC, int node, AnsiString text);
    bool __fastcall RenderFadeOff(int percent);
    bool __fastcall RenderProjTex(float zpos);
    bool __fastcall RenderINFOX(int node);
    bool __fastcall RenderELEMENTINFO(int node, AnsiString KEY);
    bool __fastcall RenderELEMENTDESC(int elementid);
    bool __fastcall RenderMCURSOR(int type);
    bool __fastcall RenderInformation(int type);
    bool __fastcall RenderTUTORIAL(int type);
    bool __fastcall RenderFPS();
    bool __fastcall renderpanview(float trans, int frameheight, int pans);
    bool __fastcall renderfadeoff(float trans);
    bool __fastcall renderhitcolor(int r, int g, int b, int a);
    bool __fastcall rendercompass(float trans, int size, double angle);
    bool __fastcall rendertext(int x, int y, double scale, AnsiString astext);
    bool __fastcall renderpointerx(double sizepx, int sw, int sh);
    bool __fastcall RenderRain(float rf, float s, int d, int m);
    bool __fastcall RenderSnow();
    bool __fastcall SnowInit();
    bool __fastcall setBR(int x, int y, int w, int h);
  private:
    AnsiString OutText01; // teksty na ekranie
    AnsiString OutText02;
    AnsiString OutText03;
    AnsiString OutText04;
    AnsiString OutText05;
    AnsiString OutText06;
    AnsiString OutText07;
    AnsiString OutText08;
    AnsiString OutText09;
    AnsiString OutText10;
    AnsiString OutText11;
    AnsiString OutText12;

int BRx;
int BRy;
int BRw;
int BRh;



float LDR_COLOR_R;
float LDR_COLOR_G;
float LDR_COLOR_B;
float LDR_STR_1_R;
float LDR_STR_1_G;
float LDR_STR_1_B;
float LDR_STR_1_A;
float LDR_TBACK_R;
float LDR_TBACK_G;
float LDR_TBACK_B;
float LDR_TBACK_A;
float LDR_PBARLEN;
float LDR_PBAR__R;
float LDR_PBAR__G;
float LDR_PBAR__B;
float LDR_PBAR__A;
float LDR_FINIT_R;
float LDR_FINIT_G;
float LDR_FINIT_B;
float LDR_FINIT_A;
float LDR_LOGOVIS;
float LDR_MLOGO_X;
float LDR_MLOGO_Y;
float LDR_MLOGO_A;
float LDR_DESCVIS;
float LDR_BRIEF_X;
float LDR_BRIEF_Y;
AnsiString LDR_STR_LOAD;
AnsiString LDR_STR_FRST;

 
    void ShowHints();
    bool Render();
    bool RenderCab(bool colormode);
    TCamera Camera;
    TGround Ground;
    TTrain *Train;
    TDynamicObject *pDynamicNearest;
    bool Paused;
    GLuint base; // numer DL dla znak�w w napisach
    GLuint light; // numer tekstury dla smugi
    TSky Clouds;
    TEvent *KeyEvents[10]; // eventy wyzwalane z klawiaury
    TMoverParameters *mvControlled; // wska�nik na cz�on silnikowy, do wy�wietlania jego parametr�w
    int iCheckFPS; // kiedy zn�w sprawdzi� FPS, �eby wy��cza� optymalizacji od razu do zera
    double fTime50Hz; // bufor czasu dla komunikacji z PoKeys
    double fTimeBuffer; // bufor czasu aktualizacji dla sta�ego kroku fizyki
    double fMaxDt; //[s] krok czasowy fizyki (0.01 dla normalnych warunk�w)
    int iPause; // wykrywanie zmian w zapauzowaniu
  public:
    void ModifyTGA(const AnsiString &dir = "");
    void CreateE3D(const AnsiString &dir = "", bool dyn = false);
    void CabChange(TDynamicObject *old, TDynamicObject *now);
};
//---------------------------------------------------------------------------
#endif
