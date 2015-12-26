//---------------------------------------------------------------------------

#ifndef frm_debuggerH
#define frm_debuggerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "FtpCli.hpp"
//---------------------------------------------------------------------------
class TDEBUGGER : public TForm
{
__published:	// IDE-managed Components
        TFtpClient *FTP;
        TMemo *Memo1;
        void __fastcall FTPSessionConnected(TObject *Sender, WORD Error);
        void __fastcall FTPCommand(TObject *Sender, AnsiString &Cmd);
        void __fastcall FTPReadyToTransmit(TObject *Sender, bool &bCancel);
        void __fastcall FTPRequestDone(TObject *Sender, TFtpRequest RqType,
          WORD Error);
        void __fastcall FTPDisplay(TObject *Sender, AnsiString &Msg);
private:	// User declarations
public:		// User declarations
        __fastcall TDEBUGGER(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDEBUGGER *DEBUGGER;
//---------------------------------------------------------------------------
#endif
