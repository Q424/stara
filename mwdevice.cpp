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
#include <cstdio>
#include <cstdlib>

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
#include "train.h"

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
 bool BL_DEPARTURESIGN = false;
 bool DOORR = false;
 bool DOORL = false;

 bool czuwak_sw_state = false;	//zmienne do obslugi haslera
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
 recvpacket *rps;
 std::vector<std::string> skeybind;

 void LOADKEYBINDINGS();
 void PROCESSIOACTION(std::string str, bool state);

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

AnsiString str(int i)
{
  return IntToStr(i);
}

bool TWorld::manipopen(char* commname, bool log)
{
 if (QGlobal::bOPCOM) rps = new recvpacket;
 if (QGlobal::bOPCOM) QGlobal::DLL->rp = new recvpacket;
 if (QGlobal::bOPCOM) QGlobal::DLL->opencommp(commname, log);
}

double sendf = 0;


// ***********************************************************************************************************
// WYWYLANIE/ODBIERANIE
// ***********************************************************************************************************
bool TWorld::manipsend(int id)
{
double dt = Timer::GetDeltaTime();
sendf += dt;

if (QGlobal::hDLL)
 if (sendf > QGlobal::fRXSPD)
  if (QGlobal::bOPCOM && Controlled && Train)
  {
   sendf = 0;
   Velocity = Controlled->MoverParameters->Vel;
   BrakePress = Controlled->MoverParameters->BrakePress;
   PipePress = Controlled->MoverParameters->PipePress;
   Compressor = Controlled->MoverParameters->Compressor;
   Voltage1 = Controlled->MoverParameters->Voltage;
   Current1 = Controlled->MoverParameters->ShowCurrent(1);
   Current2 = Controlled->MoverParameters->ShowCurrent(2);
   Current3 = Controlled->MoverParameters->ShowCurrent(3);
   /*
   VoltageBat = Controlled->MoverParameters->VoltageBat		   // zmienic oby dwa na czytanie napiecia baterii
   CurrentBat = Controlled->MoverParameters->ShowCurrent(1);	   // i pradu baterii
   */

   // skalowanie mierników analogowych (od 0 do 255)
   byte bSPEED = (unsigned char)floor(Velocity + 0.5);             // predkosc, BYTE NA INT DLA TESTU CZY JEST OK: int spd =  (int) bSPEED;

   float fbrakepress = BrakePress / (QGlobal::fmaxCH * 10);  	   // ciœnienie w cylindrach hamulcowych // np 0.40/10 = 0.04
   byte bbrakepress = (unsigned char)floor(fbrakepress + 0.5);	   //zaokr¹glanie zgodnie z prawami matematyki

   float fpipepress = (PipePress / (QGlobal::fmaxPG * 10))*255;    // ciœnienie w przewodznie g³ównym
   byte bpipepress = (unsigned char)floor(fpipepress + 0.5);

   float fcomppress = (Compressor / (QGlobal::fmaxZG * 10))*255;   // ciœnienie w zbiorniku g³ownym
   byte bcomppress = (unsigned char) floor(fcomppress + 0.5);

   float fvoltage = (Voltage1 / QGlobal::fmaxV1)*255;              // napiêcie w sieci trakcyjnej
   byte bvoltagev1 = (unsigned char)floor(fvoltage + 0.5);

   float fcurrent1 = (Current1 / QGlobal::fmaxA1)*255;             // pr¹d pierwszego uk³¹du
   byte bcurrentA1 = (unsigned char) floor(fcurrent1 + 0.5);

   float fcurrent2 = (Current2 / QGlobal::fmaxA1)*255;             // pr¹d drugiego uk³¹du
   byte bcurrentA2 = (unsigned char) floor(fcurrent2 + 0.5);

   float fcurrent3 = (Current3 / QGlobal::fmaxA1)*255;             // pr¹d trzeciego uk³¹du
   byte bcurrentA3 = (unsigned char) floor(fcurrent3 + 0.5);
   /*
   // napiêcie baterii dodaæ odpowiednie rzeczy :)
   float fvoltagebat = (VoltageBat / QGlobal::fmaxVbat)*255;
   byte bcurrentA3 = (unsigned char) floor(fvoltagebat + 0.5);
   // pr¹d ³adowania/roz³adowywania baterii
   float fcurrentbat = (CurrentBat / QGlobal::fmaxAbat)*255;
   byte bcurrentbat = (unsigned char) floor(fcurrentbat + 0.5);
   */
   
   //                    Train->btLampkaOporyB.bOn;
   //                    Train->btLampkaStycznB.bOn;
   //                    Train->btLampkaWylSzybkiB.bOn;
   //                    Train->btLampkaNadmPrzetwB.bOn;
   //                    Train->btLampkaPrzetwB.bOn;
   //                    btLampkaNapNastHam.bOn;
   //                    btLampkaBoczniki.bOn;

   BL_WYSOKIROZRUCH    = Train->btLampkaWysRozr.bOn;            //P2.1
   BL_SPREZARKAA       = Train->btLampkaSprezarka.bOn;          //P2.2
   BL_SPREZARKAB       = Train->btLampkaSprezarkaB.bOn;         //P2.3
   BL_FORW             = Train->btLampkaForward.bOn;            //P2.4
   BL_BACK             = Train->btLampkaBackward.bOn;           //P2.5

   BL_RADIOTEL         = Train->btLampkaRadiotelefon.bOn;       //P3.1
   BL_JAZDA            = Train->btLampkaJazda.bOn;              //P3.3
   BL_BOCZNIK1         = Train->btLampkaBocznik1.bOn;           //P3.4
   BL_BOCZNIK2         = Train->btLampkaBocznik2.bOn;           //P3.5
   BL_DOORL            = Train->btLampkaDoorLeft.bOn;           //P3.6
   BL_DOORR            = Train->btLampkaDoorRight.bOn;          //P3.7
   BL_DEPARTURESIG     = Train->btLampkaDepartureSignal.bOn;    //P3.8

   BL_SHP              = Train->btLampkaSHP.bOn;                //P4.1
   BL_CZUWAK           = Train->btLampkaCzuwaka.bOn;            //P4.2
 //BL_BUCZEK1		   = ;					//P4.3
 //BL_BUCZEK2		   = ;					//P4.4
   BL_WENTZALUZJE      = Train->btLampkaWentZaluzje.bOn;        //P4.5
   BL_POSLIZG          = Train->btLampkaPoslizg.bOn;		//P4.6
   BL_OPORYROZRUCHOWE  = Train->btLampkaOpory.bOn;              //P4.7
   BL_OGRZEWANIESKLADU = Train->btLampkaOgrzewanieSkladu.bOn;   //P4.8

   BL_NADMIARSPREZARKI = Train->btLampkaNadmSpr.bOn;            //P5.1
   BL_NADMIARWENTYL = Train->btLampkaNadmWent.bOn;	       	//P5.2
   BL_WYLACZNIKSZYBKI  = Train->btLampkaWylSzybki.bOn;          //P5.3
   BL_NADMIARSILNIK    = Train->btLampkaNadmSil.bOn;            //P5.4
   BL_PROZNICOWY       = Train->btLampkaPrzekRozn.bOn;          //P5.5
   BL_NADMIARPRZETW    = Train->btLampkaNadmPrzetw.bOn;         //P5.6
 //BL_PRZEKANIK RÓ¯NICOWY OBWODÓW POMOCNICZYCH = ;	        //P5.7

   BL_HAMOWANIE        = Train->btLampkaHamienie.bOn;           //P6.6
   BL_STYCZNIKILINIOWE = Train->btLampkaStyczn.bOn;		//P6.8	P5.8

   byte dataout0 = 0;
   byte dataout1 = 0;
   byte dataout2 = 0;
   byte dataout3 = 0;
   byte dataout4 = 0;
   byte dataout5 = 0;
   byte dataout6 = 0;
   byte datarcv0 = 0;

	if (BL_HAMOWANIE) czuwak_sw_state = false;		//logika rysika 2
	if (BL_SHP){						//wykrywanie przejazdu nad rezonatorem torowym
		if (shp_sw_state) shp_sw_state = false;
		else shp_sw_state = true;
	}
	if (BL_FORW){						//logika rysika 3
		if (shp_sw_state) kabina1 = false;
		else kabina1 = true;
	}
	if (BL_BACK){
		if (shp_sw_state) kabina2 = false;
		else kabina2 = true;
	}



/*############################################################################################################
Opis kontrolek pulpitów:
                EU07  - L.przek.nadmiar.sprê¿arek - L.przek.nadmiar.sprê¿arek - L.wy³.szybki - L.przek.nadmiar.sil.trakcyjnych - L.przek.ró¿nic.obw.g³ - L.przek.nadmiar.przetwor.i ogrzew.
- L.styczniki liniowe - L.poœlizg - L.sygn.wys.rozruch. - L.jazda na oporach - L.ogrzew.poci¹gu
		ET22  - L.przek.nadmiar.sprê¿arek - L.przek.nadmiar.wentyl. - L.wy³.szybki - L.przek.nadmiar.sil.trakcyjnych - L.przek.ró¿nic.obw.g³ -  L.przek.nadmiar.przetwor.i ogrzew.
- L.przek.ró¿nic.obw.pomoc. - L.styczniki liniowe - L.poœlizg - L.jazda na oporach - L.ogrzew.poci¹gu
		EZT   - L.sprê¿arka.g³ - L.napiêcie na nastaw.hamulc. - L.przek.nadmiar.siln.trakc - L.jazda na oporach - L.wy³.przetw.g³ - L.za³.radiotel. - L.gotowoœæ za³.wy³.g³ -
L.wy³.wy³.szybki
############################################################################################################*/

   dataout0 = SETBYTE0(0, 0, 0, 0, 0, 0, 0, 0);
   dataout1 = SETBYTE1(0, 0, 0, 0, 0, 0, 0, 0);
   dataout2 = SETBYTE1(0, 0, 0, BL_BACK, BL_FORW, BL_SPREZARKAB, BL_SPREZARKAA, BL_WYSOKIROZRUCH);
   dataout3 = SETBYTE1(BL_DEPARTURESIGN, DOORR, DOORL, BL_BOCZNIK2, BL_BOCZNIK1, BL_JAZDA, 0, BL_RADIOTEL);
   dataout4 = SETBYTE1(BL_OGRZEWANIESKLADU, BL_OPORYROZRUCHOWE, BL_POSLIZG, BL_WENTZALUZJE, 0, 0, BL_CZUWAK, BL_SHP);	//zamiast zer dodaæ buczek 1 i 2
   dataout5 = SETBYTE1(BL_STYCZNIKILINIOWE, 0, BL_NADMIARPRZETW, BL_PROZNICOWY, BL_NADMIARSILNIK, BL_WYLACZNIKSZYBKI, BL_NADMIARWENTYL, BL_NADMIARSPREZARKI); // 7 -> przekaznik roznicowy obwodow pomocniczych,
// sterowanie rysikami Hastera: jazda pr¹dowa, u¿ycie przycisku czujnoœci, hamowanie, kierunek(przejazd nad rezonatorem SHP), buczek1, budzek2
   dataout6 = SETBYTE1(BL_STYCZNIKILINIOWE, czuwak_sw_state, BL_HAMOWANIE, kabina1, kabina2, 0, 0, 0);
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

     QGlobal::DLL->recvcommp(QGlobal::DLL->rp);    //  , 1, QGlobal::DEV_P01, QGlobal::DEV_P02, QGlobal::DEV_P03, QGlobal::DEV_P04, QGlobal::DEV_P05, QGlobal::DEV_P06, QGlobal::DEV_P07, &QGlobal::DEV_P08, &QGlobal::DEV_P09, &QGlobal::DEV_P10, &QGlobal::DEV_P11, &QGlobal::DEV_P12, &QGlobal::DEV_P13

     rps = QGlobal::DLL->rp;   // coby skrocic odwolanie

     // BORZE (SLASKI), DAJ BY DZIALALO...

     QGlobal::portstate[1][1] = rps->P1B1;
     QGlobal::portstate[1][2] = rps->P1B2;
     QGlobal::portstate[1][3] = rps->P1B3;
     QGlobal::portstate[1][4] = rps->P1B4;
     QGlobal::portstate[1][5] = rps->P1B5;
     QGlobal::portstate[1][6] = rps->P1B6;
     QGlobal::portstate[1][7] = rps->P1B7;
     QGlobal::portstate[1][8] = rps->P1B8;

     QGlobal::portstate[2][1] = rps->P2B1;
     QGlobal::portstate[2][2] = rps->P2B2;
     QGlobal::portstate[2][3] = rps->P2B3;
     QGlobal::portstate[2][4] = rps->P2B4;
     QGlobal::portstate[2][5] = rps->P2B5;
     QGlobal::portstate[2][6] = rps->P2B6;
     QGlobal::portstate[2][7] = rps->P2B7;
     QGlobal::portstate[2][8] = rps->P2B8;

     QGlobal::portstate[3][1] = rps->P3B1;
     QGlobal::portstate[3][2] = rps->P3B2;
     QGlobal::portstate[3][3] = rps->P3B3;
     QGlobal::portstate[3][4] = rps->P3B4;
     QGlobal::portstate[3][5] = rps->P3B5;
     QGlobal::portstate[3][6] = rps->P3B6;
     QGlobal::portstate[3][7] = rps->P3B7;
     QGlobal::portstate[3][8] = rps->P3B8;

     QGlobal::portstate[4][1] = rps->P4B1;
     QGlobal::portstate[4][2] = rps->P4B2;
     QGlobal::portstate[4][3] = rps->P4B3;
     QGlobal::portstate[4][4] = rps->P4B4;
     QGlobal::portstate[4][5] = rps->P4B5;
     QGlobal::portstate[4][6] = rps->P4B6;
     QGlobal::portstate[4][7] = rps->P4B7;
     QGlobal::portstate[4][8] = rps->P4B8;

     QGlobal::portstate[5][1] = rps->P5B1;
     QGlobal::portstate[5][2] = rps->P5B2;
     QGlobal::portstate[5][3] = rps->P5B3;
     QGlobal::portstate[5][4] = rps->P5B4;
     QGlobal::portstate[5][5] = rps->P5B5;
     QGlobal::portstate[5][6] = rps->P5B6;
     QGlobal::portstate[5][7] = rps->P5B7;
     QGlobal::portstate[5][8] = rps->P5B8;

     QGlobal::portstate[6][1] = rps->P6B1;
     QGlobal::portstate[6][2] = rps->P6B2;
     QGlobal::portstate[6][3] = rps->P6B3;
     QGlobal::portstate[6][4] = rps->P6B4;
     QGlobal::portstate[6][5] = rps->P6B5;
     QGlobal::portstate[6][6] = rps->P6B6;
     QGlobal::portstate[6][7] = rps->P6B7;
     QGlobal::portstate[6][8] = rps->P6B8;
     /*
//     if (rps->P1B1 == 1)
      PROCESSIOACTION("P1B1", rps->P1B1);
//     if (rps->P1B2 == 1)
      PROCESSIOACTION("P1B2", rps->P1B2);
//     if (rps->P1B3 == 1)
      PROCESSIOACTION("P1B3", rps->P1B3);
//     if (rps->P1B4 == 1)
      PROCESSIOACTION("P1B4", rps->P1B4);
//     if (rps->P1B5 == 1)
      PROCESSIOACTION("P1B5", rps->P1B5);
//     if (rps->P1B6 == 1)
      PROCESSIOACTION("P1B6", rps->P1B6);
//     if (rps->P1B7 == 1)
      PROCESSIOACTION("P1B7", rps->P1B7);
//     if (rps->P1B8 == 1)
      PROCESSIOACTION("P1B8", rps->P1B8);

//     if (rps->P2B1 == 1)
      PROCESSIOACTION("P2B1", rps->P2B1);
//     if (rps->P2B2 == 1)
      PROCESSIOACTION("P2B2", rps->P2B2);
//     if (rps->P2B3 == 1)
      PROCESSIOACTION("P2B3", rps->P2B3);
//     if (rps->P2B4 == 1)
      PROCESSIOACTION("P2B4", rps->P2B4);
//     if (rps->P2B5 == 1)
      PROCESSIOACTION("P2B5", rps->P2B5);
//     if (rps->P2B6 == 1)
      PROCESSIOACTION("P2B6", rps->P2B6);
//     if (rps->P2B7 == 1)
      PROCESSIOACTION("P2B7", rps->P2B7);
//     if (rps->P2B8 == 1)
      PROCESSIOACTION("P2B8", rps->P2B8);

//     if (rps->P3B1 == 1)
      PROCESSIOACTION("P3B1", rps->P3B1);
//     if (rps->P3B2 == 1)
      PROCESSIOACTION("P3B2", rps->P3B2);
//     if (rps->P3B3 == 1)
      PROCESSIOACTION("P3B3", rps->P3B3);
//     if (rps->P3B4 == 1)
      PROCESSIOACTION("P3B4", rps->P3B4);
//     if (rps->P3B5 == 1)
      PROCESSIOACTION("P3B5", rps->P3B5);
//     if (rps->P3B6 == 1)
      PROCESSIOACTION("P3B6", rps->P3B6);
//     if (rps->P3B7 == 1)
      PROCESSIOACTION("P3B7", rps->P3B7);
//     if (rps->P3B8 == 1)
      PROCESSIOACTION("P3B8", rps->P3B8);
      */
     /*
     if (QGlobal::bLOGIT)
      WriteLog(str(rps->P1B1) + ", " +
               str(rps->P1B2) + ", " +
               str(rps->P1B3) + ", " +
               str(rps->P1B4) + ", " +
               str(rps->P1B5) + ", " +
               str(rps->P1B6) + ", " +
               str(rps->P1B7) + ", " +
               str(rps->P1B8));
               */

    }
  }
}


