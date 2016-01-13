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

#include "opengl/glew.h"
#include "opengl/glut.h"
#pragma hdrstop

#include "Timer.h"
#include "mtable.hpp"
#include "Sound.h"
#include "World.h"
#include "Logs.h"
#include "Globals.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "Event.h"
#include "Train.h"
#include "Driver.h"
#include "Console.h"
#include "qutils.h"
#include "screen.h"
#include "freetype.h"		// Header for our little font library.
#include "effects2d.h"

#define TEXTURE_FILTER_CONTROL_EXT 0x8500
#define TEXTURE_LOD_BIAS_EXT 0x8501
//---------------------------------------------------------------------------
#pragma package(smart_init)

typedef void(APIENTRY *FglutBitmapCharacter)(void *font, int character); // typ funkcji
FglutBitmapCharacter glutBitmapCharacterDLL = NULL; // deklaracja zmiennej
HINSTANCE hinstGLUT32 = NULL; // wskaŸnik do GLUT32.DLL
// GLUTAPI void APIENTRY glutBitmapCharacterDLL(void *font, int character);
//TDynamicObject *Controlled = NULL; // pojazd, który prowadzimy     // Q: przenosze do klasy TWorld

using namespace Timer;

const double fTimeMax = 1.00; //[s] maksymalny czas aktualizacji w jednek klatce

GLuint TWorld::loaderbackg;
GLuint TWorld::loaderbrief;
GLuint TWorld::loaderlogo;
GLuint TWorld::bfonttex;
GLuint TWorld::consolebackg;

TGroundNode *tmp;
TDynamicObject *DO;





bool __fastcall TWorld::STARTSIMULATION()
{
   TSCREEN::CFOV = 45;
   QGlobal::bSHOWBRIEFING = FALSE;
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(TSCREEN::CFOV, (GLdouble)Global::iWindowWidth/(GLdouble)Global::iWindowHeight, 0.1f, 13234566.0f);  //1999950600.0f
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity( );
   Camera.Reset();
   Global::iPause = false;
   loaderbrief = NULL;      // USUNIECIE TEKSTURY
   loaderbackg = NULL;
   QGlobal::splashscreen = NULL;
   QGlobal::bSIMSTARTED = true;
}


// *****************************************************************************
// Konstruktor klasy TWorld
// *****************************************************************************
TWorld::TWorld()
{
    Train = NULL;
    // Aspect=1;
    for (int i = 0; i < 10; ++i)
        KeyEvents[i] = NULL; // eventy wyzwalane klawiszami cyfrowymi
    Global::iSlowMotion = 0;
    // Global::changeDynObj=NULL;
    OutText01 = ""; // teksty wyœwietlane na ekranie
    OutText02 = "";
    OutText03 = "";
    OutText04 = "";
    OutText05 = "";
    OutText06 = "";
    OutText07 = "";
    OutText08 = "";
    OutText09 = "";
    OutText10 = "";
    OutText11 = "";
    OutText12 = "";
    iCheckFPS = 0; // kiedy znów sprawdziæ FPS, ¿eby wy³¹czaæ optymalizacji od razu do zera
    pDynamicNearest = NULL;
    fTimeBuffer = 0.0; // bufor czasu aktualizacji dla sta³ego kroku fizyki
    fMaxDt = 0.01; //[s] pocz¹tkowy krok czasowy fizyki
    fTime50Hz = 0.0; // bufor czasu dla komunikacji z PoKeys

    QGlobal::bfonttex = NULL;
    bfonttex = NULL;
    loaderbackg = NULL;
    loaderbrief = NULL;
    consolebackg = NULL;

    Controlled = NULL; // pojazd, który prowadzimy

    QGlobal::CONSISTF = new TStringList;
    QGlobal::CONSISTB = new TStringList;
    QGlobal::CONSISTA = new TStringList;
}


// *****************************************************************************
// Destruktor klasy TWorld
// *****************************************************************************
TWorld::~TWorld()
{
    Global::bManageNodes = false; // Ra: wy³¹czenie wyrejestrowania, bo siê sypie
    TrainDelete();
    // Ground.Free(); //Ra: usuniêcie obiektów przed usuniêciem dŸwiêków - sypie siê
    TSoundsManager::Free();
    TModelsManager::Free();
    TTexturesManager::Free();
    glDeleteLists(base, 96);
    if (hinstGLUT32)
        FreeLibrary(hinstGLUT32);
}


// *****************************************************************************
// USUWANIE POJAZDU
// *****************************************************************************
void TWorld::TrainDelete(TDynamicObject *d)
{ // usuniêcie pojazdu prowadzonego przez u¿ytkownika
    if (d)
        if (Train)
            if (Train->Dynamic() != d)
                return; // nie tego usuwaæ
    delete Train; // i nie ma czym sterowaæ
    Train = NULL;
    Controlled = NULL; // tego te¿ ju¿ nie ma
    mvControlled = NULL;
    Global::pUserDynamic = NULL; // tego te¿ nie ma
};


// *****************************************************************************
// FUNKCJA DO WYPISYWANIA ROZNYCH INFORMACJI W 2D
// *****************************************************************************
GLvoid TWorld::glPrint(const char *txt) // custom GL "Print" routine
{ // wypisywanie tekstu 2D na ekranie
    if (!txt)
        return;
    if (Global::bGlutFont)
    { // tekst generowany przez GLUT
        int i, len = strlen(txt);
        for (i = 0; i < len; i++)
            glutBitmapCharacterDLL(GLUT_BITMAP_8_BY_13, txt[i]); // funkcja linkowana dynamicznie
    }
    else
    { // generowanie przez Display Lists
        glPushAttrib(GL_LIST_BIT); // pushes the display list bits
        glListBase(base - 32); // sets the base character to 32
        glCallLists(strlen(txt), GL_UNSIGNED_BYTE, txt); // draws the display list text
        glPopAttrib(); // pops the display list bits
    }
}

/* Ra: do opracowania: wybor karty graficznej ~Intel gdy s¹ dwie...
BOOL GetDisplayMonitorInfo(int nDeviceIndex, LPSTR lpszMonitorInfo)
{
    FARPROC EnumDisplayDevices;
    HINSTANCE  hInstUser32;
    DISPLAY_DEVICE DispDev;
    char szSaveDeviceName[33];  // 32 + 1 for the null-terminator
    BOOL bRet = TRUE;
        HRESULT hr;

    hInstUser32 = LoadLibrary("c:\\windows\User32.DLL");
    if (!hInstUser32) return FALSE;

    // Get the address of the EnumDisplayDevices function
    EnumDisplayDevices = (FARPROC)GetProcAddress(hInstUser32,"EnumDisplayDevicesA");
    if (!EnumDisplayDevices) {
        FreeLibrary(hInstUser32);
        return FALSE;
    }

    ZeroMemory(&DispDev, sizeof(DispDev));
    DispDev.cb = sizeof(DispDev);

    // After the first call to EnumDisplayDevices,
    // DispDev.DeviceString is the adapter name
    if (EnumDisplayDevices(NULL, nDeviceIndex, &DispDev, 0))
        {
                hr = StringCchCopy(szSaveDeviceName, 33, DispDev.DeviceName);
                if (FAILED(hr))
                {
                // TODO: write error handler
                }

        // After second call, DispDev.DeviceString is the
        // monitor name for that device
        EnumDisplayDevices(szSaveDeviceName, 0, &DispDev, 0);

                // In the following, lpszMonitorInfo must be 128 + 1 for
                // the null-terminator.
                hr = StringCchCopy(lpszMonitorInfo, 129, DispDev.DeviceString);
                if (FAILED(hr))
                {
                // TODO: write error handler
                }

    } else    {
        bRet = FALSE;
    }

    FreeLibrary(hInstUser32);

    return bRet;
}
*/


// **********************************************************************************************************
// INICJALIZACJA USTAWIEN OpenGL, WCZYTYWANIE SCENERII - FUNKCJA WYWOLYWANA W EU07.CPP W int InitGL(GLvoid)
// **********************************************************************************************************
bool TWorld::Init(HWND NhWnd, HDC hDC)
{
 //WriteLog("USTAWIANIE KATALOGU DLA ZRZUTOW EKRANU...");
    CreateDir(QGlobal::asAPPDIR + QGlobal::asSSHOTDIR);
    CreateDir(QGlobal::asAPPDIR + QGlobal::asSSHOTDIR + QGlobal::asSSHOTSUB);         // SCREENSHOTS DIRECTORY CONTAINER
    CreateDir(QGlobal::asAPPDIR + "data\\");
    CreateDir(QGlobal::asAPPDIR + "data\\logs\\");

    QGlobal::gtc1 =  GetTickCount();
    QGlobal::iRANDTABPOS = 0;
    QGlobal::iNODES = 0;
    QGlobal::postep = 0;
    QGlobal::bSCNLOADED = false;
    QGlobal::bfirstloadingscn = true;
    QGlobal::bKBDREVERSED = false;
    ShowWindow(NhWnd,SW_SHOW);
    SetForegroundWindow(NhWnd);                                                    // slightly higher priority
    SetFocus(NhWnd);

    LOADLOADERFONTS();
    LOADLOADERCONFIG();
    LOADLOADERTEXTURES();

    if (QGlobal::bSPLASHSCR)
    QGlobal::splashscreen = TTexturesManager::GetTextureID("data/lbacks/", Global::asCurrentTexturePath.c_str(), AnsiString("data/lbacks/splashscreen" + QGlobal::asLBACKEXT).c_str());
    QGlobal::mousepoint = TTexturesManager::GetTextureID("data/menu/", Global::asCurrentTexturePath.c_str(), AnsiString("data/menu/menu_point.bmp").c_str());
    QGlobal::mousesymbol = TTexturesManager::GetTextureID("data/menu/", Global::asCurrentTexturePath.c_str(), AnsiString("data/gfx/ismouse.bmp").c_str());

    Global::LoadStationsBase(); // Q 030116: Wczytywanie informacji o stacjach

    WriteLog("");
    WriteLog("");

    double time = (double)Now();
    Global::hWnd = NhWnd; // do WM_COPYDATA
    Global::pCamera = &Camera; // Ra: wskaŸnik potrzebny do likwidacji drgañ
    Global::detonatoryOK = true;
    WriteLog("Starting MaSzyna rail vehicle simulator.");
    WriteLog(Global::asVersion);
#if sizeof(TSubModel) != 256
    Error("Wrong sizeof(TSubModel) is " + AnsiString(sizeof(TSubModel)));
    return false;
#endif
    WriteLog("Online documentation and additional files on http://eu07.pl");
    WriteLog("Authors: Marcin_EU, McZapkie, ABu, Winger, Tolaris, nbmx_EU, OLO_EU, Bart, Quark-t, " "ShaXbee, Oli_EU, youBy, KURS90, Ra, hunter, Q and others");
    WriteLog("");
    WriteLog("Renderer: " + AnsiString((char*) glGetString(GL_RENDERER)));
    WriteLog("Vendor: " + AnsiString((char*) glGetString(GL_VENDOR)));
 
//Winger030405: sprawdzanie sterownikow
    AnsiString glver=((char*)glGetString(GL_VERSION));
    WriteLog("OpenGL Version: " + AnsiString((char*)glGetString(GL_VERSION)));
    WriteLog("");

    if ((glver == "1.5.1") || (glver == "1.5.2"))
    {
        Error("Niekompatybilna wersja openGL - dwuwymiarowy tekst nie bedzie wyswietlany!");
        WriteLog("WARNING! This OpenGL version is not fully compatible with simulator!");
        WriteLog("UWAGA! Ta wersja OpenGL nie jest w pelni kompatybilna z symulatorem!");
        Global::detonatoryOK = false;
    }
    else
        Global::detonatoryOK = true;
    // Ra: umieszczone w EU07.cpp jakoœ nie chce dzia³aæ
    while (glver.LastDelimiter(".") > glver.Pos("."))
        glver = glver.SubString(1, glver.LastDelimiter(".") - 1); // obciêcie od drugiej kropki
    double ogl;
    try
    {
        ogl = glver.ToDouble();
    }
    catch (...)
    {
        ogl = 0.0;
    }
    if (Global::fOpenGL > 0.0) // jeœli by³a wpisane maksymalna wersja w EU07.INI
    {
        if (ogl > 0.0) // zak³adaj¹c, ¿e siê odczyta³o dobrze
            if (ogl < Global::fOpenGL) // a karta oferuje ni¿sz¹ wersjê ni¿ wpisana
                Global::fOpenGL = ogl; // to przyj¹c to z karty
    }
    else if (ogl < 1.3) // sprzêtowa deompresja DDS zwykle wymaga 1.3
        Error("Missed OpenGL 1.3+ drivers!"); // b³¹d np. gdy wersja 1.1, a nie ma wpisu w EU07.INI
    Global::bOpenGL_1_5 = (Global::fOpenGL >= 1.5); // s¹ fragmentaryczne animacje VBO

    WriteLog("Supported extensions:");
    LISTGLEXTENSIONS(); // Q 261215: W QUTILS.CPP

    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");

    WriteLog(" ");

    WriteLog("CHECKING NEEDED EXTENSIONS...");

    if (!GL_ARB_shader_objects || !GLEW_ARB_vertex_shader || !GL_ARB_fragment_shader || !GLEW_ARB_shading_language_100)
     {
      WriteLog("No GLSL support (GL_ARB_shader_objects, GL_ARB_vertex_shader, GL_ARB_fragment_shader, GL_ARB_shading_language_100)");
     }
     else WriteLog("Shader language 100 supported");

    if(!GLEW_ARB_texture_non_power_of_two)
     {
      WriteLog("GL_ARB_texture_non_power_of_two not supported!");
     }

    if(!GLEW_ARB_depth_texture)
     {
      WriteLog("GLEW_ARB_depth_texture not supported!");
     }

    if(!GLEW_ARB_framebuffer_object)
     {
      WriteLog("GLEW_ARB_framebuffer_object not supported!");
     }

    if(!glMultiTexCoord2fARB && !glActiveTextureARB)
     {
      WriteLog("MultiTexturing is not supported by your hardware");
     }

    if (glewGetExtension("GL_ARB_vertex_buffer_object")) // czy jest VBO w karcie graficznej
    {
        if (AnsiString((char *)glGetString(GL_VENDOR))
                .Pos("Intel")) // wymuszenie tylko dla kart Intel
        { // karty Intel nie nadaj¹ siê do grafiki 3D, ale robimy wyj¹tek, bo to w koñcu symulator
            Global::iMultisampling =
                0; // to robi problemy na "Intel(R) HD Graphics Family" - czarny ekran
            if (Global::fOpenGL >=
                1.4) // 1.4 mia³o obs³ugê VBO, ale bez opcji modyfikacji fragmentu bufora
                Global::bUseVBO = true; // VBO w³¹czane tylko, jeœli jest obs³uga oraz nie ustawiono
            // ni¿szego numeru
        }
        if (Global::bUseVBO)
            WriteLog("Ra: The VBO is found and will be used.");
        else
            WriteLog("Ra: The VBO is found, but Display Lists are selected.");
    }
    else
    {
        WriteLog("Ra: No VBO found - Display Lists used. Graphics card too old?");
        Global::bUseVBO = false; // mo¿e byæ w³¹czone parametrem w INI
    }
    if (Global::bDecompressDDS) // jeœli sprzêtowa (domyœlnie jest false)
        WriteLog("DDS textures support at OpenGL level is disabled in INI file.");
    else
    {
        Global::bDecompressDDS =
            !glewGetExtension("GL_EXT_texture_compression_s3tc"); // czy obs³ugiwane?
        if (Global::bDecompressDDS) // czy jest obs³uga DDS w karcie graficznej
            WriteLog("DDS textures are not supported.");
        else // brak obs³ugi DDS - trzeba w³¹czyæ programow¹ dekompresjê
            WriteLog("DDS textures are supported.");
    }
    if (Global::iMultisampling)
        WriteLog("Used multisampling of " + AnsiString(Global::iMultisampling) + " samples.");
    { // ograniczenie maksymalnego rozmiaru tekstur - parametr dla skalowania tekstur
        GLint i;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
        if (i < Global::iMaxTextureSize)
            Global::iMaxTextureSize = i;
        WriteLog("Max texture size: " + AnsiString(Global::iMaxTextureSize));
    }
    /*-----------------------Render Initialization----------------------*/
    if (Global::fOpenGL >= 1.2) // poni¿sze nie dzia³a w 1.1
        glTexEnvf(TEXTURE_FILTER_CONTROL_EXT, TEXTURE_LOD_BIAS_EXT, -1);
    GLfloat FogColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer
    glLoadIdentity();
    // WriteLog("glClearColor (FogColor[0], FogColor[1], FogColor[2], 0.0); ");
    // glClearColor (1.0, 0.0, 0.0, 0.0);                  // Background Color
    // glClearColor (FogColor[0], FogColor[1], FogColor[2], 0.0);                  // Background
    // Color
    glClearColor(0.2, 0.4, 0.33, 1.0); // Background Color

    WriteLog("glFogfv(GL_FOG_COLOR, FogColor);");
    glFogfv(GL_FOG_COLOR, FogColor); // Set Fog Color

    WriteLog("glClearDepth(1.0f);  ");
    glClearDepth(1.0f); // ZBuffer Value

    //  glEnable(GL_NORMALIZE);
    //    glEnable(GL_RESCALE_NORMAL);

    //    glEnable(GL_CULL_FACE);
    WriteLog("glEnable(GL_TEXTURE_2D);");
    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
    WriteLog("glShadeModel(GL_SMOOTH);");
    glShadeModel(GL_SMOOTH); // Enable Smooth Shading
    WriteLog("glEnable(GL_DEPTH_TEST);");
    glEnable(GL_DEPTH_TEST);

    // McZapkie:261102-uruchomienie polprzezroczystosci (na razie linie) pod kierunkiem Marcina
    // if (Global::bRenderAlpha) //Ra: wywalam tê flagê
    {
        WriteLog("glEnable(GL_BLEND);");
        glEnable(GL_BLEND);
        WriteLog("glEnable(GL_ALPHA_TEST);");
        glEnable(GL_ALPHA_TEST);
        WriteLog("glAlphaFunc(GL_GREATER,0.04);");
        glAlphaFunc(GL_GREATER, 0.04);
        WriteLog("glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);");
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        WriteLog("glDepthFunc(GL_LEQUAL);");
        glDepthFunc(GL_LEQUAL);
    }
    /*
        else
        {
          WriteLog("glEnable(GL_ALPHA_TEST);");
          glEnable(GL_ALPHA_TEST);
          WriteLog("glAlphaFunc(GL_GREATER,0.5);");
          glAlphaFunc(GL_GREATER,0.5);
          WriteLog("glDepthFunc(GL_LEQUAL);");
          glDepthFunc(GL_LEQUAL);
          WriteLog("glDisable(GL_BLEND);");
          glDisable(GL_BLEND);
        }
    */
    /* zakomentowanie to co bylo kiedys mieszane
        WriteLog("glEnable(GL_ALPHA_TEST);");
        glEnable(GL_ALPHA_TEST);//glGetIntegerv()
        WriteLog("glAlphaFunc(GL_GREATER,0.5);");
    //    glAlphaFunc(GL_LESS,0.5);
        glAlphaFunc(GL_GREATER,0.5);
    //    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        WriteLog("glDepthFunc(GL_LEQUAL);");
        glDepthFunc(GL_LEQUAL);//EQUAL);
    // The Type Of Depth Testing To Do
      //  glEnable(GL_BLEND);
    //    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    */

    WriteLog("glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);");
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations

    WriteLog("glPolygonMode(GL_FRONT, GL_FILL);");
    glPolygonMode(GL_FRONT, GL_FILL);
    WriteLog("glFrontFace(GL_CCW);");
    glFrontFace(GL_CCW); // Counter clock-wise polygons face out
    WriteLog("glEnable(GL_CULL_FACE);	");
    glEnable(GL_CULL_FACE); // Cull back-facing triangles
    WriteLog("glLineWidth(1.0f);");
    glLineWidth(1.0f);
    WriteLog("glPointSize(2.0f);");
    glPointSize(2.0f);

    // ----------- LIGHTING SETUP -----------
    // Light values and coordinates

    vector3 lp = Normalize(vector3(-500, 500, 200));

    Global::lightPos[0] = lp.x;
    Global::lightPos[1] = lp.y;
    Global::lightPos[2] = lp.z;
    Global::lightPos[3] = 0.0f;

    // Ra: œwiat³a by sensowniej by³o ustawiaæ po wczytaniu scenerii

    // Ra: szcz¹tkowe œwiat³o rozproszone - ¿eby by³o cokolwiek widaæ w ciemnoœci
    WriteLog("glLightModelfv(GL_LIGHT_MODEL_AMBIENT,darkLight);");
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Global::darkLight);

    // Ra: œwiat³o 0 - g³ówne œwiat³o zewnêtrzne (S³oñce, Ksiê¿yc)
    WriteLog("glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);");
    glLightfv(GL_LIGHT0, GL_AMBIENT, Global::ambientDayLight);
    WriteLog("glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);");
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Global::diffuseDayLight);
    WriteLog("glLightfv(GL_LIGHT0,GL_SPECULAR,specularLight);");
    glLightfv(GL_LIGHT0, GL_SPECULAR, Global::specularDayLight);
    WriteLog("glLightfv(GL_LIGHT0,GL_POSITION,lightPos);");
    glLightfv(GL_LIGHT0, GL_POSITION, Global::lightPos);
    WriteLog("glEnable(GL_LIGHT0);");
    glEnable(GL_LIGHT0);

    // glColor() ma zmieniaæ kolor wybrany w glColorMaterial()
    WriteLog("glEnable(GL_COLOR_MATERIAL);");
    glEnable(GL_COLOR_MATERIAL);

    WriteLog("glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);");
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    //    WriteLog("glMaterialfv( GL_FRONT, GL_AMBIENT, whiteLight );");
    //	glMaterialfv( GL_FRONT, GL_AMBIENT, Global::whiteLight );

    WriteLog("glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, whiteLight );");
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Global::whiteLight);

    /*
        WriteLog("glMaterialfv( GL_FRONT, GL_SPECULAR, noLight );");
            glMaterialfv( GL_FRONT, GL_SPECULAR, Global::noLight );
    */

    WriteLog("glEnable(GL_LIGHTING);");
    glEnable(GL_LIGHTING);

    WriteLog("glFogi(GL_FOG_MODE, GL_LINEAR);");
    glFogi(GL_FOG_MODE, GL_LINEAR); // Fog Mode
    WriteLog("glFogfv(GL_FOG_COLOR, FogColor);");
    glFogfv(GL_FOG_COLOR, FogColor); // Set Fog Color
    //	glFogf(GL_FOG_DENSITY, 0.594f);						// How Dense Will The
    //Fog
    // Be
    //	glHint(GL_FOG_HINT, GL_NICEST);					        // Fog Hint Value
    WriteLog("glFogf(GL_FOG_START, 1000.0f);");
    glFogf(GL_FOG_START, 10.0f); // Fog Start Depth
    WriteLog("glFogf(GL_FOG_END, 2000.0f);");
    glFogf(GL_FOG_END, 200.0f); // Fog End Depth
    WriteLog("glEnable(GL_FOG);");
    glEnable(GL_FOG); // Enables GL_FOG

    // Ra: ustawienia testowe
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    WriteLog("");

    if (QGlobal::bSPLASHSCR) RenderSPLASHSCR(hDC, 77, "SS", 1);                 // Pierwsza czesc splasha (7s)
    if (QGlobal::bSPLASHSCR) RenderLoaderU(hDC, 77, "SS");                      // Zaraz po splashu stopniowe wylonienie sie z czerni ekranu wczytywania
    QGlobal::fscreenfade2 = 1;

    /*--------------------Render Initialization End---------------------*/

    WriteLog("Font init"); // pocz¹tek inicjacji fontów 2D
    if (Global::bGlutFont) // jeœli wybrano GLUT font, próbujemy zlinkowaæ GLUT32.DLL
    {
        UINT mode = SetErrorMode(SEM_NOOPENFILEERRORBOX); // aby nie wrzeszcza³, ¿e znaleŸæ nie mo¿e
        hinstGLUT32 = LoadLibrary(TEXT("GLUT32.DLL")); // get a handle to the DLL module
        SetErrorMode(mode);
        // If the handle is valid, try to get the function address.
        if (hinstGLUT32)
            glutBitmapCharacterDLL = (FglutBitmapCharacter)GetProcAddress(hinstGLUT32, "glutBitmapCharacter");
        else
            WriteLog("Missed GLUT32.DLL.");
        if (glutBitmapCharacterDLL)
            WriteLog("Used font from GLUT32.DLL.");
        else
            Global::bGlutFont = false; // nie uda³o siê, trzeba spróbowaæ na Display List
    }
    if (!Global::bGlutFont)
    { // jeœli bezGLUTowy font
        HFONT font; // Windows Font ID
        base = glGenLists(96); // storage for 96 characters
        font = CreateFont(-15, // height of font
                          0, // width of font
                          0, // angle of escapement
                          0, // orientation angle
                          FW_BOLD, // font weight
                          FALSE, // italic
                          FALSE, // underline
                          FALSE, // strikeout
                          ANSI_CHARSET, // character set identifier
                          OUT_TT_PRECIS, // output precision
                          CLIP_DEFAULT_PRECIS, // clipping precision
                          ANTIALIASED_QUALITY, // output quality
                          FF_DONTCARE | DEFAULT_PITCH, // family and pitch
                          "Courier New"); // font name
        SelectObject(hDC, font); // selects the font we want
        wglUseFontBitmapsA(hDC, 32, 96, base); // builds 96 characters starting at character 32
        WriteLog("Display Lists font used."); //+AnsiString(glGetError())
    }
    WriteLog("Font init OK"); //+AnsiString(glGetError())
    WriteLog("");

    SetForegroundWindow(hWnd);

    Timer::ResetTimers();

    hWnd = NhWnd;
    glColor4f(1.0f, 3.0f, 3.0f, 0.0f);
    //    SwapBuffers(hDC);					                // Swap Buffers (Double Buffering)
    //    glClear(GL_COLOR_BUFFER_BIT);
    //    glFlush();

    RenderLoader(hDC, 77, "SOUND INITIALIZATION...");
    WriteLog("Sound Init");

