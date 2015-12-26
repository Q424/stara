object DEBUGGER: TDEBUGGER
  Left = 312
  Top = 781
  Width = 872
  Height = 199
  BorderStyle = bsSizeToolWin
  Caption = 'DEBUGGER'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 8
    Top = 8
    Width = 849
    Height = 153
    Lines.Strings = (
      'Memo1')
    TabOrder = 0
  end
  object FTP: TFtpClient
    Timeout = 15
    MultiThreaded = False
    Port = 'ftp'
    LocalAddr = '0.0.0.0'
    DisplayFileFlag = False
    Binary = True
    ShareMode = ftpShareExclusive
    Options = [ftpAcceptLF]
    ConnectionType = ftpDirect
    OnDisplay = FTPDisplay
    OnCommand = FTPCommand
    OnSessionConnected = FTPSessionConnected
    OnRequestDone = FTPRequestDone
    OnReadyToTransmit = FTPReadyToTransmit
    Left = 16
    Top = 16
  end
end