// ***********************************************************************************************************
//
// ***********************************************************************************************************
void TTrain::OnMWDCommand()
{
 if (QGlobal::IOCOMMAND == "COS-01")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-02")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-03")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-04")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-05")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-06")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-07")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-08")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-09")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-10")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-11")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-12")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-13")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-14")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-15")
  {

  }
 if (QGlobal::IOCOMMAND == "COS-16")
  {

  }
 QGlobal::IOCOMMAND = "";
}


// ***********************************************************************************************************
// This procedure compare 'key string' previously generated in keyboard callback witch items collected
// in io binding table. If it hits in the same string, that happens download command string
void TWorld::PROCESSIOACTION(std::string str, bool state)
{
	std::string test, command;

	for (int i = 0; i < QGlobal::iobindsnum; i++)
	{
		test = QGlobal::IOSET[i].bitid; // + ":" + QGlobal::IOSET[i].ifbit + ":" + QGlobal::IOSET[i].command;

		if (test == str)
		{
		     if (state == atoi(QGlobal::IOSET[i].ifbit.c_str()))
                        {
                          QGlobal::IOCOMMAND = QGlobal::IOSET[i].command;
                          if (Train) Train->OnMWDCommand();
                        }
		     break;
		}

	}
        
}


// ***********************************************************************************************************
// Reading I/O mapping
// ***********************************************************************************************************
void LOADKEYBINDINGS()
{
 WriteLog("Loading I/O mapping...");
	int cl = 0;
	std::ifstream file("data/iomap.txt");
	std::string str;
	QGlobal::iobindsnum = 0;
	while (std::getline(file, str))
	{ 
		int ishash;
		ishash = str.find('#');

		if ((str.length() > 5) && (ishash < 0))
		{// example line: ? 1 P3B1 "APPEXIT"
			//MASZYNA_TRACE_WRITELINE(AppMain, Debug, "line %i: %s", cl, str.c_str());
			skeybind = split(str, ' ');
			QGlobal::IOSET[cl].bit = skeybind[0];
			QGlobal::IOSET[cl].ifbit = skeybind[1];
			QGlobal::IOSET[cl].bitid = skeybind[2];
			QGlobal::IOSET[cl].command = skeybind[3];
			QGlobal::iobindsnum++;

			WriteLogSS("IOMAP: ",  QGlobal::IOSET[cl].bitid + ":" + QGlobal::IOSET[cl].ifbit + ":" + QGlobal::IOSET[cl].command );
			cl++;
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

   LOADKEYBINDINGS();

 //manipsetv(1, 0, 0, 3, 4, 0, 1, 2, 3, 9, 9, 5, 4, 1, 5, 2, 3, 0, 13, 19);
 //maniptest(1);
 //manipsetv(0, 1, 0, 1, 0, 1, 1, 2, 3, 9, 9, 5, 4, 1, 5, 2, 3, 0, 13, 19);
 //maniptest(1);
   manipopen(stdstrtochar(QGlobal::COM_port), QGlobal::bLOGIT);
// manipsend(1);

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










