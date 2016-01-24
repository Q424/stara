//---------------------------------------------------------------------------

#ifndef frm_debuggerH
#define frm_debuggerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

//---------------------------------------------------------------------------
class TDEBUGGER : public TForm
{
__published:	// IDE-managed Components
        TSpeedButton *SpeedButton1;
        TTimer *Timer1;
        TShape *Shape1;
        TLabel *Label1;
        TShape *Shape3;
        TLabel *Label3;
        TShape *Shape4;
        TLabel *Label4;
        TShape *Shape5;
        TBevel *Bevel1;
        TSpeedButton *SpeedButton2;
        TBevel *Bevel2;
        TShape *Shape6;
        TLabel *Label5;
        TLabel *lversion;
        TLabel *Label6;
        TLabel *Label7;
        TListBox *LBSCN;
        TListBox *ListBox1;
        TPanel *Panel1;
        TImage *Image1;
        TLabel *Label8;
        TTrackBar *TrackBar1;
        TScrollBox *ScrollBox1;
        TShape *Shape2;
        TLabel *Label2;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall SpeedButton1Click(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall SpeedButton2Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall LBSCNClick(TObject *Sender);
        void __fastcall TrackBar1Change(TObject *Sender);
        void __fastcall FormDblClick(TObject *Sender);
        void __fastcall Label2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TDEBUGGER(TComponent* Owner);
        bool launch;
        bool appdone;
        AnsiString SCENERYDIR;
};
//---------------------------------------------------------------------------
extern PACKAGE TDEBUGGER *DEBUGGER;
//---------------------------------------------------------------------------
#endif
