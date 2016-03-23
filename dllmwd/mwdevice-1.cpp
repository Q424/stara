//---------------------------------------------------------------------------
/*

    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak, Maciej Czapkiewicz and others
                                                                                                                 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include <fstream>
#include <string>

#include "system.hpp"
#include "classes.hpp"
#include "sysutils.hpp"
#include "shellapi.h"

#pragma hdrstop

#include "Timer.h"
#include "mtable.hpp"
#include "Sound.h"
#include "World.h"
#include "dynobj.h"
#include "train.h"
#include "logs.h"
#include "Globals.h"
#include "Camera.h"
#include "qutils.h"

 bool BL_WYLACZNIKSZYBKI = false;
 bool BL_POSLIZG = false;
 bool BL_STYCZNIKILINIOWE = false;
 bool BL_NADMIARPRZETW = false;
 bool BL_PROZNICOWY = false;
 bool BL_NADMIARSILNIK = false;
 bool BL_NADMIARWENTYL = false;
 bool BL_NADMIARSPREZARKI = false;
 bool BL_OGRZEWANIESKLADU = false;
 bool BL_OPORYROZRUCHOWE = false;
 bool BL_WYSOKIROZRUCH = false;
 bool BL_SHP = false;
 bool BL_CZUWAK = false;
 bool BL_WENTZALUZJE = false;
 bool BL_SPREZARKAA = false;
 bool BL_SPREZARKAB = false;
 bool BL_HAMOWANIE = false;
 bool BL_JAZDA = false;
 bool BL_RADIOTEL = false;
 bool BL_BOCZNIK1 = false;
 bool BL_BOCZNIK2 = false;
 bool BL_DOORL = false;
 bool BL_DOORR = false;
 bool BL_DEPARTURESIG = false;
 bool BL_FORW = false;
 bool BL_BACK = false;
 bool BL_KABINA = false;
 bool BL_SRJ = false;
 bool BL_PRZYZADY = false;

 bool czuwak_sw_state = false;	//zmienne do obs³ugi haslera
 bool shp_sw_state = false;
 bool kabina1 = false;			
 bool kabina2 = false;
 
 float Velocity = 0.0f;
 float BrakePress = 0.0f;
 float PipePress = 0.0f;
 float Compressor = 0.0f;
 float Voltage1 = 0.0f;
 float Current1 = 0.0f;
 float Current2 = 0.0f;
 float Current3 = 0.0f;

 
byte SETBYTE0(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
  byte data = 0;

  data |= (b1<<0);
  data |= (b2<<1);
  data |= (b3<<2);
  data |= (b4<<3);
  data |= (b5<<4);
  data |= (b6<<5);
  data |= (b7<<6);
  data |= (b8<<7);

  return data;
}

byte SETBYTE1(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
  byte data = 0;

  data |= (b1<<0);
  data |= (b2<<1);
  data |= (b3<<2);
  data |= (b4<<3);
  data |= (b5<<4);
  data |= (b6<<5);
  data |= (b7<<6);
  data |= (b8<<7);

  return data;
}


bool TWorld::manipopen(char* commname, bool log)
{
 if (QGlobal::bOPCOM) QGlobal::DLL->opencommp(commname, log);
}


bool TWorld::manipsend(int id)
{

if (QGlobal::hDLL)
 if (Controlled && Train)
  {
   Velocity = Controlled->MoverParameters->Vel;
   BrakePress = Controlled->MoverParameters->BrakePress;
   PipePress = Controlled->MoverParameters->PipePress;
   Compressor = Controlled->MoverParameters->Compressor;
   Voltage1 = Controlled->MoverParameters->Voltage;
   Current1 = Controlled->MoverParameters->ShowCurrent(1);
   Current2 = Controlled->MoverParameters->ShowCurrent(2);
   Current3 = Controlled->MoverParameters->ShowCurrent(3);


   float fspeed = Velocity;                                                             // POBIERANIE PREDKOSCI
   int speed = (int) floor(fspeed + 0.5);                                               // KONWERSJA FLOAT DO INT
   byte bSPEED = (unsigned char)speed;                                                  // INT NA BYTE
 //int spd =  (int) bSPEED;                                                             // BYTE NA INT DLA TESTU CZY JEST OK

   float fbrakepressdiv = BrakePress / (QGlobal::fmaxCH * 10);  // np 0.40/10 = 0.04
   float fbrakepress255 = (fbrakepressdiv * 255);                                          // 0.04 * 255 = 10.2
   int ibrakepress = (int) floor(fbrakepress255 + 0.5);
   byte bbrakepress = (unsigned char)ibrakepress;                                          // INT NA BYTE

   float fpipepressdiv = PipePress / (QGlobal::fmaxPG * 10);      // np 0.40/10 = 0.04
   float fpipepress255 = (fpipepressdiv * 255);                                            // 0.04 * 255 = 10.2
   int ipipepress = (int) floor(fpipepress255 + 0.5);
   byte bpipepress = (unsigned char)ipipepress;                                            // INT NA BYTE

   float fcomppressdiv = Compressor / (QGlobal::fmaxZG * 10);      // np 0.40/10 = 0.04
   float fcomppress255 = (fcomppressdiv * 255);                                            // 0.04 * 255 = 10.2
   int icomppress = (int) floor(fcomppress255 + 0.5);
   byte bcomppress = (unsigned char)icomppress;                                            // INT NA BYTE

   float fvoltagediv = Voltage1 / QGlobal::fmaxV1;
   float fvoltage255 = (fvoltagediv * 255);
   int ivoltage = (int) floor(fvoltage255 + 0.5);
   byte bvoltagev1 = (unsigned char)ivoltage;                                              // INT NA BYTE

   float fcurrent1div = Current1 / QGlobal::fmaxA1;
   float fcurrent1255 = (fcurrent1div * 255);
   int icurrent1 = (int) floor(fcurrent1255 + 0.5);
   byte bcurrentA1 = (unsigned char)icurrent1;                                             // INT NA BYTE

   float fcurrent2div = Current2 / QGlobal::fmaxA1;
   float fcurrent2255 = (fcurrent2div * 255);
   int icurrent2 = (int) floor(fcurrent2255 + 0.5);
   byte bcurrentA2 = (unsigned char)icurrent2;                                             // INT NA BYTE

   float fcurrent3div = Current3 / QGlobal::fmaxA1;
   float fcurrent3255 = (fcurrent3div * 255);
   int icurrent3 = (int) floor(fcurrent3255 + 0.5);
   byte bcurrentA3 = (unsigned char)icurrent3;                                             // INT NA BYTE


   BL_WYLACZNIKSZYBKI  = Train->btLampkaWylSzybki.bOn;               // P1
   BL_POSLIZG          = Train->btLampkaPoslizg.bOn;                 // P1       //
   BL_STYCZNIKILINIOWE = Train->btLampkaStyczn.bOn;                  // P1       //
   BL_NADMIARPRZETW    = Train->btLampkaNadmPrzetw.bOn;                          //
   BL_PROZNICOWY       = Train->btLampkaPrzekRozn.bOn;                           //
   BL_NADMIARSILNIK    = Train->btLampkaNadmSil.bOn;                             //
   BL_NADMIARWENTYL    = Train->btLampkaNadmWent.bOn;                // P2       //
   BL_NADMIARSPREZARKI = Train->btLampkaNadmSpr.bOn;                             //
   BL_OGRZEWANIESKLADU = Train->btLampkaOgrzewanieSkladu.bOn;                    //
   //                    Train->btLampkaOporyB.bOn;
   //                    Train->btLampkaStycznB.bOn;
   //                    Train->btLampkaWylSzybkiB.bOn;
   //                    Train->btLampkaNadmPrzetwB.bOn;
   //                    Train->btLampkaPrzetwB.bOn;
   //                    btLampkaNapNastHam.bOn;
   //                    btLampkaBoczniki.bOn;
   BL_OPORYROZRUCHOWE  = Train->btLampkaOpory.bOn;                               //
   BL_WYSOKIROZRUCH    = Train->btLampkaWysRozr.bOn;                             //
   BL_SHP              = Train->btLampkaSHP.bOn;                                 //
   BL_CZUWAK           = Train->btLampkaCzuwaka.bOn;                             //
   BL_WENTZALUZJE      = Train->btLampkaWentZaluzje.bOn;                         //
   BL_SPREZARKAA       = Train->btLampkaSprezarka.bOn;                           //
   BL_SPREZARKAB       = Train->btLampkaSprezarkaB.bOn;                          //
   BL_HAMOWANIE        = Train->btLampkaHamienie.bOn;                            //
   BL_JAZDA            = Train->btLampkaJazda.bOn;                               //
   BL_RADIOTEL         = Train->btLampkaRadiotelefon.bOn;                        //
   BL_BOCZNIK1         = Train->btLampkaBocznik1.bOn;                            //
   BL_BOCZNIK2         = Train->btLampkaBocznik2.bOn;                            //
   BL_DOORL            = Train->btLampkaDoorLeft.bOn;                            //
   BL_DOORR            = Train->btLampkaDoorRight.bOn;                           //
   BL_DEPARTURESIG     = Train->btLampkaDepartureSignal.bOn;                     //
   BL_FORW             = Train->btLampkaForward.bOn;                             //
   BL_BACK             = Train->btLampkaBackward.bOn;                            //

   byte dataout0 = 0;
   byte dataout1 = 0;
   byte dataout2 = 0;
   byte dataout3 = 0;
   byte dataout4 = 0;
   byte dataout5 = 0;
   byte dataout6 = 0;

	if (BL_HAMOWANIE)				//logika rysika 2
	{
		czuwak_sw_state = false;
	}

	if (BL_SHP)				        //wykrywanie przejazdu nad rezonatorem torowym
	{
		if (shp_sw_state)
		{
			shp_sw_state = false;
		}
		else
		{
			shp_sw_state = true;
		}
	}
		
	if (BL_FORW)					//logika rysika 3
	{
		if (shp_sw_state)
		{
			kabina1 = false;
		}
		else
		{
			kabina1 = true;
		}
	}
	if (BL_BACK)
	{
		if (shp_sw_state)
		{
			kabina2 = false;
		}
		else
		{
			kabina2 = true;
		}
	}

   dataout0 = SETBYTE0(0, 0, 0, 0, 0, 0, 0, 0);
   dataout1 = SETBYTE1(BL_WYLACZNIKSZYBKI, BL_STYCZNIKILINIOWE, BL_CZUWAK, BL_NADMIARSILNIK, BL_OGRZEWANIESKLADU, BL_WYSOKIROZRUCH, BL_OPORYROZRUCHOWE, BL_POSLIZG);
   dataout2 = SETBYTE1(BL_WYLACZNIKSZYBKI, BL_STYCZNIKILINIOWE, BL_CZUWAK, BL_NADMIARSILNIK, BL_OGRZEWANIESKLADU, BL_WYSOKIROZRUCH, BL_OPORYROZRUCHOWE, BL_POSLIZG);
   dataout3 = SETBYTE1(1, 0, 0, 0, 0, 0, 0, 0);
//dla ET22
   dataout4 = SETBYTE1(BL_OGRZEWANIESKLADU, BL_OPORYROZRUCHOWE, BL_POSLIZG, BL_WENTZALUZJE, 0, 0, BL_CZUWAK, BL_SHP);
   dataout5 = SETBYTE1(BL_STYCZNIKILINIOWE, 0, BL_NADMIARPRZETW, BL_PROZNICOWY, BL_NADMIARSILNIK, BL_WYLACZNIKSZYBKI, 0, BL_NADMIARSPREZARKI); // 7 -> przekaŸnik ró¿nicowy obwodów pomocniczych, 2 -> sygn zaniku pr¹du przy jeŸdzie na oporach
   dataout6 = SETBYTE1(BL_STYCZNIKILINIOWE, czuwak_sw_state, BL_HAMOWANIE, kabina1, kabina2, 0, 0, 0); //do haslera jazda z pr¹dem, u¿ycie przycisku czujnoœci, hamowanie, kabina(kierunek?), przejazd nad rezonatorem
   czuwak_sw_state = false;


   // BAJT 1 WYJSCIA NISKOPRADOWE PRZY KRAWEDZI
   QGlobal::DLL->sendcommp(dataout1);    // WYJSCIA NISKOPRADOWE PORT 1
   QGlobal::DLL->sendcommp(dataout2);    // WYJSCIA NISKOPRADOWE PORT 2
   QGlobal::DLL->sendcommp(dataout3);    // WYJSCIA NISKOPRADOWE PORT 3
   QGlobal::DLL->sendcommp(dataout4);    // WYJSCIA NISKOPRADOWE PORT 4
   QGlobal::DLL->sendcommp(dataout5);    // WYJSCIA NISKOPRADOWE PORT 5
   QGlobal::DLL->sendcommp(dataout6);    // WYJSCIA NISKOPRADOWE PORT 6
   QGlobal::DLL->sendcommp(bbrakepress); // PWM 01 	                -> ciœnienie w cylindrze hamulcowym
   QGlobal::DLL->sendcommp(bpipepress);  // PWM 02		        -> ciœnienie w przewodzie g³ównym
   QGlobal::DLL->sendcommp(bcomppress);  // PWM 03		        -> ciœnienie w zbiorniku g³ownym
   QGlobal::DLL->sendcommp(bvoltagev1);  // PWM 04		        -> woltomierz WN
   QGlobal::DLL->sendcommp(bcurrentA1);  // PWM 05		        -> amperomierz silników trakcyjnych 1
   QGlobal::DLL->sendcommp(bcurrentA2);  // PWM 06		        -> amperomierz silników trakcyjnych 2
   QGlobal::DLL->sendcommp(bcurrentA3);  // PWM 07		        -> amperomierz silników trakcyjnych 3
   QGlobal::DLL->sendcommp(bcurrentA3);  // PWM 07
   QGlobal::DLL->sendcommp(dataout0);    // PWM 08
   QGlobal::DLL->sendcommp(dataout0);    // PWM 09
   QGlobal::DLL->sendcommp(dataout0);    // PWM 10
   QGlobal::DLL->sendcommp(dataout0);    // PWM 11
   QGlobal::DLL->sendcommp(dataout0);    // PWM 12
   QGlobal::DLL->sendcommp(dataout0);    // PWM 13
   QGlobal::DLL->sendcommp(dataout0);    // PWM 14
   QGlobal::DLL->sendcommp(dataout0);    // PWM 15
   QGlobal::DLL->sendcommp(bSPEED);      // VELOCITY


//############################################################################################################
//odbieranie i przetwarzanie danych z MWD *** START ***

   if (QGlobal::fMWDInEnable)
    {

    }
  }
}


// ***********************************************************************************************************
// INICJALIZACJA
// ***********************************************************************************************************
bool TWorld::manipinit(bool test)
{
 if (FEX("dlls\\device.dll"))
  {
   QGlobal::hDLL = LoadLibrary("dlls\\device.dll");

   typedef cLib* (*regTYP)();
   regTYP RegFunkcje = (regTYP)GetProcAddress(QGlobal::hDLL, "_RegFunkcje");
   QGlobal::DLL = RegFunkcje();                                           // zmiana typu obiektu z cLib na cImport
   byte data;

   manipsetv(1, 0, 0, 3, 4, 0, 1, 2, 3, 9, 9, 5, 4, 1, 5, 2, 3, 0, 13, 19);
   maniptest(1);
   manipsetv(0, 1, 0, 1, 0, 1, 1, 2, 3, 9, 9, 5, 4, 1, 5, 2, 3, 0, 13, 19);
   maniptest(1);
   manipopen(stdstrtochar(QGlobal::COM_port), QGlobal::bLOGIT);
   manipsend(1);

// FreeLibrary(QGlobal::hDLL);
 }
}


// ***********************************************************************************************************
// USTAWIENIE ZMIENNYCH W DLL
// ***********************************************************************************************************
bool TWorld::manipsetv(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n, int o, int p, int r, int s, int t, int u)
{

   QGlobal::DLL->sw_MainCtrlGauge = a;
   QGlobal::DLL->sw_ScndCtrlGauge = b;
   QGlobal::DLL->sw_DirKeyGauge = c;
   QGlobal::DLL->sw_BrakeCtrlGauge = d;
   QGlobal::DLL->sw_LocalBrakeGauge = e;
   QGlobal::DLL->sw_BrakeProfileCtrlGauge = f;
   QGlobal::DLL->sw_MaxCurrentCtrlGauge = g;
   QGlobal::DLL->sw_MainOffButtonGauge = h;
   QGlobal::DLL->sw_MainOnButtonGauge =  i;
   QGlobal::DLL->sw_MainButtonGauge = j;
   QGlobal::DLL->sw_SecurityResetButtonGauge = k;
   QGlobal::DLL->sw_ReleaserButtonGauge = l;
   QGlobal::DLL->sw_AntiSlipButtonGauge =  m;
   QGlobal::DLL->sw_FuseButtonGauge = n;
   QGlobal::DLL->sw_StLinOffButtonGauge = o;
   QGlobal::DLL->sw_RadioButtonGauge = p;
   QGlobal::DLL->sw_UpperLightButtonGauge = r;
   QGlobal::DLL->sw_LeftLightButtonGauge = s;
   QGlobal::DLL->sw_RightLightButtonGauge = t;
   QGlobal::DLL->sw_LeftEndLightButtonGauge = u;

}

bool TWorld::manipgetv(bool get)
{

}


// ***********************************************************************************************************
// SPRAWDZAINE POPRAWNOSCI USTAWIENIA DANYCH
// ***********************************************************************************************************
bool TWorld::maniptest(bool checkit)
{
 if (checkit)
 {
     WriteLog(AnsiString(IntToStr(QGlobal::DLL->sw_MainCtrlGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_ScndCtrlGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_DirKeyGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_BrakeCtrlGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_LocalBrakeGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_BrakeProfileCtrlGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_MaxCurrentCtrlGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_MainOffButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_MainOnButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_MainButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_SecurityResetButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_ReleaserButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_AntiSlipButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_FuseButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_StLinOffButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_RadioButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_UpperLightButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_LeftLightButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_RightLightButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_LeftEndLightButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_RightEndLightButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_IgnitionKeyGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_CompressorButtonGauge) + ", " +
                         IntToStr(QGlobal::DLL->sw_ConverterButtonGauge)

              ));
       }
}




//---------------------------------------------------------------------------
#pragma package(smart_init)










