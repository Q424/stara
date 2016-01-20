object DEBUGGER: TDEBUGGER
  Left = 272
  Top = 202
  BorderStyle = bsNone
  Caption = 'DEBUGGER'
  ClientHeight = 623
  ClientWidth = 892
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object SpeedButton1: TSpeedButton
    Left = 760
    Top = 584
    Width = 121
    Height = 30
    Caption = 'Uruchom'
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Gabriola'
    Font.Style = []
    ParentFont = False
    OnClick = SpeedButton1Click
  end
  object Shape1: TShape
    Left = 8
    Top = 8
    Width = 153
    Height = 33
    Brush.Color = clGray
    Pen.Style = psDot
    Shape = stRoundRect
  end
  object Label1: TLabel
    Left = 32
    Top = 12
    Width = 60
    Height = 24
    Caption = 'DŸwiêk'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape2: TShape
    Left = 176
    Top = 8
    Width = 153
    Height = 33
    Brush.Color = clGray
    Pen.Style = psDot
    Shape = stRoundRect
  end
  object Label2: TLabel
    Left = 200
    Top = 12
    Width = 57
    Height = 24
    Caption = 'Grafika'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape3: TShape
    Left = 560
    Top = 8
    Width = 153
    Height = 33
    Brush.Color = clGray
    Pen.Style = psDot
    Shape = stRoundRect
  end
  object Label3: TLabel
    Left = 584
    Top = 12
    Width = 106
    Height = 24
    Caption = 'O programie'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape4: TShape
    Left = 728
    Top = 8
    Width = 153
    Height = 33
    Brush.Color = clGray
    Pen.Style = psDot
    Shape = stRoundRect
  end
  object Label4: TLabel
    Left = 752
    Top = 12
    Width = 93
    Height = 24
    Caption = 'O autorach'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Shape5: TShape
    Left = 8
    Top = 56
    Width = 873
    Height = 2
    Brush.Color = clSilver
    Pen.Style = psDot
  end
  object Bevel1: TBevel
    Left = 8
    Top = 48
    Width = 873
    Height = 9
    Shape = bsTopLine
  end
  object SpeedButton2: TSpeedButton
    Left = 624
    Top = 584
    Width = 121
    Height = 30
    Caption = 'WyjdŸ'
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Gabriola'
    Font.Style = []
    ParentFont = False
    OnClick = SpeedButton2Click
  end
  object Bevel2: TBevel
    Left = 8
    Top = 576
    Width = 873
    Height = 9
    Shape = bsTopLine
  end
  object Shape6: TShape
    Left = 344
    Top = 8
    Width = 153
    Height = 33
    Brush.Color = clGray
    Pen.Style = psDot
    Shape = stRoundRect
  end
  object Label5: TLabel
    Left = 368
    Top = 12
    Width = 90
    Height = 24
    Caption = 'Multiplayer'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object lversion: TLabel
    Left = 8
    Top = 580
    Width = 47
    Height = 16
    Caption = 'lversion'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 5460819
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label6: TLabel
    Left = 8
    Top = 600
    Width = 72
    Height = 13
    Caption = 'http://eu07.pl'
    Color = clBlue
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGray
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsItalic]
    ParentColor = False
    ParentFont = False
    Transparent = True
  end
  object Label7: TLabel
    Left = 88
    Top = 600
    Width = 70
    Height = 13
    Caption = 'http:/eu07.es'
    Color = clBlue
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGray
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsItalic]
    ParentColor = False
    ParentFont = False
    Transparent = True
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 848
    Top = 168
  end
end
