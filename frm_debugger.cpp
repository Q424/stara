//---------------------------------------------------------------------------

#include <vcl.h>
#include <iostream>
#include <vector.h>
#include <algorithm>
#include "assert.h"
#include <string>
#include <fstream>
#include <sstream>
#include <istream>
#pragma hdrstop

#include "frm_debugger.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma resource "*.dfm"
TDEBUGGER *DEBUGGER;
AnsiString SCNPREVIEW, SCENERYNAME;
//---------------------------------------------------------------------------
__fastcall TDEBUGGER::TDEBUGGER(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------



int roundsize = 15;
 HWND WindowHandle;


// *****************************************************************************
// TWORZENIE LISTY PLIKOW Z WYBRANEGO KATALOGU
// *****************************************************************************
int listdir(const char *szDir, bool bCountHidden, AnsiString ext, TStringList &SL)
{
	char path[MAX_PATH];
	WIN32_FIND_DATA fd;
	DWORD dwAttr = FILE_ATTRIBUTE_DIRECTORY;
	if( !bCountHidden) dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	sprintf( path, "%s\\*", szDir);
	HANDLE hFind = FindFirstFile( path, &fd);
        AnsiString noext, sinext;
        
	if(hFind != INVALID_HANDLE_VALUE)
	{
		int count = 0;
		do
		{
			if( !(fd.dwFileAttributes & dwAttr))
                          {
				puts( fd.cFileName);

                              // TWORZENIE LISTY SCENERII ^^^^^^^^^^^^^^^^^^^^^^
                              if (ext == "scn")
                              if (ExtractFileExt(fd.cFileName) == ".scn")
                                  {
                                   noext = fd.cFileName;
                                   sinext = noext.SubString(1, noext.Length()-4);
                                   SL.Add( sinext );
                                 //WriteLog( sinext );
                                  }

                           }
		}while( FindNextFile( hFind, &fd));
		FindClose( hFind);
		return count;
	}
	return -1;
}

void __fastcall TDEBUGGER::FormCreate(TObject *Sender)
{

  HRGN frmrgn;  
  HANDLE hwnd;
  TStringList *slFiles;

  slFiles = new TStringList;
  slFiles->Clear();
  LBSCN->Clear();

  SetWindowLong(Handle, GWL_EXSTYLE, GetWindowLong(Handle, GWL_STYLE) | WS_EX_LAYERED);
  SetLayeredWindowAttributes(Handle, 0, (255 * 90) /100, LWA_ALPHA);

  frmrgn = CreateRoundRectRgn (0, 0, ClientWidth, ClientHeight,roundsize,roundsize);
  SetWindowRgn(Handle,frmrgn,true);

//  SetWindowLong(Handle,GWL_STYLE, GetWindowLong(Handle,GWL_STYLE) && !WS_CAPTION);
//  ClientHeight = Height - 48;


  listdir("scenery\\", false, "scn", *slFiles);

  LBSCN->Items = slFiles;

}
//---------------------------------------------------------------------------


void __fastcall TDEBUGGER::SpeedButton1Click(TObject *Sender)
{
DEBUGGER->Height = 0;
Application->ProcessMessages();
launch = true;
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
  int gbAlpha = 205;
  BLENDFUNCTION blend;
  blend.BlendOp = AC_SRC_OVER;
  blend.BlendFlags = 0;
  blend.AlphaFormat = 0;
  blend.SourceConstantAlpha = gbAlpha;

  UpdateLayeredWindow(Handle, NULL, NULL ,NULL ,NULL ,NULL, NULL, &blend, ULW_ALPHA);
  //if(gbAlpha > 35)
  //{
 //   gbAlpha -= 35;
 // }
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::SpeedButton2Click(TObject *Sender)
{
 DEBUGGER->Hide();
 appdone = true;
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::FormShow(TObject *Sender)
{
// DEBUGGER->BringToFront();

//   int gbAlpha = 205;
//  BLENDFUNCTION blend;
//  blend.BlendOp = AC_SRC_OVER;
//  blend.BlendFlags = 0;
//  blend.AlphaFormat = 0;
//  blend.SourceConstantAlpha = gbAlpha;

//  UpdateLayeredWindow(DEBUGGER->Handle, NULL, NULL ,NULL ,NULL ,NULL, NULL, &blend, ULW_ALPHA);


}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::LBSCNClick(TObject *Sender)
{
 SCNPREVIEW = SCENERYDIR + "\\images\\" + LBSCN->Items->Strings[LBSCN->ItemIndex] + ".jpg";
 Label8->Caption = LBSCN->Items->Strings[LBSCN->ItemIndex];
 if (FileExists(SCNPREVIEW))
   Image1->Picture->LoadFromFile(SCNPREVIEW);
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::TrackBar1Change(TObject *Sender)
{
  HANDLE hwnd;
  HWND hHandle = FindWindow (NULL, "Kalkulator");
  SetWindowLong(Handle, GWL_EXSTYLE, GetWindowLong(Handle, GWL_STYLE) | WS_EX_LAYERED);

  SetLayeredWindowAttributes(Handle, 0, (255 * TrackBar1->Position) /100, LWA_ALPHA);
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::FormDblClick(TObject *Sender)
{
 TrackBar1->Visible = !TrackBar1->Visible;        
}
//---------------------------------------------------------------------------

void __fastcall TDEBUGGER::Label2Click(TObject *Sender)
{
 DEBUGGER->Hide();
 appdone = true;        
}
//---------------------------------------------------------------------------