//--    glLoadIdentity();
    //    glColor4f(0.3f,0.0f,0.0f,0.0f);
//--    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*
    glTranslatef(0.0f, 0.0f, -0.50f);
    //    glRasterPos2f(-0.25f, -0.10f);
    glDisable(GL_DEPTH_TEST); // Disables depth testing
    glColor3f(3.0f, 3.0f, 3.0f);

    GLuint logo;
    logo = TTexturesManager::GetTextureID(szTexturePath, szSceneryPath, "logo", 6);
    glBindTexture(GL_TEXTURE_2D, logo); // Select our texture

    glBegin(GL_QUADS); // Drawing using triangles
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.28f, -0.22f, 0.0f); // bottom left of the texture and quad
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.28f, -0.22f, 0.0f); // bottom right of the texture and quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.28f, 0.22f, 0.0f); // top right of the texture and quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.28f, 0.22f, 0.0f); // top left of the texture and quad
    glEnd();
    //~logo; Ra: to jest bez sensu zapis
    */




    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.09f);
    //    glPrint("Uruchamianie / Initializing...");
    //    glRasterPos2f(-0.25f, -0.10f);
    //    glPrint("Dzwiek / Sound...");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)


    //WriteLog(".");
    //SetCurrentDirectory(ExtractFileDir(ParamStr(0)).c_str());
    //WriteLog("SetCurrentDirectory();");
    /*-----------------------Sound Initialization-----------------------*/
    TSoundsManager::Init(hWnd);
    //WriteLog(".");
    // TSoundsManager::LoadSounds( "" );
    /*---------------------Sound Initialization End---------------------*/
    WriteLog("Sound Init OK");
    WriteLog("");
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.11f);
    //    glPrint("OK.");
    //}
   // SwapBuffers(hDC); // Swap Buffers (Double Buffering)

    //int i;
    glColor3f(0.0f, 0.0f, 100.0f);
    glEnable(GL_LIGHTING);
    RenderLoader(hDC, 77, "TEXTUREMANAGER INITIALIZATION...");
    Paused = true;
    WriteLog("Textures init");
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.12f);
    //    glPrint("Tekstury / Textures...");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)

    TTexturesManager::Init();
    WriteLog("Textures init OK");
    WriteLog("");
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.13f);
    //    glPrint("OK.");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)

    RenderLoader(hDC, 77, "MODELMANAGER INITIALIZATION...");
    WriteLog("Models init");
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.14f);
    //    glPrint("Modele / Models...");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)
    // McZapkie: dodalem sciezke zeby mozna bylo definiowac skad brac modele ale to malo eleganckie
    //    TModelsManager::LoadModels(asModelsPatch);
    TModelsManager::Init();
    WriteLog("Models init OK");
    WriteLog("");
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.15f);
    //    glPrint("OK.");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)
    
    RenderLoader(hDC, 77, "GROUND INITIALIZATION...");
    WriteLog("Ground init");
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.16f);
    //    glPrint("Sceneria / Scenery (please wait)...");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)

    Ground.Init(Global::szSceneryFile, hDC);
    //    Global::tSinceStart= 0;
    WriteLog("Ground init OK");
    WriteLog("");

    RenderLoader(hDC, 77, "SKY INITIALIZATION...");
    WriteLog("Sky init");
    Clouds.Init();
    WriteLog("Sky init OK");
    WriteLog("");
    
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.17f);
    //    glPrint("OK.");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)

    //    TTrack *Track=Ground.FindGroundNode("train_start",TP_TRACK)->pTrack;

    //    Camera.Init(vector3(2700,10,6500),0,M_PI,0);
    //    Camera.Init(vector3(00,40,000),0,M_PI,0);
    //    Camera.Init(vector3(1500,5,-4000),0,M_PI,0);
    // McZapkie-130302 - coby nie przekompilowywac:
    //      Camera.Init(Global::pFreeCameraInit,0,M_PI,0);
    Camera.Init(Global::pFreeCameraInit[0], Global::pFreeCameraInitAngle[0]);

    RenderLoader(hDC, 77, "Player Train initialization...");
    char buff[255] = "Player train init: ";
    //if (Global::detonatoryOK)
    //{
    //    glRasterPos2f(-0.25f, -0.18f);
    //    glPrint("Przygotowanie kabiny do sterowania...");
    //}
    //SwapBuffers(hDC); // Swap Buffers (Double Buffering)

    strcat(buff, Global::asHumanCtrlVehicle.c_str());
    WriteLog(buff);
    TGroundNode *nPlayerTrain = NULL;
    if (Global::asHumanCtrlVehicle != "ghostview")
        nPlayerTrain = Ground.DynamicFind(Global::asHumanCtrlVehicle); // szukanie w tych z obsad¹
    if (nPlayerTrain)
    {
        Train = new TTrain();
        if (Train->Init(nPlayerTrain->DynamicObject))
        {
            Controlled = Train->Dynamic();
            mvControlled = Controlled->ControlledFind()->MoverParameters;
            Global::pUserDynamic = Controlled; // renerowanie pojazdu wzglêdem kabiny
            WriteLog("Player train init OK");
            RenderLoader(hDC, 77, "Player Train initialization OK.");
            Sleep(200);
            //if (Global::detonatoryOK)
            //{
            //    glRasterPos2f(-0.25f, -0.19f);
            //    glPrint("OK.");
            //}
            FollowView();
            //SwapBuffers(hDC); // Swap Buffers (Double Buffering)
        }
        else
        {
            RenderLoader(hDC, 77, "Player Train initialization FAILED!");
            Sleep(200);
            Error("Player train init failed!", false);
            FreeFlyModeFlag = true; // Ra: automatycznie w³¹czone latanie
            QGlobal::bSIMSTARTED = true;
            //if (Global::detonatoryOK)
            //{
            //    glRasterPos2f(-0.25f, -0.20f);
            //    glPrint("Blad inicjalizacji sterowanego pojazdu!");
            //}
            //SwapBuffers(hDC); // Swap Buffers (Double Buffering)
            Controlled = NULL;
            mvControlled = NULL;
            Camera.Type = tp_Free;
        }
    }
    else
    {
        if (Global::asHumanCtrlVehicle != "ghostview")
        {
          RenderLoader(hDC, 77, "Player Train NOT EXIST!");
          Sleep(200);
          Error("Player train not exist!", false);
            //if (Global::detonatoryOK)
            //{
            //    glRasterPos2f(-0.25f, -0.20f);
            //    glPrint("Wybrany pojazd nie istnieje w scenerii!");
            //}
        }
        FreeFlyModeFlag = true; // Ra: automatycznie w³¹czone latanie
        QGlobal::bSIMSTARTED = true;
        //SwapBuffers(hDC); // swap buffers (double buffering)
        Controlled = NULL;
        mvControlled = NULL;
        Camera.Type = tp_Free;
        Camera.Pos.y += 4.0f;
    }
    glEnable(GL_DEPTH_TEST);
    // Ground.pTrain=Train;
    // if (!Global::bMultiplayer) //na razie w³¹czone
    { // eventy aktywowane z klawiatury tylko dla jednego u¿ytkownika
        KeyEvents[0] = Ground.FindEvent("keyctrl00");
        KeyEvents[1] = Ground.FindEvent("keyctrl01");
        KeyEvents[2] = Ground.FindEvent("keyctrl02");
        KeyEvents[3] = Ground.FindEvent("keyctrl03");
        KeyEvents[4] = Ground.FindEvent("keyctrl04");
        KeyEvents[5] = Ground.FindEvent("keyctrl05");
        KeyEvents[6] = Ground.FindEvent("keyctrl06");
        KeyEvents[7] = Ground.FindEvent("keyctrl07");
        KeyEvents[8] = Ground.FindEvent("keyctrl08");
        KeyEvents[9] = Ground.FindEvent("keyctrl09");
    }
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);  //{Texture blends with object background}
    light = TTexturesManager::GetTextureID(szTexturePath, szSceneryPath, "smuga.tga");
    // Camera.Reset();
    ResetTimers();
    WriteLog("Load time: " + FloatToStrF((86400.0 * ((double)Now() - time)), ffFixed, 7, 1) + " seconds");

    AnsiString logdate = FormatDateTime("yymmdd hhmmss", Now());
    CopyFile("log.txt", AnsiString("data\\logs\\" + logdate + ".txt").c_str(), false);

    if (DebugModeFlag) // w Debugmode automatyczne w³¹czenie AI
        if (Train)
            if (Train->Dynamic()->Mechanik)
                Train->Dynamic()->Mechanik->TakeControl(true);


    if (QGlobal::bmodelpreview)
       {
        Global::SetCameraPosition(Global::pFreeCameraInit[1]); //nowa pozycja dla generowania obiektów
        Ground.Silence(Camera.Pos); //wyciszenie wszystkiego z poprzedniej pozycji
        Camera.Init(Global::pFreeCameraInit[1], Global::pFreeCameraInitAngle[1]); // przestawienie

        if (FreeFlyModeFlag) Camera.RaLook(); // jednorazowe przestawienie kamery
       }

    QGlobal::bSCNLOADED = true;
    QGlobal::SLTEMP->Clear();
    QGlobal::SLTEMP->Add(IntToStr(QGlobal::iNODES));
    QGlobal::SLTEMP->SaveToFile("data\\pbars\\" + AnsiString(Global::szSceneryFile));
    Global::iPause = true;

    if (!QGlobal::bmodelpreview) generatenoisetex(); // W efects2d.cpp

    if (mvControlled) Controlled->GetConsist_f(1, Controlled);

    return true;
};



