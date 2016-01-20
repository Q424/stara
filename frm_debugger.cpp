//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_debugger.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma resource "*.dfm"
TDEBUGGER *DEBUGGER;
//---------------------------------------------------------------------------
__fastcall TDEBUGGER::TDEBUGGER(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------



int roundsize = 15;


void __fastcall TDEBUGGER::FormCreate(TObject *Sender)
{
//SetWindowLong(Handle,GWL_STYLE, GetWindowLong(Handle,GWL_STYLE) && !WS_CAPTION);
//ClientHeight = Height - 48;

  HRGN frmrgn;  
  HANDLE hwnd;

  SetWindowLong(Handle, GWL_EXSTYLE, GetWindowLong(Handle, GWL_STYLE) | WS_EX_LAYERED);
  SetLayeredWindowAttributes(Handle, 0, (255 * 255) /100, LWA_ALPHA);

  frmrgn = CreateRoundRectRgn (0, 0, ClientWidth, ClientHeight,roundsize,roundsize);
  SetWindowRgn(Handle,frmrgn,true);
}
//---------------------------------------------------------------------------


void __fastcall TDEBUGGER::SpeedButton1Click(TObject *Sender)
{
launch = true;
DEBUGGER->Visible = false;
}
//---------------------------------------------------------------------------



void __fastcall TDEBUGGER::FormPaint(TObject *Sender)
{
HRGN hRegion = ::CreateRoundRectRgn (0, 0, ClientWidth, ClientHeight,roundsize+1,roundsize+1);
Canvas->Brush->Style = bsSolid;
Canvas->Brush->Color = RGB(255, 180, 26);
::FrameRgn(Canvas->Handle, hRegion, Canvas->Brush->Handle, 4, 4);
::DeleteObject(hRegion); // Don't leak a GDI object
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::Timer1Timer(TObject *Sender)
{
  int gbAlpha = 245;
  BLENDFUNCTION blend;
  blend.BlendOp = AC_SRC_OVER;
  blend.BlendFlags = 0;
  blend.AlphaFormat = 0;
  blend.SourceConstantAlpha = gbAlpha;

  UpdateLayeredWindow(DEBUGGER->Handle, NULL, NULL ,NULL ,NULL ,NULL, NULL, &blend, ULW_ALPHA);
  if(gbAlpha > 35)
  {
    gbAlpha -= 35;
  }
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::SpeedButton2Click(TObject *Sender)
{
 appdone = true;        
}
//---------------------------------------------------------------------------

