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
#define GLEW_STATIC       // Q 241215
#include "opengl/glew.h"
#include "opengl/glut.h"
#include "opengl/ARB_Multisample.h"

#include <vcl.h>
#include <winuser.h>
#include <windows.h>
#include <registry.hpp>  // Q 241215
#include <filectrl.hpp>  // Q 241215
#include "system.hpp"
#include "classes.hpp"
#include "windows.h"
#include "winuser.h"
#include <wininet.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <istream>

#include <ostream>
#include <iostream>
#include <wtypes.h>

#include "Globals.h"
#include "Console.h"
#include "QueryParserComp.hpp"
#include "Mover.h"
#include "Logs.h"
#include "qutils.h"
#include "modelpreview.h"
#include "frm_debugger.h"
#include "screen.h"
#pragma hdrstop
#pragma comment (lib, "Wininet.lib");

#include <dsound.h> //_clear87() itp.

USERES("EU07.res");
USEUNIT("dumb3d.cpp");
USEUNIT("Camera.cpp");
USEUNIT("Texture.cpp");
USEUNIT("World.cpp");
USELIB("opengl\glut32.lib");
USEUNIT("Model3d.cpp");
USEUNIT("MdlMngr.cpp");
USEUNIT("Train.cpp");
USEUNIT("wavread.cpp");
USEUNIT("Timer.cpp");
USEUNIT("Event.cpp");
USEUNIT("MemCell.cpp");
USEUNIT("Logs.cpp");
USELIB("DirectX\Dsound.lib");
USEUNIT("Spring.cpp");
USEUNIT("Button.cpp");
USEUNIT("Globals.cpp");
USEUNIT("Gauge.cpp");
USEUNIT("AnimModel.cpp");
USEUNIT("Ground.cpp");
USEUNIT("TrkFoll.cpp");
USEUNIT("Segment.cpp");
USEUNIT("Sound.cpp");
USEUNIT("AdvSound.cpp");
USEUNIT("Track.cpp");
USEUNIT("DynObj.cpp");
USEUNIT("RealSound.cpp");
USEUNIT("EvLaunch.cpp");
USEUNIT("QueryParserComp.pas");
USEUNIT("FadeSound.cpp");
USEUNIT("Traction.cpp");
USEUNIT("TractionPower.cpp");
USEUNIT("parser.cpp");
USEUNIT("sky.cpp");
USEUNIT("AirCoupler.cpp");
USEUNIT("opengl\glew.c");
USEUNIT("ResourceManager.cpp");
USEUNIT("VBO.cpp");
USEUNIT("McZapkie\mtable.pas");
USEUNIT("TextureDDS.cpp");
USEUNIT("opengl\ARB_Multisample.cpp");
USEUNIT("Float3d.cpp");
USEUNIT("Classes.cpp");
USEUNIT("Driver.cpp");
USEUNIT("Names.cpp");
USEUNIT("Console.cpp");
USEUNIT("Mover.cpp");
USEUNIT("McZapkie\_mover.pas");
USEUNIT("McZapkie\hamulce.pas");
USEUNIT("Console\PoKeys55.cpp");
USEUNIT("Forth.cpp");
USEUNIT("Console\LPT.cpp");
USEUNIT("qutils.cpp");
USEUNIT("modelpreview.cpp");
USEUNIT("freetype.cpp");
USELIB("freetype.lib");
USEUNIT("screen.cpp");
USEUNIT("menu\bitmap_Font.cpp");
USEUNIT("orthorender.cpp");
USEUNIT("effects2d.cpp");
USEUNIT("submodelsops.cpp");
USEUNIT("dynamicmodules.cpp");
USEUNIT("env_SNOW.cpp");
USEUNIT("pixmap.cpp");
USEUNIT("rgbtexture.cpp");
USEUNIT("particlesys1.cpp");
USEFORM("frm_debugger.cpp", DEBUGGER);
//---------------------------------------------------------------------------
#include "World.h"


TWorld World;

HDC hDC = NULL; // Private GDI Device Context
HGLRC hRC = NULL; // Permanent Rendering Context
HWND hWnd = NULL; // Holds Our Window Handle
HWND hWnd2 = NULL;
// bool active=TRUE;	//window active flag set to TRUE by default
bool fullscreen = true; // fullscreen flag set to fullscreen mode by default
int WindowWidth = 800;
int WindowHeight = 600;
int Bpp = 32;

int MouseButton = -1;         // mouse button down
static POINT mouse;
static POINT xmouse;
static int mx=0, my=0;

AnsiString appath, commandline, filetoopen, FDT;

TStringList *ss;
HMENU hMenubar;
HMENU hMenu;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Declaration For WndProc

// Function pointers that will be used for the DLL functions.
typedef int (*AddFunc)(int,int);
typedef void (*FunctionFunc)();
void AddMenus(HWND);
// Typedef functions to hold what is in the DLL
AddFunc _AddFunc;
FunctionFunc _FunctionFunc;

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#define IDM_FILE_CONTINUE 4
//---------------------------------------------------------------------------


void AddMenus(HWND hwnd)
{
  hMenubar = CreateMenu();
  hMenu = CreateMenu();

  AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&Nowy");
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Otworz...");
  AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Zakoncz");
  AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_CONTINUE, L"&Kontynuuj");

  AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
  SetMenu(hwnd, hMenubar);
}


// ***********************************************************************************************************
// InitGl() - Taka se wstepna funkcja inicjalizacji OpenGL
// ***********************************************************************************************************

int InitGL(GLvoid) // All Setup For OpenGL Goes Here
{
    _clear87();
    _control87(MCW_EM, MCW_EM);
    WriteLog("GLEW Init...");
    glewInit();
    // hunter-271211: przeniesione
    // AllocConsole();
    // SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);

    // ShaXbee-121209: Wlaczenie obslugi tablic wierzcholkow
 if (glGenFramebuffersEXT==NULL) WriteLog("glGenFramebuffersEXT not found!");   // Q 24.12.15: Sprawdzanie rozszerzenia
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    Global::pWorld = &World; // Ra: wskaŸnik potrzebny do usuwania pojazdów

    WriteLog("World Init...");

    if (QGlobal::bmodelpreview) strcpy(Global::szSceneryFile, "modelpreview.scn");        // Q: bedzie wczytywanie scenerii z podgladem modelu

    return World.Init(hWnd, hDC); // true jeœli wszystko pójdzie dobrze
}


//---------------------------------------------------------------------------
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // resize and initialize the GL Window
{
    WindowWidth = width;
    WindowHeight = height;
    if (height == 0) // prevent a divide by zero by
        height = 1; // making height equal one
    glViewport(0, 0, width, height); // Reset The Current Viewport
    glMatrixMode(GL_PROJECTION); // select the Projection Matrix
    glLoadIdentity(); // reset the Projection Matrix

 // calculate the aspect ratio of the window
    gluPerspective(45.0f, (GLdouble)width / (GLdouble)height, 0.2f, 2500.0f);
  //gluPerspective(45.0f, (GLdouble)width / (GLdouble)height, 2.2f, 1999950600.0f);  // Q 24.12.15: Nadpisuje
    glMatrixMode(GL_MODELVIEW); // select the Modelview Matrix
    glLoadIdentity(); // reset the Modelview Matrix
}