// ***********************************************************************************************************
// OBSLUGA WCISNIECIA KLAWISZY W ROZNYCH KOMBINACJACH
// ***********************************************************************************************************
void TWorld::OnKeyDown(int cKey)
{ //(cKey) to kod klawisza, cyfrowe i literowe siê zgadzaj¹
  // Ra 2014-09: tu by mo¿na dodaæ tabelê konwersji: 256 wirtualnych kodów w kontekœcie dwóch prze³¹czników
  // [Shift] i [Ctrl] na ka¿dy kod wirtualny niech przypadaj¹ 4 bajty: 2 dla naciœniêcia i 2 dla zwolnienia
  // powtórzone 256 razy da 1kB na ka¿dy stan prze³¹czników, ³¹cznie bêdzie 4kB pierwszej tabeli przekodowania

 QGlobal::isshift = false;

 // 18 nowych klawiszy do przemyslenia czy jest sens (klawiatura multimedialna)
 if (Console::Pressed(VK_BROWSER_BACK)) WriteLog("VK_BROWSER_BACK");
 if (Console::Pressed(VK_BROWSER_FORWARD)) WriteLog("VK_BROWSER_FORWARD");
 if (Console::Pressed(VK_BROWSER_HOME)) WriteLog("VK_BROWSER_HOME");
 if (Console::Pressed(VK_BROWSER_SEARCH)) WriteLog("VK_BROWSER_SEARCH");
 if (Console::Pressed(VK_BROWSER_REFRESH)) WriteLog("VK_BROWSER_REFRESH");
 if (Console::Pressed(VK_BROWSER_STOP)) WriteLog("VK_BROWSER_STOP");
 if (Console::Pressed(VK_VOLUME_DOWN)) WriteLog("VK_VOLUME_DOWN");
 if (Console::Pressed(VK_VOLUME_UP)) WriteLog("VK_VOLUME_UP");                                                  
 if (Console::Pressed(VK_VOLUME_MUTE)) WriteLog("VK_VOLUME_MUTE");
 if (Console::Pressed(VK_LAUNCH_MAIL)) WriteLog("VK_LAUNCH_MAIL");
 if (Console::Pressed(VK_LAUNCH_MEDIA_SELECT)) WriteLog("VK_LAUNCH_MEDIA_SELECT");
 if (Console::Pressed(VK_MEDIA_PLAY_PAUSE)) WriteLog("VK_MEDIA_PLAY_PAUSE");
 if (Console::Pressed(VK_MEDIA_STOP)) WriteLog("VK_MEDIA_STOP");
 if (Console::Pressed(VK_MEDIA_NEXT_TRACK)) WriteLog("VK_MEDIA_NEXT_TRACK");
 if (Console::Pressed(VK_MEDIA_PREV_TRACK)) WriteLog("VK_MEDIA_PREV_TRACK");
 if (Console::Pressed(VK_SLEEP)) WriteLog("VK_SLEEP");
 if (Console::Pressed(VK_LAUNCH_APP1)) WriteLog("VK_LAUNCH_APP1");
 if (Console::Pressed(VK_LAUNCH_APP2)) WriteLog("VK_LAUNCH_APP2");

 if (Console::Pressed(VK_RMENU)) WriteLog("R ALT");                             // PRAWY ALT
 if (Console::Pressed(VK_CONTROL) && Console::Pressed(VK_SHIFT) && Console::Pressed(VkKeyScan('f'))) QGlobal::bscrfilter = !QGlobal::bscrfilter;
 if (Console::Pressed(VK_CONTROL) && Console::Pressed(VK_SHIFT) && Console::Pressed(VkKeyScan('n'))) QGlobal::bscrnoise = !QGlobal::bscrnoise;
 
 if (Global::iPause && cKey == Global::Keys[k_Czuwak]) STARTSIMULATION();       // Q 291215: Bo po zaladowaniu symulacji jest pauza i pozostaje obraz wczytywania jako tlo pauzy

 if (!Console::Pressed(VK_SHIFT) && cKey == VK_F11) SCR->SaveScreen_xxx();      // Q 261215: zrut ekranu do jpg, tga lub bmp w zaleznosci od opcji w config.txt

 if (GetAsyncKeyState(VK_SHIFT) < 0) { QGlobal::isshift = true; }               // USUNALEM SPRAWDZANIE W TTrain::OnKeyDown() zastepujac parametrem funkcji;

 if (QGlobal::bEXITQUERY)
    {
      if ((cKey == 89))   // 89.Y lub 90.Z (bo czasami sie zamieni gdy ktos ma krzywe palce
       {
        DeleteFile("templog.txt"); // usuniêcie starego
        DeleteFile("myconsist.txt"); // usuniêcie starego
        DeleteFile(AnsiString(QGlobal::asAPPDIR + "models\\temp\\temp.e3d").c_str());
        char logfile[200];
        sprintf(logfile,"%s%s", QGlobal::asAPPDIR.c_str() , QGlobal::logfilenm1.c_str());
        if (QGlobal::bOPENLOGONX) ShellExecute(0, "open", logfile, NULL, NULL, SW_MAXIMIZE);
        exit(0);
       } 
      if (Console::Pressed(VkKeyScan('n'))) { Global::iTextMode = -999; Global::iPause = false; QGlobal::bEXITQUERY = false;}
    }
   else 
  {
    if (!Global::iPause)
    { // podczas pauzy klawisze nie dzia³aj¹
        AnsiString info = "Key pressed: [";
        if (Console::Pressed(VK_SHIFT))
            info += "Shift]+[";
        if (Console::Pressed(VK_CONTROL))
            info += "Ctrl]+[";
        if (cKey > 192) // coœ tam jeszcze ciekawego jest?
        {
            if (cKey < 255) // 255 to [Fn] w laptopach
                WriteLog(info + AnsiString(char(cKey - 128)) + "]");
        }
        else if (cKey >= 186)
            WriteLog(info + AnsiString(";=,-./~").SubString(cKey - 185, 1) + "]");
        else if (cKey > 123) // coœ tam jeszcze ciekawego jest?
            WriteLog(info + AnsiString(cKey) + "]"); // numer klawisza
        else if (cKey >= 112) // funkcyjne
            WriteLog(info + "F" + AnsiString(cKey - 111) + "]");
        else if (cKey >= 96)
            WriteLog(info + "Num" + AnsiString("0123456789*+?-./").SubString(cKey - 95, 1) + "]");
        else if (((cKey >= '0') && (cKey <= '9')) || ((cKey >= 'A') && (cKey <= 'Z')) || (cKey == ' '))
            WriteLog(info + AnsiString(char(cKey)) + "]");
        else if (cKey == '-')
            WriteLog(info + "Insert]");
        else if (cKey == '.')
            WriteLog(info + "Delete]");
        else if (cKey == '$')
            WriteLog(info + "Home]");
        else if (cKey == '#')
            WriteLog(info + "End]");
        else if (cKey > 'Z') //¿eby nie logowaæ kursorów
            WriteLog(info + AnsiString(cKey) + "]"); // numer klawisza
    }



    if ((cKey <= '9') ? (cKey >= '0') : false) // klawisze cyfrowe
    {
      if (Console::Pressed(VK_LBUTTON) && cKey == '0') QGlobal::infotype = 0;
      if (Console::Pressed(VK_LBUTTON) && cKey == '1') QGlobal::infotype = 1;
      if (Console::Pressed(VK_LBUTTON) && cKey == '2') QGlobal::infotype = 2;
      if (Console::Pressed(VK_LBUTTON) && cKey == '3') QGlobal::infotype = 3;
      if (Console::Pressed(VK_LBUTTON) && cKey == '4') QGlobal::infotype = 4;
      if (Console::Pressed(VK_LBUTTON) && cKey == '5') QGlobal::infotype = 5;
      if (Console::Pressed(VK_LBUTTON) && cKey == '6') QGlobal::infotype = 6;
      if (Console::Pressed(VK_LBUTTON) && cKey == '7') QGlobal::infotype = 7;
      if (Console::Pressed(VK_LBUTTON) && cKey == '8') QGlobal::infotype = 8;
      if (Console::Pressed(VK_LBUTTON) && cKey == '9') QGlobal::infotype = 9;
      if (Console::Pressed(VK_LBUTTON) && cKey  > '0') Global::iTextMode = -999;

        int i = cKey - '0'; // numer klawisza
        if (Console::Pressed(VK_SHIFT))
        { // z [Shift] uruchomienie eventu
            if (!Global::iPause) // podczas pauzy klawisze nie dzia³aj¹
                if (KeyEvents[i])
                    Ground.AddToQuery(KeyEvents[i], NULL);
        }
        else // zapamiêtywanie kamery mo¿e dzia³aæ podczas pauzy
            if (FreeFlyModeFlag) // w trybie latania mo¿na przeskakiwaæ do ustawionych kamer
            if ((Global::iTextMode != VK_F12) && (Global::iTextMode != VK_F3) && (QGlobal::infotype ==0)) // ograniczamy u¿ycie kamer
            {
                if ((!Global::pFreeCameraInit[i].x && !Global::pFreeCameraInit[i].y && !Global::pFreeCameraInit[i].z))
                { // jeœli kamera jest w punkcie zerowym, zapamiêtanie wspó³rzêdnych i k¹tów
                    Global::pFreeCameraInit[i] = Camera.Pos;
                    Global::pFreeCameraInitAngle[i].x = Camera.Pitch;
                    Global::pFreeCameraInitAngle[i].y = Camera.Yaw;
                    Global::pFreeCameraInitAngle[i].z = Camera.Roll;
                    // logowanie, ¿eby mo¿na by³o do scenerii przepisaæ
                    WriteLog("camera " + FloatToStrF(Global::pFreeCameraInit[i].x, ffFixed, 7, 3) + " " +
                                         FloatToStrF(Global::pFreeCameraInit[i].y, ffFixed, 7, 3) + " " +
                                         FloatToStrF(Global::pFreeCameraInit[i].z, ffFixed, 7, 3) + " " +
                                         FloatToStrF(RadToDeg(Global::pFreeCameraInitAngle[i].x), ffFixed, 7, 3) + " " +
                                         FloatToStrF(RadToDeg(Global::pFreeCameraInitAngle[i].y), ffFixed, 7, 3) + " " +
                                         FloatToStrF(RadToDeg(Global::pFreeCameraInitAngle[i].z), ffFixed, 7, 3) + " " + AnsiString(i) + " endcamera");
                }
                else // równie¿ przeskakiwanie
                { // Ra: to z t¹ kamer¹ (Camera.Pos i Global::pCameraPosition) jest trochê bez sensu
                    Global::SetCameraPosition(Global::pFreeCameraInit[i]); // nowa pozycja dla generowania obiektów
                    Ground.Silence(Camera.Pos); // wyciszenie wszystkiego z poprzedniej pozycji
                    Camera.Init(Global::pFreeCameraInit[i], Global::pFreeCameraInitAngle[i]); // przestawienie
                }
            }  // Z SHIFTEM
        // bêdzie jeszcze za³¹czanie sprzêgów z [Ctrl]
    } //if ((cKey<='9')?(cKey>='0'):false)




// KLAWISZE FUNKCYJNE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    else if ((cKey >= VK_F1) ? (cKey <= VK_F12) : false)
    {
     if ((cKey == VK_F1)) QGlobal::infotype = 0;   // Coby wylaczyc panele informacyjne Q
     
     if (mvControlled) Controlled->GetConsist_f(1, Controlled);  // Q 040116: Tworzenie listy pojazdow w skladzie, liczenie masy brutto i dlugosci
                                                                 //            Liste te docelowo mozna wyswietlic na jakims panelu informacyjnym
        switch (cKey)
        {
        case VK_F1: // czas i relacja
        case VK_F3:
        case VK_F5: // przesiadka do innego pojazdu
        case VK_F8: // FPS
        case VK_F9: // wersja, typ wyœwietlania, b³êdy OpenGL
            if (Global::iTextMode == cKey)
                Global::iTextMode =
                    (Global::iPause && (cKey != VK_F1) ? VK_F1 : 0); // wy³¹czenie napisów, chyba ¿e pauza
            else
                Global::iTextMode = cKey;
            break;
        case VK_F2: // parametry pojazdu
            if (Global::iTextMode == cKey) // jeœli kolejne naciœniêcie
                ++Global::iScreenMode[cKey - VK_F1]; // kolejny ekran
            else
            { // pierwsze naciœniêcie daje pierwszy (tzn. zerowy) ekran
                Global::iTextMode = cKey;
                Global::iScreenMode[cKey - VK_F1] = 0;
            }
            break;
        case VK_F12: // coœ tam jeszcze
            if (Console::Pressed(VK_CONTROL) && Console::Pressed(VK_SHIFT))
                DebugModeFlag = !DebugModeFlag; // taka opcjonalna funkcja, mo¿e siê czasem przydaæ
            /* //Ra 2F1P: teraz w³¹czanie i wy³¹czanie klawiszami cyfrowymi po u¿yciu [F12]
                else if (Console::Pressed(VK_SHIFT))
                {//odpalenie logu w razie "W"
                 if ((Global::iWriteLogEnabled&2)==0) //nie by³o okienka
                 {//otwarcie okna
                  AllocConsole();
                  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
                 }
                 Global::iWriteLogEnabled|=3;
                } */
            else
                Global::iTextMode = cKey;
            break;
        case VK_F4:
            InOutKey();
            break;
        case VK_F6:
            if (DebugModeFlag)
            { // przyspieszenie symulacji do testowania scenerii... uwaga na FPS!
                // Global::iViewMode=VK_F6;
                if (Console::Pressed(VK_CONTROL))
                    Global::fTimeSpeed = (Console::Pressed(VK_SHIFT) ? 10.0 : 5.0);
                else
                    Global::fTimeSpeed = (Console::Pressed(VK_SHIFT) ? 2.0 : 1.0);
            }
            break;
        }
        // if (cKey!=VK_F4)
        return; // nie s¹ przekazywane do pojazdu wcale
    }  // if ((cKey>=VK_F1)?(cKey<=VK_F12):false)

// klawisze queueda
//if ((Console::Pressed(VK_CONTROL)) && (Console::Pressed(VkKeyScan('z'))) ) SCR->FOVREM();
//if ((Console::Pressed(VK_CONTROL)) && (Console::Pressed(VkKeyScan('x'))) ) SCR->FOVADD();

// Q 020116: niezalezne od ewentow przestawianie rozjazdu w odleglosci 1m od kamery
    TGroundNode *tmp, *tmptrk;
    tmptrk = Ground.FindGroundNodeDist(10, TP_TRACK);
    if ((Console::Pressed(VK_CONTROL)) && (Console::Pressed(VkKeyScan('z'))) )
    if (tmptrk != NULL)
     {
           AnsiString statestr;
           int state = tmptrk->pTrack->GetSwitchState();
           if (state == 0) state = 1; else state = 0;
           if (state == 0) statestr = "(straight)"; else statestr = "(aside)";
           tmptrk->pTrack->Switch(state, 0.05, 0.03);
           WriteLog(tmptrk->pTrack->NameGet() + " switched to " + IntToStr(state) + " " + statestr);
     }

//    if (Global::iTextMode == VK_F10) // wyœwietlone napisy klawiszem F10
//    { // i potwierdzenie
//        Global::iPause = false;
//        Global::iTextMode = 0;
//        Global::iTextMode = (cKey == 'Y') ? -1 : 0; // flaga wyjœcia z programu
//
//        return; // nie przekazujemy do poci¹gu
//    }
//    else
    if ((Global::iTextMode == VK_F12) ? (cKey >= '0') && (cKey <= '9') : false)
    { // tryb konfiguracji debugmode (przestawianie kamery ju¿ wy³¹czone
        if (!Console::Pressed(VK_SHIFT)) // bez [Shift]
        {
            if (cKey == '1')
                Global::iWriteLogEnabled ^= 1; // w³¹cz/wy³¹cz logowanie do pliku
            else if (cKey == '2')
            { // w³¹cz/wy³¹cz okno konsoli
                Global::iWriteLogEnabled ^= 2;
                if ((Global::iWriteLogEnabled & 2) == 0) // nie by³o okienka
                { // otwarcie okna
                    AllocConsole(); // jeœli konsola ju¿ jest, to zwróci b³¹d; uwalniaæ nie ma po
                    // co, bo siê od³¹czy
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
                }
            }
            // else if (cKey=='3') Global::iWriteLogEnabled^=4; //wypisywanie nazw torów
        }
    }
    else if (cKey == 3) //[Ctrl]+[Break]
    { // hamowanie wszystkich pojazdów w okolicy
        Ground.RadioStop(Camera.Pos);
    }
    else if (!Global::iPause) //||(cKey==VK_F4)) //podczas pauzy sterownaie nie dzia³a, F4 tak
        if (Train)
            if (Controlled)
                if ((Controlled->Controller == Humandriver) ? true : DebugModeFlag || (cKey == 'Q'))
                    Train->OnKeyDown(cKey); // przekazanie klawisza do kabiny
    if (FreeFlyModeFlag) // aby nie odluŸnia³o wagonu za lokomotyw¹
    { // operacje wykonywane na dowolnym pojeŸdzie, przeniesione tu z kabiny
        if (cKey == Global::Keys[k_Releaser]) // odluŸniacz
        { // dzia³a globalnie, sprawdziæ zasiêg
            TDynamicObject *temp = Global::DynamicNearest();
            if (temp)
            {
                if (GetAsyncKeyState(VK_CONTROL) < 0) // z ctrl odcinanie
                {
                    temp->MoverParameters->BrakeStatus ^= 128;
                }
                else if (temp->MoverParameters->BrakeReleaser(1))
                {
                    // temp->sBrakeAcc->
                    // dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
                    // dsbPneumaticRelay->Play(0,0,0); //temp->Position()-Camera.Pos //???
                }
            }
        }
        else if (cKey == Global::Keys[k_Heating]) // Ra: klawisz nie jest najszczêœliwszy
        { // zmiana pró¿ny/³adowny; Ra: zabrane z kabiny
            TDynamicObject *temp = Global::DynamicNearest();
            if (temp)
            {
                if (Console::Pressed(VK_SHIFT) ? temp->MoverParameters->IncBrakeMult() :
                                                 temp->MoverParameters->DecBrakeMult())
                    if (Train)
                    { // dŸwiêk oczywiœcie jest w kabinie
                        Train->dsbSwitch->SetVolume(DSBVOLUME_MAX);
                        Train->dsbSwitch->Play(0, 0, 0);
                    }
            }
        }
        else if (cKey == Global::Keys[k_EndSign])
        { // Ra 2014-07: zabrane z kabiny
            TDynamicObject *tmp = Global::CouplerNearest(); // domyœlnie wyszukuje do 20m
            if (tmp)
            {
                int CouplNr = (LengthSquared3(tmp->HeadPosition() - Camera.Pos) >
                                       LengthSquared3(tmp->RearPosition() - Camera.Pos) ?
                                   1 :
                                   -1) *
                              tmp->DirectionGet();
                if (CouplNr < 0)
                    CouplNr = 0; // z [-1,1] zrobiæ [0,1]
                int mask, set = 0; // Ra: [Shift]+[Ctrl]+[T] odpala mi jak¹œ idiotyczn¹ zmianê
                // tapety pulpitu :/
                if (GetAsyncKeyState(VK_SHIFT) < 0) // z [Shift] zapalanie
                    set = mask = 64; // bez [Ctrl] za³o¿yæ tabliczki
                else if (GetAsyncKeyState(VK_CONTROL) < 0)
                    set = mask = 2 + 32; // z [Ctrl] zapaliæ œwiat³a czerwone
                else
                    mask = 2 + 32 + 64; // wy³¹czanie œci¹ga wszystko
                if (((tmp->iLights[CouplNr]) & mask) != set)
                {
                    tmp->iLights[CouplNr] = (tmp->iLights[CouplNr] & ~mask) | set;
                    if (Train)
                    { // Ra: ten dŸwiêk z kabiny to przegiêcie, ale na razie zostawiam
                        Train->dsbSwitch->SetVolume(DSBVOLUME_MAX);
                        Train->dsbSwitch->Play(0, 0, 0);
                    }
                }
            }
        }
        else if (cKey == Global::Keys[k_IncLocalBrakeLevel])
        { // zahamowanie dowolnego pojazdu
            TDynamicObject *temp = Global::DynamicNearest();
            if (temp)
            {
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                    if ((temp->MoverParameters->LocalBrake == ManualBrake) ||
                        (temp->MoverParameters->MBrake == true))
                        temp->MoverParameters->IncManualBrakeLevel(1);
                    else
                        ;
                else if (temp->MoverParameters->LocalBrake != ManualBrake)
                    if (temp->MoverParameters->IncLocalBrakeLevelFAST())
                        if (Train)
                        { // dŸwiêk oczywiœcie jest w kabinie
                            Train->dsbPneumaticRelay->SetVolume(-80);
                            Train->dsbPneumaticRelay->Play(0, 0, 0);
                        }
            }
        }
        else if (cKey == Global::Keys[k_DecLocalBrakeLevel])
        { // odhamowanie dowolnego pojazdu
            TDynamicObject *temp = Global::DynamicNearest();
            if (temp)
            {
                if (GetAsyncKeyState(VK_CONTROL) < 0)
                    if ((temp->MoverParameters->LocalBrake == ManualBrake) ||
                        (temp->MoverParameters->MBrake == true))
                        temp->MoverParameters->DecManualBrakeLevel(1);
                    else
                        ;
                else if (temp->MoverParameters->LocalBrake != ManualBrake)
                    if (temp->MoverParameters->DecLocalBrakeLevelFAST())
                        if (Train)
                        { // dŸwiêk oczywiœcie jest w kabinie
                            Train->dsbPneumaticRelay->SetVolume(-80);
                            Train->dsbPneumaticRelay->Play(0, 0, 0);
                        }
            }
        }
    }

  }   //q
    // switch (cKey)
    //{case 'a': //ignorowanie repetycji
    // case 'A': Global::iKeyLast=cKey; break;
    // default: Global::iKeyLast=0;
    //}
}


// *****************************************************************************
// OBSLUGA PUSZCZENIA KLAWISZA
// *****************************************************************************
void TWorld::OnKeyUp(int cKey)
{ // zwolnienie klawisza; (cKey) to kod klawisza, cyfrowe i literowe siê zgadzaj¹

    if (!Global::iPause) // podczas pauzy sterownaie nie dzia³a
        if (Train)
            if (Controlled)
                if ((Controlled->Controller == Humandriver) ? true : DebugModeFlag || (cKey == 'Q'))
                    Train->OnKeyUp(cKey); // przekazanie zwolnienia klawisza do kabiny
};


// *****************************************************************************
// W ten sposob rozgraniczylem klawisze F10 i lewy ALT - normalnie w WM_KEYDOWN
// generuja ten sam kod a w WM_SYSKEYUP nie
// *****************************************************************************
void TWorld::OnSysKeyUp(int cKey)
{
 if (QGlobal::bSIMSTARTED)
  if (!QGlobal::bEXITQUERY)
   if (cKey == 121)  // F10
     {
       std::string skbdl;
       skbdl = Global::GetKbdLayout();

       if (skbdl != "415") LoadKeyboardLayout("00000415", KLF_ACTIVATE);  // Q 120116: Gdy odwroci klawiature krzywymi palcami to wracamy do polskiego ukladu

       QGlobal::bEXITQUERY = true;
       Global::iPause = true;
       WriteLog("PAUSED I EXIT QUERY");
     }
 return;
};


// *****************************************************************************
// OBSLUGA PORUSZANIA MYSZA
// *****************************************************************************
void TWorld::OnMouseMove(double x, double y)
{ // McZapkie:060503-definicja obracania myszy
    Camera.OnCursorMove(x * Global::fMouseXScale, -y * Global::fMouseYScale);
}

void TWorld::OnMouseLpush(double x, double y)
{
// if ( Console::Pressed(VK_SHIFT)) SCR->FOVADD();
}

void TWorld::OnMouseRpush(double x, double y)
{
// if (Console::Pressed(VK_SHIFT)) SCR->FOVREM();
}

void TWorld::OnMouseMpush(double x, double y)
{
// WriteLog("MOUSE M DOWN");
}

void TWorld::OnMouseWheel(int zDelta)
{
 //if (zDelta > 0) SCR->FOVADD(5.00f);
 //if (zDelta < 0) SCR->FOVREM(5.00f);
}


// *****************************************************************************
// OBSLUGA WEJSCIA/WYJSCIA DO/Z KABINY
// *****************************************************************************
void TWorld::InOutKey()
{ // prze³¹czenie widoku z kabiny na zewnêtrzny i odwrotnie
    FreeFlyModeFlag = !FreeFlyModeFlag; // zmiana widoku
    if (FreeFlyModeFlag)
    { // je¿eli poza kabin¹, przestawiamy w jej okolicê - OK
        Global::pUserDynamic = NULL; // bez renderowania wzglêdem kamery
        if (Train)
        { // Train->Dynamic()->ABuSetModelShake(vector3(0,0,0));
            Train->Silence(); // wy³¹czenie dŸwiêków kabiny
            Train->Dynamic()->bDisplayCab = false;
            DistantView();
        }
    }
    else
    { // jazda w kabinie
        if (Train)
        {
            Global::pUserDynamic = Controlled; // renerowanie wzglêdem kamery
            Train->Dynamic()->bDisplayCab = true;
            Train->Dynamic()->ABuSetModelShake(
                vector3(0, 0, 0)); // zerowanie przesuniêcia przed powrotem?
            // Camera.Stop(); //zatrzymanie ruchu
            Train->MechStop();
            FollowView(); // na pozycjê mecha
        }
        else
            FreeFlyModeFlag = true; // nadal poza kabin¹
    }
};


// *****************************************************************************
// TAKI SE WIDOK Z DYSTANSU NA POJAZDY
// *****************************************************************************
void TWorld::DistantView()
{ // ustawienie widoku pojazdu z zewn¹trz
    if (Controlled) // jest pojazd do prowadzenia?
    { // na prowadzony
        Camera.Pos =
            Controlled->GetPosition() +
            (Controlled->MoverParameters->ActiveCab >= 0 ? 30 : -30) * Controlled->VectorFront() +
            vector3(0, 5, 0);
        Camera.LookAt = Controlled->GetPosition();
        Camera.RaLook(); // jednorazowe przestawienie kamery
    }
    else if (pDynamicNearest) // jeœli jest pojazd wykryty blisko
    { // patrzenie na najbli¿szy pojazd
        Camera.Pos = pDynamicNearest->GetPosition() +
                     (pDynamicNearest->MoverParameters->ActiveCab >= 0 ? 30 : -30) *
                         pDynamicNearest->VectorFront() +
                     vector3(0, 5, 0);
        Camera.LookAt = pDynamicNearest->GetPosition();
        Camera.RaLook(); // jednorazowe przestawienie kamery
    }
};


// *****************************************************************************
// SLEDZENIE POJAZDU
// *****************************************************************************
void TWorld::FollowView(bool wycisz)
{ // ustawienie œledzenia pojazdu
    // ABu 180404 powrot mechanika na siedzenie albo w okolicê pojazdu
    // if (Console::Pressed(VK_F4)) Global::iViewMode=VK_F4;
    // Ra: na zewn¹trz wychodzimy w Train.cpp
    Camera.Reset(); // likwidacja obrotów - patrzy horyzontalnie na po³udnie
    if (Controlled) // jest pojazd do prowadzenia?
    {
        vector3 camStara =
            Camera.Pos; // przestawianie kamery jest bez sensu: do przerobienia na potem
        // Controlled->ABuSetModelShake(vector3(0,0,0));
        if (FreeFlyModeFlag)
        { // je¿eli poza kabin¹, przestawiamy w jej okolicê - OK
            if (Train)
                Train->Dynamic()->ABuSetModelShake(
                    vector3(0, 0, 0)); // wy³¹czenie trzêsienia na si³ê?
            // Camera.Pos=Train->pMechPosition+Normalize(Train->GetDirection())*20;
            DistantView(); // przestawienie kamery
            //¿eby nie bylo numerów z 'fruwajacym' lokiem - konsekwencja bujania pud³a
            Global::SetCameraPosition(
                Camera.Pos); // tu ustawiæ now¹, bo od niej licz¹ siê odleg³oœci
            Ground.Silence(camStara); // wyciszenie dŸwiêków z poprzedniej pozycji
        }
        else if (Train)
        { // korekcja ustawienia w kabinie - OK
            vector3 camStara =
                Camera.Pos; // przestawianie kamery jest bez sensu: do przerobienia na potem
            // Ra: czy to tu jest potrzebne, bo przelicza siê kawa³ek dalej?
            Camera.Pos = Train->pMechPosition; // Train.GetPosition1();
            Camera.Roll = atan(Train->pMechShake.x * Train->fMechRoll); // hustanie kamery na boki
            Camera.Pitch -=
                atan(Train->vMechVelocity.z * Train->fMechPitch); // hustanie kamery przod tyl
            if (Train->Dynamic()->MoverParameters->ActiveCab == 0)
                Camera.LookAt = Train->pMechPosition + Train->GetDirection();
            else // patrz w strone wlasciwej kabiny
                Camera.LookAt =
                    Train->pMechPosition +
                    Train->GetDirection() * Train->Dynamic()->MoverParameters->ActiveCab;
            Train->pMechOffset.x = Train->pMechSittingPosition.x;
            Train->pMechOffset.y = Train->pMechSittingPosition.y;
            Train->pMechOffset.z = Train->pMechSittingPosition.z;
            Global::SetCameraPosition(
                Train->Dynamic()
                    ->GetPosition()); // tu ustawiæ now¹, bo od niej licz¹ siê odleg³oœci
            if (wycisz) // trzymanie prawego w kabinie daje marny efekt
                Ground.Silence(camStara); // wyciszenie dŸwiêków z poprzedniej pozycji
        }
    }
    else
        DistantView();
};


