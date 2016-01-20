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

    TDynamicObject *Controlled; // pojazd, który prowadzimy

    bool __fastcall menuinitctrls();
    bool __fastcall STARTSIMULATION();
    bool __fastcall LOADLOADERFONTS();
    bool __fastcall LOADLOADERCONFIG();
    bool __fastcall LOADLOADERTEXTURES();
    bool __fastcall LOADQCONFIG();
    bool __fastcall RenderFPS(double alpha);
    bool __fastcall RenderFILTER(double alpha);
    bool __fastcall RenderMOUSE(double alpha);
    bool __fastcall RenderSPLASHSCR(HDC hDC, int node, AnsiString text, double alpha);
    bool __fastcall RenderEXITQUERY(double alpha);
    bool __fastcall RenderINFOPANEL(int num, double alpha);
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
    bool __fastcall RenderLoader(HDC hDC, int node, AnsiString text);
    bool __fastcall RenderLoaderU(HDC hDC, int node, AnsiString text);
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
    void ShowHints();
    bool Render();
    bool RenderCab(bool colormode);
    TCamera Camera;
    TGround Ground;
    TTrain *Train;
    TDynamicObject *pDynamicNearest;
    bool Paused;
    GLuint base; // numer DL dla znaków w napisach
    GLuint light; // numer tekstury dla smugi
    TSky Clouds;
    TEvent *KeyEvents[10]; // eventy wyzwalane z klawiaury
    TMoverParameters *mvControlled; // wskaŸnik na cz³on silnikowy, do wyœwietlania jego parametrów
    int iCheckFPS; // kiedy znów sprawdziæ FPS, ¿eby wy³¹czaæ optymalizacji od razu do zera
    double fTime50Hz; // bufor czasu dla komunikacji z PoKeys
    double fTimeBuffer; // bufor czasu aktualizacji dla sta³ego kroku fizyki
    double fMaxDt; //[s] krok czasowy fizyki (0.01 dla normalnych warunków)
    int iPause; // wykrywanie zmian w zapauzowaniu
  public:
    void ModifyTGA(const AnsiString &dir = "");
    void CreateE3D(const AnsiString &dir = "", bool dyn = false);
    void CabChange(TDynamicObject *old, TDynamicObject *now);
};
//---------------------------------------------------------------------------
#endif