//---------------------------------------------------------------------------
GLvoid KillGLWindow(GLvoid) // properly kill the window
{
    if (hRC) // Do We Have A Rendering Context?
    {
        if (!wglMakeCurrent(NULL, NULL)) // are we able to release the DC and RC contexts?
        {
            ErrorLog("Fail: window releasing");
            MessageBox(NULL, "Release of DC and RC failed.", "SHUTDOWN ERROR",
                       MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC)) // are we able to delete the RC?
        {
            ErrorLog("Fail: rendering context releasing");
            MessageBox(NULL, "Release rendering context failed.", "SHUTDOWN ERROR",
                       MB_OK | MB_ICONINFORMATION);
        }
        hRC = NULL; // set RC to NULL
    }

    if (hDC && !ReleaseDC(hWnd, hDC)) // are we able to release the DC?
    {
        ErrorLog("Fail: device context releasing");
        MessageBox(NULL, "Release device context failed.", "SHUTDOWN ERROR",
                   MB_OK | MB_ICONINFORMATION);
        hDC = NULL; // set DC to NULL
    }

    if (hWnd && !DestroyWindow(hWnd)) // are we able to destroy the window?
    {
        ErrorLog("Fail: window destroying");
        MessageBox(NULL, "Could not release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hWnd = NULL; // set hWnd to NULL
    }

    if (fullscreen) // Are We In Fullscreen Mode?
    {
        ChangeDisplaySettings(NULL, 0); // if so switch back to the desktop
     if (!QGlobal::brendermenu) ShowCursor(FALSE); // show mouse pointer
      else  ShowCursor(TRUE);
    }
    //    KillFont();
}


// *****************************************************************************
/*	This code creates our OpenGL Window.  Parameters are:			*
 *	title			- title to appear at the top of the window	*
 *	width			- width of the GL Window or fullscreen mode	*
 *	height			- height of the GL Window or fullscreen mode	*
 *	bits			- number of bits to use for color (8/16/24/32)	*
 *	fullscreenflag	- use fullscreen mode (TRUE) or windowed mode (FALSE)	*/

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
    GLuint PixelFormat; // holds the results after searching for a match
    HINSTANCE hInstance; // holds the instance of the application
    WNDCLASS wc; // windows class structure
    DWORD dwExStyle; // window extended style
    DWORD dwStyle; // window style
    RECT WindowRect; // grabs rectangle upper left / lower right values
    WindowRect.left = (long)0; // set left value to 0
    WindowRect.right = (long)width; // set right value to requested width
    WindowRect.top = (long)0; // set top value to 0
    WindowRect.bottom = (long)height; // set bottom value to requested height
    bool fullscreen;

    fullscreen = fullscreenflag; // set the global fullscreen flag

    hInstance = GetModuleHandle(NULL); // grab an instance for our window
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // redraw on size, and own DC for window.
    wc.lpfnWndProc = (WNDPROC)WndProc; // wndproc handles messages
    wc.cbClsExtra = 0; // no extra window data
    wc.cbWndExtra = 0; // no extra window data
    wc.hInstance = hInstance; // set the instance
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); // load the default icon
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // load the arrow pointer
    wc.hbrBackground = NULL; // no background required for GL
    wc.lpszMenuName = NULL; // we don't want a menu
    wc.lpszClassName = "EU07"; // nazwa okna do komunikacji zdalnej

    QGlobal::hINST = hInstance;
   // Set The Class Name

    if (!arbMultisampleSupported) // tylko dla pierwszego okna
        if (!RegisterClass(&wc)) // Attempt To Register The Window Class
        {
            ErrorLog("Fail: window class registeration");
            MessageBox(NULL, "Failed to register the window class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
            return FALSE; // Return FALSE
        }

    if (fullscreen) // Attempt Fullscreen Mode?
    {
        DEVMODE dmScreenSettings; // device mode
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // makes sure memory's cleared
        dmScreenSettings.dmSize = sizeof(dmScreenSettings); // size of the devmode structure

        // tolaris-240403: poprawka na odswiezanie monitora
        // locate primary monitor...
        if (Global::bAdjustScreenFreq)
        {
            POINT point;
            point.x = 0;
            point.y = 0;
            MONITORINFOEX monitorinfo;
            monitorinfo.cbSize = sizeof(MONITORINFOEX);
            ::GetMonitorInfo(::MonitorFromPoint(point, MONITOR_DEFAULTTOPRIMARY), &monitorinfo);
            //  ..and query for highest supported refresh rate
            unsigned int refreshrate = 0;
            int i = 0;
            while (::EnumDisplaySettings(monitorinfo.szDevice, i, &dmScreenSettings))
            {
                if (i > 0)
                    if (dmScreenSettings.dmPelsWidth == (unsigned int)width)
                        if (dmScreenSettings.dmPelsHeight == (unsigned int)height)
                            if (dmScreenSettings.dmBitsPerPel == (unsigned int)bits)
                                if (dmScreenSettings.dmDisplayFrequency > refreshrate)
                                    refreshrate = dmScreenSettings.dmDisplayFrequency;
                ++i;
            }
            // fill refresh rate info for screen mode change
            dmScreenSettings.dmDisplayFrequency = refreshrate;
            dmScreenSettings.dmFields = DM_DISPLAYFREQUENCY;
        }
        dmScreenSettings.dmPelsWidth = width; // selected screen width
        dmScreenSettings.dmPelsHeight = height; // selected screen height
        dmScreenSettings.dmBitsPerPel = bits; // selected bits per pixel
        dmScreenSettings.dmFields = dmScreenSettings.dmFields | DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Try to set selected mode and get results.  NOTE: CDS_FULLSCREEN gets rid of start bar.
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            // If the mode fails, offer two options.  Quit or use windowed mode.
            ErrorLog("Fail: full screen");
            if (MessageBox(NULL, AnsiString("The requested fullscreen mode is not supported by\nyour video card. Use windowed mode instead? (" + IntToStr(width) + "x" + IntToStr(height) + ")").c_str() ,
                           "EU07", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
            {
                fullscreen = false; // Windowed Mode Selected.  Fullscreen = FALSE
                Global::bFullScreen = false;
            }
            else
            {
                // Pop Up A Message Box Letting User Know The Program Is Closing.
                Error("Program will now close.");
                return FALSE; // Return FALSE
            }
        }
    }

    if (fullscreen) // Are We Still In Fullscreen Mode?
    {
        dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
        dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ; // Windows Style

        //ShowCursor(FALSE); // Hide Mouse Pointer
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN; // Windows Style
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size

    // Create The Window
    if (NULL == (hWnd = CreateWindowEx(dwExStyle, // Extended Style For The Window
                               "EU07", // Class Name
                               title, // Window Title
                               dwStyle | // Defined Window Style
                               WS_CLIPSIBLINGS | // Required Window Style
                               WS_CLIPCHILDREN, // Required Window Style
                               0,
                               0, // Window Position
                               WindowRect.right - WindowRect.left, // Calculate Window Width
                               WindowRect.bottom - WindowRect.top, // Calculate Window Height
                               NULL, // No Parent Window
                               NULL, // No Menu
                               hInstance, // Instance
                               NULL))) // Dont Pass Anything To WM_CREATE
    {
        KillGLWindow(); // Reset The Display
        ErrorLog("Fail: window creation");
        MessageBox(NULL, "Window creation error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE; // Return FALSE
    }

    static PIXELFORMATDESCRIPTOR pfd =                                          // pfd Tells Windows How We Want Things To Be
        {
         sizeof(PIXELFORMATDESCRIPTOR),                                         // Size Of This Pixel Format Descriptor
         1, // Version Number
         PFD_DRAW_TO_WINDOW |                                                   // Format Must Support Window
         PFD_SUPPORT_OPENGL |                                                   // Format Must Support OpenGL
         PFD_DOUBLEBUFFER,                                                      // Must Support Double Buffering
         PFD_TYPE_RGBA,                                                         // Request An RGBA Format
         bits,                                                                  // Select Our Color Depth
         0,
         0, 0, 0, 0, 0,                                                         // Color Bits Ignored
         0,                                                                     // No Alpha Buffer
         0,                                                                     // Shift Bit Ignored
         0,                                                                     // No Accumulation Buffer
         0, 0, 0, 0,                                                            // Accumulation Bits Ignored
         24,                                                                    // 32Bit Z-Buffer (Depth Buffer)
         0,                                                                     // No Stencil Buffer
         0,                                                                     // No Auxiliary Buffer
         PFD_MAIN_PLANE,                                                        // Main Drawing Layer
         0,                                                                     // Reserved
         0, 0, 0                                                                // Layer Masks Ignored
        };

    if (NULL == (hDC = GetDC(hWnd))) // Did We Get A Device Context?
    {
        KillGLWindow(); // Reset The Display
        ErrorLog("Fail: device context");
        MessageBox(NULL, "Can't create a GL device context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE; // Return FALSE
    }

    /*
     Our first pass, Multisampling hasn't been created yet, so we create a window normally
     If it is supported, then we're on our second pass
     that means we want to use our pixel format for sampling
     so set PixelFormat to arbMultiSampleformat instead
    */
    if (!arbMultisampleSupported)
    {
        if (NULL == (PixelFormat =
                         ChoosePixelFormat(hDC, &pfd))) // Did Windows Find A Matching Pixel Format?
        {
            KillGLWindow(); // Reset The Display
            ErrorLog("Fail: pixelformat");
            MessageBox(NULL, "Can't find a suitable pixelformat.", "ERROR",
                       MB_OK | MB_ICONEXCLAMATION);
            return FALSE; // Return FALSE
        }
    }
    else
        PixelFormat = arbMultisampleFormat;

    if (!SetPixelFormat(hDC, PixelFormat, &pfd)) // Are We Able To Set The Pixel Format?
    {
        KillGLWindow(); // Reset The Display
        ErrorLog("Fail: pixelformat");
        MessageBox(NULL, "Can't set the pixelformat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE; // Return FALSE
    }

    if (NULL == (hRC = wglCreateContext(hDC))) // Are We Able To Get A Rendering Context?
    {
        KillGLWindow(); // Reset The Display
        ErrorLog("Fail: OpenGL rendering context creation");
        MessageBox(NULL, "Can't create a GL rendering context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE; // Return FALSE
    }

    if (!wglMakeCurrent(hDC, hRC)) // Try To Activate The Rendering Context
    {
        KillGLWindow(); // Reset The Display
        ErrorLog("Fail: OpenGL rendering context activation");
        MessageBox(NULL, "Can't activate the GL rendering context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE; // Return FALSE
    }

    QGlobal::glHDC = hDC;
    QGlobal::glHGLRC = hRC;
    QGlobal::glHWND = hWnd;
    Global::hWnd = hWnd;
    /*
    Now that our window is created, we want to queary what samples are available
    we call our InitMultiSample window
    if we return a valid context, we want to destroy our current window
    and create a new one using the multisample interface.
    */
    if (Global::iMultisampling)
        if (!arbMultisampleSupported)
            if ((Global::iMultisampling =
                     InitMultisample(hInstance, hWnd, pfd, 1 << Global::iMultisampling)) != 0)
            {
                // WriteConsoleOnly("Opening second window for multisampling of
                // "+AnsiString(Global::iMultisampling)+" samples.");
                KillGLWindow(); // reset the display
                return CreateGLWindow(title, width, height, bits, fullscreenflag); // rekurencja
            }

    ShowWindow(hWnd, SW_SHOW); // show the window
    ReSizeGLScene(width, height); // set up our perspective GL screen
    SetForegroundWindow(hWnd); // slightly higher priority
    SetFocus(hWnd); // sets keyboard focus to the window
    Sleep(5);
    Application->ProcessMessages();
    if (!InitGL()) // initialize our newly created GL Window
    {
        KillGLWindow(); // reset the display
        ErrorLog("Fail: OpenGL initialization");
        MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return FALSE; // return FALSE
    }
    return TRUE; // success
}

BOOL fEatKeystroke = FALSE;
static int test = 0;
#define MYDISPLAY 1
PCOPYDATASTRUCT pDane;


// ***********************************************************************************************************
// TAKIE CIEZKIE KODZENIE DLA OBSLUGI PrtScrn (VK_SNAPSHOT), TRZEBA WYKRYWAC NA NISKIM POZIOMIE
// TEN KLAWISZ. CO ZA OKRUTNE JEST ZYCIE Q: 261215
// ***********************************************************************************************************
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  BOOL fEatKeystroke = FALSE;
 
  if (nCode == HC_ACTION) 
  {
     switch (wParam)
     {
       // case WM_KEYDOWN:
       // case WM_SYSKEYDOWN:
        case WM_KEYUP:
       // case WM_SYSKEYUP:
            {
                PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
                fEatKeystroke=(p->vkCode == VK_SNAPSHOT);
                if (fEatKeystroke)
                {
                   Sleep(100);
                   WriteLog("VK_SNAPSHOT");//Here goes your printkey code
                   World.SCR->SaveScreen_xxx();
                }
                break;
            }
     }
  }
  return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}


// ***********************************************************************************************************
// OBSLUGA KOMUNIKATOW ZWROTNYCH
// ***********************************************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//HINSTANCE hINST;
//hINST = GetModuleHandle(NULL);

    TRect rect;
    switch (uMsg) // check for windows messages
    {
     case WM_HOTKEY:
    {
        WinExec(" The Java Application ", SW_SHOWNORMAL);
    }

    case WM_PASTE: //[Ctrl]+[V] potrzebujemy do innych celów
        return 0;

    case WM_COPYDATA: // obs³uga danych przes³anych przez program steruj¹cy
        pDane = (PCOPYDATASTRUCT)lParam;
        if (pDane->dwData == 'EU07') // sygnatura danych
            World.OnCommandGet((DaneRozkaz *)(pDane->lpData));
        break;

    case WM_ACTIVATE: // watch for window activate message
        // case WM_ACTIVATEAPP:
        { // Ra: uzale¿nienie aktywnoœci od bycia na wierzchu
            Global::bActive = (LOWORD(wParam) != WA_INACTIVE);
            if (Global::bInactivePause) // jeœli ma byæ pauzowanie okna w tle
                if (Global::bActive)
                    Global::iPause &= ~4; // odpauzowanie, gdy jest na pierwszym planie
                else
                    Global::iPause |= 4; // w³¹czenie pauzy, gdy nieaktywy
            if (Global::bActive)
                SetCursorPos(mx, my);
            ShowCursor(!Global::bActive);
            /*
               if (!HIWORD(wParam))	//check minimization state
                    active=TRUE;	//program is active
               else
                    active=FALSE;	//program is no longer active
            */
            return 0; // return to the message loop
        }
    case WM_SYSCOMMAND: // intercept system commands
    {
        switch (wParam) // check system calls
        {
        case 61696: // F10
            World.OnKeyDown(VK_F10);
            return 0;
        case SC_SCREENSAVE: // screensaver trying to start?
        case SC_MONITORPOWER: // monitor trying to enter powersave?
            return 0; // prevent from happening
        }
        break; // exit
    }
    case WM_CLOSE: // did we receive a close message?
    {
        PostQuitMessage(0); // send a quit message [Alt]+[F4]
        return 0; // jump back
    }
    case WM_MOUSEMOVE:
    {
        // mx= 100;//Global::iWindowWidth/2;
        // my= 100;//Global::iWindowHeight/2;
        // SetCursorPos(Global::iWindowWidth/2,Global::iWindowHeight/2);
        // m_x= LOWORD(lParam);
        // m_y= HIWORD(lParam);
        GetCursorPos(&mouse);
        
       //-- QGlobal::iMPX = LOWORD(lParam);
       //-- QGlobal::iMPY = HIWORD(lParam);

        if (Global::bActive && ((mouse.x != mx) || (mouse.y != my)))
        {
            if (!Console::Pressed(VK_RMENU)) SetCursorPos(mx, my);
            if (!Console::Pressed(VK_RMENU)) World.OnMouseMove(double(mouse.x - mx) * 0.005, double(mouse.y - my) * 0.01);
          //World.OnMouseMove(double(mouse.x - mx) * 0.005, double(mouse.y - my) * 0.01,  LOWORD(lParam), HIWORD(lParam));
        }
        return 0; // jump back
    }
    
    case WM_LBUTTONDOWN :
        {
         ReleaseCapture();   // need them here, because if mouse moves off
         SetCapture(hWnd);  // window and returns, it needs to reset status
         MouseButton = 1;

         QGlobal::iMPX = LOWORD(lParam);
         QGlobal::iMPY = HIWORD(lParam);

         //World.LM_RetrieveObjectColor(Global::iMPX, Global::iMPY);

         World.OnMouseLpush(QGlobal::iMPX, QGlobal::iMPY);
         return 0;
        };

    case WM_RBUTTONDOWN :
        {
         ReleaseCapture();   // need them here, because if mouse moves off
         SetCapture(hWnd);  // window and returns, it needs to reset status
         MouseButton = 1;

         QGlobal::iMPX = LOWORD(lParam);
         QGlobal::iMPY = HIWORD(lParam);

         //World.LM_RetrieveObjectColor(Global::iMPX, Global::iMPY);

         World.OnMouseRpush(QGlobal::iMPX, QGlobal::iMPY);
         return 0;
        };

    case WM_MBUTTONDOWN :
        {
         ReleaseCapture();   // need them here, because if mouse moves off
         SetCapture(hWnd);  // window and returns, it needs to reset status
         MouseButton = 1;

         QGlobal::iMPX = LOWORD(lParam);
         QGlobal::iMPY = HIWORD(lParam);

         //World.LM_RetrieveObjectColor(Global::iMPX, Global::iMPY);

         World.OnMouseMpush(QGlobal::iMPX, QGlobal::iMPY);
         return 0;
        };
        
    case WM_MOUSEWHEEL:
        {
         int zDelta = ((short) HIWORD(wParam));
         World.OnMouseWheel(zDelta);
         return 0;
        };

    case WM_SYSKEYUP:
        if (Global::bActive)
        {
            World.OnSysKeyUp(wParam);
            return 0;
        }
        
    case WM_KEYUP:
        if (Global::bActive)
        {
            World.OnKeyUp(wParam);
            return 0;
        }

    case WM_KEYDOWN:
        if (Global::bActive)
        {
            if (wParam != 17) // bo naciœniêcia [Ctrl] nie ma po co przekazywaæ
                if (wParam != 145) //[Scroll Lock] te¿ nie
                    World.OnKeyDown(wParam);
            switch (wParam)
            {
            case VK_ESCAPE: //[Esc] pauzuje tylko bez Debugmode
                if (!QGlobal::bSCNLOADED) exit(0);
                if (DebugModeFlag)
                    break;
            case 19: //[Pause]
                if (Global::iPause & 1) // jeœli pauza startowa
                    Global::iPause &= ~1; // odpauzowanie, gdy po wczytaniu mia³o nie startowaæ
                else if (!(Global::iMultiplayer & 2)) // w multiplayerze pauza nie ma sensu
                    if (!Console::Pressed(VK_CONTROL)) // z [Ctrl] to radiostop jest
                        // Ra: poni¿sze nie ma sensu, bo brak komunikacji natychmiast zapauzuje
                        // ponownie
                        // if (Global::iPause&8) //jeœli pauza zwi¹zana z brakiem komunikacji z
                        // PoKeys
                        // Global::iPause&=~10; //odpauzowanie pauzy PoKeys (chyba nic nie da) i
                        // ewentualnie klawiszowej równie¿
                        // else
                        Global::iPause ^= 2; // zmiana stanu zapauzowania
                if (Global::iPause) // jak pauza
                    Global::iTextMode = VK_F1; // to wyœwietliæ zegar i informacjê
                break;
            case VK_F7:
               // if (DebugModeFlag)
                { // siatki wyœwietlane tyko w trybie testowym
                    Global::bWireFrame = !Global::bWireFrame;
                    ++Global::iReCompile; // odœwie¿yæ siatki
                    // Ra: jeszcze usun¹æ siatki ze skompilowanych obiektów!
                }
                break;
            }
        }
        return 0; // jump back
    case WM_CHAR:
    {
        /*
           switch ((TCHAR) wParam)
           {
            // case 'q':
            //  done= true;
            //  KillGLWindow();
            //  PostQuitMessage(0);
            //  DestroyWindow( hWnd );
            //  break;
           };
        */
        return 0;
    }
    case WM_SIZE: // resize the OpenGL window
    {
        ReSizeGLScene(LOWORD(lParam), HIWORD(lParam)); // LoWord=Width, HiWord=Height
        if (GetWindowRect(hWnd, &rect))
        { // Ra: zmiana rozmiaru okna bez przesuwania myszy
            // mx=WindowWidth/2+rect.left;    // horizontal position
            // my=WindowHeight/2+rect.top;    // vertical position
            // SetCursorPos(mx,my);
        }
        return 0; // jump back
    }
    case WM_MOVE: // przesuwanie okna?
    {
        mx = WindowWidth / 2 + LOWORD(lParam); // horizontal position
        my = WindowHeight / 2 + HIWORD(lParam); // vertical position
        // SetCursorPos(mx,my);
        break;
    }
    case WM_PAINT:
    { // odrysowanie okna

        break;
    }
    // case WM_ERASEBKGND: //Process this message to keep Windows from erasing background.
    case MM_JOY1BUTTONDOWN:
    {
        // WriteLog("Joystick button "+AnsiString(wParam));
        break;
    }
    case WM_CREATE:
        /* Capture the joystick. If this fails, beep and display error, then quit.*/
        //--AddMenus(hWnd);
        //AddButton(hWnd, hINST);

        if (joySetCapture(hWnd, JOYSTICKID1, 0, FALSE))
        {
            // MessageBeep(MB_ICONEXCLAMATION);
            // MessageBox(hWnd,"Couldn't capture the joystick",NULL,MB_OK|MB_ICONEXCLAMATION);
            // return -1;
        }
        break;

    case WM_COMMAND:
          switch(LOWORD(wParam))
           {
              case IDM_FILE_NEW:
              case IDM_FILE_OPEN:
                  Beep(50, 100);
                  break;
              case IDM_FILE_QUIT:
                  SendMessage(hWnd, WM_CLOSE, 0, 0);
                  break;
              case IDM_FILE_CONTINUE:

                  break;
           }
           break;

    }   // Switch


    // pass all unhandled messages to DefWindowProc
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
};


// ***********************************************************************************************************
// POCZATEK WSZYSTKIEGO
// ***********************************************************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

// The Instance of the DLL.
// LoadLibrary used to load a DLL
/*
	HINSTANCE hInstLibrary = LoadLibrary("DLL_Tutorial.dll");

	if (hInstLibrary)
	{
		// Our DLL is loaded and ready to go.

		// Set up our function pointers.
		_AddFunc = (AddFunc)GetProcAddress(hInstLibrary, "Add");
		_FunctionFunc = (FunctionFunc)GetProcAddress(hInstLibrary, "Function");

		// Check if _AddFunc is currently holding a function, if not don't run it.
		if (_AddFunc)
		{
			WriteLog(  _AddFunc(23, 43) );
		}
		// Check if _FunctionFunc is currently holding a function, if not don't run it.
		if (_FunctionFunc)
		{
			_FunctionFunc();
		}

		// We're done with the DLL so we need to release it from memory.
		FreeLibrary(hInstLibrary);
	}
	else
	{
		// Our DLL failed to load!

               // WriteLog( "DLL Failed To Load!" );
	}
*/

 // Install the low-level keyboard & mouse hooks
 HHOOK hhkLowLevelKybd  = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) &LowLevelKeyboardProc, hInstance, 0); //Q 261215: niskopoziomowe przechwytywanie klawiary specjalnie dla klawisza PrntScr :)

 MSG msg; // windows message structure
 WIN32_FILE_ATTRIBUTE_DATA attr;
 SYSTEMTIME creation;
 int sh, sv, argc, tcl;
 char screendim[100];
 char tolog[100];
 char cmdline[100];
 char appvers[100];
 char appdate[100];
 char apppath[100];
 char shotdir[100];
 char kbdlayo[100];
 char szFILE[200];
 char szFILELIST[200];
 char szCFGFILE[200];
 std::string line;
 WORD vmajor, vminor, vbuild, vrev;
 TModelViewer *MV;

 BOOL askforfull = false;
 BOOL getscreenb = false;
 BOOL openlogonx = false;
 BOOL rege3dt3d = false;
 BOOL done = FALSE; // bool variable to exit loop
 BOOL bISDYNAMIC = false;
 BOOL bMV = false;
 BOOL bDM = false;
 BOOL bFILEOK = false;
 fullscreen = true;
 DecimalSeparator = '.';

 QGlobal::SLTEMP = new TStringList;
 QGlobal::CONFIG = new TStringList;
 QGlobal::LOKTUT = new TStringList;
 QGlobal::MBRIEF = new TStringList;
 QGlobal::MISSIO = new TStringList;
 DEBUGGER = new TDEBUGGER(NULL);                                                 // UTWORZENIE FORMY DEBUGGERA
 DEBUGGER->Hide();
 MV = new TModelViewer;
 AnsiString FULLEXE = Application->ExeName;

 QGlobal::brendermenu = false;

 srand( time( NULL ) );
 
 Global::GetKbdLayout();                                                        // POBIERANIE UKLADU KLAWIATURY

 appath = GETCWD();                                                             // POBIERA SCIEZKE APLIKACJI DO ZMIENNEJ GLOBALNEJ Global::asCWD
 QGlobal::asAPPDIR = appath; //ExtractFilePath(ParamStr(0));

 GetDesktopResolution(sh, sv);
 SetCurrentDirectory(QGlobal::asAPPDIR.c_str());                                // BO PODCZAS OTWIERANIA MODELU Z INNEGO KATALOGU USTAWIAL TAM GLOWNY

 DeleteFile(QGlobal::asAPPDIR + "errors.txt");                                  // usuniêcie starego
 DeleteFile(QGlobal::asAPPDIR + "templog.txt");                                 // usuniêcie starego
 DeleteFile(QGlobal::asAPPDIR + "models\\temp.e3d");
 DeleteFile(QGlobal::asAPPDIR + "models\\temp.t3d");                            

 commandline = lpCmdLine;

 WriteLog("CMDLINE: " + commandline);

 MV->Update(QGlobal::asAPPDIR);

 MV->isdynamicmodel(commandline.c_str());                                       // sprawdza czy z katalogu dynamic (ustawia flage globalna i w klasie)

 bMV = MV->copymodelfiletotemp(commandline.c_str(), bFILEOK);                   // KOPIOWANIE PLIKU Z MIEJSCA URUCHOMIENIA NA models/temp.ext

 MV->UnifyCmdLine(commandline);

 MV->PrepareCompare(commandline);

 // PODGLAD GDY KLIKNIETO NA PLIK W KATALOGU MODELS\ Z KATALOGU TEJ MASZYNY ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
   if ((bMV) && (commandline.Pos("-s") == 0) && (commandline.Pos("models") > 0) && (MV->testp1 == MV->testp2))
   {
    WriteLog("Lauching EXE from model file double click (in habitat)...");
    filetoopen = commandline.Delete( 1, MV->tocutlen);                          // usuwa F:\MaSzyna_15_04\models\ zostaje linia053\lamp-y.t3d
    WriteLog("MODEL: " + commandline);
    MV->modelpreview(filetoopen.c_str(), "", "", "");                           // tworzenie tymczasowego pliku scenerii
    commandline = AnsiString("-vm " + filetoopen).c_str();
   }

 // JEZELI KLIKNIETO NA PLIK MODELU POZA KATALOGIEM MODELS\ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  else if ((bMV) && (commandline.Pos("-s") == 0) && (MV->testp1 != MV->testp2))
   {
    WriteLog("Lauching EXE from model file double click (out of habitat)...");
    WriteLog("MODEL: " + commandline);
    QGlobal::asDynamicTexturePath = ExtractFilePath(commandline.c_str());
if (QGlobal::bISDYNAMIC) WriteLog(QGlobal::asDynamicTexturePath.c_str());
    MV->modelpreview("temp.t3d", "", "", "");                                   // tworzenie tymczasowego pliku scenerii
    commandline = "-vm temp.t3d";
   }

 // JEZLEI W POLECENIU NIE BYLO -s NP ODPALENIE EXE BEZPOSREDNIO... ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  else if ((commandline.Pos("-s") > 0))
   {
    WriteLog("Lauching program from EXE file with parameters...");
   }

 // JEZLEI W POLECENIU NIE BYLO -s NP ODPALENIE EXE BEZPOSREDNIO ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  else if ((commandline.Pos("-s") == 0) && (commandline.Pos("") == 0))
   {
    WriteLog("Lauching program from EXE file");
   }
 WriteLog("CMDLINE: " + commandline);

 QGlobal::USERPID = AnsiString(GetMachineID("C:\\"));
 
 FDT = FormatDateTime("ddmmyy-hhmmss", Now());

// argc = ParseCommandline1();


// CHECKINGF ILE SYTEM (FOLDERS) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 WriteLog("");
 WriteLog("CHECKING FILE SYSTEM...");

 sprintf(szFILE,"%s%s", appath.c_str() , "\\fsys.txt");
 std::ifstream File(szFILE);

 int errc = 0;
   if (File)
    {
        while(getline(File, line))
       {
		std::string test, par1;
                int pos1 = 0;
		int pos2 = 0;
		pos1 = line.find(" ");
		pos2 = line.find(":");
		test = line.substr(0, pos1);
                par1 = line.substr(pos2+1, 20);

		if (test == "texpath")  Global::asCurrentTexturePath = par1.c_str();
		if (test == "mdlpath")  QGlobal::asCurrentModelsPath = par1.c_str();
		if (test == "scnpath")  Global::asCurrentSceneryPath = par1.c_str();
	        if (test == "sndpath")  Global::asCurrentSoundPath = par1.c_str();
                if (test == "dynpath")  Global::asCurrentDynamicPath = par1.c_str();

                if (DirectoryExists(QGlobal::asAPPDIR + par1.c_str()))
                WriteLog("FLDR: " + AnsiString(test.c_str()) + ", " + AnsiString(par1.c_str())  + ", OK");
                else
                {
                 errc++;
                 WriteLog("FLDR: " + AnsiString(test.c_str()) + ", " + AnsiString(par1.c_str())  + ", MISSED!");
                }
		//sprintf(tolog,"varname: [%s] = [%s]", test.c_str(), par1.c_str());

		//MessageBox(NULL,tolog, "ERROR",MB_OK|MB_ICONEXCLAMATION);

        }
     if (errc == 0) WriteLog("FILESYS OK.");
     if (errc > 0) WriteLog("FILESYS ERRORS. (" + IntToStr(errc) + ")");
     WriteLog("");
     File.close();

	// TU SA TRZYMANE DEFAULTOWE SCIEZKI
	//Global::asFSYSTEXTUREPATH = Global::asCurrentTexturePath;
	//Global::asFSYSMODELSPATH = Global::asCurrentModelsPath;
	//Global::asFSYSSCENERYPATH = Global::asCurrentSceneryPath;
    }
    else
    {
     WriteLog("FILE SYSTEM CHECK ERROR, CHECK FILE 'FSYS.TXT'.");
     MessageBox(NULL,"FILE SYSTEM READ ERROR, CHECK FILE 'FSYS.TXT'.", "ERROR",MB_OK|MB_ICONEXCLAMATION);
    }


// CHECKING FILE SYTEM (FILES) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 WriteLog("");
 WriteLog("CHECKING IMPORTANT FILES...");

 sprintf(szFILELIST,"%s%s", appath.c_str() , "\\filelist.txt");
 std::ifstream filelist(szFILELIST);
 AnsiString chkfull, chkfile, chkflag, xstr;
 bool isfile = false;

  errc = 0;
   if (File)
    {
        while(getline(filelist, line))
       {
		std::string test, par1;

                isfile = false;

                std::vector<std::string> x = split(line, ':');

                chkfile = Trim(x[0].c_str());
                chkflag = Trim(x[1].c_str());


                if (chkfile.Pos(".") > 0) isfile = true;

                if (isfile) xstr = "FILE"; else xstr = "FLDR";

                chkfull = QGlobal::asAPPDIR + chkfile;

                if (isfile)
                 {
                  if (FileExists(chkfull)) WriteLog(xstr + ": " + chkfile + ", [" + chkflag + "] OK.");
                   else
                    { WriteLog(xstr + ": " + chkfile + ", [" + chkflag + "] FAIL."); errc++;}
                 }
                if (!isfile)
                 {
                  if (DirectoryExists(chkfull)) WriteLog(xstr + ": " + chkfile + ", [" + chkflag + "] OK.");
                   else
                    { WriteLog(xstr + ": " + chkfile + ", [" + chkflag + "] FAIL.");  errc++;}
                 }
                 
                //if (DirectoryExists(QGlobal::asAPPDIR + par1.c_str()))
                //WriteLog("varname: " + AnsiString(test.c_str()) + ", " + AnsiString(par1.c_str())  + ", OK");
                //else
                //{
                // errc++;
                // WriteLog("varname: " + AnsiString(test.c_str()) + ", " + AnsiString(par1.c_str())  + ", MISSED!");
                //}
		//sprintf(tolog,"varname: [%s] = [%s]", test.c_str(), par1.c_str());

		//MessageBox(NULL,tolog, "ERROR",MB_OK|MB_ICONEXCLAMATION);

        }
     if (errc == 0) WriteLog("FILESYS OK.");
     if (errc > 0) WriteLog("FILESYS ERRORS. (" + IntToStr(errc) + ")");
     WriteLog("");
     filelist.close();

	// TU SA TRZYMANE DEFAULTOWE SCIEZKI
	//Global::asFSYSTEXTUREPATH = Global::asCurrentTexturePath;
	//Global::asFSYSMODELSPATH = Global::asCurrentModelsPath;
	//Global::asFSYSSCENERYPATH = Global::asCurrentSceneryPath;
    }
    else
    {
     WriteLog("FILE SYSTEM CHECK ERROR, CHECK FILE 'FILELIST.TXT'.");
     MessageBox(NULL,"FILE SYSTEM READ ERROR, CHECK FILE 'FILELIST.TXT'.", "ERROR",MB_OK|MB_ICONEXCLAMATION);
    }

    WriteLog("Reading eu07.ini...");
    Global::LoadIniFile("eu07.ini"); // teraz dopiero mo¿na przejrzeæ plik z ustawieniami
    Global::InitKeys("keys.ini"); // wczytanie mapowania klawiszy - jest na sta³e


// READING CONFIG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    sprintf(szCFGFILE,"%s%s", appath.c_str() , "\\config.txt");
    WriteLog("READING CONFIG FILE... (" + AnsiString(szCFGFILE) + ")");

    std::ifstream FileCFG(szCFGFILE);

    if(FileCFG)
    {
        while(getline(FileCFG,line))
		{
		AnsiString test,par1;
                int pos1 = 0;
		int pos2 = 0;
                int pos3 = 0;
		pos1 = line.find(" ");
		pos2 = line.find(":");                      //14
                pos3 = line.find("//");                     //33
		test = line.substr(0, pos1).c_str();
                int aa= pos3 - pos2;
                par1 = Trim(line.substr(pos2+1, aa-1).c_str());      //14+1

                //WriteLog(test + "=[" + par1 + "]");

	      //if (test == "getscreenb") getscreenb = atoi(par1.c_str());
	        if (test == "screenresw") QGlobal::iWW = StrToInt(par1.c_str());
	        if (test == "screenresh") QGlobal::iWH = StrToInt(par1.c_str());
		if (test == "fullscreen") Global::bFullScreen = atoi(par1.c_str());
                if (test == "askforfull") askforfull = atoi(par1.c_str());
                if (test == "aspectratio") QGlobal::aspectratio = atoi(par1.c_str());
              //if (test == "debugmode1") DebugMode1 = atoi(par1.c_str());
	        if (test == "openlogonx") QGlobal::bOPENLOGONX = atoi(par1.c_str());
          	if (test == "logfilenm1") QGlobal::logfilenm1 = par1;
	        if (test == "exitlogons") QGlobal::logwinname = par1;
                if (test == "sendlogftp") QGlobal::bSENDLOGFTP = atoi(par1.c_str());
                if (test == "scrshotext") QGlobal::asSSHOTEXT = par1;
                if (test == "scrshotqlt") QGlobal::asSSHOTQLT = par1;
                if (test == "scrshotdir") QGlobal::asSSHOTDIR = par1;
                if (test == "scrshotsub") QGlobal::asSSHOTSUB = par1;
                if (test == "ldrbackext") QGlobal::asLBACKEXT = par1;
              //if (test == "ldrrefresh") QGlobal::LDRREFRESH = atoi(par1.c_str());
                if (test == "z-fightfix") QGlobal::bzfightfix = atoi(par1.c_str());
                if (test == "reg-t3de3d") rege3dt3d = atoi(par1.c_str());
                if (test == "guitutopac") QGlobal::GUITUTOPAC = atof(par1.c_str());
                if (test == "sshotexif+") QGlobal::bWRITEEXIF = atoi(par1.c_str());
                if (test == "splashscrn") QGlobal::bSPLASHSCR = atoi(par1.c_str());
                if (test == "rendermenu") QGlobal::brendermenu = atoi(par1.c_str());
                if (test == "exifauthor") QGlobal::asEXIFAUTHOR = par1;
                if (test == "exifcpyrgt") QGlobal::asEXIFCOPYRIGHT = par1;
                if (test == "rail_model") QGlobal::asRAILTYPE = UpperCase(par1);
                if (test == "rail_ties") QGlobal::bRTIES = atoi(par1.c_str());
                if (test == "tiemaxdist") QGlobal::fTIEMAXDIST = StrToFloat(Trim(par1));
                if (test == "swtchbllst") QGlobal::bAUTOSWITCHBALLAST = atoi(par1.c_str());
                if (test == "env_sky001") QGlobal::bRENDERSKY1 = atoi(par1.c_str());
                if (test == "env_sky002") QGlobal::bRENDERSKY2 = atoi(par1.c_str());
                if (test == "env--cloud") QGlobal::bRENDERCLDS = atoi(par1.c_str());
                if (test == "env---rain") QGlobal::bRENDERRAIN = atoi(par1.c_str());
                if (test == "env---snow") QGlobal::bRENDERSNOW = atoi(par1.c_str());
                if (test == "env---sun1") QGlobal::bRENDERSUN = atoi(par1.c_str());
                if (test == "env---moon") QGlobal::bRENDERMOON = atoi(par1.c_str());
                if (test == "snowflakes") QGlobal::iSNOWFLAKES = atoi(par1.c_str());
                if (test == "snowsquare") QGlobal::iSNOWSQUARE = atoi(par1.c_str());
                if (test == "trwiresize") QGlobal::ftrwiresize = StrToFloat(Trim(par1));
                if (test == "noisealpha") QGlobal::fnoisealpha = StrToFloat(Trim(par1));
                if (test == "fovonstart") QGlobal::ffov = StrToFloat(Trim(par1));

                if (test == "font10file") QGlobal::font10file = Trim(par1);
                if (test == "font11file") QGlobal::font11file = Trim(par1);
                if (test == "font12file") QGlobal::font12file = Trim(par1);
                if (test == "font14file") QGlobal::font14file = Trim(par1);
                if (test == "font16file") QGlobal::font16file = Trim(par1);
                if (test == "font18file") QGlobal::font18file = Trim(par1);
              //if (test == "deftextext") Global::szDefaultExt = par1;

               TSCREEN::CFOV = QGlobal::ffov;
               if (QGlobal::iWW > 0) Global::iWindowWidth = QGlobal::iWW;
               if (QGlobal::iWH > 0) Global::iWindowHeight = QGlobal::iWH;
               if (QGlobal::bRTIES) Global::bRollFix = true;

               fullscreen = Global::bFullScreen;
        }

     WriteLog("CONFIG FILE OK.");
     WriteLog("");
     FileCFG.close();
    }
    else
    {
     WriteLog("CONFIG READ ERROR, CHECK FILE 'CONFIG.TXT'. BE APPLY DEFAULT SETS...");
     MessageBox(NULL,"CONFIG READ ERROR, CHECK FILE 'CONFIG.TXT'. BE APPLY DEFAULT SETS...", "ERROR",MB_OK|MB_ICONEXCLAMATION);
     //WriteLog("");
    }


 // !!! REJESTROWANIE DZIALA TYLKO W TRYBIE ZGODNOSCI NA WINDOWS 8
 if (rege3dt3d) WriteLog("Registering model file extensions e3d/t3d...");
// if (rege3dt3d) RegisterFileExtansion(".e3d", "maszynamodelbin", "Binarny plik modelu MaSZyna", "\\data\\icons\\e3d.ico,0" );
// if (rege3dt3d) RegisterFileExtansion(".t3d", "maszynamodeltxt", "Tekstowy plik modelu MaSZyna", "\\data\\icons\\t3d.ico,0" );
 if (rege3dt3d) MV->CreateREGfile(".e3d", "maszynamodelbin", "Binarny plik modelu MaSZyna", "\\data\\icons\\e3d.ico,0", "e3d");
 if (rege3dt3d) MV->CreateREGfile(".t3d", "maszynamodeltxt", "Tekstowy plik modelu MaSZyna", "\\data\\icons\\t3d.ico,0", "t3d");


 WriteLog("");
 WriteLog("environment informations: ");

 sprintf(cmdline, "appfile: [%s]", FULLEXE);    //argv[0]
 WriteLog(cmdline);

 sprintf(apppath, "apppath: [%s]", appath.c_str());
 WriteLog(apppath);

 sprintf(shotdir, "shotdir: [%s\%s]", appath.c_str(), QGlobal::asSSHOTDIR.c_str());
 WriteLog(shotdir);

 sprintf(cmdline, "aparams: [%s]", commandline);
 WriteLog(cmdline);

 GetAppVersion(FULLEXE.c_str(), &vmajor, &vminor, &vbuild, &vrev);
 sprintf(appvers, "appvers: %i %i %i %i", vmajor, vminor, vbuild, vrev);
 WriteLog(appvers);

 GetFileAttributesEx(FULLEXE.c_str(), GetFileExInfoStandard, &attr);
 FileTimeToSystemTime(&attr.ftLastWriteTime, &creation);
 sprintf(appdate, "appdate: %04d%02d%02d %02d%02d%02d", creation.wYear, creation.wMonth, creation.wDay, creation.wHour+2, creation.wMinute, creation.wSecond);
 WriteLog(appdate);

 sprintf(appvers, "%i.%i.%i.%i", vmajor, vminor, vbuild, vrev);
 sprintf(appdate, "%04d%02d%02d %02d%02d%02d", creation.wYear, creation.wMonth, creation.wDay, creation.wHour+2, creation.wMinute, creation.wSecond);

 QGlobal::asAPPCOMP = "release: " + AnsiString(appdate) + ", " + appvers;
 WriteLog(QGlobal::asAPPCOMP);

 //GetFileAttributesEx(ParamStr(0).c_str(), GetFileExInfoStandard, &attr);
 //FileTimeToSystemTime(&attr.ftLastWriteTime, &creation);
 sprintf(appdate, "%04d%02d%02d %02d%02d%02d", creation.wYear, creation.wMonth, creation.wDay, creation.wHour+2, creation.wMinute, creation.wSecond);

 QGlobal::asAPPVERS = "ver. " + IntToStr(vmajor) + "." + IntToStr(vminor) + "." + IntToStr(vbuild) + "." + IntToStr(vrev) + "++, " + appdate;

 sprintf(appdate, "rundate: %s", FormatDateTime("yyyymmdd hhmmss", Now()));
 WriteLog(appdate);

 sprintf(screendim, "deskdim: %ix%i", Global::iWindowWidth, Global::iWindowHeight);
 WriteLog(screendim);

 QGlobal::USERPID =  AnsiString(GetMachineID("C:\\"));
 WriteLog("userpid: " + QGlobal::USERPID);

 ss = new TStringList();
 GetOSVersionVCL(ss);

 for (int l = 0; l < ss->Count; l++) WriteLog(ss->Strings[l]);

 SetCurrentDirectory(ExtractFileDir(ParamStr(0)).c_str());

 //WriteLog("cmpconf: windows 7 Ultimate 64, AMD FX 4170 4.2GHz 4 core, 8gb ram, GFX GF8600GT CORE 560Mhz 256MB PCIE,  BIOS VIDEO: 60.84.5E.00.00, OpenGL 3.3 driver rev 2009-01-16 6.14.11.8151 ");
 WriteLog("");
 WriteLog("");

 HWND aHWnd;
 aHWnd = FindWindow(NULL, QGlobal::logwinname.c_str());                 // Szukanie okna z otwartm logiem znajac etykiete
 SendMessage(aHWnd, WM_CLOSE, 0, 0);                                    // ZAMYKAMY OTWARTY LOG

    //SetCurrentDirectory(QGlobal::asAPPDIR.c_str());
 // TUTAJ BYLO CZYTANIE .INI!!!

    // hunter-271211: ukrywanie konsoli
  //  if (Global::iWriteLogEnabled & 2)
  //  {
  //      WriteLog("Creating Console...");
  //      if (!Global::bHideConsole) AllocConsole();
  //      if (!Global::bHideConsole) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
  //  }

    WriteLog("Parsing command line (" + AnsiString(lpCmdLine) + ")...");
    AnsiString str = lpCmdLine; // parametry uruchomienia
    if (!str.IsEmpty())
    { // analizowanie parametrów
        TQueryParserComp *Parser;
        Parser = new TQueryParserComp(NULL);
        Parser->TextToParse = lpCmdLine;
        Parser->First();
        while (!Parser->EndOfFile)
        {
            str = Parser->GetNextSymbol().LowerCase();
            if (str == AnsiString("-s"))
            { // nazwa scenerii
                str = Parser->GetNextSymbol().LowerCase();
                strcpy(Global::szSceneryFile, str.c_str());
                QGlobal::bRAINSTED = true;
            }
            else if (str == AnsiString("-v"))
            { // nazwa wybranego pojazdu
                str = Parser->GetNextSymbol().LowerCase();
                Global::asHumanCtrlVehicle = str;
            }
            else if (str == AnsiString("-modifytga"))
            { // wykonanie modyfikacji wszystkich plików TGA
                Global::iModifyTGA = -1; // specjalny tryb wykonania totalnej modyfikacji
            }
            else if (str == AnsiString("-e3d"))
            { // wygenerowanie wszystkich plików E3D
                if (Global::iConvertModels > 0)
                    Global::iConvertModels = -Global::iConvertModels; // specjalny tryb
                else
                    Global::iConvertModels = -7; // z optymalizacj¹, bananami i prawid³owym Opacity
            }
            else if (str==AnsiString("-vm"))  //Q 25.12.15: podglad modelu
            {
             str = Trim(Parser->GetNextSymbol().LowerCase());

             str = str.SubString(1, 255);

             MV->modelpreview(str.c_str(), "", "", "");

             if (QGlobal::breplacescn) strcpy(Global::szSceneryFile, "modelpreview.scn");
            }
            else
                Error("Program usage: EU07 [-s sceneryfilepath] [-v vehiclename] [-modifytga] [-e3d]", !Global::iWriteLogEnabled);
        }
        delete Parser; // ABu 050205: tego wczesniej nie bylo
    }
    /* MC: usunalem tymczasowo bo sie gryzlo z nowym parserem - 8.6.2003
        AnsiString csp=AnsiString(Global::szSceneryFile);
        csp=csp.Delete(csp.Pos(AnsiString(strrchr(Global::szSceneryFile,'/')))+1,csp.Length());
        Global::asCurrentSceneryPath=csp;
    */

    //fullscreen = Global::bFullScreen;
    WindowWidth = Global::iWindowWidth;
    WindowHeight = Global::iWindowHeight;

    Bpp = Global::iBpp;
    if (Bpp != 32)
        Bpp = 16;

    if (getscreenb) // JEZELI OPCJA ROZDZIELCZOSCI IDENTYCZNEJ JAK PULPIT
        {

         Global::iWindowWidth = WindowWidth = QGlobal::iWW;
         Global::iWindowHeight = WindowHeight = QGlobal::iWH;
        }

    if (askforfull)  // OKIENKO DIALOGOWE Z ZAPYTANIEM O TRYB OKNA
        {
         if (MessageBox(NULL,"Would You Like To Run Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
         Global::bFullScreen = false;
         else Global::bFullScreen  = true;
         fullscreen = Global::bFullScreen;
        }
        
    if (!getscreenb) // JEZELI PODANA ROZDZIELCZOSC W config.txt
        {
         if (QGlobal::iWH > 0) Global::iWindowHeight = WindowHeight = QGlobal::iWH;
         if (QGlobal::iWW > 0) Global::iWindowWidth = WindowWidth = QGlobal::iWW;
        }

    if (!CreateGLWindow(Global::asHumanCtrlVehicle.c_str(), Global::iWindowWidth, Global::iWindowHeight, Bpp, fullscreen))// create our OpenGL window
     return 0; // quit if window was not created

    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    // CreateGLWindow() -> InitGL() - > World.Init() -> World.RenderMenu() -> (World.Load()

    //if (!QGlobal::brendermenu) {ShowCursor(FALSE); World.Load(hWnd, hDC); }

    if (QGlobal::brendermenu)
    {
     DEBUGGER->Left = Global::iWindowWidth / 2 - (DEBUGGER->Width / 2);
     DEBUGGER->Top = Global::iWindowHeight / 2 - (DEBUGGER->Height / 2);
     DEBUGGER->SCENERYDIR = QGlobal::asAPPDIR + "scenery\\";
     DEBUGGER->Show();
     DEBUGGER->DoubleBuffered = true;
     DEBUGGER->BringToFront();
     SetWindowPos(hWnd, HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);             // OKNO OpenGL NA SPOD
     EnableWindow(hWnd, false);                                                 // DEAKTYWUJEMY OKNO OPENGL CONY NA WIERZCH NIE WYLAZILO
     SetWindowPos(DEBUGGER->Handle, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);// OKNO STARTERA NA WIERZCH
     DEBUGGER->lversion->Caption = QGlobal::asAPPVERS;

     while (QGlobal::brendermenu) // loop that runs while done=FALSE
        {
          DEBUGGER->BringToFront();
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))                       // is there a message waiting?
            {
              //if ( DEBUGGER->launch) SetFocus(hWnd);
                if (msg.message == WM_QUIT)                                     // have we received a quit message?
                    done = TRUE;
                else
                {
                    TranslateMessage(&msg);                                     // translate the message
                    DispatchMessage(&msg);                                      // dispatch the message
                }
              SwapBuffers(hDC);
            }
            else // if there are no messages
            {
                if ( DEBUGGER->appdone) QGlobal::brendermenu = false;
                if ( DEBUGGER->launch) QGlobal::brendermenu = false;
                if ( DEBUGGER->launch) SetForegroundWindow(hWnd);
                if ( DEBUGGER->launch) SetFocus(hWnd);
                if ( DEBUGGER->launch) EnableWindow(hWnd, true);
                if (DEBUGGER->appdone) QGlobal::bAPPDONE = true;

                if (World.RenderMenu(hDC)) SwapBuffers(hDC); // Swap Buffers (Double Buffering)
                if (!QGlobal::bAPPDONE && !QGlobal::brendermenu) ShowCursor(FALSE);
                if (!QGlobal::bAPPDONE && !QGlobal::brendermenu) World.Load(hWnd, hDC);  // !! WLASCIWE WCZYTYWANIE SCENERII JUZ PO WYBORZE W MENU
                if ( QGlobal::bAPPDONE && hRC) KillGLWindow();
            }
        }
    }  // if (QGlobal::brendermenu)

    //delete DEBUGGER;
    DEBUGGER->Release();
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    
    // McZapkie: proba przeplukania klawiatury
    Console *pConsole = new Console(); // Ra: nie wiem, czy ma to sens, ale jakoœ zainicjowac trzeba
    while (Console::Pressed(VK_F10)) Error("Keyboard buffer problem - press F10"); // na Windows 98 lubi siê to pojawiaæ

    int iOldSpeed, iOldDelay;
    SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &iOldSpeed, 0);
    SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &iOldDelay, 0);
    SystemParametersInfo(SPI_SETKEYBOARDSPEED, 20, NULL, 0);


    if (!joyGetNumDevs()) WriteLog("No joystick");
    if (Global::iModifyTGA < 0)
    { // tylko modyfikacja TGA, bez uruchamiania symulacji
        Global::iMaxTextureSize = 64; //¿eby nie zamulaæ pamiêci
        World.ModifyTGA(); // rekurencyjne przegl¹danie katalogów
    }
   else
    {
        if (Global::iConvertModels < 0)
        {
            Global::iConvertModels = -Global::iConvertModels;
            World.CreateE3D("models\\"); // rekurencyjne przegl¹danie katalogów
            World.CreateE3D("dynamic\\", true);
        } // po zrobieniu E3D odpalamy normalnie sceneriê, by j¹ zobaczyæ

        Console::On(); // w³¹czenie konsoli

       if (QGlobal::bAPPDONE ) done = true;

       //if (QGlobal::bSENDLOGFTP > 0) SENDLOGTOFTP();   // PRZENOSZE DO WORLD.CPP

       while (!done) // loop that runs while done=FALSE
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // is there a message waiting?
            {
                if (msg.message == WM_QUIT) // have we received a quit message?
                    done = TRUE; // if so
                else // if not, deal with window messages
                {
                    // if (msg.message==WM_CHAR)
                    // World.OnKeyDown(msg.wParam);
                    TranslateMessage(&msg); // translate the message
                    DispatchMessage(&msg); // dispatch the message
                }
            }
            else // if there are no messages
            {
                // draw the scene, watch for quit messages
                // DrawGLScene()
                // if (!pause)
                // if (Global::bInactivePause?Global::bActive:true) //tak nie, bo spada z góry
                if (World.Update()) // Was There A Quit Received?
                    SwapBuffers(hDC); // Swap Buffers (Double Buffering)
                else
                    done = true; //[F10] or DrawGLScene signalled a quit
            }
        }
        Console::Off(); // wy³¹czenie konsoli (komunikacji zwrotnej)
    }
    SystemParametersInfo(SPI_SETKEYBOARDSPEED, iOldSpeed, NULL, 0);
    SystemParametersInfo(SPI_SETKEYBOARDDELAY, iOldDelay, NULL, 0);

    DeleteFile("templog.txt"); // usuniêcie starego
    DeleteFile("myconsist.txt"); // usuniêcie starego
    DeleteFile(AnsiString(QGlobal::asAPPDIR + "models\\temp\\temp.e3d").c_str());

    delete pConsole; // deaktywania sterownika

    if (hRC) KillGLWindow(); // kill the window
    return (msg.wParam); // exit the program
}
