// *****************************************************************************
// FUNKCJA AKTUALIZUJACA, FIZYKE, RUCH MODELI I NA KONIEC WYWOLANIE RENDERINGU
// *****************************************************************************
bool TWorld::Update()
{
#ifdef USE_SCENERY_MOVING
    vector3 tmpvector = Global::GetCameraPosition();
    tmpvector = vector3(-int(tmpvector.x) + int(tmpvector.x) % 10000,
                        -int(tmpvector.y) + int(tmpvector.y) % 10000,
                        -int(tmpvector.z) + int(tmpvector.z) % 10000);
    if (tmpvector.x || tmpvector.y || tmpvector.z)
    {
        WriteLog("Moving scenery");
        Ground.MoveGroundNode(tmpvector);
        WriteLog("Scenery moved");
    };
#endif



    if (iCheckFPS)
        --iCheckFPS;
    else
    { // jak dosz³o do zera, to sprawdzamy wydajnoœæ
     QGlobal::fps = GetFPS();

        if (GetFPS() < Global::fFpsMin)
        {
            Global::iSegmentsRendered -=
                random(10); // floor(0.5+Global::iSegmentsRendered/Global::fRadiusFactor);
            if (Global::iSegmentsRendered < 10) // jeœli jest co zmniejszaæ
                Global::iSegmentsRendered = 10; // 10=minimalny promieñ to 600m
        }
        else if (GetFPS() > Global::fFpsMax) // jeœli jest du¿o FPS
            if (Global::iSegmentsRendered < Global::iFpsRadiusMax) // jeœli jest co zwiêkszaæ
            {
                Global::iSegmentsRendered +=
                    random(5); // floor(0.5+Global::iSegmentsRendered*Global::fRadiusFactor);
                if (Global::iSegmentsRendered > Global::iFpsRadiusMax) // 5.6km (22*22*M_PI)
                    Global::iSegmentsRendered = Global::iFpsRadiusMax;
            }
        if ((GetFPS() < 12) && (Global::iSlowMotion < 7))
        {
            Global::iSlowMotion = (Global::iSlowMotion << 1) + 1; // zapalenie kolejnego bitu
            if (Global::iSlowMotionMask & 1)
                if (Global::iMultisampling) // a multisampling jest w³¹czony
                    glDisable(GL_MULTISAMPLE); // wy³¹czenie multisamplingu powinno poprawiæ FPS
        }
        else if ((GetFPS() > 20) && Global::iSlowMotion)
        { // FPS siê zwiêkszy³, mo¿na w³¹czyæ bajery
            Global::iSlowMotion = (Global::iSlowMotion >> 1); // zgaszenie bitu
            if (Global::iSlowMotion == 0) // jeœli jest pe³na prêdkoœæ
                if (Global::iMultisampling) // a multisampling jest w³¹czony
                    glEnable(GL_MULTISAMPLE);
        }
        /*
          if (!Global::bPause)
           if (GetFPS()<=5)
           {//zwiêkszenie kroku fizyki przy s³abym FPS
            if (fMaxDt<0.05)
            {fMaxDt=0.05; //Ra: tak nie mo¿e byæ, bo s¹ problemy na sprzêgach
             WriteLog("Phisics step switched to 0.05s!");
            }
           }
           else if (GetFPS()>12)
            if (fMaxDt>0.01)
            {//powrót do podstawowego kroku fizyki
             fMaxDt=0.01;
             WriteLog("Phisics step switched to 0.01s!");
            }
        */
        iCheckFPS = 0.25 * GetFPS(); // tak za 0.25 sekundy sprawdziæ ponownie (jeszcze przycina?)
    }
    
    if (Controlled) QGlobal::iSTATIONPOSINTAB = Global::findstationbyname(Trim(Controlled->asStation.LowerCase())); // Q 030116: Pobieranie pozycji itemu stacji na liscie

    UpdateTimers(Global::iPause);

    //WriteLog("----------------------------------");
    QGlobal::currententrypoint = 0;
    
    if (!Global::iPause)
    { // jak pauza, to nie ma po co tego przeliczaæ
        GlobalTime->UpdateMTableTime(GetDeltaTime()); // McZapkie-300302: czas rozkladowy
        // Ra 2014-07: przeliczenie k¹ta czasu (do animacji zale¿nych od czasu)
        Global::fTimeAngleDeg =
            GlobalTime->hh * 15.0 + GlobalTime->mm * 0.25 + GlobalTime->mr / 240.0;
        Global::fClockAngleDeg[0] = 36.0 * (int(GlobalTime->mr) % 10); // jednostki sekund
        Global::fClockAngleDeg[1] = 36.0 * (int(GlobalTime->mr) / 10); // dziesi¹tki sekund
        Global::fClockAngleDeg[2] = 36.0 * (GlobalTime->mm % 10); // jednostki minut
        Global::fClockAngleDeg[3] = 36.0 * (GlobalTime->mm / 10); // dziesi¹tki minut
        Global::fClockAngleDeg[4] = 36.0 * (GlobalTime->hh % 10); // jednostki godzin
        Global::fClockAngleDeg[5] = 36.0 * (GlobalTime->hh / 10); // dziesi¹tki godzin
        if (Global::fMoveLight >= 0.0)
        { // testowo ruch œwiat³a
            // double a=Global::fTimeAngleDeg/180.0*M_PI-M_PI; //k¹t godzinny w radianach
            double a = fmod(Global::fTimeAngleDeg, 360.0) / 180.0 * M_PI -
                       M_PI; // k¹t godzinny w radianach
            //(a) jest traktowane jako czas miejscowy, nie uwzglêdniaj¹cy stref czasowych ani czasu
            // letniego
            // aby wyznaczyæ strefê czasow¹, trzeba uwzglêdniæ po³udnik miejscowy
            // aby uwzglêdniæ czas letni, trzeba sprawdziæ dzieñ roku
            double L = Global::fLatitudeDeg / 180.0 * M_PI; // szerokoœæ geograficzna
            double H = asin(cos(L) * cos(Global::fSunDeclination) * cos(a) +
                            sin(L) * sin(Global::fSunDeclination)); // k¹t ponad horyzontem
            // double A=asin(cos(d)*sin(M_PI-a)/cos(H));
            // Declination=((0.322003-22.971*cos(t)-0.357898*cos(2*t)-0.14398*cos(3*t)+3.94638*sin(t)+0.019334*sin(2*t)+0.05928*sin(3*t)))*Pi/180
            // Altitude=asin(sin(Declination)*sin(latitude)+cos(Declination)*cos(latitude)*cos((15*(time-12))*(Pi/180)));
            // Azimuth=(acos((cos(latitude)*sin(Declination)-cos(Declination)*sin(latitude)*cos((15*(time-12))*(Pi/180)))/cos(Altitude)));
            // double A=acos(cos(L)*sin(d)-cos(d)*sin(L)*cos(M_PI-a)/cos(H));
            // dAzimuth = atan2(-sin( dHourAngle ),tan( dDeclination )*dCos_Latitude -
            // dSin_Latitude*dCos_HourAngle );
            double A = atan2(sin(a), tan(Global::fSunDeclination) * cos(L) - sin(L) * cos(a));
            vector3 lp = vector3(sin(A), tan(H), cos(A));
            lp = Normalize(lp); // przeliczenie na wektor d³ugoœci 1.0
            Global::lightPos[0] = (float)lp.x;
            Global::lightPos[1] = (float)lp.y;
            Global::lightPos[2] = (float)lp.z;
            glLightfv(GL_LIGHT0, GL_POSITION, Global::lightPos); // daylight position
            if (H > 0)
            { // s³oñce ponad horyzontem
                Global::ambientDayLight[0] = Global::ambientLight[0];
                Global::ambientDayLight[1] = Global::ambientLight[1];
                Global::ambientDayLight[2] = Global::ambientLight[2];
                if (H > 0.02) // ponad 1.146° zaczynaj¹ siê cienie
                {
                    Global::diffuseDayLight[0] =
                        Global::diffuseLight[0]; // od wschodu do zachodu maksimum ???
                    Global::diffuseDayLight[1] = Global::diffuseLight[1];
                    Global::diffuseDayLight[2] = Global::diffuseLight[2];
                    Global::specularDayLight[0] = Global::specularLight[0]; // podobnie specular
                    Global::specularDayLight[1] = Global::specularLight[1];
                    Global::specularDayLight[2] = Global::specularLight[2];
                }
                else
                {
                    Global::diffuseDayLight[0] =
                        50 * H * Global::diffuseLight[0]; // wschód albo zachód
                    Global::diffuseDayLight[1] = 50 * H * Global::diffuseLight[1];
                    Global::diffuseDayLight[2] = 50 * H * Global::diffuseLight[2];
                    Global::specularDayLight[0] =
                        50 * H * Global::specularLight[0]; // podobnie specular
                    Global::specularDayLight[1] = 50 * H * Global::specularLight[1];
                    Global::specularDayLight[2] = 50 * H * Global::specularLight[2];
                }
            }
            else
            { // s³oñce pod horyzontem
                GLfloat lum = 3.1831 * (H > -0.314159 ? 0.314159 + H :
                                                        0.0); // po zachodzie ambient siê œciemnia
                Global::ambientDayLight[0] = lum * Global::ambientLight[0];
                Global::ambientDayLight[1] = lum * Global::ambientLight[1];
                Global::ambientDayLight[2] = lum * Global::ambientLight[2];
                Global::diffuseDayLight[0] =
                    Global::noLight[0]; // od zachodu do wschodu nie ma diffuse
                Global::diffuseDayLight[1] = Global::noLight[1];
                Global::diffuseDayLight[2] = Global::noLight[2];
                Global::specularDayLight[0] = Global::noLight[0]; // ani specular
                Global::specularDayLight[1] = Global::noLight[1];
                Global::specularDayLight[2] = Global::noLight[2];
            }
            // Calculate sky colour according to time of day.
            // GLfloat sin_t = sin(PI * time_of_day / 12.0);
            // back_red = 0.3 * (1.0 - sin_t);
            // back_green = 0.9 * sin_t;
            // back_blue = sin_t + 0.4, 1.0;
            // aktualizacja œwiate³
            glLightfv(GL_LIGHT0, GL_AMBIENT, Global::ambientDayLight);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, Global::diffuseDayLight);
            glLightfv(GL_LIGHT0, GL_SPECULAR, Global::specularDayLight);
        }
        Global::fLuminance = // to pos³u¿y równie¿ do zapalania latarñ
            +0.150 * (Global::diffuseDayLight[0] + Global::ambientDayLight[0]) // R
            + 0.295 * (Global::diffuseDayLight[1] + Global::ambientDayLight[1]) // G
            + 0.055 * (Global::diffuseDayLight[2] + Global::ambientDayLight[2]); // B
        if (Global::fMoveLight >= 0.0)
        { // przeliczenie koloru nieba
            vector3 sky = vector3(Global::AtmoColor[0], Global::AtmoColor[1], Global::AtmoColor[2]);
            if (Global::fLuminance < 0.25)
            { // przyspieszenie zachodu/wschodu
                sky *= 4.0 * Global::fLuminance; // nocny kolor nieba
                GLfloat fog[3];
                fog[0] = Global::FogColor[0] * 4.0 * Global::fLuminance;
                fog[1] = Global::FogColor[1] * 4.0 * Global::fLuminance;
                fog[2] = Global::FogColor[2] * 4.0 * Global::fLuminance;
                glFogfv(GL_FOG_COLOR, fog); // nocny kolor mg³y
            }
            else
                glFogfv(GL_FOG_COLOR, Global::FogColor); // kolor mg³y
            glClearColor(sky.x, sky.y, sky.z, 0.0); // kolor nieba
        }
    } // koniec dzia³añ niewykonywanych podczas pauzy
    // Console::Update(); //tu jest zale¿ne od FPS, co nie jest korzystne
    if (Global::bActive)
    { // obs³uga ruchu kamery tylko gdy okno jest aktywne

        if (Console::Pressed(VK_LBUTTON))
        {
        //if (!Console::Pressed(VK_CONTROL) ) Global::ffov = 45.0;
         //if ( Console::Pressed(VK_SHIFT)) SCR->ReSizeGLSceneEx(Global::ffov, Global::iWindowWidth, Global::iWindowHeight);
         if ( Console::Pressed(VK_SHIFT)) SCR->FOVADD(0.1f);
        }

        if (Console::Pressed(VK_RBUTTON))
        {
         if (Console::Pressed(VK_SHIFT)) SCR->FOVREM(0.1f);
        }
        
        if (Console::Pressed(VK_MBUTTON))
        {
            //WriteLog("VK_LBUTTON");
            Camera.Reset(); // likwidacja obrotów - patrzy horyzontalnie na po³udnie
            // if (!FreeFlyModeFlag) //jeœli wewn¹trz - patrzymy do ty³u
            // Camera.LookAt=Train->pMechPosition-Normalize(Train->GetDirection())*10;
            if (Controlled ? LengthSquared3(Controlled->GetPosition() - Camera.Pos) < 2250000 : false) // gdy bli¿ej ni¿ 1.5km
              {
                Camera.LookAt = Controlled->GetPosition();
                }
            else
            {
                TDynamicObject *d = Ground.DynamicNearest(Camera.Pos, 300); // szukaj w promieniu 300m
                if (!d)
                    d = Ground.DynamicNearest(Camera.Pos, 1000); // dalej szukanie, jesli bli¿ej nie ma
                if (d && pDynamicNearest) // jeœli jakiœ jest znaleziony wczeœniej
                    if (100.0 * LengthSquared3(d->GetPosition() - Camera.Pos) >
                        LengthSquared3(pDynamicNearest->GetPosition() - Camera.Pos))
                        d = pDynamicNearest; // jeœli najbli¿szy nie jest 10 razy bli¿ej ni¿
                // poprzedni najbli¿szy, zostaje poprzedni
                if (d)
                    pDynamicNearest = d; // zmiana na nowy, jeœli coœ znaleziony niepusty
                if (pDynamicNearest)
                    Camera.LookAt = pDynamicNearest->GetPosition();
            }
            if (FreeFlyModeFlag)
                Camera.RaLook(); // jednorazowe przestawienie kamery
        }
        else if (Console::Pressed(VK_RBUTTON) && !Console::Pressed(VK_SHIFT)) //||Console::Pressed(VK_F4))
            FollowView(false); // bez wyciszania dŸwiêków
        else if (Global::iTextMode == -1)
        { // tu mozna dodac dopisywanie do logu przebiegu lokomotywy
            WriteLog("Number of textures used: " + AnsiString(Global::iTextures));
            return false;
        }
        Camera.Update(); // uwzglêdnienie ruchu wywo³anego klawiszami
    } // koniec bloku pomijanego przy nieaktywnym oknie
    // poprzednie jakoœ tam dzia³a³o
    double dt = GetDeltaRenderTime(); // nie uwzglêdnia pauzowania ani mno¿enia czasu
    fTime50Hz +=
        dt; // w pauzie te¿ trzeba zliczaæ czas, bo przy du¿ym FPS bêdzie problem z odczytem ramek
    if (fTime50Hz >= 0.2)
        Console::Update(); // to i tak trzeba wywo³ywaæ
    dt = GetDeltaTime(); // 0.0 gdy pauza
    fTimeBuffer += dt; //[s] dodanie czasu od poprzedniej ramki
    if (fTimeBuffer >= fMaxDt) // jest co najmniej jeden krok; normalnie 0.01s
    { // Ra: czas dla fizyki jest skwantowany - fizykê lepiej przeliczaæ sta³ym krokiem
        // tak mo¿na np. moc silników itp., ale ruch musi byæ przeliczany w ka¿dej klatce, bo
        // inaczej skacze
        Global::tranTexts.Update(); // obiekt obs³uguj¹cy stenogramy dŸwiêków na ekranie
        Console::Update(); // obs³uga cykli PoKeys (np. aktualizacja wyjœæ analogowych)
        double iter =
            ceil(fTimeBuffer / fMaxDt); // ile kroków siê zmieœci³o od ostatniego sprawdzania?
        int n = int(iter); // ile kroków jako int
        fTimeBuffer -= iter * fMaxDt; // reszta czasu na potem (do bufora)
        if (n > 20)
            n = 20; // Ra: je¿eli FPS jest zatrwa¿aj¹co niski, to fizyka nie mo¿e zaj¹æ ca³kowicie
// procesora
#if 0
  Ground.UpdatePhys(fMaxDt,n); //Ra: teraz czas kroku jest (wzglêdnie) sta³y

  if (DebugModeFlag)
   if (Global::bActive) //nie przyspieszaæ, gdy jedzie w tle :)
    if (QGlobal::bSCNLOADED)
     if (GetAsyncKeyState(VK_ESCAPE)<0)
     {//yB doda³ przyspieszacz fizyki
      Ground.UpdatePhys(fMaxDt,n);
      Ground.UpdatePhys(fMaxDt,n);
      Ground.UpdatePhys(fMaxDt,n);
      Ground.UpdatePhys(fMaxDt,n); //w sumie 5 razy
     }
#endif
    }
    // awaria PoKeys mog³a w³¹czyæ pauzê - przekazaæ informacjê
    if (Global::iMultiplayer) // dajemy znaæ do serwera o wykonaniu
        if (iPause != Global::iPause)
        { // przes³anie informacji o pauzie do programu nadzoruj¹cego
            Ground.WyslijParam(5, 3); // ramka 5 z czasem i stanem zapauzowania
            iPause = Global::iPause;
        }
    double iter;
    int n = 1;
    if (dt > fMaxDt) // normalnie 0.01s
    {
        iter = ceil(dt / fMaxDt);
        n = iter;
        dt = dt / iter; // Ra: fizykê lepiej by by³o przeliczaæ ze sta³ym krokiem
        if (n > 20)
            n = 20; // McZapkie-081103: przesuniecie granicy FPS z 10 na 5
    }
    // else n=1;
    // blablabla
    // Ground.UpdatePhys(dt,n); //na razie tu //2014-12: yB przeniós³ do Ground.Update() :(
    Ground.Update(dt, n); // tu zrobiæ tylko coklatkow¹ aktualizacjê przesuniêæ
    if (DebugModeFlag)
        if (Global::bActive) // nie przyspieszaæ, gdy jedzie w tle :)
            if (GetAsyncKeyState(VK_ESCAPE) < 0)
            { // yB doda³ przyspieszacz fizyki
                Ground.Update(dt, n);
                Ground.Update(dt, n);
                Ground.Update(dt, n);
                Ground.Update(dt, n); // 5 razy
            }
    dt = GetDeltaTime(); // czas niekwantowany
    if (Camera.Type == tp_Follow)
    {
        if (Train)
        { // jeœli jazda w kabinie, przeliczyæ trzeba parametry kamery
            Train->UpdateMechPosition(dt /
                                      Global::fTimeSpeed); // ograniczyæ telepanie po przyspieszeniu
            vector3 tempangle;
            double modelrotate;
            tempangle =
                Controlled->VectorFront() * (Controlled->MoverParameters->ActiveCab == -1 ? -1 : 1);
            modelrotate = atan2(-tempangle.x, tempangle.z);
            if (Console::Pressed(VK_CONTROL) ? (Console::Pressed(Global::Keys[k_MechLeft]) ||
                                                Console::Pressed(Global::Keys[k_MechRight])) :
                                               false)
            { // jeœli lusterko lewe albo prawe (bez rzucania na razie)
                bool lr = Console::Pressed(Global::Keys[k_MechLeft]);
#if 0
    Camera.Pos=Train->MirrorPosition(lr); //robocza wartoœæ
    if (Controlled->MoverParameters->ActiveCab<0) lr=!lr; //w drugiej kabinie odwrotnie jest œrodek
    Camera.LookAt=Controlled->GetPosition()+vector3(lr?2.0:-2.0,Camera.Pos.y,0); //trochê na zewn¹trz, u¿yæ szerokoœci pojazdu
    //Camera.LookAt=Train->pMechPosition+Train->GetDirection()*Train->Dynamic()->MoverParameters->ActiveCab;
    Camera.Pos+=Controlled->GetPosition();
    //Camera.RaLook(); //jednorazowe przestawienie kamery
    Camera.Yaw=0; //odchylenie na bok od Camera.LookAt
#else
                // Camera.Yaw powinno byæ wyzerowane, aby po powrocie patrzeæ do przodu
                Camera.Pos =
                    Controlled->GetPosition() + Train->MirrorPosition(lr); // pozycja lusterka
                Camera.Yaw = 0; // odchylenie na bok od Camera.LookAt
                if (Train->Dynamic()->MoverParameters->ActiveCab == 0)
                    Camera.LookAt = Camera.Pos - Train->GetDirection(); // gdy w korytarzu
                else if (Console::Pressed(VK_SHIFT))
                { // patrzenie w bok przez szybê
                    Camera.LookAt = Camera.Pos -
                                    (lr ? -1 : 1) * Train->Dynamic()->VectorLeft() *
                                        Train->Dynamic()->MoverParameters->ActiveCab;
                    Global::SetCameraRotation(-modelrotate);
                }
                else
                { // patrzenie w kierunku osi pojazdu, z uwzglêdnieniem kabiny - jakby z lusterka,
                    // ale bez odbicia
                    Camera.LookAt = Camera.Pos -
                                    Train->GetDirection() *
                                        Train->Dynamic()->MoverParameters->ActiveCab; //-1 albo 1
                    Global::SetCameraRotation(M_PI -
                                              modelrotate); // tu ju¿ trzeba uwzglêdniæ lusterka
                }
#endif
                Camera.Roll =
                    atan(Train->pMechShake.x * Train->fMechRoll); // hustanie kamery na boki
                Camera.Pitch =
                    atan(Train->vMechVelocity.z * Train->fMechPitch); // hustanie kamery przod tyl
                Camera.vUp = Controlled->VectorUp();
            }
            else
            { // patrzenie standardowe
                Camera.Pos = Train->pMechPosition; // Train.GetPosition1();
                if (!Global::iPause)
                { // podczas pauzy nie przeliczaæ k¹tów przypadkowymi wartoœciami
                    Camera.Roll =
                        atan(Train->pMechShake.x * Train->fMechRoll); // hustanie kamery na boki
                    Camera.Pitch -= atan(Train->vMechVelocity.z *
                                         Train->fMechPitch); // hustanie kamery przod tyl //Ra: tu
                    // jest uciekanie kamery w górê!!!
                }
                // ABu011104: rzucanie pudlem
                vector3 temp;
                if (abs(Train->pMechShake.y) < 0.25)
                    temp = vector3(0, 0, 6 * Train->pMechShake.y);
                else if ((Train->pMechShake.y) > 0)
                    temp = vector3(0, 0, 6 * 0.25);
                else
                    temp = vector3(0, 0, -6 * 0.25);
                if (Controlled)
                    Controlled->ABuSetModelShake(temp);
                // ABu: koniec rzucania

                if (Train->Dynamic()->MoverParameters->ActiveCab == 0)
                    Camera.LookAt = Train->pMechPosition + Train->GetDirection(); // gdy w korytarzu
                else // patrzenie w kierunku osi pojazdu, z uwzglêdnieniem kabiny
                    Camera.LookAt = Train->pMechPosition +
                                    Train->GetDirection() *
                                        Train->Dynamic()->MoverParameters->ActiveCab; //-1 albo 1
                Camera.vUp = Train->GetUp();
                Global::SetCameraRotation(Camera.Yaw -
                                          modelrotate); // tu ju¿ trzeba uwzglêdniæ lusterka
            }
        }
    }
    else
    { // kamera nieruchoma
        Global::SetCameraRotation(Camera.Yaw - M_PI);
    }
    Ground.CheckQuery();
    // przy 0.25 smuga gaœnie o 6:37 w Quarku, a mog³aby ju¿ 5:40
    // Ra 2014-12: przy 0.15 siê skar¿yli, ¿e nie widaæ smug => zmieni³em na 0.25
    if (Train) // jeœli nie usuniêty
        Global::bSmudge =
            FreeFlyModeFlag ? false : ((Train->Dynamic()->fShade <= 0.0) ?
                                           (Global::fLuminance <= 0.25) :
                                           (Train->Dynamic()->fShade * Global::fLuminance <= 0.25));

    if (!Render())
        return false;

