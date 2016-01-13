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












void __fastcall TDEBUGGER::FormCreate(TObject *Sender)
{
SetWindowLong(Handle,GWL_STYLE, GetWindowLong(Handle,GWL_STYLE) && !WS_CAPTION);
ClientHeight = Height - 48;
}
//---------------------------------------------------------------------------

