//
// lib.h - nag³ówek klasy bazowej
//
#include <windows.h>

struct recvpacket
{
 bool P1B1, P1B2, P1B3, P1B4, P1B5, P1B6, P1B7, P1B8;
 bool P2B1, P2B2, P2B3, P2B4, P2B5, P2B6, P2B7, P2B8;
 bool P3B1, P3B2, P3B3, P3B4, P3B5, P3B6, P3B7, P3B8;
 bool P4B1, P4B2, P4B3, P4B4, P4B5, P4B6, P4B7, P4B8;
 bool P5B1, P5B2, P5B3, P5B4, P5B5, P5B6, P5B7, P5B8;
 bool P6B1, P6B2, P6B3, P6B4, P6B5, P6B6, P6B7, P6B8;
 bool P7B1, P7B2, P7B3, P7B4, P7B5, P7B6, P7B7, P7B8;
};

class cLib
{
 public:

  // AP ANALOG POINTER
  // SW SWITCH
  // LT LIGHT

  int ap_VelocityGauge;
  int ap_I1Gauge;
  int ap_I2Gauge;
  int ap_I3Gauge;
  int ap_ItotalGauge;
  int ap_CylHamGauge;
  int ap_PrzGlGauge;
  int ap_ZbGlGauge;
  int ap_ClockMInd;
  int ap_ClockHInd;
  int ap_HVoltageGauge;
  int ap_LVoltageGauge;
  int ap_enrot1mGauge;
  int ap_enrot2mGauge;
  int ap_enrot3mGauge;
  int ap_engageratioGauge;
  int ap_maingearstatusGauge;

  int sw_MainCtrlGauge;
  int sw_ScndCtrlGauge;
  int sw_DirKeyGauge;
  int sw_BrakeCtrlGauge;
  int sw_LocalBrakeGauge;
  int sw_BrakeProfileCtrlGauge;
  int sw_MaxCurrentCtrlGauge;
  int sw_MainOffButtonGauge;
  int sw_MainOnButtonGauge;
  int sw_MainButtonGauge; //EZT
  int sw_SecurityResetButtonGauge;
  int sw_ReleaserButtonGauge;
  int sw_AntiSlipButtonGauge;
  int sw_FuseButtonGauge;
  int sw_StLinOffButtonGauge;
  int sw_RadioButtonGauge;
  int sw_UpperLightButtonGauge;
  int sw_LeftLightButtonGauge;
  int sw_RightLightButtonGauge;
  int sw_LeftEndLightButtonGauge;
  int sw_RightEndLightButtonGauge;
  int sw_IgnitionKeyGauge;
  int sw_CompressorButtonGauge;
  int sw_ConverterButtonGauge;
  int sw_ConverterOffButtonGauge;
  int sw_HornButtonGauge;
  int sw_NextCurrentButtonGauge;
  int sw_PantFrontButtonGauge;
  int sw_PantRearButtonGauge;
  int sw_PantFrontButtonOffGauge; //EZT
  int sw_PantAllDownButtonGauge;

  bool lt_btLampkaPoslizg;
  bool lt_btLampkaStyczn;
  bool lt_btLampkaNadmPrzetw;
  bool lt_btLampkaPrzekRozn;
  bool lt_btLampkaPrzekRoznPom;
  bool lt_btLampkaNadmSil;
  bool lt_btLampkaWylSzybki;
  bool lt_btLampkaNadmWent;
  bool lt_btLampkaNadmSpr;
  bool lt_btLampkaOpory;
  bool lt_btLampkaWysRozr;
  bool lt_btLampkaWentZaluzje;     //ET22
  bool lt_btLampkaOgrzewanieSkladu;
  bool lt_btLampkaSHP;
  bool lt_btLampkaCzuwaka;
  bool lt_btLampkaRezerwa;
  bool lt_btLampkaDoorLeft;     // EZT
  bool lt_btLampkaDoorRight;     // EZT
  bool lt_btLampkaDepartureSignal; // EZT

  recvpacket *rp;

  virtual void Pokaz1(char* txt) = 0;
  virtual bool opencommp(char* txt, bool log) = 0;
  virtual bool clearvars(bool mode) = 0;
  virtual bool sendcommp(byte data) = 0;
  virtual bool recvcommp(recvpacket *rp) = 0;  // , int bn, bool *p1, bool *p2, bool *p3, bool *p4, bool *p5, bool *p6, bool *p7, byte *p8, byte *p9, byte *p10, byte *p11, byte *p12, byte *p13
  virtual long state_patykA();
  virtual long state_patykB();
  virtual bool setstates(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n, int o, int p, int r, int s, int t, int u) = 0;
  virtual bool getstates(int num) = 0;

};