//**********************************************************************************************************
// Q: TU BYLO RENDEROWANIE KABINY, PRZENIOSLEM DO OSOBNEJ FUNKCJI I WOLAM Z TWorld::Render()


    if (DebugModeFlag && !Global::iTextMode)
    {
        OutText01 = "  FPS: ";
        OutText01 += FloatToStrF(GetFPS(), ffFixed, 6, 2);
        OutText01 += Global::iSlowMotion ? "s" : "n";

        OutText01 += (GetDeltaTime() >= 0.2) ? "!" : " ";
        // if (GetDeltaTime()>=0.2) //Ra: to za bardzo miota tekstem!
        // {
        //     OutText01+= " Slowing Down !!! ";
        // }
    }
    /*if (Console::Pressed(VK_F5))
       {Global::slowmotion=true;};
    if (Console::Pressed(VK_F6))
       {Global::slowmotion=false;};*/

    if (Global::iTextMode == VK_F8)
    {
        bool ctr = Controlled;
        OutText01 = "";
        OutText02 = "";
        OutText03 = "";
        OutText04 = "";
        OutText05 = "";
        OutText06 = "";
        OutText07 = "";
        OutText08 = "";
        OutText09 = "";
        OutText10 = "";
        OutText11 = "";
        OutText12 = "";
        Global::iViewMode = VK_F8;
        OutText01 = "FPS: ";
        OutText01 += FloatToStrF(GetFPS(), ffFixed, 6, 2);
        if (Global::iSlowMotion)
            OutText01 += " (slowmotion " + AnsiString(Global::iSlowMotion) + ")";
        OutText01 += ", sectors: ";
        OutText01 += AnsiString(Ground.iRendered);
if(ctr) OutText02 = "STATION NAME: " + Controlled->asStation;
if(ctr) OutText03 = "TRACK NUMBER: " + Controlled->asTrackNum;

        if (QModelInfo::bnearestobjengaged)
        {
        OutText04 += "OBJ INCF: " + QModelInfo::snearestobj + ", ";
        OutText05 += "OBJ E3DF: " + QModelInfo::sNI_file + ", ";
        OutText06 += "OBJ NODE: " + QModelInfo::sNI_name + ", ";
        OutText07 += "OBJ TYPE: " + QModelInfo::sNI_type + ", ";
        OutText08 += "OBJ TRIS: " + AnsiString(QModelInfo::iNI_numtri) + ", ";
        OutText09 += "OBJ SUBS: " + AnsiString(QModelInfo::iNI_submodels) + ", ";
        }
    }

    // if (Console::Pressed(VK_F7))
    //{
    //  OutText01=FloatToStrF(Controlled->MoverParameters->Couplers[0].CouplingFlag,ffFixed,2,0)+",
    //  ";
    //  OutText01+=FloatToStrF(Controlled->MoverParameters->Couplers[1].CouplingFlag,ffFixed,2,0);
    //}

    /*
    if (Console::Pressed(VK_F5))
       {
          int line=2;
          OutText01="Time: "+FloatToStrF(GlobalTime->hh,ffFixed,2,0)+":"
                  +FloatToStrF(GlobalTime->mm,ffFixed,2,0)+", ";
          OutText01+="distance: ";
          OutText01+="34.94";
          OutText02="Next station: ";
          OutText02+=FloatToStrF(Controlled->TrainParams->TimeTable[line].km,ffFixed,2,2)+" km, ";
          OutText02+=AnsiString(Controlled->TrainParams->TimeTable[line].StationName)+", ";
          OutText02+=AnsiString(Controlled->TrainParams->TimeTable[line].StationWare);
          OutText03="Arrival: ";
          if(Controlled->TrainParams->TimeTable[line].Ah==-1)
          {
             OutText03+="--:--";
          }
          else
          {
             OutText03+=FloatToStrF(Controlled->TrainParams->TimeTable[line].Ah,ffFixed,2,0)+":";
             OutText03+=FloatToStrF(Controlled->TrainParams->TimeTable[line].Am,ffFixed,2,0)+" ";
          }
          OutText03+=" Departure: ";
          OutText03+=FloatToStrF(Controlled->TrainParams->TimeTable[line].Dh,ffFixed,2,0)+":";
          OutText03+=FloatToStrF(Controlled->TrainParams->TimeTable[line].Dm,ffFixed,2,0)+" ";
       };
//    */
    /*
    if (Console::Pressed(VK_F6))
    {
       //GlobalTime->UpdateMTableTime(100);
       //OutText01=FloatToStrF(SquareMagnitude(Global::pCameraPosition-Controlled->GetPosition()),ffFixed,10,0);
       //OutText01=FloatToStrF(Global::TnijSzczegoly,ffFixed,7,0)+", ";
       //OutText01+=FloatToStrF(dta,ffFixed,2,4)+", ";
       OutText01+= FloatToStrF(GetFPS(),ffFixed,6,2);
       OutText01+= FloatToStrF(Global::ABuDebug,ffFixed,6,15);
    };
    */
    if (Global::changeDynObj)
    { // ABu zmiana pojazdu - przejœcie do innego
        // Ra: to nie mo¿e byæ tak robione, to zbytnia proteza jest
        Train->Silence(); // wy³¹czenie dŸwiêków opuszczanej kabiny
        if (Train->Dynamic()->Mechanik) // AI mo¿e sobie samo pójœæ
            if (!Train->Dynamic()->Mechanik->AIControllFlag) // tylko jeœli rêcznie prowadzony
            { // jeœli prowadzi AI, to mu nie robimy dywersji!
                Train->Dynamic()->MoverParameters->CabDeactivisation();
                Train->Dynamic()->Controller = AIdriver;
                // Train->Dynamic()->MoverParameters->SecuritySystem.Status=0; //rozwala CA w EZT
                Train->Dynamic()->MoverParameters->ActiveCab = 0;
                Train->Dynamic()->MoverParameters->BrakeLevelSet(-2);
                Train->Dynamic()->MechInside = false;
            }
        // int CabNr;
        TDynamicObject *temp = Global::changeDynObj;
        // CabNr=temp->MoverParameters->ActiveCab;
        /*
             if (Train->Dynamic()->MoverParameters->ActiveCab==-1)
             {
              temp=Train->Dynamic()->NextConnected; //pojazd od strony sprzêgu 1
              CabNr=(Train->Dynamic()->NextConnectedNo==0)?1:-1;
             }
             else
             {
              temp=Train->Dynamic()->PrevConnected; //pojazd od strony sprzêgu 0
              CabNr=(Train->Dynamic()->PrevConnectedNo==0)?1:-1;
             }
        */
        Train->Dynamic()->bDisplayCab = false;
        Train->Dynamic()->ABuSetModelShake(vector3(0, 0, 0));
        /// Train->Dynamic()->MoverParameters->LimPipePress=-1;
        /// Train->Dynamic()->MoverParameters->ActFlowSpeed=0;
        /// Train->Dynamic()->Mechanik->CloseLog();
        /// SafeDelete(Train->Dynamic()->Mechanik);

        // Train->Dynamic()->mdKabina=NULL;
        if (Train->Dynamic()->Mechanik) // AI mo¿e sobie samo pójœæ
            if (!Train->Dynamic()->Mechanik->AIControllFlag) // tylko jeœli rêcznie prowadzony
                Train->Dynamic()->Mechanik->MoveTo(temp); // przsuniêcie obiektu zarz¹dzaj¹cego
        // Train->DynamicObject=NULL;
        Train->DynamicSet(temp);
        Controlled = temp;
        mvControlled = Controlled->ControlledFind()->MoverParameters;
        Global::asHumanCtrlVehicle = Train->Dynamic()->GetName();
        if (Train->Dynamic()->Mechanik) // AI mo¿e sobie samo pójœæ
            if (!Train->Dynamic()->Mechanik->AIControllFlag) // tylko jeœli rêcznie prowadzony
            {
                Train->Dynamic()->MoverParameters->LimPipePress =
                    Controlled->MoverParameters->PipePress;
                // Train->Dynamic()->MoverParameters->ActFlowSpeed=0;
                // Train->Dynamic()->MoverParameters->SecuritySystem.Status=1;
                // Train->Dynamic()->MoverParameters->ActiveCab=CabNr;
                Train->Dynamic()
                    ->MoverParameters->CabActivisation(); // za³¹czenie rozrz¹du (wirtualne kabiny)
                Train->Dynamic()->Controller = Humandriver;
                Train->Dynamic()->MechInside = true;
                // Train->Dynamic()->Mechanik=new
                // TController(l,r,Controlled->Controller,&Controlled->MoverParameters,&Controlled->TrainParams,Aggressive);
                // Train->InitializeCab(CabNr,Train->Dynamic()->asBaseDir+Train->Dynamic()->MoverParameters->TypeName+".mmd");
            }
        Train->InitializeCab(Train->Dynamic()->MoverParameters->CabNo,
                             Train->Dynamic()->asBaseDir +
                                 Train->Dynamic()->MoverParameters->TypeName + ".mmd");
        if (!FreeFlyModeFlag)
        {
            Global::pUserDynamic = Controlled; // renerowanie wzglêdem kamery
            Train->Dynamic()->bDisplayCab = true;
            Train->Dynamic()->ABuSetModelShake(
                vector3(0, 0, 0)); // zerowanie przesuniêcia przed powrotem?
            Train->MechStop();
            FollowView(); // na pozycjê mecha
        }
        Global::changeDynObj = NULL;
    }

    glDisable(GL_LIGHTING);
    if (Controlled)
        SetWindowText(hWnd, AnsiString(Controlled->MoverParameters->Name).c_str());
    else
        SetWindowText(hWnd, Global::szSceneryFile); // nazwa scenerii
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -0.50f);

    if (Global::iTextMode == VK_F1)
    { // tekst pokazywany po wciœniêciu [F1]
        // Global::iViewMode=VK_F1;
        glColor3f(1.0f, 1.0f, 1.0f); // a, damy bia³ym
        OutText01 = "Time: " + AnsiString((int)GlobalTime->hh) + ":";
        int i = GlobalTime->mm; // bo inaczej potrafi zrobiæ "hh:010"
        if (i < 10)
            OutText01 += "0";
        OutText01 += AnsiString(i); // minuty
        OutText01 += ":";
        i = floor(GlobalTime->mr); // bo inaczej potrafi zrobiæ "hh:mm:010"
        if (i < 10)
            OutText01 += "0";
        OutText01 += AnsiString(i);
        if (Global::iPause)
            OutText01 += " - paused";
        if (Controlled)
            if (Controlled->Mechanik)
            {
                OutText02 = Controlled->Mechanik->Relation();
                if (!OutText02.IsEmpty()) // jeœli jest podana relacja, to dodajemy punkt nastêpnego
                    // zatrzymania
                    OutText02 =
                        Global::Bezogonkow(OutText02 + ": -> " + Controlled->Mechanik->NextStop(),true); // dopisanie punktu zatrzymania
            }

            
      if (Controlled)
      {
        OutText03 = "STATION NAME: " + Controlled->asStation + ", TOR " + Controlled->asTrackNum;

        if (QGlobal::iSTATIONPOSINTAB != -1)
        {
          OutText04 += "ST Name     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].Name;
          OutText05 += "ST Info     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].Info;
          OutText06 += "ST Type     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].Type;
          OutText07 += "ST SubT     : " + QGlobal::station[QGlobal::iSTATIONPOSINTAB].SubType;
          OutText08 += "ST Platforms: " + IntToStr(QGlobal::station[QGlobal::iSTATIONPOSINTAB].platforms);
          OutText09 += "ST edges    : " + IntToStr(QGlobal::station[QGlobal::iSTATIONPOSINTAB].platformedges);
          OutText10 += "ST tracks n : " + IntToStr(QGlobal::station[QGlobal::iSTATIONPOSINTAB].tracksnum);
        }
      }

        // double CtrlPos=mvControlled->MainCtrlPos;
        // double CtrlPosNo=mvControlled->MainCtrlPosNo;
        // OutText02="defrot="+FloatToStrF(1+0.4*(CtrlPos/CtrlPosNo),ffFixed,2,5);
        //OutText03 = ""; // Pomoc w sterowaniu - [F9]";
        // OutText03=AnsiString(Global::pCameraRotationDeg); //k¹t kamery wzglêdem pó³nocy
    }
    else if (Global::iTextMode == VK_F12)
    { // opcje w³¹czenia i wy³¹czenia logowania
        OutText01 = "[0] Debugmode " + AnsiString(DebugModeFlag ? "(on)" : "(off)");
        OutText02 = "[1] log.txt " + AnsiString((Global::iWriteLogEnabled & 1) ? "(on)" : "(off)");
        OutText03 = "[2] Console " + AnsiString((Global::iWriteLogEnabled & 2) ? "(on)" : "(off)");
    }
    else if (Global::iTextMode == VK_F2)
    { // ABu: info dla najblizszego pojazdu!
        TDynamicObject *tmp = FreeFlyModeFlag ? Ground.DynamicNearest(Camera.Pos) :
                                                Controlled; // w trybie latania lokalizujemy wg mapy
        if (tmp)
        {
            if (Global::iScreenMode[Global::iTextMode - VK_F1] == 0)
            { // jeœli domyœlny ekran po pierwszym naciœniêciu
                OutText03 = "";
                OutText01 = "Vehicle name: " + AnsiString(tmp->MoverParameters->Name);
                // yB       OutText01+="; d:  "+FloatToStrF(tmp->ABuGetDirection(),ffFixed,2,0);
                // OutText01=FloatToStrF(tmp->MoverParameters->Couplers[0].CouplingFlag,ffFixed,3,2)+",
                // ";
                // OutText01+=FloatToStrF(tmp->MoverParameters->Couplers[1].CouplingFlag,ffFixed,3,2);
                if (tmp->Mechanik) // jeœli jest prowadz¹cy
                { // ostatnia komenda dla AI
                    OutText01 += ", command: " + tmp->Mechanik->OrderCurrent();
                }
                else if (tmp->ctOwner)
                    OutText01 += ", owned by " + AnsiString(tmp->ctOwner->OwnerName());
                if (!tmp->MoverParameters->CommandLast.IsEmpty())
                    OutText01 += AnsiString(", put: ") + tmp->MoverParameters->CommandLast;
                // OutText01+="; Cab="+AnsiString(tmp->MoverParameters->CabNo);
                OutText02 = "Damage status: " +
                           tmp->MoverParameters->EngineDescription(0); //+" Engine status: ";
                OutText02 += "; Brake delay: ";
                if ((tmp->MoverParameters->BrakeDelayFlag & bdelay_G) == bdelay_G)
                    OutText02 += "G";
                if ((tmp->MoverParameters->BrakeDelayFlag & bdelay_P) == bdelay_P)
                    OutText02 += "P";
                if ((tmp->MoverParameters->BrakeDelayFlag & bdelay_R) == bdelay_R)
                    OutText02 += "R";
                if ((tmp->MoverParameters->BrakeDelayFlag & bdelay_M) == bdelay_M)
                    OutText02 += "+Mg";
                OutText02 += AnsiString(", BTP:") +
                            FloatToStrF(tmp->MoverParameters->LoadFlag, ffFixed, 5, 0);
                // if ((tmp->MoverParameters->EnginePowerSource.SourceType==CurrentCollector) ||
                // (tmp->MoverParameters->TrainType==dt_EZT))
                {
                    OutText02 += AnsiString("; pant. ") +
                                FloatToStrF(tmp->MoverParameters->PantPress, ffFixed, 8, 2);
                    OutText02 += (tmp->MoverParameters->bPantKurek3 ? "<ZG" : "|ZG");
                }

                //          OutText02+=AnsiString(",
                //          u:")+FloatToStrF(tmp->MoverParameters->u,ffFixed,3,3);
                //          OutText02+=AnsiString(",
                //          N:")+FloatToStrF(tmp->MoverParameters->Ntotal,ffFixed,4,0);
                OutText02 +=
                    AnsiString(", Ft:") + FloatToStrF(tmp->MoverParameters->Ft, ffFixed, 4, 0);
                //          OutText03= AnsiString("BP:
                //          ")+FloatToStrF(tmp->MoverParameters->BrakePress,ffFixed,5,2)+AnsiString(",
                //          ");
                //          OutText03+= AnsiString("PP:
                //          ")+FloatToStrF(tmp->MoverParameters->PipePress,ffFixed,5,2)+AnsiString(",
                //          ");
                //          OutText03+= AnsiString("BVP:
                //          ")+FloatToStrF(tmp->MoverParameters->Volume,ffFixed,5,3)+AnsiString(",
                //          ");
                //          OutText03+=
                //          FloatToStrF(tmp->MoverParameters->CntrlPipePress,ffFixed,5,3)+AnsiString(",
                //          ");
                //          OutText03+=
                //          FloatToStrF(tmp->MoverParameters->Hamulec->GetCRP(),ffFixed,5,3)+AnsiString(",
                //          ");
                //          OutText03+=
                //          FloatToStrF(tmp->MoverParameters->BrakeStatus,ffFixed,5,0)+AnsiString(",
                //          ");
                //          OutText03+= AnsiString("HP:
                //          ")+FloatToStrF(tmp->MoverParameters->ScndPipePress,ffFixed,5,2)+AnsiString(".
                //          ");
                //      OutText02+=
                //      FloatToStrF(tmp->MoverParameters->CompressorPower,ffFixed,5,0)+AnsiString(",
                //      ");
                // yB      if(tmp->MoverParameters->BrakeSubsystem==Knorr) OutText02+=" Knorr";
                // yB      if(tmp->MoverParameters->BrakeSubsystem==Oerlikon) OutText02+=" Oerlikon";
                // yB      if(tmp->MoverParameters->BrakeSubsystem==Hik) OutText02+=" Hik";
                // yB      if(tmp->MoverParameters->BrakeSubsystem==WeLu) OutText02+=" £estingha³s";
                // OutText02= " GetFirst:
                // "+AnsiString(tmp->GetFirstDynamic(1)->MoverParameters->Name)+" Damage
                // status="+tmp->MoverParameters->EngineDescription(0)+" Engine status: ";
                // OutText02+= " GetLast:
                // "+AnsiString(tmp->GetLastDynamic(1)->MoverParameters->Name)+" Damage
                // status="+tmp->MoverParameters->EngineDescription(0)+" Engine status: ";
                OutText03 = AnsiString("BP: ") +
                           FloatToStrF(tmp->MoverParameters->BrakePress, ffFixed, 5, 2) +
                           AnsiString(", ");
                OutText03 += FloatToStrF(tmp->MoverParameters->BrakeStatus, ffFixed, 5, 0) +
                            AnsiString(", ");
                OutText03 += AnsiString("PP: ") +
                            FloatToStrF(tmp->MoverParameters->PipePress, ffFixed, 5, 2) +
                            AnsiString("/");
                OutText03 += FloatToStrF(tmp->MoverParameters->ScndPipePress, ffFixed, 5, 2) +
                            AnsiString("/");
                OutText03 += FloatToStrF(tmp->MoverParameters->EqvtPipePress, ffFixed, 5, 2) +
                            AnsiString(", ");
                OutText03 += AnsiString("BVP: ") +
                            FloatToStrF(tmp->MoverParameters->Volume, ffFixed, 5, 3) +
                            AnsiString(", ");
                OutText03 += FloatToStrF(tmp->MoverParameters->CntrlPipePress, ffFixed, 5, 3) +
                            AnsiString(", ");
                OutText03 += FloatToStrF(tmp->MoverParameters->Hamulec->GetCRP(), ffFixed, 5, 3) +
                            AnsiString(", ");
                OutText03 += FloatToStrF(tmp->MoverParameters->BrakeStatus, ffFixed, 5, 0) +
                            AnsiString(", ");
                //      OutText03+=AnsiString("BVP:
                //      ")+FloatToStrF(tmp->MoverParameters->BrakeVP(),ffFixed,5,2)+AnsiString(",
                //      ");

                //      OutText03+=FloatToStrF(tmp->MoverParameters->CntrlPipePress,ffFixed,5,2)+AnsiString(",
                //      ");
                //      OutText03+=FloatToStrF(tmp->MoverParameters->HighPipePress,ffFixed,5,2)+AnsiString(",
                //      ");
                //      OutText03+=FloatToStrF(tmp->MoverParameters->LowPipePress,ffFixed,5,2)+AnsiString(",
                //      ");

                if (tmp->MoverParameters->ManualBrakePos > 0)
                    OutText03 += AnsiString("manual brake active. ");
                else if (tmp->MoverParameters->LocalBrakePos > 0)
                    OutText03 += AnsiString("local brake active. ");
                else
                    OutText03 += AnsiString("local brake inactive. ");
                /*
                       //OutText03+=AnsiString("LSwTim:
                   ")+FloatToStrF(tmp->MoverParameters->LastSwitchingTime,ffFixed,5,2);
                       //OutText03+=AnsiString(" Physic:
                   ")+FloatToStrF(tmp->MoverParameters->PhysicActivation,ffFixed,5,2);
                       //OutText03+=AnsiString(" ESF:
                   ")+FloatToStrF(tmp->MoverParameters->EndSignalsFlag,ffFixed,5,0);
                       OutText03+=AnsiString(" dPAngF: ")+FloatToStrF(tmp->dPantAngleF,ffFixed,5,0);
                       OutText03+=AnsiString(" dPAngFT:
                   ")+FloatToStrF(-(tmp->PantTraction1*28.9-136.938),ffFixed,5,0);
                       if (tmp->lastcabf==1)
                       {
                        OutText03+=AnsiString(" pcabc1:
                   ")+FloatToStrF(tmp->MoverParameters->PantFrontUp,ffFixed,5,0);
                        OutText03+=AnsiString(" pcabc2:
                   ")+FloatToStrF(tmp->MoverParameters->PantRearUp,ffFixed,5,0);
                       }
                       if (tmp->lastcabf==-1)
                       {
                        OutText03+=AnsiString(" pcabc1:
                   ")+FloatToStrF(tmp->MoverParameters->PantRearUp,ffFixed,5,0);
                        OutText03+=AnsiString(" pcabc2:
                   ")+FloatToStrF(tmp->MoverParameters->PantFrontUp,ffFixed,5,0);
                       }
                */
                OutText04 = "";
                if (tmp->Mechanik)
                { // o ile jest ktoœ w œrodku
                    // OutText04=tmp->Mechanik->StopReasonText();
                    // if (!OutText04.IsEmpty()) OutText04+="; "; //aby ³adniejszy odstêp by³
                    // if (Controlled->Mechanik && (Controlled->Mechanik->AIControllFlag==AIdriver))
                    AnsiString flags = "bwaccmlshhhoibsgvdp; "; // flagi AI (definicja w Driver.h)
                    for (int i = 0, j = 1; i < 19; ++i, j <<= 1)
                        if (tmp->Mechanik->DrivigFlags() & j) // jak bit ustawiony
                            flags[i + 1] ^= 0x20; // to zmiana na wielk¹ literê
                    OutText04 = flags;
					OutText04 +=
						AnsiString("Driver: Vd=") +
						FloatToStrF(tmp->Mechanik->VelDesired, ffFixed, 4, 0) + AnsiString(" ad=") +
						FloatToStrF(tmp->Mechanik->AccDesired, ffFixed, 5, 2) + AnsiString(" Pd=") +
						FloatToStrF(tmp->Mechanik->ActualProximityDist, ffFixed, 4, 0) +
						AnsiString(" Vn=") + FloatToStrF(tmp->Mechanik->VelNext, ffFixed, 4, 0) +
						AnsiString(" VSm=") + FloatToStrF(tmp->Mechanik->VelSignalLast, ffFixed, 4, 0) +
						AnsiString(" VLm=") + FloatToStrF(tmp->Mechanik->VelLimitLast, ffFixed, 4, 0) +
						AnsiString(" VRd=") + FloatToStrF(tmp->Mechanik->VelRoad, ffFixed, 4, 0);
                    if (tmp->Mechanik->VelNext == 0.0)
                        if (tmp->Mechanik->eSignNext)
                        { // jeœli ma zapamiêtany event semafora
                            // if (!OutText04.IsEmpty()) OutText04+=", "; //aby ³adniejszy odstêp by³
                            OutText04 += " (" +
                                        Global::Bezogonkow(tmp->Mechanik->eSignNext->asName) +
                                        ")"; // nazwa eventu semafora
                        }
                }
                if (!OutText04.IsEmpty())
                    OutText04 += "; "; // aby ³adniejszy odstêp by³
                // informacja o sprzêgach nawet bez mechanika
                OutText04 +=
                    "C0=" + (tmp->PrevConnected ?
                                 tmp->PrevConnected->GetName() + ":" +
                                     AnsiString(tmp->MoverParameters->Couplers[0].CouplingFlag) :
                                 AnsiString("NULL"));
                OutText04 +=
                    " C1=" + (tmp->NextConnected ?
                                  tmp->NextConnected->GetName() + ":" +
                                      AnsiString(tmp->MoverParameters->Couplers[1].CouplingFlag) :
                                  AnsiString("NULL"));
                if (Console::Pressed(VK_F2))
                {
                    WriteLog(OutText01);
                    WriteLog(OutText02);
                    WriteLog(OutText03);
                    WriteLog(OutText04);
                }
            } // koniec treœci podstawowego ekranu FK_V2
            else
            { // ekran drugi, czyli tabelka skanowania AI
                if (tmp->Mechanik) //¿eby by³a tabelka, musi byæ AI
                { // tabelka jest na u¿ytek testuj¹cych scenerie, wiêc nie musi byæ "³adna"
                    glColor3f(1.0f, 1.0f, 1.0f); // a, damy zielony. GF: jednak bia³y
                    // glTranslatef(0.0f,0.0f,-0.50f);
                    glRasterPos2f(-0.25f, 0.20f);
                    // OutText01="Scan distance: "+AnsiString(tmp->Mechanik->scanmax)+", back:
                    // "+AnsiString(tmp->Mechanik->scanback);
                    OutText01 = "Time: " + AnsiString((int)GlobalTime->hh) + ":";
                    int i = GlobalTime->mm; // bo inaczej potrafi zrobiæ "hh:010"
                    if (i < 10)
                        OutText01 += "0";
                    OutText01 += AnsiString(i); // minuty
                    OutText01 += ":";
                    i = floor(GlobalTime->mr); // bo inaczej potrafi zrobiæ "hh:mm:010"
                    if (i < 10)
                        OutText01 += "0";
                    OutText01 += AnsiString(i);
                    OutText01 +=
                        AnsiString(". Vel: ") + FloatToStrF(tmp->GetVelocity(), ffFixed, 6, 1);
                    OutText01 += ". Scan table:";
                    glPrint(Global::Bezogonkow(OutText01).c_str());
                    i = -1;
                    while ((OutText01 = tmp->Mechanik->TableText(++i)) != "")
                    { // wyœwietlenie pozycji z tabelki
                        glRasterPos2f(-0.25f, 0.19f - 0.01f * i);
                        glPrint(Global::Bezogonkow(OutText01).c_str());
                    }
                    // podsumowanie sensu tabelki
                    OutText04 =
                        AnsiString("Driver: Vd=") +
                        FloatToStrF(tmp->Mechanik->VelDesired, ffFixed, 4, 0) + AnsiString(" ad=") +
                        FloatToStrF(tmp->Mechanik->AccDesired, ffFixed, 5, 2) + AnsiString(" Pd=") +
                        FloatToStrF(tmp->Mechanik->ActualProximityDist, ffFixed, 4, 0) +
                        AnsiString(" Vn=") + FloatToStrF(tmp->Mechanik->VelNext, ffFixed, 4, 0) +
						AnsiString("\n VSm=") + FloatToStrF(tmp->Mechanik->VelSignalLast, ffFixed, 4, 0) +
						AnsiString(" VLm=") + FloatToStrF(tmp->Mechanik->VelLimitLast, ffFixed, 4, 0) +
						AnsiString(" VRd=") + FloatToStrF(tmp->Mechanik->VelRoad, ffFixed, 4, 0) +
						AnsiString(" VSig=") + FloatToStrF(tmp->Mechanik->VelSignal, ffFixed, 4, 0);
                    if (tmp->Mechanik->VelNext == 0.0)
                        if (tmp->Mechanik->eSignNext)
                        { // jeœli ma zapamiêtany event semafora
                            // if (!OutText04.IsEmpty()) OutText04+=", "; //aby ³adniejszy odstêp by³
                            OutText04 += " (" +
                                        Global::Bezogonkow(tmp->Mechanik->eSignNext->asName) +
                                        ")"; // nazwa eventu semafora
                        }
                    glRasterPos2f(-0.25f, 0.19f - 0.01f * i);
                    glPrint(Global::Bezogonkow(OutText04).c_str());
                }
            } // koniec ekanu skanowania
        } // koniec obs³ugi, gdy mamy wskaŸnik do pojazdu
        else
        { // wyœwietlenie wspó³rzêdnych w scenerii oraz k¹ta kamery, gdy nie mamy wskaŸnika
            OutText01 = "Camera position: " + FloatToStrF(Camera.Pos.x, ffFixed, 6, 2) + " " +
                       FloatToStrF(Camera.Pos.y, ffFixed, 6, 2) + " " +
                       FloatToStrF(Camera.Pos.z, ffFixed, 6, 2);
            OutText01 += ", azimuth: " +
                        FloatToStrF(180.0 - RadToDeg(Camera.Yaw), ffFixed, 3,
                                    0); // ma byæ azymut, czyli 0 na pó³nocy i roœnie na wschód
            OutText01 +=
                " " +
                AnsiString("S SEE NEN NWW SW")
                    .SubString(1 + 2 * floor(fmod(8 + (Camera.Yaw + 0.5 * M_PI_4) / M_PI_4, 8)), 2);
        }
        // OutText03= AnsiString("  Online documentation (PL, ENG, DE, soon CZ):
        // http://www.eu07.pl");
        // OutText03="enrot="+FloatToStrF(Controlled->MoverParameters->enrot,ffFixed,6,2);
        // OutText03="; n="+FloatToStrF(Controlled->MoverParameters->n,ffFixed,6,2);
    } // koniec treœci podstawowego ekranu FK_V2
    else if (Global::iTextMode == VK_F5)
    { // przesiadka do innego pojazdu
        if (FreeFlyModeFlag) // jeœli tryb latania
        {
            TDynamicObject *tmp = Ground.DynamicNearest(Camera.Pos, 50, true); //³apiemy z obsad¹
            if (tmp)
                if (tmp != Controlled)
                {
                    if (Controlled) // jeœli mielismy pojazd
                        if (Controlled->Mechanik) // na skutek jakiegoœ b³êdu mo¿e czasem znikn¹æ
                            Controlled->Mechanik->TakeControl(true); // oddajemy dotychczasowy AI
                    if (DebugModeFlag ? true : tmp->MoverParameters->Vel <= 5.0)
                    {
                        Controlled = tmp; // przejmujemy nowy
                        mvControlled = Controlled->ControlledFind()->MoverParameters;
                        if (Train)
                            Train->Silence(); // wyciszenie dŸwiêków opuszczanego pojazdu
                        else
                            Train = new TTrain(); // jeœli niczym jeszcze nie jeŸdzilismy
                        if (Train->Init(Controlled))
                        { // przejmujemy sterowanie
                            if (!DebugModeFlag) // w DebugMode nadal prowadzi AI
                                Controlled->Mechanik->TakeControl(false);
                        }
                        else
                            SafeDelete(Train); // i nie ma czym sterowaæ
                        // Global::pUserDynamic=Controlled; //renerowanie pojazdu wzglêdem kabiny
                        // Global::iTextMode=VK_F4;
                        if (Train)
                            InOutKey(); // do kabiny
                    }
                }
            Global::iTextMode = 0; // tryb neutralny
        }
        /*

             OutText01=OutText02=OutText03=OutText04="";
             AnsiString flag[10]={"vmax", "tory", "smfr", "pjzd", "mnwr", "pstk", "brak", "brak",
           "brak", "brak"};
             if(tmp)
             if(tmp->Mechanik)
             {
              for(int i=0;i<15;i++)
              {
               int tmppar=floor(tmp->Mechanik->ProximityTable[i].Vel);
               OutText02+=(tmppar<1000?(tmppar<100?((tmppar<10)&&(tmppar>=0)?"   ":"  "):"
           "):"")+IntToStr(tmppar)+" ";
               tmppar=floor(tmp->Mechanik->ProximityTable[i].Dist);
               OutText03+=(tmppar<1000?(tmppar<100?((tmppar<10)&&(tmppar>=0)?"   ":"  "):"
           "):"")+IntToStr(tmppar)+" ";
               OutText01+=flag[tmp->Mechanik->ProximityTable[i].Flag]+" ";
              }
              for(int i=0;i<6;i++)
              {
               int tmppar=floor(tmp->Mechanik->ReducedTable[i]);
               OutText04+=flag[i]+":"+(tmppar<1000?(tmppar<100?((tmppar<10)&&(tmppar>=0)?"   ":"
           "):" "):"")+IntToStr(tmppar)+" ";
              }
             }
        */
    }
