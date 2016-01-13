//---------------------------------------------------------------------------

#ifndef frm_debuggerH
#define frm_debuggerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
class TDEBUGGER : public TForm
{
__published:	// IDE-managed Components
        TButton *Button1;
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TDEBUGGER(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDEBUGGER *DEBUGGER;
//---------------------------------------------------------------------------
#endif
