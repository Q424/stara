//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frm_debugger.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "FtpCli"
#pragma resource "*.dfm"
TDEBUGGER *DEBUGGER;
//---------------------------------------------------------------------------
__fastcall TDEBUGGER::TDEBUGGER(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDEBUGGER::FTPSessionConnected(TObject *Sender, WORD Error)
{
Memo1->Lines->Add("Session opened");
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::FTPCommand(TObject *Sender, AnsiString &Cmd)
{
Memo1->Lines->Add("command " + Cmd);        
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::FTPReadyToTransmit(TObject *Sender,
      bool &bCancel)
{
Memo1->Lines->Add("ready to transmit");        
}
//---------------------------------------------------------------------------


void __fastcall TDEBUGGER::FTPRequestDone(TObject *Sender,
      TFtpRequest RqType, WORD Error)
{
Memo1->Lines->Add("rd");        
}
//---------------------------------------------------------------------------





void __fastcall TDEBUGGER::FTPDisplay(TObject *Sender, AnsiString &Msg)
{
Memo1->Lines->Add("display " + Msg);        
}
//---------------------------------------------------------------------------