//    else if (Global::iTextMode == VK_F10)
//    { // tu mozna dodac dopisywanie do logu przebiegu lokomotywy
//        // Global::iViewMode=VK_F10;
//        // return false;
//        OutText01 = AnsiString("To quit press [Y] key.");
//        OutText03 = AnsiString("Aby zakonczyc program, przycisnij klawisz [Y].");
//        Global::iPause = true;
//    }
    else if (Controlled && DebugModeFlag && !Global::iTextMode)
    {
        OutText01 += AnsiString(";  vel ") + FloatToStrF(Controlled->GetVelocity(), ffFixed, 6, 2);
        OutText01 += AnsiString(";  pos ") + FloatToStrF(Controlled->GetPosition().x, ffFixed, 6, 2);
        OutText01 += AnsiString(" ; ") + FloatToStrF(Controlled->GetPosition().y, ffFixed, 6, 2);
        OutText01 += AnsiString(" ; ") + FloatToStrF(Controlled->GetPosition().z, ffFixed, 6, 2);
        OutText01 += AnsiString("; dist=") +
                    FloatToStrF(Controlled->MoverParameters->DistCounter, ffFixed, 8, 4);

        // double a= acos( DotProduct(Normalize(Controlled->GetDirection()),vWorldFront));
        //      OutText0+= AnsiString(";   angle ")+FloatToStrF(a/M_PI*180,ffFixed,6,2);
        OutText01 += AnsiString("; d_omega ") +
                    FloatToStrF(Controlled->MoverParameters->dizel_engagedeltaomega, ffFixed, 6, 3);
        OutText02 = AnsiString("HamZ=") +
                   FloatToStrF(Controlled->MoverParameters->fBrakeCtrlPos, ffFixed, 6, 1);
        OutText02 += AnsiString("; HamP=") + AnsiString(Controlled->MoverParameters->LocalBrakePos);
        // mvControlled->MainCtrlPos;
        // if (mvControlled->MainCtrlPos<0)
        //    OutText02+= AnsiString("; nastawnik 0");
        //      if (mvControlled->MainCtrlPos>iPozSzereg)
        OutText02 += AnsiString("; NasJ=") + AnsiString(mvControlled->MainCtrlPos);
        //      else
        //          OutText02+= AnsiString("; nastawnik S") + mvControlled->MainCtrlPos;
        OutText02 += AnsiString("(") + AnsiString(mvControlled->MainCtrlActualPos);

        OutText02 += AnsiString("); NasB=") + AnsiString(mvControlled->ScndCtrlPos);
        OutText02 += AnsiString("(") + AnsiString(mvControlled->ScndCtrlActualPos);
        if (mvControlled->TrainType == dt_EZT)
            OutText02 += AnsiString("); I=") + AnsiString(int(mvControlled->ShowCurrent(0)));
        else
            OutText02 += AnsiString("); I=") + AnsiString(int(mvControlled->Im));
        // OutText02+=AnsiString(";
        // I2=")+FloatToStrF(Controlled->NextConnected->MoverParameters->Im,ffFixed,6,2);
        OutText02 += AnsiString("; U=") +
                    AnsiString(int(mvControlled->RunningTraction.TractionVoltage + 0.5));
        // OutText02+=AnsiString("; rvent=")+FloatToStrF(mvControlled->RventRot,ffFixed,6,2);
        OutText02 += AnsiString("; R=") +
                    FloatToStrF(Controlled->MoverParameters->RunningShape.R, ffFixed, 4, 1);
        OutText02 += AnsiString(" An=") + FloatToStrF(Controlled->MoverParameters->AccN, ffFixed, 4,
                                                     2); // przyspieszenie poprzeczne
        OutText02 += AnsiString("; As=") + FloatToStrF(Controlled->MoverParameters->AccS, ffFixed, 4,
                                                      2); // przyspieszenie wzd³u¿ne
        // OutText02+=AnsiString("; P=")+FloatToStrF(mvControlled->EnginePower,ffFixed,6,1);
        OutText03 += AnsiString("cyl.ham. ") +
                    FloatToStrF(Controlled->MoverParameters->BrakePress, ffFixed, 5, 2);
        OutText03 += AnsiString("; prz.gl. ") +
                    FloatToStrF(Controlled->MoverParameters->PipePress, ffFixed, 5, 2);
        OutText03 += AnsiString("; zb.gl. ") +
                    FloatToStrF(Controlled->MoverParameters->CompressedVolume, ffFixed, 6, 2);
        // youBy - drugi wezyk
        OutText03 += AnsiString("; p.zas. ") +
                    FloatToStrF(Controlled->MoverParameters->ScndPipePress, ffFixed, 6, 2);

        if (Controlled->MoverParameters->EngineType == ElectricInductionMotor)
        {
            // glTranslatef(0.0f,0.0f,-0.50f);
            glColor3f(1.0f, 1.0f, 1.0f); // a, damy bia³ym
            for (int i = 0; i <= 20; i++)
            {
                glRasterPos2f(-0.25f, 0.16f - 0.01f * i);
                if (Controlled->MoverParameters->eimc[i] < 10)
                    OutText04 = FloatToStrF(Controlled->MoverParameters->eimc[i], ffFixed, 6, 3);
                else
                    OutText04 = FloatToStrF(Controlled->MoverParameters->eimc[i], ffGeneral, 5, 3);
                glPrint(OutText04.c_str());
            }
            for (int i = 0; i <= 20; i++)
            {
                glRasterPos2f(-0.2f, 0.16f - 0.01f * i);
                if (Controlled->MoverParameters->eimv[i] < 10)
                    OutText04 = FloatToStrF(Controlled->MoverParameters->eimv[i], ffFixed, 6, 3);
                else
                    OutText04 = FloatToStrF(Controlled->MoverParameters->eimv[i], ffGeneral, 5, 3);
                glPrint(OutText04.c_str());
            }
            OutText04 = "";
            // glTranslatef(0.0f,0.0f,+0.50f);
            glColor3f(1.0f, 0.0f, 0.0f); // a, damy czerwonym
        }

        // ABu: testy sprzegow-> (potem przeniesc te zmienne z public do protected!)
        // OutText03+=AnsiString("; EnginePwr=")+FloatToStrF(mvControlled->EnginePower,ffFixed,1,5);
        // OutText03+=AnsiString("; nn=")+FloatToStrF(Controlled->NextConnectedNo,ffFixed,1,0);
        // OutText03+=AnsiString("; PR=")+FloatToStrF(Controlled->dPantAngleR,ffFixed,3,0);
        // OutText03+=AnsiString("; PF=")+FloatToStrF(Controlled->dPantAngleF,ffFixed,3,0);
        // if(Controlled->bDisplayCab==true)
        // OutText03+=AnsiString("; Wysw. kab");//+Controlled->mdKabina->GetSMRoot()->Name;
        // else
        // OutText03+=AnsiString("; test:")+AnsiString(Controlled->MoverParameters->TrainType[1]);

        // OutText03+=FloatToStrF(Train->Dynamic()->MoverParameters->EndSignalsFlag,ffFixed,3,0);;

        // OutText03+=FloatToStrF(Train->Dynamic()->MoverParameters->EndSignalsFlag&byte(((((1+Train->Dynamic()->MoverParameters->CabNo)/2)*30)+2)),ffFixed,3,0);;

        // OutText03+=AnsiString(";
        // Ftmax=")+FloatToStrF(Controlled->MoverParameters->Ftmax,ffFixed,3,0);
        // OutText03+=AnsiString(";
        // FTotal=")+FloatToStrF(Controlled->MoverParameters->FTotal/1000.0f,ffFixed,3,2);
        // OutText03+=AnsiString(";
        // FTrain=")+FloatToStrF(Controlled->MoverParameters->FTrain/1000.0f,ffFixed,3,2);
        // Controlled->mdModel->GetSMRoot()->SetTranslate(vector3(0,1,0));

        // McZapkie: warto wiedziec w jakim stanie sa przelaczniki
        if (mvControlled->ConvOvldFlag)
            OutText03 += " C! ";
        else if (mvControlled->FuseFlag)
            OutText03 += " F! ";
        else if (!mvControlled->Mains)
            OutText03 += " () ";
        else
            switch (mvControlled->ActiveDir * (mvControlled->Imin == mvControlled->IminLo ? 1 : 2))
            {
            case 2:
            {
                OutText03 += " >> ";
                break;
            }
            case 1:
            {
                OutText03 += " -> ";
                break;
            }
            case 0:
            {
                OutText03 += " -- ";
                break;
            }
            case -1:
            {
                OutText03 += " <- ";
                break;
            }
            case -2:
            {
                OutText03 += " << ";
                break;
            }
            }
        // OutText03+=AnsiString("; dpLocal
        // ")+FloatToStrF(Controlled->MoverParameters->dpLocalValve,ffFixed,10,8);
        // OutText03+=AnsiString("; dpMain
        // ")+FloatToStrF(Controlled->MoverParameters->dpMainValve,ffFixed,10,8);
        // McZapkie: predkosc szlakowa
        if (Controlled->MoverParameters->RunningTrack.Velmax == -1)
        {
            OutText03 += AnsiString(" Vtrack=Vmax");
        }
        else
        {
            OutText03 +=
                AnsiString(" Vtrack ") +
                FloatToStrF(Controlled->MoverParameters->RunningTrack.Velmax, ffFixed, 8, 2);
        }
        //      WriteLog(Controlled->MoverParameters->TrainType.c_str());
        if ((mvControlled->EnginePowerSource.SourceType == CurrentCollector) ||
            (mvControlled->TrainType == dt_EZT))
        {
            OutText03 +=
                AnsiString("; pant. ") + FloatToStrF(mvControlled->PantPress, ffFixed, 8, 2);
            OutText03 += (mvControlled->bPantKurek3 ? "=ZG" : "|ZG");
        }
        // McZapkie: komenda i jej parametry
        if (Controlled->MoverParameters->CommandIn.Command != AnsiString(""))
            OutText04 = AnsiString("C:") +
                       AnsiString(Controlled->MoverParameters->CommandIn.Command) +
                       AnsiString(" V1=") +
                       FloatToStrF(Controlled->MoverParameters->CommandIn.Value1, ffFixed, 5, 0) +
                       AnsiString(" V2=") +
                       FloatToStrF(Controlled->MoverParameters->CommandIn.Value2, ffFixed, 5, 0);
        if (Controlled->Mechanik && (Controlled->Mechanik->AIControllFlag == AIdriver))
            OutText04 +=
                AnsiString("AI: Vd=") +
                FloatToStrF(Controlled->Mechanik->VelDesired, ffFixed, 4, 0) + AnsiString(" ad=") +
                FloatToStrF(Controlled->Mechanik->AccDesired, ffFixed, 5, 2) + AnsiString(" Pd=") +
                FloatToStrF(Controlled->Mechanik->ActualProximityDist, ffFixed, 4, 0) +
                AnsiString(" Vn=") + FloatToStrF(Controlled->Mechanik->VelNext, ffFixed, 4, 0);
    }

    // ABu 150205: prosty help, zeby sie na forum nikt nie pytal, jak ma ruszyc :)

    if (Global::detonatoryOK)
    {
        // if (Console::Pressed(VK_F9)) ShowHints(); //to nie dzia³a prawid³owo - prosili wy³¹czyæ
        if (Global::iTextMode == VK_F9)
        { // informacja o wersji, sposobie wyœwietlania i b³êdach OpenGL
            // Global::iViewMode=VK_F9;
            OutText01 = Global::asVersion; // informacja o wersji
            OutText02 = AnsiString("Rendering mode: ") + (Global::bUseVBO ? "VBO" : "Display Lists");
            if (Global::iMultiplayer)
                OutText02 += ". Multiplayer is active";
            OutText02 += ".";
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
            {
                OutText03 = "OpenGL error " + AnsiString(err) + ": " +
                           Global::Bezogonkow(AnsiString((char *)gluErrorString(err)));
            }
        }
        if (Global::iTextMode == VK_F3)
        { // wyœwietlenie rozk³adu jazdy, na razie jakkolwiek
            TDynamicObject *tmp = FreeFlyModeFlag ?
                                      Ground.DynamicNearest(Camera.Pos) :
                                      Controlled; // w trybie latania lokalizujemy wg mapy
            Mtable::TTrainParameters *tt = NULL;
            if (tmp)
                if (tmp->Mechanik)
                {
                    tt = tmp->Mechanik->Timetable();
                    if (tt) // musi byæ rozk³ad
                    { // wyœwietlanie rozk³adu
                        glColor3f(1.0f, 1.0f, 1.0f); // a, damy bia³ym
                        // glTranslatef(0.0f,0.0f,-0.50f);
                        glRasterPos2f(-0.25f, 0.20f);
                        OutText01 = tmp->Mechanik->Relation() + " (" +
                                   tmp->Mechanik->Timetable()->TrainName + ")";
                        glPrint(Global::Bezogonkow(OutText01, true).c_str());
                        glRasterPos2f(-0.25f, 0.19f);
                        // glPrint("|============================|=======|=======|=====|");
                        // glPrint("| Posterunek                 | Przyj.| Odjazd| Vmax|");
                        // glPrint("|============================|=======|=======|=====|");
                        glPrint("|----------------------------|-------|-------|-----|");
                        TMTableLine *t;
                        for (int i = tmp->Mechanik->iStationStart; i <= tt->StationCount; ++i)
                        { // wyœwietlenie pozycji z rozk³adu
                            t = tt->TimeTable + i; // linijka rozk³adu
                            OutText01 = AnsiString(AnsiString(t->StationName) +
                                                  "                          ").SubString(1, 26);
                            OutText02 = (t->Ah >= 0) ?
                                           AnsiString(int(100 + t->Ah)).SubString(2, 2) + ":" +
                                               AnsiString(int(100 + t->Am)).SubString(2, 2) :
                                           AnsiString("     ");
                            OutText03 = (t->Dh >= 0) ?
                                           AnsiString(int(100 + t->Dh)).SubString(2, 2) + ":" +
                                               AnsiString(int(100 + t->Dm)).SubString(2, 2) :
                                           AnsiString("     ");
                            OutText04 = "   " + FloatToStrF(t->vmax, ffFixed, 3, 0);
                            OutText04 = OutText04.SubString(OutText04.Length() - 2,
                                                          3); // z wyrównaniem do prawej
                            // if (AnsiString(t->StationWare).Pos("@"))
                            OutText01 = "| " + OutText01 + " | " + OutText02 + " | " + OutText03 +
                                       " | " + OutText04 + " | " + AnsiString(t->StationWare);
                            glRasterPos2f(-0.25f,
                                          0.18f - 0.02f * (i - tmp->Mechanik->iStationStart));
                            if ((tmp->Mechanik->iStationStart < tt->StationIndex) ?
                                    (i < tt->StationIndex) :
                                    false)
                            { // czas min¹³ i odjazd by³, to nazwa stacji bêdzie na zielono
                                glColor3f(0.0f, 1.0f, 0.0f); // zielone
                                glRasterPos2f(
                                    -0.25f,
                                    0.18f - 0.02f * (i - tmp->Mechanik->iStationStart)); // dopiero
                                // ustawienie
                                // pozycji
                                // ustala
                                // kolor,
                                // dziwne...
                                glPrint(Global::Bezogonkow(OutText01, true).c_str());
                                glColor3f(1.0f, 1.0f, 1.0f); // a reszta bia³ym
                            }
                            else // normalne wyœwietlanie, bez zmiany kolorów
                                glPrint(Global::Bezogonkow(OutText01, true).c_str());
                            glRasterPos2f(-0.25f,
                                          0.17f - 0.02f * (i - tmp->Mechanik->iStationStart));
                            glPrint("|----------------------------|-------|-------|-----|");
                        }
                    }
                }
            OutText01 = OutText02 = OutText03 = OutText04 = "";
        }
        else if (OutText01 != "")
        { // ABu: i od razu czyszczenie tego, co bylo napisane
            // glTranslatef(0.0f,0.0f,-0.50f);
            if (Global::iViewMode == VK_F8)       glColor3f(1.0f, 0.8f, 0.1f);
            glRasterPos2f(-0.25f, 0.20f);
            glPrint(OutText01.c_str());
            OutText01 = "";
            if (OutText02 != "")
            {
                glRasterPos2f(-0.25f, 0.19f);
                glPrint(OutText02.c_str());
                OutText02 = "";
            }
            if (OutText03 != "")
            {
                glRasterPos2f(-0.25f, 0.18f);
                glPrint(OutText03.c_str());
                OutText03 = "";
            if (OutText04 != "")
                {
                    glRasterPos2f(-0.25f, 0.17f);
                    glPrint(OutText04.c_str());
                    OutText04 = "";
                }
            if (OutText05 != "")
                {
                    glRasterPos2f(-0.25f, 0.16f);
                    glPrint(OutText05.c_str());
                    OutText05 = "";
                }
            if (OutText06 != "")
                {
                    glRasterPos2f(-0.25f, 0.15f);
                    glPrint(OutText06.c_str());
                    OutText06 = "";
                }
            if (OutText07 != "")
                {
                    glRasterPos2f(-0.25f, 0.14f);
                    glPrint(OutText07.c_str());
                    OutText07 = "";
                }
            if (OutText08 != "")
                {
                    glRasterPos2f(-0.25f, 0.13f);
                    glPrint(OutText08.c_str());
                    OutText08 = "";
                }
            if (OutText09 != "")
                {
                    glRasterPos2f(-0.25f, 0.12f);
                    glPrint(OutText09.c_str());
                    OutText09 = "";
                }
            if (OutText10 != "")
                {
                    glRasterPos2f(-0.25f, 0.11f);
                    glPrint(OutText10.c_str());
                    OutText10 = "";
                }
            if (OutText11 != "")
                {
                    glRasterPos2f(-0.25f, 0.10f);
                    glPrint(OutText11.c_str());
                    OutText11 = "";
                }
            if (OutText12 != "")
                {
                    glRasterPos2f(-0.25f, 0.09f);
                    glPrint(OutText12.c_str());
                    OutText12 = "";
                }
            }
        }
        // if ((Global::iTextMode!=VK_F3))
        { // stenogramy dŸwiêków (ukryæ, gdy tabelka skanowania lub rozk³ad?)
            glColor3f(1.0f, 1.0f, 0.0f); //¿ó³te
            for (int i = 0; i < 5; ++i)
            { // kilka linijek
                if (Global::asTranscript[i].IsEmpty())
                    break; // dalej nie trzeba
                else
                {
                    glRasterPos2f(-0.20f, -0.05f - 0.01f * i);
                    glPrint(Global::Bezogonkow(Global::asTranscript[i]).c_str());
                }
            }
        }
    }
    // if (Global::iViewMode!=Global::iTextMode)
    //{//Ra: taka maksymalna prowizorka na razie
    // WriteLog("Pressed function key F"+AnsiString(Global::iViewMode-111));
    // Global::iTextMode=Global::iViewMode;
    //}
    glEnable(GL_LIGHTING);
    return (true);
};


// *****************************************************************************
// PIERWSZA FUNKCJA NA DRODZE RENDERINGU SCENY - Wywolywana z TWorld::Update()
// *****************************************************************************
bool TWorld::Render()
{
    glColor3b(255, 255, 255);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(TSCREEN::CFOV, (GLdouble)Global::iWindowWidth/(GLdouble)Global::iWindowHeight, 0.05f, 19961.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, Global::iWindowWidth, Global::iWindowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    Camera.SetMatrix(); // ustawienie macierzy kamery wzglêdem pocz¹tku scenerii
    glLightfv(GL_LIGHT0, GL_POSITION, Global::lightPos);

    if (!Global::bWireFrame)
    { // bez nieba w trybie rysowania linii
        glDisable(GL_FOG);
        Clouds.Render();
        glEnable(GL_FOG);
    }

    if (QGlobal::bmodelpreview) DRAW_XYGRID();
    if (QGlobal::bmodelpreview) Draw_SCENE000(0, 0, 0);

    if (Global::bUseVBO)                                                        // renderowanie przez VBO
    {
        if (!Ground.RenderVBO(Camera.Pos)) return false;
        if (!Ground.RenderAlphaVBO(Camera.Pos)) return false;
    }
    else                                                                        // renderowanie przez DL
    {
        if (!Ground.RenderDL(Camera.Pos)) return false;
        if (!Ground.RenderAlphaDL(Camera.Pos)) return false;
    }


    TSubModel::iInstance = (int)(Train ? Train->Dynamic() : 0);                 //¿eby nie robiæ cudzych animacji

    if (Train) Train->Update();

    if (!FreeFlyModeFlag) RenderCab(false);  // RENDEROWANIE KABINY GDY W KABINIE, RENDEROWANIE W TRYBIE FREEFLY REALIZOWANE JEST W DYNOBJ.CPP

    if (!FreeFlyModeFlag)
    if (QGlobal::bSHOWBRIEFING) RenderLoader(QGlobal::glHDC, 77, "Zakonczono wczytywanie, wcisnij spacjê");

    if (QGlobal::bscrnoise ||
        QGlobal::bTUTORIAL ||
        QGlobal::bKEYBOARD ||
        QGlobal::bSHOWBRIEFING ||
        QGlobal::bscrfilter ||
        QGlobal::bEXITQUERY ||
        QGlobal::infotype > 0 ||
        QGlobal::mousemode){

        switch2dRender();

        if (QGlobal::bEXITQUERY) RenderEXITQUERY(0.30f);
        if (QGlobal::infotype > 0) RenderINFOPANEL(QGlobal::infotype, 0.25f);

        if (QGlobal::bscrfilter) RenderFILTER(0.20f);                           // WYSYPUJE SIE NA TYM ZARAZ NA STARCIE, CZEMU?!
        if (QGlobal::bscrnoise) drawNoise(1, QGlobal::fnoisealpha);             // W efects2d.cpp
     }

     
    if ((Console::Pressed(VK_DELETE)) || (Console::Pressed(VK_INSERT)))
     if (mvControlled) Controlled->GetConsist_f(1, Controlled);                 // Q 040116: Tworzenie listy pojazdow w skladzie, liczenie masy brutto i dlugosci


    // Global::bReCompile=false; //Ra: ju¿ zrobiona rekompilacja
    ResourceManager::Sweep(Timer::GetSimulationTime());

    glFlush();

    return true;
};


// *****************************************************************************
// RenderCab()
// *****************************************************************************

bool TWorld::RenderCab(bool colormode)
{

    if (Train)
    { // rendering kabiny gdy jest oddzielnym modelem i ma byc wyswietlana
        glPushMatrix();
        // ABu: Rendering kabiny jako ostatniej, zeby bylo widac przez szyby, tylko w widoku ze
        // srodka
        if ((Train->Dynamic()->mdKabina != Train->Dynamic()->mdModel) &&
            Train->Dynamic()->bDisplayCab && !FreeFlyModeFlag)
        {
            vector3 pos = Train->Dynamic()->GetPosition(); // wszpó³rzêdne pojazdu z kabin¹
            // glTranslatef(pos.x,pos.y,pos.z); //przesuniêcie o wektor (tak by³o i trzês³o)
            // aby pozbyæ siê choæ trochê trzêsienia, trzeba by nie przeliczaæ kabiny do punktu
            // zerowego scenerii
            glLoadIdentity(); // zacz¹æ od macierzy jedynkowej
            Camera.SetCabMatrix(pos); // widok z kamery po przesuniêciu
            glMultMatrixd(Train->Dynamic()->mMatrix.getArray()); // ta macierz nie ma przesuniêcia

            //*yB: moje smuuugi 1
            if (Global::bSmudge)
            { // Ra: uwzglêdni³em zacienienie pojazdu przy zapalaniu smug
                // 1. warunek na smugê wyznaczyc wczeœniej
                // 2. jeœli smuga w³¹czona, nie renderowaæ pojazdu u¿ytkownika w DynObj
                // 3. jeœli smuga w³aczona, wyrenderowaæ pojazd u¿ytkownia po dodaniu smugi do sceny
                if (Train->Controlled()->Battery)
                { // trochê na skróty z t¹ bateri¹
                    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE);
                    //    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_DST_COLOR);
                    //    glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
                    glDisable(GL_DEPTH_TEST);
                    glDisable(GL_LIGHTING);
                    glDisable(GL_FOG);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                    glBindTexture(GL_TEXTURE_2D, light); // Select our texture
                    glBegin(GL_QUADS);
                    float fSmudge =
                        Train->Dynamic()->MoverParameters->DimHalf.y + 7; // gdzie zaczynaæ smugê
                    if (Train->Controlled()->iLights[0] & 21)
                    { // wystarczy jeden zapalony z przodu
                        glTexCoord2f(0, 0);
                        glVertex3f(15.0, 0.0, +fSmudge); // rysowanie wzglêdem po³o¿enia modelu
                        glTexCoord2f(1, 0);
                        glVertex3f(-15.0, 0.0, +fSmudge);
                        glTexCoord2f(1, 1);
                        glVertex3f(-15.0, 2.5, 250.0);
                        glTexCoord2f(0, 1);
                        glVertex3f(15.0, 2.5, 250.0);
                    }
                    if (Train->Controlled()->iLights[1] & 21)
                    { // wystarczy jeden zapalony z ty³u
                        glTexCoord2f(0, 0);
                        glVertex3f(-15.0, 0.0, -fSmudge);
                        glTexCoord2f(1, 0);
                        glVertex3f(15.0, 0.0, -fSmudge);
                        glTexCoord2f(1, 1);
                        glVertex3f(15.0, 2.5, -250.0);
                        glTexCoord2f(0, 1);
                        glVertex3f(-15.0, 2.5, -250.0);
                    }
                    glEnd();

                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glEnable(GL_DEPTH_TEST);
                    // glEnable(GL_LIGHTING); //i tak siê w³¹czy potem
                    glEnable(GL_FOG);
                }
                glEnable(GL_LIGHTING); // po renderowaniu smugi jest to wy³¹czone
                // Ra: pojazd u¿ytkownika nale¿a³o by renderowaæ po smudze, aby go nie rozœwietla³a
                Global::bSmudge = false; // aby model u¿ytkownika siê teraz wyrenderowa³
                Train->Dynamic()->Render();
                Train->Dynamic()->RenderAlpha(); // przezroczyste fragmenty pojazdów na torach
            } // yB: moje smuuugi 1 - koniec*/
            else
                glEnable(GL_LIGHTING); // po renderowaniu drutów mo¿e byæ to wy³¹czone

            if (Train->Dynamic()->mdKabina) // bo mog³a znikn¹æ przy przechodzeniu do innego pojazdu
            { // oswietlenie kabiny
                GLfloat ambientCabLight[4] = {0.5f, 0.5f, 0.5f, 1.0f};
                GLfloat diffuseCabLight[4] = {0.5f, 0.5f, 0.5f, 1.0f};
                GLfloat specularCabLight[4] = {0.5f, 0.5f, 0.5f, 1.0f};
                for (int li = 0; li < 3; li++)
                { // przyciemnienie standardowe
                    ambientCabLight[li] = Global::ambientDayLight[li] * 0.9;
                    diffuseCabLight[li] = Global::diffuseDayLight[li] * 0.5;
                    specularCabLight[li] = Global::specularDayLight[li] * 0.5;
                }
                switch (Train->Dynamic()->MyTrack->eEnvironment)
                { // wp³yw œwiet³a zewnêtrznego
                case e_canyon:
                {
                    for (int li = 0; li < 3; li++)
                    {
                        diffuseCabLight[li] *= 0.6;
                        specularCabLight[li] *= 0.7;
                    }
                }
                break;
                case e_tunnel:
                {
                    for (int li = 0; li < 3; li++)
                    {
                        ambientCabLight[li] *= 0.3;
                        diffuseCabLight[li] *= 0.1;
                        specularCabLight[li] *= 0.2;
                    }
                }
                break;
                }
                switch (Train->iCabLightFlag) // Ra: uzele¿nic od napiêcia w obwodzie sterowania
                { // hunter-091012: uzaleznienie jasnosci od przetwornicy
                case 0: //œwiat³o wewnêtrzne zgaszone
                    break;
                case 1: //œwiat³o wewnêtrzne przygaszone (255 216 176)
                    if (Train->Dynamic()->MoverParameters->ConverterFlag ==
                        true) // jasnosc dla zalaczonej przetwornicy
                    {
                        ambientCabLight[0] = Max0R(0.700, ambientCabLight[0]) * 0.75; // R
                        ambientCabLight[1] = Max0R(0.593, ambientCabLight[1]) * 0.75; // G
                        ambientCabLight[2] = Max0R(0.483, ambientCabLight[2]) * 0.75; // B

                        for (int i = 0; i < 3; i++)
                            if (ambientCabLight[i] <= (Global::ambientDayLight[i] * 0.9))
                                ambientCabLight[i] = Global::ambientDayLight[i] * 0.9;
                    }
                    else
                    {
                        ambientCabLight[0] = Max0R(0.700, ambientCabLight[0]) * 0.375; // R
                        ambientCabLight[1] = Max0R(0.593, ambientCabLight[1]) * 0.375; // G
                        ambientCabLight[2] = Max0R(0.483, ambientCabLight[2]) * 0.375; // B

                        for (int i = 0; i < 3; i++)
                            if (ambientCabLight[i] <= (Global::ambientDayLight[i] * 0.9))
                                ambientCabLight[i] = Global::ambientDayLight[i] * 0.9;
                    }
                    break;
                case 2: //œwiat³o wewnêtrzne zapalone (255 216 176)
                    if (Train->Dynamic()->MoverParameters->ConverterFlag ==
                        true) // jasnosc dla zalaczonej przetwornicy
                    {
                        ambientCabLight[0] = Max0R(1.000, ambientCabLight[0]); // R
                        ambientCabLight[1] = Max0R(0.847, ambientCabLight[1]); // G
                        ambientCabLight[2] = Max0R(0.690, ambientCabLight[2]); // B

                        for (int i = 0; i < 3; i++)
                            if (ambientCabLight[i] <= (Global::ambientDayLight[i] * 0.9))
                                ambientCabLight[i] = Global::ambientDayLight[i] * 0.9;
                    }
                    else
                    {
                        ambientCabLight[0] = Max0R(1.000, ambientCabLight[0]) * 0.5; // R
                        ambientCabLight[1] = Max0R(0.847, ambientCabLight[1]) * 0.5; // G
                        ambientCabLight[2] = Max0R(0.690, ambientCabLight[2]) * 0.5; // B

                        for (int i = 0; i < 3; i++)
                            if (ambientCabLight[i] <= (Global::ambientDayLight[i] * 0.9))
                                ambientCabLight[i] = Global::ambientDayLight[i] * 0.9;
                    }
                    break;
                }
                glLightfv(GL_LIGHT0, GL_AMBIENT, ambientCabLight);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseCabLight);
                glLightfv(GL_LIGHT0, GL_SPECULAR, specularCabLight);
                if (Global::bUseVBO)
                { // renderowanie z u¿yciem VBO
                    Train->Dynamic()->mdKabina->RaRender(0.0, Train->Dynamic()->ReplacableSkinID,
                                                         Train->Dynamic()->iAlpha);
                    Train->Dynamic()->mdKabina->RaRenderAlpha(
                        0.0, Train->Dynamic()->ReplacableSkinID, Train->Dynamic()->iAlpha);
                }
                else
                { // renderowanie z Display List
                    Train->Dynamic()->mdKabina->Render(0.0, Train->Dynamic()->ReplacableSkinID,
                                                       Train->Dynamic()->iAlpha);
                    Train->Dynamic()->mdKabina->RenderAlpha(0.0, Train->Dynamic()->ReplacableSkinID,
                                                            Train->Dynamic()->iAlpha);
                }
                // przywrócenie standardowych, bo zawsze s¹ zmieniane
                glLightfv(GL_LIGHT0, GL_AMBIENT, Global::ambientDayLight);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, Global::diffuseDayLight);
                glLightfv(GL_LIGHT0, GL_SPECULAR, Global::specularDayLight);
            }
        } // koniec: if (Train->Dynamic()->mdKabina)
        glPopMatrix();
        //**********************************************************************************************************
    } // koniec: if (Train)
    
  return true;
}


// *****************************************************************************
// TAKI SE TUTORIAL DLA POCZATKUJACYCH
// *****************************************************************************
void TWorld::ShowHints(void)
{ // Ra: nie u¿ywaæ tego, bo Ÿle dzia³a
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor4f(0.3f, 1.0f, 0.3f, 1.0f);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -0.50f);
    // glRasterPos2f(-0.25f, 0.20f);
    // OutText01="Uruchamianie lokomotywy - pomoc dla niezaawansowanych";
    // glPrint(OutText01.c_str());

    // if(TestFlag(Controlled->MoverParameters->SecuritySystem.Status,s_ebrake))
    // hunter-091012
    if (TestFlag(Controlled->MoverParameters->SecuritySystem.Status, s_SHPebrake) ||
        TestFlag(Controlled->MoverParameters->SecuritySystem.Status, s_CAebrake))
    {
        OutText01 = "Gosciu, ale refleks to ty masz szachisty. Teraz zaczekaj.";
        OutText02 = "W tej sytuacji czuwak mozesz zbic dopiero po zatrzymaniu pociagu. ";
        if (Controlled->MoverParameters->Vel == 0)
            OutText03 = "   (mozesz juz nacisnac spacje)";
    }
    else
        // if(TestFlag(Controlled->MoverParameters->SecuritySystem.Status,s_alarm))
        if (TestFlag(Controlled->MoverParameters->SecuritySystem.Status, s_CAalarm) ||
            TestFlag(Controlled->MoverParameters->SecuritySystem.Status, s_SHPalarm))
    {
        OutText01 = "Natychmiast zbij czuwak, bo pociag sie zatrzyma!";
        OutText02 = "   (szybko nacisnij spacje!)";
    }
    else if (TestFlag(Controlled->MoverParameters->SecuritySystem.Status, s_aware))
    {
        OutText01 = "Zbij czuwak, zeby udowodnic, ze nie spisz :) ";
        OutText02 = "   (nacisnij spacje)";
    }
    else if (mvControlled->FuseFlag)
    {
        OutText01 = "Czlowieku, delikatniej troche! Gdzie sie spieszysz?";
        OutText02 = "Wybilo Ci bezpiecznik nadmiarowy, teraz musisz wlaczyc go ponownie.";
        OutText03 = "   ('N', wczesniej nastawnik i boczniki na zero -> '-' oraz '*' do oporu)";
    }
    else if (mvControlled->V == 0)
    {
        if ((mvControlled->PantFrontVolt == 0.0) || (mvControlled->PantRearVolt == 0.0))
        {
            OutText01 = "Jezdziles juz kiedys lokomotywa? Pierwszy raz? Dobra, to zaczynamy.";
            OutText02 = "No to co, trzebaby chyba podniesc pantograf?";
            OutText03 = "   (wcisnij 'shift+P' - przedni, 'shift+O' - tylny)";
        }
        else if (!mvControlled->Mains)
        {
            OutText01 = "Dobra, mozemy zalaczyc wylacznik szybki lokomotywy.";
            OutText02 = "   (wcisnij 'shift+M')";
        }
        else if (!mvControlled->ConverterAllow)
        {
            OutText01 = "Teraz wlacz przetwornice.";
            OutText02 = "   (wcisnij 'shift+X')";
        }
        else if (!mvControlled->CompressorAllow)
        {
            OutText01 = "Teraz wlacz sprezarke.";
            OutText02 = "   (wcisnij 'shift+C')";
        }
        else if (mvControlled->ActiveDir == 0)
        {
            OutText01 = "Ustaw nastawnik kierunkowy na kierunek, w ktorym chcesz jechac.";
            OutText02 = "   ('d' - do przodu, 'r' - do tylu)";
        }
        else if (Controlled->GetFirstDynamic(1)->MoverParameters->BrakePress > 0)
        {
            OutText01 = "Odhamuj sklad i zaczekaj az Ci powiem - to moze troche potrwac.";
            OutText02 = "   ('.' na klawiaturze numerycznej)";
        }
        else if (Controlled->MoverParameters->BrakeCtrlPos != 0)
        {
            OutText01 = "Przelacz kran hamulca w pozycje 'jazda'.";
            OutText02 = "   ('4' na klawiaturze numerycznej)";
        }
        else if (mvControlled->MainCtrlPos == 0)
        {
            OutText01 = "Teraz juz mozesz ruszyc ustawiajac pierwsza pozycje na nastawniku jazdy.";
            OutText02 = "   (jeden raz '+' na klawiaturze numerycznej)";
        }
        else if ((mvControlled->MainCtrlPos > 0) && (mvControlled->ShowCurrent(1) != 0))
        {
            OutText01 = "Dobrze, mozesz teraz wlaczac kolejne pozycje nastawnika.";
            OutText02 = "   ('+' na klawiaturze numerycznej, tylko z wyczuciem)";
        }
        if ((mvControlled->MainCtrlPos > 1) && (mvControlled->ShowCurrent(1) == 0))
        {
            OutText01 = "Spieszysz sie gdzies? Zejdz nastawnikiem na zero i probuj jeszcze raz!";
            OutText02 = "   (teraz do oporu '-' na klawiaturze numerycznej)";
        }
    }
    else
    {
        OutText01 = "Aby przyspieszyc mozesz wrzucac kolejne pozycje nastawnika.";
        if (mvControlled->MainCtrlPos == 28)
        {
            OutText01 = "Przy tym ustawienu mozesz bocznikowac silniki - sprobuj: '/' i '*' ";
        }
        if (mvControlled->MainCtrlPos == 43)
        {
            OutText01 = "Przy tym ustawienu mozesz bocznikowac silniki - sprobuj: '/' i '*' ";
        }

        OutText02 = "Aby zahamowac zejdz nastawnikiem do 0 ('-' do oporu) i ustaw kran hamulca";
        OutText03 =
            "w zaleznosci od sily hamowania, jakiej potrzebujesz ('2', '5' lub '8' na kl. num.)";

        // else
        // if() OutText01="teraz mozesz ruszyc naciskajac jeden raz '+' na klawiaturze numerycznej";
        // else
        // if() OutText01="teraz mozesz ruszyc naciskajac jeden raz '+' na klawiaturze numerycznej";
    }
    // OutText03=FloatToStrF(Controlled->MoverParameters->SecuritySystem.Status,ffFixed,3,0);

    if (OutText01 != "")
    {
        glRasterPos2f(-0.25f, 0.19f);
        glPrint(OutText01.c_str());
        OutText01 = "";
    }
    if (OutText02 != "")
    {
        glRasterPos2f(-0.25f, 0.18f);
        glPrint(OutText02.c_str());
        OutText02 = "";
    }
    if (OutText03 != "")
    {
        glRasterPos2f(-0.25f, 0.17f);
        glPrint(OutText03.c_str());
        OutText03 = "";
    }
};


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void TWorld::OnCommandGet(DaneRozkaz *pRozkaz)
{ // odebranie komunikatu z serwera
    if (pRozkaz->iSygn == 'EU07')
        switch (pRozkaz->iComm)
        {
        case 0: // odes³anie identyfikatora wersji
            Ground.WyslijString(Global::asVersion, 0); // przedsatwienie siê
            break;
        case 1: // odes³anie identyfikatora wersji
            Ground.WyslijString(Global::szSceneryFile, 1); // nazwa scenerii
            break;
        case 2: // event
            if (Global::iMultiplayer)
            { // WriteLog("Komunikat: "+AnsiString(pRozkaz->Name1));
                TEvent *e = Ground.FindEvent(
                    AnsiString(pRozkaz->cString + 1, (unsigned)(pRozkaz->cString[0])));
                if (e)
                    if ((e->Type == tp_Multiple) || (e->Type == tp_Lights) ||
                        bool(e->evJoined)) // tylko jawne albo niejawne Multiple
                        Ground.AddToQuery(e, NULL); // drugi parametr to dynamic wywo³uj¹cy - tu
                // brak
            }
            break;
        case 3: // rozkaz dla AI
            if (Global::iMultiplayer)
            {
                int i =
                    int(pRozkaz->cString[8]); // d³ugoœæ pierwszego ³añcucha (z przodu dwa floaty)
                TGroundNode *t = Ground.DynamicFind(
                    AnsiString(pRozkaz->cString + 11 + i,
                               (unsigned)pRozkaz->cString[10 + i])); // nazwa pojazdu jest druga
                if (t)
                    if (t->DynamicObject->Mechanik)
                    {
                        t->DynamicObject->Mechanik->PutCommand(AnsiString(pRozkaz->cString + 9, i),
                                                               pRozkaz->fPar[0], pRozkaz->fPar[1],
                                                               NULL, stopExt); // floaty s¹ z przodu
                        WriteLog("AI command: " + AnsiString(pRozkaz->cString + 9, i));
                    }
            }
            break;
        case 4: // badanie zajêtoœci toru
        {
            TGroundNode *t = Ground.FindGroundNode(
                AnsiString(pRozkaz->cString + 1, (unsigned)(pRozkaz->cString[0])), TP_TRACK);
            if (t)
                if (t->pTrack->IsEmpty())
                    Ground.WyslijWolny(t->asName);
        }
        break;
        case 5: // ustawienie parametrów
        {
            if (*pRozkaz->iPar & 1) // ustawienie czasu
            {
                double t = pRozkaz->fPar[1];
                GlobalTime->dd = floor(t); // niby nie powinno byæ dnia, ale...
                if (Global::fMoveLight >= 0)
                    Global::fMoveLight = t; // trzeba by deklinacjê S³oñca przeliczyæ
                GlobalTime->hh = floor(24 * t) - 24.0 * GlobalTime->dd;
                GlobalTime->mm =
                    floor(60 * 24 * t) - 60.0 * (24.0 * GlobalTime->dd + GlobalTime->hh);
                GlobalTime->mr =
                    floor(60 * 60 * 24 * t) -
                    60.0 * (60.0 * (24.0 * GlobalTime->dd + GlobalTime->hh) + GlobalTime->mm);
            }
            if (*pRozkaz->iPar & 2)
            { // ustawienie flag zapauzowania
                Global::iPause = pRozkaz->fPar[2]; // zak³adamy, ¿e wysy³aj¹cy wie, co robi
            }
        }
        break;
        case 6: // pobranie parametrów ruchu pojazdu
            if (Global::iMultiplayer)
            { // Ra 2014-12: to ma dzia³aæ równie¿ dla pojazdów bez obsady
                if (pRozkaz->cString[0]) // jeœli d³ugoœæ nazwy jest niezerowa
                { // szukamy pierwszego pojazdu o takiej nazwie i odsy³amy parametry ramk¹ #7
                    TGroundNode *t;
                    if (pRozkaz->cString[1] == '*')
                        t = Ground.DynamicFind(
                            Global::asHumanCtrlVehicle); // nazwa pojazdu u¿ytkownika
                    else
                        t = Ground.DynamicFindAny(AnsiString(
                            pRozkaz->cString + 1, (unsigned)pRozkaz->cString[0])); // nazwa pojazdu
                    if (t)
                        Ground.WyslijNamiary(t); // wys³anie informacji o pojeŸdzie
                }
                else
                { // dla pustego wysy³amy ramki 6 z nazwami pojazdów AI (jeœli potrzebne wszystkie,
                    // to rozpoznaæ np. "*")
                    Ground.DynamicList();
                }
            }
            break;
        case 8: // ponowne wys³anie informacji o zajêtych odcinkach toru
            Ground.TrackBusyList();
            break;
        case 9: // ponowne wys³anie informacji o zajêtych odcinkach izolowanych
            Ground.IsolatedBusyList();
            break;
        case 10: // badanie zajêtoœci jednego odcinka izolowanego
            Ground.IsolatedBusy(AnsiString(pRozkaz->cString + 1, (unsigned)(pRozkaz->cString[0])));
            break;
        case 11: // ustawienie paerametrów ruchu pojazdu
            //    Ground.IsolatedBusy(AnsiString(pRozkaz->cString+1,(unsigned)(pRozkaz->cString[0])));
            break;
        }
};


//------------------------------------------------------------------------------
void TWorld::ModifyTGA(const AnsiString &dir)
{ // rekurencyjna modyfikacje plików TGA
    TSearchRec sr;
    if (FindFirst(dir + "*.*", faDirectory | faArchive, sr) == 0)
    {
        do
        {
            if (sr.Name[1] != '.')
                if ((sr.Attr & faDirectory)) // jeœli katalog, to rekurencja
                    ModifyTGA(dir + sr.Name + "/");
                else if (sr.Name.LowerCase().SubString(sr.Name.Length() - 3, 4) == ".tga")
                    TTexturesManager::GetTextureID(NULL, NULL, AnsiString(dir + sr.Name).c_str());
        } while (FindNext(sr) == 0);
        FindClose(sr);
    }
};


//---------------------------------------------------------------------------
AnsiString last; // zmienne u¿ywane w rekurencji
double shift = 0;
void TWorld::CreateE3D(const AnsiString &dir, bool dyn)
{ // rekurencyjna generowanie plików E3D
    TTrack *trk;
    double at;
    TSearchRec sr;
    if (FindFirst(dir + "*.*", faDirectory | faArchive, sr) == 0)
    {
        do
        {
            if (sr.Name[1] != '.')
                if ((sr.Attr & faDirectory)) // jeœli katalog, to rekurencja
                    CreateE3D(dir + sr.Name + "\\", dyn);
                else if (dyn)
                {
                    if (sr.Name.LowerCase().SubString(sr.Name.Length() - 3, 4) == ".mmd")
                    {
                        // konwersja pojazdów bêdzie u³omna, bo nie poustawiaj¹ siê animacje na
                        // submodelach okreœlonych w MMD
                        // TModelsManager::GetModel(AnsiString(dir+sr.Name).c_str(),true);
                        if (last != dir)
                        { // utworzenie toru dla danego pojazdu
                            last = dir;
                            trk = TTrack::Create400m(1, shift);
                            shift += 10.0; // nastêpny tor bêdzie deczko dalej, aby nie zabiæ FPS
                            at = 400.0;
                            // if (shift>1000) break; //bezpiecznik
                        }
                        TGroundNode *tmp = new TGroundNode();
                        tmp->DynamicObject = new TDynamicObject();
                        // Global::asCurrentTexturePath=dir; //pojazdy maj¹ tekstury we w³asnych
                        // katalogach
                        at -= tmp->DynamicObject->Init(
                            "", dir.SubString(9, dir.Length() - 9), "none",
                            sr.Name.SubString(1, sr.Name.Length() - 4), trk, at, "nobody", 0.0,
                            "none", 0.0, "", false, "");
                        // po wczytaniu CHK zrobiæ pêtlê po ³adunkach, aby ka¿dy z nich skonwertowaæ
                        AnsiString loads, load;
                        loads = tmp->DynamicObject->MoverParameters->LoadAccepted; // typy ³adunków
                        if (!loads.IsEmpty())
                        {
                            loads += ","; // przecinek na koñcu
                            int i = loads.Pos(",");
                            while (i > 1)
                            { // wypada³o by sprawdziæ, czy T3D ³adunku jest
                                load = loads.SubString(1, i - 1);
                                if (FileExists(dir + load + ".t3d")) // o ile jest plik ³adunku, bo
                                    // inaczej nie ma to sensu
                                    if (!FileExists(
                                            dir + load +
                                            ".e3d")) // a nie ma jeszcze odpowiednika binarnego
                                        at -= tmp->DynamicObject->Init(
                                            "", dir.SubString(9, dir.Length() - 9), "none",
                                            sr.Name.SubString(1, sr.Name.Length() - 4), trk, at,
                                            "nobody", 0.0, "none", 1.0, load, false, "");
                                loads.Delete(1, i); // usuniêcie z nastêpuj¹cym przecinkiem
                                i = loads.Pos(",");
                            }
                        }
                        if (tmp->DynamicObject->iCabs)
                        { // jeœli ma jak¹kolwiek kabinê
                            delete Train;
                            Train = new TTrain();
                            if (tmp->DynamicObject->iCabs & 1)
                            {
                                tmp->DynamicObject->MoverParameters->ActiveCab = 1;
                                Train->Init(tmp->DynamicObject, true);
                            }
                            if (tmp->DynamicObject->iCabs & 4)
                            {
                                tmp->DynamicObject->MoverParameters->ActiveCab = -1;
                                Train->Init(tmp->DynamicObject, true);
                            }
                            if (tmp->DynamicObject->iCabs & 2)
                            {
                                tmp->DynamicObject->MoverParameters->ActiveCab = 0;
                                Train->Init(tmp->DynamicObject, true);
                            }
                        }
                        Global::asCurrentTexturePath =
                            AnsiString(szTexturePath); // z powrotem defaultowa sciezka do tekstur
                    }
                }
                else if (sr.Name.LowerCase().SubString(sr.Name.Length() - 3, 4) == ".t3d")
                { // z modelami jest proœciej
                    Global::asCurrentTexturePath = dir;
                    TModelsManager::GetModel(AnsiString(dir + sr.Name).c_str(), false);
                }
        } while (FindNext(sr) == 0);
        FindClose(sr);
    }
};
//---------------------------------------------------------------------------
void TWorld::CabChange(TDynamicObject *old, TDynamicObject *now)
{ // ewentualna zmiana kabiny u¿ytkownikowi
    if (Train)
        if (Train->Dynamic() == old)
            Global::changeDynObj = now; // uruchomienie protezy
};
//---------------------------------------------------------------------------
