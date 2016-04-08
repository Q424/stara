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
#include "Driver.h"
#include "train.h"
#include "timer.h"
#include "DynObj.h"
class MWDTTrain : public TTrain
{
public:

  void f()
    {
        bCabLight = 1; //brak dostepu
        bCabLightDim = 0;
    }
};
 void LOADKEYBINDINGS();
 void PROCESSIOACTION(std::string str, bool state);

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


  bool isEztOer;


// ***********************************************************************************************************
//
// ***********************************************************************************************************
/*
void MWDTTrain::_mwd_BatterySw(bool state, bool sound)
{
	if (mvControlled->Radio == false)
	if (GetAsyncKeyState(VK_CONTROL) >= 0)
	{
		if (sound)
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
		}
		mvControlled->Radio = true;
	}
}
*/


void TTrain::_mwd_IncMainCtrl(bool sound)
{
	if (mvControlled->IncMainCtrl(1))
	{
                WriteLog("_mwd_IncMainCtrl");
		dsbNastawnikJazdy->SetCurrentPosition(0);
		if (sound) dsbNastawnikJazdy->Play(0, 0, 0);
	}
}

void TTrain::_mwd_DecMainCtrl(bool sound)
{
	if (mvControlled->DecMainCtrl(1))
	{
                WriteLog("_mwd_DecMainCtrl");
		dsbNastawnikJazdy->SetCurrentPosition(0);
		if (sound) dsbNastawnikJazdy->Play(0, 0, 0);
	}
}

void TTrain::_mwd_IncScndCtrl(bool sound)
{
	//        if (MoverParameters->ScndCtrlPos<MoverParameters->ScndCtrlPosNo)
	//         if (mvControlled->EnginePowerSource.SourceType==CurrentCollector)
	if (mvControlled->ShuntMode)
	{
		mvControlled->AnPos += (Timer::GetDeltaTime() / 0.85f);
		if (mvControlled->AnPos > 1)
			mvControlled->AnPos = 1;
	}
	else if (mvControlled->IncScndCtrl(1))
	{
		if (dsbNastawnikBocz) // hunter-081211
		{
			dsbNastawnikBocz->SetCurrentPosition(0);
			if (sound) dsbNastawnikBocz->Play(0, 0, 0);
		}
		else if (!dsbNastawnikBocz)
		{
			dsbNastawnikJazdy->SetCurrentPosition(0);
			if (sound) dsbNastawnikJazdy->Play(0, 0, 0);
		}
	}
}

void TTrain::_mwd_DecScndCtrl(bool sound)
{	// if (mvControlled->EnginePowerSource.SourceType==CurrentCollector)
	if (mvControlled->ShuntMode)
	{
		mvControlled->AnPos -= (Timer::GetDeltaTime() / 0.55f);
		if (mvControlled->AnPos < 0)
			mvControlled->AnPos = 0;
	}
	else if (mvControlled->DecScndCtrl(1))
	// if (MoverParameters->ScndCtrlPos>0)
	{
		if (dsbNastawnikBocz) // hunter-081211
		{
			dsbNastawnikBocz->SetCurrentPosition(0);
			if (sound) dsbNastawnikBocz->Play(0, 0, 0);
		}
		else if (!dsbNastawnikBocz)
		{
			dsbNastawnikJazdy->SetCurrentPosition(0);
			if (sound) dsbNastawnikJazdy->Play(0, 0, 0);
		}
	}
}

void TTrain::_mwd_IncLocalBrake(void)
{
	{ // Ra 2014-09: w trybie latania obs³uga jest w World.cpp
		if (!FreeFlyModeFlag)
		{
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			if ((mvOccupied->LocalBrake == ManualBrake) || (mvOccupied->MBrake == true))
			{
				mvOccupied->IncManualBrakeLevel(1);
			}
			else
				;
			else if (mvOccupied->LocalBrake != ManualBrake)
				mvOccupied->IncLocalBrakeLevel(1);
		}
	}
}

void TTrain::_mwd_DecLocalBrake(void)
{
	{ // Ra 2014-06: wersja dla swobodnego latania przeniesiona do World.cpp
		if (!FreeFlyModeFlag)
		{
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			if ((mvOccupied->LocalBrake == ManualBrake) || (mvOccupied->MBrake == true))
				mvOccupied->DecManualBrakeLevel(1);
			else
				;
			else // Ra 1014-06: AI potrafi zahamowaæ pomocniczym mimo jego braku - odhamowaæ
				// jakoœ trzeba
			if ((mvOccupied->LocalBrake != ManualBrake) || mvOccupied->LocalBrakePos)
				mvOccupied->DecLocalBrakeLevel(1);
		}
	}
}

void TTrain::_mwd_IncTrainBrake(void)
{
	// if (mvOccupied->IncBrakeLevel())
	if (mvOccupied->BrakeLevelAdd(
		Global::fBrakeStep)) // nieodpowiedni warunek; true, jeœli mo¿na dalej krêciæ
	{
		keybrakecount = 0;
		if ((isEztOer) && (mvOccupied->BrakeCtrlPos < 3))
		{ // Ra: uzale¿niæ dŸwiêk od zmiany stanu EP, nie od klawisza
			dsbPneumaticSwitch->SetVolume(-10);
			dsbPneumaticSwitch->Play(0, 0, 0);
		}
	}
}

void TTrain::_mwd_DecTrainBrake(void)
{
	// now¹ wersjê dostarczy³ ZiomalCl ("fixed looped sound in ezt when using NUM_9 key")
	if ((mvOccupied->BrakeCtrlPos > -1) || (keybrakecount > 1))
	{
		if ((isEztOer) && (mvControlled->Mains) && (mvOccupied->BrakeCtrlPos != -1))
		{ // Ra: uzale¿niæ dŸwiêk od zmiany stanu EP, nie od klawisza
			dsbPneumaticSwitch->SetVolume(-10);
			dsbPneumaticSwitch->Play(0, 0, 0);
		}
		// mvOccupied->DecBrakeLevel();
		mvOccupied->BrakeLevelAdd(-Global::fBrakeStep);
	}
	else
		keybrakecount += 1;
	// koniec wersji dostarczonej przez ZiomalCl
	/* wersja poprzednia - ten pierwszy if ze œrednikiem nie dzia³a³ jak warunek
	if ((mvOccupied->BrakeCtrlPos>-1)|| (keybrakecount>1))
	{
	if (mvOccupied->DecBrakeLevel());
	{
	if ((isEztOer) && (mvOccupied->BrakeCtrlPos<2)&&(keybrakecount<=1))
	{
	dsbPneumaticSwitch->SetVolume(-10);
	dsbPneumaticSwitch->Play(0,0,0);
	}
	}
	}
	else keybrakecount+=1;
	*/
}

void TTrain::_mwd_EmergencyBrake(void)
{
	// while (mvOccupied->IncBrakeLevel());
	mvOccupied->BrakeLevelSet(mvOccupied->Handle->GetPos(bh_EB));
	if (mvOccupied->BrakeCtrlPosNo <= 0.1) // hamulec bezpieczeñstwa dla wagonów
		mvOccupied->EmergencyBrakeFlag = true;
}


void TTrain::_mwd_Brake3(void)
{
	if ((isEztOer) && ((mvOccupied->BrakeCtrlPos == 1) || (mvOccupied->BrakeCtrlPos == -1)))
	{
		dsbPneumaticSwitch->SetVolume(-10);
		dsbPneumaticSwitch->Play(0, 0, 0);
	}
	// while (mvOccupied->BrakeCtrlPos>mvOccupied->BrakeCtrlPosNo-1 &&
	// mvOccupied->DecBrakeLevel());
	// while (mvOccupied->BrakeCtrlPos<mvOccupied->BrakeCtrlPosNo-1 &&
	// mvOccupied->IncBrakeLevel());
	mvOccupied->BrakeLevelSet(mvOccupied->BrakeCtrlPosNo - 1);
}

void TTrain::_mwd_Brake2(void)
{
	if ((isEztOer) && ((mvOccupied->BrakeCtrlPos == 1) || (mvOccupied->BrakeCtrlPos == -1)))
	{
		dsbPneumaticSwitch->SetVolume(-10);
		dsbPneumaticSwitch->Play(0, 0, 0);
	}
	// while (mvOccupied->BrakeCtrlPos>mvOccupied->BrakeCtrlPosNo/2 &&
	// mvOccupied->DecBrakeLevel());
	// while (mvOccupied->BrakeCtrlPos<mvOccupied->BrakeCtrlPosNo/2 &&
	// mvOccupied->IncBrakeLevel());
	mvOccupied->BrakeLevelSet(mvOccupied->BrakeCtrlPosNo / 2 +
		(mvOccupied->BrakeHandle == FV4a ? 1 : 0));
	if (GetAsyncKeyState(VK_CONTROL) < 0)
		mvOccupied->BrakeLevelSet(mvOccupied->Handle->GetPos(bh_NP)); // yB: czy ten stos
	// funkcji nie powinien byæ jako oddzielna funkcja movera?
}

void TTrain::_mwd_Brake1(void)
{
	if ((isEztOer) && (mvOccupied->BrakeCtrlPos != 1))
	{
		dsbPneumaticSwitch->SetVolume(-10);
		dsbPneumaticSwitch->Play(0, 0, 0);
	}
	// while (mvOccupied->BrakeCtrlPos>1 && mvOccupied->DecBrakeLevel());
	// while (mvOccupied->BrakeCtrlPos<1 && mvOccupied->IncBrakeLevel());
	mvOccupied->BrakeLevelSet(1);
}

void TTrain::_mwd_Brake0(void)
{
       //	if (Console::Pressed(VK_CONTROL))
       //	{
	//	mvOccupied->BrakeCtrlPos2 = 0; // wyrownaj kapturek
       //	}
       //	else
	{
		if ((isEztOer) &&
			((mvOccupied->BrakeCtrlPos == 1) || (mvOccupied->BrakeCtrlPos == -1)))
		{
			dsbPneumaticSwitch->SetVolume(-10);
			dsbPneumaticSwitch->Play(0, 0, 0);
		}
		// while (mvOccupied->BrakeCtrlPos>0 && mvOccupied->DecBrakeLevel());
		// while (mvOccupied->BrakeCtrlPos<0 && mvOccupied->IncBrakeLevel());
		mvOccupied->BrakeLevelSet(0);
	}
}

void TTrain::_mwd_WaveBrake(void)
{
	if ((isEztOer) && (mvControlled->Mains) && (mvOccupied->BrakeCtrlPos != -1))
	{
		dsbPneumaticSwitch->SetVolume(-10);
		dsbPneumaticSwitch->Play(0, 0, 0);
	}
	// while (mvOccupied->BrakeCtrlPos>-1 && mvOccupied->DecBrakeLevel());
	// while (mvOccupied->BrakeCtrlPos<-1 && mvOccupied->IncBrakeLevel());
	mvOccupied->BrakeLevelSet(-1);
}


// Prze³¹czniki i przyciski

void TTrain::_mwd_BatterySw(bool state, bool sound)
{
 WriteLog("_mwd_BatterySw " + IntToStr(state));
	if (state == true)
	{
		// (((mvControlled->TrainType==dt_EZT)||(mvControlled->EngineType==ElectricSeriesMotor)||(mvControlled->EngineType==DieselElectric))&&(!mvControlled->Battery))
		if (!mvControlled->Battery)
		{ // wy³¹cznik jest te¿ w SN61, ewentualnie za³¹czaæ pr¹d na sta³e z poziomu FIZ
			if (mvOccupied->BatterySwitch(true)) // bateria potrzebna np. do zapalenia œwiate³
			{
				if (sound == true) dsbSwitch->Play(0, 0, 0);
				if (TestFlag(mvOccupied->SecuritySystem.SystemType,
					2)) // Ra: znowu w kabinie jest coœ, co byæ nie powinno!
				{
					SetFlag(mvOccupied->SecuritySystem.Status, s_active);
					SetFlag(mvOccupied->SecuritySystem.Status, s_SHPalarm);
				}
			}
		}
	}else
	{
		// if ((mvControlled->TrainType==dt_EZT) ||
		// (mvControlled->EngineType==ElectricSeriesMotor)||
		// (mvControlled->EngineType==DieselElectric))
		if (mvOccupied->BatterySwitch(false))
		{ // ewentualnie zablokowaæ z FIZ, np. w samochodach siê nie od³¹cza akumulatora
			dsbSwitch->Play(0, 0, 0);
			// mvOccupied->SecuritySystem.Status=0;
			mvControlled->PantFront(false);
			mvControlled->PantRear(false);
		}
	}
}

void TTrain::_mwd_StLinOffSw(bool state, bool sound)
{
 WriteLog("_mwd_StLinOffSw " + IntToStr(state));
	if (state)
	{
		if (mvControlled->TrainType == dt_EZT)
		{
			if ((mvControlled->Signalling == false))
			{
				if (sound) dsbSwitch->Play(0, 0, 0);
				mvControlled->Signalling = true;
			}
		}
	}else
	{
		if ((mvControlled->TrainType != dt_EZT) && (mvControlled->TrainType != dt_EP05) &&
			(mvControlled->TrainType != dt_ET40))
		{
			ggStLinOffButton.PutValue(1); // Ra: by³o Fuse...
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
			if (mvControlled->MainCtrlPosNo > 0)
			{
				mvControlled->StLinFlag = false; // yBARC - zmienione na przeciwne, bo true to zalaczone
				if (sound)
				{
					dsbRelay->SetVolume(DSBVOLUME_MAX);
					dsbRelay->Play(0, 0, 0);
				}
			}
		}
		if (mvControlled->TrainType == dt_EZT)
		{
			if (mvControlled->Signalling == true)
			{
				if (sound) dsbSwitch->Play(0, 0, 0);
				mvControlled->Signalling = false;
			}
		}
	}

}

void TTrain::_mwd_SandSw(bool state, bool sound)
{
 WriteLog("_mwd_SandSw " + IntToStr(state));
	if (state)
	{
		if (mvControlled->TrainType == dt_EZT)
		{
			if (!mvControlled->DoorSignalling)
			{
				if (sound)
				{
					dsbSwitch->Play(0, 0, 0);
					mvControlled->DoorSignalling = true;
				}
			}
		}
	}else
	{
		/*
		if (mvControlled->TrainType!=dt_EZT)
		{
		if (mvControlled->SandDoseOn())
		if (mvControlled->SandDose)
		{
		dsbPneumaticRelay->SetVolume(-30);
		dsbPneumaticRelay->Play(0,0,0);
		}
		}
		*/
		if (mvControlled->TrainType == dt_EZT)
		{
			if (mvControlled->DoorSignalling)
			{
				if (sound)
				{
					dsbSwitch->Play(0, 0, 0);
					mvControlled->DoorSignalling = false;
				}
			}
		}
	}
}

void TTrain::_mwd_MainSw(bool state, bool sound)		//wy³¹cznik szybki
{
 WriteLog("_mwd_MainSw " + IntToStr(state));
	if (state)
	{
		if (fabs(ggMainOnButton.GetValue()) < 0.001)
		if (dsbSwitch && sound)
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
		}
	}else
	{
		if (fabs(ggMainOffButton.GetValue()) < 0.001)
		if (dsbSwitch && sound)
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
		}
	}
}


void TTrain::_mwd_BrakeProfileSw(bool state)
{
	if (state)
	{
		// yB://ABu: male poprawki, zeby bylo mozna ustawic dowolny wagon
		int CouplNr = -2;
		if (!FreeFlyModeFlag)
		{
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			if (mvOccupied->BrakeDelaySwitch(bdelay_R + bdelay_M))
			{
				dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
				dsbPneumaticRelay->Play(0, 0, 0);
			}
			else
				;
			else if (mvOccupied->BrakeDelaySwitch(bdelay_P))
			{
				dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
				dsbPneumaticRelay->Play(0, 0, 0);
			}
		}
		else
		{
			TDynamicObject *temp;
			temp = (DynamicObject->ABuScanNearestObject(DynamicObject->GetTrack(), -1, 1500,
				CouplNr));
			if (temp == NULL)
			{
				CouplNr = -2;
				temp = (DynamicObject->ABuScanNearestObject(DynamicObject->GetTrack(), 1, 1500,
					CouplNr));
			}
			if (temp)
			{
				if (GetAsyncKeyState(VK_CONTROL) < 0)
				if (temp->MoverParameters->BrakeDelaySwitch(bdelay_R + bdelay_M))
				{
					dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
					dsbPneumaticRelay->Play(0, 0, 0);
				}
				else
					;
				else if (temp->MoverParameters->BrakeDelaySwitch(bdelay_P))
				{
					dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
					dsbPneumaticRelay->Play(0, 0, 0);
				}
			}
		}
	}else
	{ // yB://ABu: male poprawki, zeby bylo mozna ustawic dowolny wagon
		int CouplNr = -2;
		if (!FreeFlyModeFlag)
		{
			if (GetAsyncKeyState(VK_CONTROL) < 0)
			if (mvOccupied->BrakeDelaySwitch(bdelay_R))
			{
				dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
				dsbPneumaticRelay->Play(0, 0, 0);
			}
			else
				;
			else if (mvOccupied->BrakeDelaySwitch(bdelay_G))
			{
				dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
				dsbPneumaticRelay->Play(0, 0, 0);
			}
		}
		else
		{
			TDynamicObject *temp;
			temp = (DynamicObject->ABuScanNearestObject(DynamicObject->GetTrack(), -1, 1500,
				CouplNr));
			if (temp == NULL)
			{
				CouplNr = -2;
				temp = (DynamicObject->ABuScanNearestObject(DynamicObject->GetTrack(), 1, 1500,
					CouplNr));
			}
			if (temp)
			{
				if (GetAsyncKeyState(VK_CONTROL) < 0)
				if (temp->MoverParameters->BrakeDelaySwitch(bdelay_R))
				{
					dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
					dsbPneumaticRelay->Play(0, 0, 0);
				}
				else
					;
				else if (temp->MoverParameters->BrakeDelaySwitch(bdelay_G))
				{
					dsbPneumaticRelay->SetVolume(DSBVOLUME_MAX);
					dsbPneumaticRelay->Play(0, 0, 0);
				}
			}
		}
	}
}


void TTrain::_mwd_ConverterSw(bool state, bool sound)
{
 WriteLog("_mwd_ConverterSw " + IntToStr(state));
	if (state)
	{
		if (ggConverterButton.GetValue() == 0)
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}else
	{
		if (ggConverterButton.GetValue() != 0)
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_CompressorSw(bool state, bool sound)
{
 WriteLog("_mwd_CompressorSw " + IntToStr(state));
	if (state)
	{
		if (ggCompressorButton.GetValue() == 0)
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}else
	{
		if (ggCompressorButton.GetValue() != 0)
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_SmallCompressorSw(bool state, bool sound)
{
 WriteLog("_mwd_SmallCompressorSw " + IntToStr(state));
	if (state)
	{
		// Ra: dŸwiêk, gdy razem z [Shift]
		if ((mvControlled->TrainType & dt_EZT) ? mvControlled == mvOccupied :
			!mvOccupied->ActiveCab) // tylko w maszynowym
		//if (Console::Pressed(VK_CONTROL)) // z [Ctrl]
		//	mvControlled->bPantKurek3 = true; // zbiornik pantografu po³¹czony jest ze zbiornikiem g³ównym (pompowanie nie ma sensu)
		//else
                if (!mvControlled->PantCompFlag) // jeœli wy³¹czona
		if (mvControlled->Battery) // jeszcze musi byæ za³¹czona bateria
		if (mvControlled->PantPress < 4.8) // pisz¹, ¿e to tak nie dzia³a
		{
			mvControlled->PantCompFlag = true;
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0); // dŸwiêk tylko po naciœniêciu klawisza
			}
		}
	}else
	{ // Ra: bez [Shift] te¿ daæ dŸwiêk
		if ((mvControlled->TrainType & dt_EZT) ? mvControlled == mvOccupied :
			!mvOccupied->ActiveCab) // tylko w maszynowym
		//if (Console::Pressed(VK_CONTROL)) // z [Ctrl]
		//	mvControlled->bPantKurek3 = false; // zbiornik pantografu po³¹czony jest z ma³¹ sprê¿ark¹ (pompowanie ma sens, ale potem trzeba prze³¹czyæ)
		//else
                if (!mvControlled->PantCompFlag) // jeœli wy³¹czona
		if (mvControlled->Battery) // jeszcze musi byæ za³¹czona bateria
		if (mvControlled->PantPress < 4.8) // pisz¹, ¿e to tak nie dzia³a
		{
			mvControlled->PantCompFlag = true;
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0); // dŸwiêk tylko po naciœniêciu klawisza
			}
		}
	}
}


void TTrain::_mwd_MaxCurrentSw(bool state, bool sound)
{
 WriteLog("_mwd_MaxCurrentSw " + IntToStr(state));
	if (state)
	{
		if ((mvControlled->EngineType == DieselElectric) && (mvControlled->ShuntModeAllow) &&
			(mvControlled->MainCtrlPos == 0))
		{
			mvControlled->ShuntMode = true;
		}
		if (mvControlled->CurrentSwitch(true))
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
		/* Ra: przeniesione do Mover.cpp
		if (mvControlled->TrainType!=dt_EZT) //to powinno byæ w fizyce, a nie w kabinie!
		if (mvControlled->MinCurrentSwitch(true))
		{
		dsbSwitch->SetVolume(DSBVOLUME_MAX);
		dsbSwitch->Play(0,0,0);
		}
		*/
	}else
	{
		if ((mvControlled->EngineType == DieselElectric) && (mvControlled->ShuntModeAllow) &&
			(mvControlled->MainCtrlPos == 0))
		{
			mvControlled->ShuntMode = false;
		}
		if (mvControlled->CurrentSwitch(false))
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
		/* Ra: przeniesione do Mover.cpp
		if (mvControlled->TrainType!=dt_EZT)
		if (mvControlled->MinCurrentSwitch(false))
		{
		dsbSwitch->SetVolume(DSBVOLUME_MAX);
		dsbSwitch->Play(0,0,0);
		}
		*/
	}
}

void TTrain::_mwd_CurrentAutoRelaySw(bool state, bool sound)
{
 WriteLog("_mwd_CurrentAutoRelaySw " + IntToStr(state));
	if (state)
	{
		if (mvControlled->AutoRelaySwitch(true))
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}else
	{
		if (mvControlled->AutoRelaySwitch(false))
		{
			if (sound)
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_FailedEngineCutOffSw(void)
{
 WriteLog("_mwd_FailedEngineCutOffSw ");
	if (mvControlled->CutOffEngine())
	{
		dsbSwitch->SetVolume(DSBVOLUME_MAX);
		dsbSwitch->Play(0, 0, 0);
	}
}


void TTrain::_mwd_LeftDoor(bool state)
{
	if (state)
	{
		if (mvOccupied->DoorOpenCtrl == 1)
		if (mvOccupied->CabNo < 0 ? mvOccupied->DoorRight(true) :
			mvOccupied->DoorLeft(true))
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
			if (dsbDoorOpen)
			{
				dsbDoorOpen->SetCurrentPosition(0);
				dsbDoorOpen->Play(0, 0, 0);
			}
		}
	}else
	{
		if (mvOccupied->CabNo < 0 ? mvOccupied->DoorRight(false) : mvOccupied->DoorLeft(false))
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
			if (dsbDoorClose)
			{
				dsbDoorClose->SetCurrentPosition(0);
				dsbDoorClose->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_RightDoor(bool state)
{
	if (state)
	{
		if (mvOccupied->DoorCloseCtrl == 1)
		if (mvOccupied->CabNo < 0 ? mvOccupied->DoorLeft(true) :
			mvOccupied->DoorRight(true))
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
			if (dsbDoorOpen)
			{
				dsbDoorOpen->SetCurrentPosition(0);
				dsbDoorOpen->Play(0, 0, 0);
			}
		}
	}else
	{
		if (mvOccupied->CabNo < 0 ? mvOccupied->DoorLeft(false) : mvOccupied->DoorRight(false))
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
			if (dsbDoorClose)
			{
				dsbDoorClose->SetCurrentPosition(0);
				dsbDoorClose->Play(0, 0, 0);
			}
		}
	}
}


void TTrain::_mwd_PantFrontSw(bool state, bool sound)
{
 WriteLog("_mwd_PantFrontSw " + IntToStr(state));
	if (state)// Winger 160204: podn. przedn. pantografu
	{
		if (mvOccupied->ActiveCab ==
			1) //||((mvOccupied->ActiveCab<1)&&((mvControlled->TrainType&(dt_ET40|dt_ET41|dt_ET42|dt_EZT))==0)))
		{ // przedni gdy w kabinie 1 lub (z wyj¹tkiem ET40, ET41, ET42 i EZT) gdy w kabinie -1
			mvControlled->PantFrontSP = false;
			if (mvControlled->PantFront(true))
			if (mvControlled->PantFrontStart != 1)
			{
				if (sound)
				{
					dsbSwitch->SetVolume(DSBVOLUME_MAX);
					dsbSwitch->Play(0, 0, 0);
				}
			}
		}
		else
			// if
			// ((mvOccupied->ActiveCab<1)&&(mvControlled->TrainType&(dt_ET40|dt_ET41|dt_ET42|dt_EZT)))
		{ // w kabinie -1 dla ET40, ET41, ET42 i EZT
			mvControlled->PantRearSP = false;
			if (mvControlled->PantRear(true))
			if (mvControlled->PantRearStart != 1)
			{
				if (sound)
				{
					dsbSwitch->SetVolume(DSBVOLUME_MAX);
					dsbSwitch->Play(0, 0, 0);
				}
			}
		}
	}else
	{
		if (mvOccupied->ActiveCab ==
			1) //||((mvOccupied->ActiveCab<1)&&(mvControlled->TrainType!=dt_ET40)&&(mvControlled->TrainType!=dt_ET41)&&(mvControlled->TrainType!=dt_ET42)&&(mvControlled->TrainType!=dt_EZT)))
		{
			// if (!mvControlled->PantFrontUp) //jeœli by³ opuszczony
			// if () //jeœli po³amany
			//  //to powtórzone opuszczanie naprawia
			if (mvControlled->PantFront(false))
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
		else
			// if
			// ((mvOccupied->ActiveCab<1)&&((mvControlled->TrainType==dt_ET40)||(mvControlled->TrainType==dt_ET41)||(mvControlled->TrainType==dt_ET42)||(mvControlled->TrainType==dt_EZT)))
		{
			if (mvControlled->PantRear(false))
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_PantRearSw(bool state, bool sound)
{
 WriteLog("_mwd_PantRearSw " + IntToStr(state));
	if (state)// Winger 160204: podn. tyln. pantografu wzglêdem kierunku jazdy
	{
		if (mvOccupied->ActiveCab ==
			1) //||((mvOccupied->ActiveCab<1)&&((mvControlled->TrainType&(dt_ET40|dt_ET41|dt_ET42|dt_EZT))==0)))
		{ // tylny gdy w kabinie 1 lub (z wyj¹tkiem ET40, ET41, ET42 i EZT) gdy w kabinie -1
			mvControlled->PantRearSP = false;
			if (mvControlled->PantRear(true))
			if (mvControlled->PantRearStart != 1)
			{
				if (sound)
				{
					dsbSwitch->SetVolume(DSBVOLUME_MAX);
					dsbSwitch->Play(0, 0, 0);
				}
			}
		}
		else
			// if
			// ((mvOccupied->ActiveCab<1)&&(mvControlled->TrainType&(dt_ET40|dt_ET41|dt_ET42|dt_EZT)))
		{ // przedni w kabinie -1 dla ET40, ET41, ET42 i EZT
			mvControlled->PantFrontSP = false;
			if (mvControlled->PantFront(true))
			if (mvControlled->PantFrontStart != 1)
			{
				if (sound)
				{
					dsbSwitch->SetVolume(DSBVOLUME_MAX);
					dsbSwitch->Play(0, 0, 0);
				}
			}
		}
	}else
	{
		if (mvOccupied->ActiveCab ==
			1) //||((mvOccupied->ActiveCab<1)&&(mvControlled->TrainType!=dt_ET40)&&(mvControlled->TrainType!=dt_ET41)&&(mvControlled->TrainType!=dt_ET42)&&(mvControlled->TrainType!=dt_EZT)))
		{
			if (mvControlled->PantSwitchType == "impulse")
				ggPantFrontButtonOff.PutValue(1);
			if (mvControlled->PantRear(false))
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
		else
			// if
			// ((mvOccupied->ActiveCab<1)&&((mvControlled->TrainType==dt_ET40)||(mvControlled->TrainType==dt_ET41)||(mvControlled->TrainType==dt_ET42)||(mvControlled->TrainType==dt_EZT)))
		{
			/* if (mvControlled->PantSwitchType=="impulse")
			ggPantRearButtonOff.PutValue(1);  */
			if (mvControlled->PantFront(false))
			{
				dsbSwitch->SetVolume(DSBVOLUME_MAX);
				dsbSwitch->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_ActiveSw(void)		//w³¹cznik rozrz¹du
{ // Ra 2014-06: uruchomi³em to, aby aktywowaæ czuwak w zajmowanym cz³onie, a wy³¹czyæ w
	// innych
	// Ra 2014-03: aktywacja czuwaka przepiêta na ustawienie kierunku w mvOccupied
	// if (mvControlled->Battery) //jeœli bateria jest ju¿ za³¹czona
	// mvOccupied->BatterySwitch(true); //to w ten oto durny sposób aktywuje siê CA/SHP
	//        if (mvControlled->CabActivisation())
	//           {
	//            dsbSwitch->SetVolume(DSBVOLUME_MAX);
	//            dsbSwitch->Play(0,0,0);
	//           }
}

void TTrain::_mwd_HeatingSw(bool state, bool sound)
{
 WriteLog("_mwd_HeatingSw " + IntToStr(state));
	if (state)// Ra 2014-09: w trybie latania obs³uga jest w World.cpp
	{
		if (!FreeFlyModeFlag)
		{
			if ((mvControlled->Heating == false) &&
				((mvControlled->EngineType == ElectricSeriesMotor) &&
				(mvControlled->Mains == true) ||
				(mvControlled->ConverterFlag)))
			{
				mvControlled->Heating = true;
				if (sound)
				{
					dsbSwitch->SetVolume(DSBVOLUME_MAX);
					dsbSwitch->Play(0, 0, 0);
				}
			}
		}
	}
       else
	{ // Ra 2014-09: w trybie latania obs³uga jest w World.cpp
		if (!FreeFlyModeFlag)
		{
			if (mvControlled->Heating == true)
			{
				if (sound)
				{
					dsbSwitch->SetVolume(DSBVOLUME_MAX);
					dsbSwitch->Play(0, 0, 0);
				}
				mvControlled->Heating = false;
			}
		}
	}
}

void TTrain::_mwd_CzuwakSw(void)
{ // Ra: tu zosta³ tylko dŸwiêk
 WriteLog("_mwd_CzuwakSw ");
	// dsbBuzzer->Stop();
	// if (mvOccupied->SecuritySystemReset())
	if (fabs(ggSecurityResetButton.GetValue()) < 0.001)
	{
		dsbSwitch->SetVolume(DSBVOLUME_MAX);
		dsbSwitch->Play(0, 0, 0);
	}
	// ggSecurityResetButton.PutValue(1);
}

void TTrain::_mwd_AntiSlippingSw(void)
{
 WriteLog("_mwd_AntiSlippingSw ");
	if (mvOccupied->BrakeSystem != ElectroPneumatic)
	{
		// if (mvControlled->AntiSlippingButton())
		if (fabs(ggAntiSlipButton.GetValue()) < 0.001)
		{
			// Dlaczego bylo '-50'???
			// dsbSwitch->SetVolume(-50);
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
		}
		// ggAntiSlipButton.PutValue(1);
	}
}

void TTrain::_mwd_FuseSw(void)
{
 WriteLog("_mwd_FuseSw ");
	if (GetAsyncKeyState(VK_CONTROL) < 0) // z controlem
	{
		ggConverterFuseButton.PutValue(1); // hunter-261211
		if ((mvControlled->Mains == false) && (ggConverterButton.GetValue() == 0))
			mvControlled->ConvOvldFlag = false;
	}
	else
	{
		ggFuseButton.PutValue(1);
		mvControlled->FuseOn();
	}
}

void TTrain::_mwd_DirectionForwardSw(void)
{
 WriteLog("_mwd_DirectionForwardSw ");
	if (mvOccupied->DirectionForward())
	{
		//------------
		// hunter-121211: dzwiek kierunkowego
		if (dsbReverserKey)
		{
			dsbReverserKey->SetCurrentPosition(0);
			dsbReverserKey->SetVolume(DSBVOLUME_MAX);
			dsbReverserKey->Play(0, 0, 0);
		}
		else if (!dsbReverserKey)
		if (dsbSwitch)
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
		}
		//------------
		if (mvOccupied->ActiveDir) // jeœli kierunek niezerowy
		if (DynamicObject->Mechanik) // na wszelki wypadek
			DynamicObject->Mechanik->CheckVehicles(
			Change_direction); // aktualizacja skrajnych pojazdów w sk³adzie
	}
}

void TTrain::_mwd_DirectionBackwardSw(void)
{
 WriteLog("_mwd_DirectionBackwardSw ");
	if (GetAsyncKeyState(VK_CONTROL) < 0)
	{ // wciœniêty [Ctrl]
		if (mvControlled->Radio == true)
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
			mvControlled->Radio = false;
		}
	}
	else if (mvOccupied->DirectionBackward())
	{
		//------------
		// hunter-121211: dzwiek kierunkowego
		if (dsbReverserKey)
		{
			dsbReverserKey->SetCurrentPosition(0);
			dsbReverserKey->SetVolume(DSBVOLUME_MAX);
			dsbReverserKey->Play(0, 0, 0);
		}
		else if (!dsbReverserKey)
		if (dsbSwitch)
		{
			dsbSwitch->SetVolume(DSBVOLUME_MAX);
			dsbSwitch->Play(0, 0, 0);
		}
		//------------
		if (mvOccupied->ActiveDir) // jeœli kierunek niezerowy
		if (DynamicObject->Mechanik) // na wszelki wypadek
			DynamicObject->Mechanik->CheckVehicles(
			Change_direction); // aktualizacja skrajnych pojazdów w sk³adzie
	}
}

void TTrain::_mwd_ReleaserSw(void)
{
 WriteLog("_mwd_ReleaserSw ");
	if (!FreeFlyModeFlag)
	{
		if ((mvControlled->EngineType == ElectricSeriesMotor) || (mvControlled->EngineType == DieselElectric) ||
			(mvControlled->EngineType == ElectricInductionMotor))
		if (mvControlled->TrainType != dt_EZT)
		if (mvOccupied->BrakeCtrlPosNo > 0)
		{
			ggReleaserButton.PutValue(1);
			if (mvOccupied->BrakeReleaser(1))
			{
				dsbPneumaticRelay->SetVolume(-80);
				dsbPneumaticRelay->Play(0, 0, 0);
			}
		}
	}
}

void TTrain::_mwd_RadioChDown(void)
{ // zmniejszenie numeru kana³u radiowego
  //	if (iRadioChannel > 0)
  //		--iRadioChannel; // 0=wy³¹czony
}

void TTrain::_mwd_RadioChUp(void)
{ // zmniejszenie numeru kana³u radiowego
  //	if (iRadioChannel < 8)
  //		++iRadioChannel; // 0=wy³¹czony
}

void TTrain::_mwd_HornH(void)
{
  //
}

void TTrain::_mwd_HornL(void)
{
  //
}

void TTrain::_mwd_LeftSignSw(bool state, bool sound)              //
{
  //
}

void TTrain::_mwd_UpperSignSw(bool state, bool sound)             //
{
  //
}

void TTrain::_mwd_RightSignSw(bool state, bool sound)             //
{
  //
}


// ***********************************************************************************************************
//
// ***********************************************************************************************************
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


  float fbrakepress = (BrakePress / (QGlobal::fmaxCH * 10))*255;   // ciœnienie w cylindrach hamulcowych // np 0.40/10 = 0.04
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
   BL_NADMIARWENTYL =    Train->btLampkaNadmWent.bOn;	       	//P5.2
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

   dataout0 = SETBYTE1(0, 0, 0, 0, 0, 0, 0, 0);
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

   if (Global::bMWDInEnable)
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


     PROCESSIOACTION("P1B1", QGlobal::portstate[1][1]);
     PROCESSIOACTION("P1B2", QGlobal::portstate[1][2]);
     PROCESSIOACTION("P1B3", QGlobal::portstate[1][3]);
     PROCESSIOACTION("P1B4", QGlobal::portstate[1][4]);
     PROCESSIOACTION("P1B5", QGlobal::portstate[1][5]);
     PROCESSIOACTION("P1B6", QGlobal::portstate[1][6]);
     PROCESSIOACTION("P1B7", QGlobal::portstate[1][7]);
     PROCESSIOACTION("P1B8", QGlobal::portstate[1][8]);

     PROCESSIOACTION("P2B1", QGlobal::portstate[2][1]);
     PROCESSIOACTION("P2B2", QGlobal::portstate[2][2]);
     PROCESSIOACTION("P2B3", QGlobal::portstate[2][3]);
     PROCESSIOACTION("P2B4", QGlobal::portstate[2][4]);
     PROCESSIOACTION("P2B5", QGlobal::portstate[2][5]);
     PROCESSIOACTION("P2B6", QGlobal::portstate[2][6]);
     PROCESSIOACTION("P2B7", QGlobal::portstate[2][7]);
     PROCESSIOACTION("P2B8", QGlobal::portstate[2][8]);

     PROCESSIOACTION("P3B1", QGlobal::portstate[3][1]);
     PROCESSIOACTION("P3B2", QGlobal::portstate[3][2]);
     PROCESSIOACTION("P3B3", QGlobal::portstate[3][3]);
     PROCESSIOACTION("P3B4", QGlobal::portstate[3][4]);
     PROCESSIOACTION("P3B5", QGlobal::portstate[3][5]);
     PROCESSIOACTION("P3B6", QGlobal::portstate[3][6]);
     PROCESSIOACTION("P3B7", QGlobal::portstate[3][7]);
     PROCESSIOACTION("P3B8", QGlobal::portstate[3][8]);

     PROCESSIOACTION("P4B1", QGlobal::portstate[4][1]);
     PROCESSIOACTION("P4B2", QGlobal::portstate[4][2]);
     PROCESSIOACTION("P4B3", QGlobal::portstate[4][3]);
     PROCESSIOACTION("P4B4", QGlobal::portstate[4][4]);
     PROCESSIOACTION("P4B5", QGlobal::portstate[4][5]);
     PROCESSIOACTION("P4B6", QGlobal::portstate[4][6]);
     PROCESSIOACTION("P4B7", QGlobal::portstate[4][7]);
     PROCESSIOACTION("P4B8", QGlobal::portstate[4][8]);

     PROCESSIOACTION("P5B1", QGlobal::portstate[5][1]);
     PROCESSIOACTION("P5B2", QGlobal::portstate[5][2]);
     PROCESSIOACTION("P5B3", QGlobal::portstate[5][3]);
     PROCESSIOACTION("P5B4", QGlobal::portstate[5][4]);
     PROCESSIOACTION("P5B5", QGlobal::portstate[5][5]);
     PROCESSIOACTION("P5B6", QGlobal::portstate[5][6]);
     PROCESSIOACTION("P5B7", QGlobal::portstate[5][7]);
     PROCESSIOACTION("P5B8", QGlobal::portstate[5][8]);

     PROCESSIOACTION("P6B1", QGlobal::portstate[6][1]);
     PROCESSIOACTION("P6B2", QGlobal::portstate[6][2]);
     PROCESSIOACTION("P6B3", QGlobal::portstate[6][3]);
     PROCESSIOACTION("P6B4", QGlobal::portstate[6][4]);
     PROCESSIOACTION("P6B5", QGlobal::portstate[6][5]);
     PROCESSIOACTION("P6B6", QGlobal::portstate[6][6]);
     PROCESSIOACTION("P6B7", QGlobal::portstate[6][7]);
     PROCESSIOACTION("P6B8", QGlobal::portstate[6][8]);

     //if (QGlobal::bLOGIT)
      // WriteLog(str(rps->P1B1) + ", " + str(rps->P1B2) + ", " + str(rps->P1B3) + ", " + str(rps->P1B4) + ", " + str(rps->P1B5) + ", " + str(rps->P1B6) + ", " + str(rps->P1B7) + ", " + str(rps->P1B8));

    }
  }
}


// ***********************************************************************************************************
// This procedure compare 'key string' previously generated in keyboard callback witch items collected
// in io binding table. If it hits in the same string, that happens download command string
void TWorld::PROCESSIOACTION(std::string str, bool state)
{
	std::string test, command;
        QGlobal::IOCOMMAND = "";
        
	for (int i = 0; i < QGlobal::iobindsnum; i++)
	{
		test = QGlobal::IOSET[i].bitid; // + ":" + QGlobal::IOSET[i].ifbit + ":" + QGlobal::IOSET[i].command;

		if (test == str)   // jezeli id I/O podany w wywolaniu funkcji jest identyczny z tym z tablicy mapy IO...
		{
		    // if (state == atoi(QGlobal::IOSET[i].ifbit1.c_str()))   // ...to jezeli stan bitu jest identyczny z ustawionym bitem reakcji...
                        {
                          QGlobal::IOCOMMAND = QGlobal::IOSET[i].command1;   //... to daj do zmiennej globalnej identyfikator funkcji przypisanej do IO
                           if (QGlobal::IOCOMMAND != "-")
                            if (Train) Train->OnMWDCommand(QGlobal::IOCOMMAND, state);   // wywolaj procedure wykonujaca odpowiednie polecenia (ponizsza)
                          }
		     break;
		}

	}
        
}


// ***********************************************************************************************************
//
// ***********************************************************************************************************
void TTrain::OnMWDCommand(std::string cmd, bool state)
{
 //TTrain T;
 isEztOer = ((mvControlled->TrainType == dt_EZT) && (mvControlled->Battery == true) && (mvControlled->EpFuse == true) && (mvOccupied->BrakeSubsystem == ss_ESt) && (mvControlled->ActiveDir != 0)); // od yB



if (QGlobal::IOCOMMAND != "")
 {
  WriteLog("@" + AnsiString(cmd.c_str()));
  if (cmd == "mwd_BatterySw")
  {
                         _mwd_BatterySw(state, 0);             //
  }
  if (cmd == "mwd_PantFrontSw")
  {
                         _mwd_PantFrontSw(state, 0);           //
  }
  if (cmd == "mwd_PantRearSw")
  {
                         _mwd_PantRearSw(state, 0);            //
  }
  if (cmd == "mwd_MainSw")
  {
                         _mwd_MainSw(state, 0);                //
  }
  if (cmd == "mwd_FuseSw")
  {
                         _mwd_FuseSw();                        //
  }
  if (cmd == "mwd_StLinOffSw")
  {
                         _mwd_StLinOffSw(state, 0);            //
  }
  if (cmd == "mwd_CzuwakSw")
  {
                         _mwd_CzuwakSw();                      //
  }
  if (cmd == "mwd_AntiSlippingSw")
  {
                         _mwd_AntiSlippingSw();                //
  }
  if (cmd == "mwd_ConverterSw")
  {
                         _mwd_ConverterSw(state, 0);           //
  }
  if (cmd == "mwd_CompressorSw")
  {
                         _mwd_CompressorSw(state, 0);          //
  }
  if (cmd == "mwd_SandSw")
  {
                         _mwd_SandSw(state, 0);                //
  }
  if (cmd == "mwd_HeatingSw")
  {
                         _mwd_HeatingSw(state, 0);             //
  }
  if (cmd == "mwd_BrakeProfileSw")
  {
                         _mwd_BrakeProfileSw(state);           //
  }
  if (cmd == "mwd_MaxCurrentSw")
  {
                         _mwd_MaxCurrentSw(state, 0);          //
  }
  if (cmd == "mwd_ReleaserSw")
  {
                         _mwd_ReleaserSw();                    //
  }
  if (cmd == "mwd_HornH")
  {
                         _mwd_HornH();                         //
  }
  if (cmd == "mwd_HornL")
  {
                         _mwd_HornL();                         //
  }
  if (cmd == "mwd_UpperSignSw")
  {
                         _mwd_UpperSignSw(state, 0);           //
  }
  if (cmd == "mwd_RightSignSw")
  {
                         _mwd_RightSignSw(state, 0);           //
  }
  if (cmd == "mwd_LeftSignSw")
  {
                         _mwd_LeftSignSw(state, 0);            //
  }
  if (cmd == "mwd_SmallCompressorSw")
  {
                         _mwd_SmallCompressorSw(state, 0);     //
  }
  if (cmd == "mwd_FailedEngineCutOffSw")
  {
                         _mwd_FailedEngineCutOffSw();          //
  }
  if (cmd == "mwd_RadioChDown")
  {
                         _mwd_RadioChDown();
  }
  if (cmd == "mwd_RadioChUp")
  {
                         _mwd_RadioChUp();
  }
  if (cmd == "mwd_LeftDoor")
  {
                         _mwd_LeftDoor(state);
  }
  if (cmd == "mwd_RightDoor")
  {
                         _mwd_RightDoor(state);
  }
  if (cmd == "mwd_ActiveSw")
  {
                         _mwd_ActiveSw();
  }
  if (cmd == "mwd_DirectionForwardSw")
  {
                         _mwd_DirectionForwardSw();
  }
  if (cmd == "mwd_DirectionBackwardSw")
  {
                         _mwd_DirectionBackwardSw();
  }
  if (cmd == "mwd_CurrentAutoRelaySw")
  {
                         _mwd_CurrentAutoRelaySw(state, 0);
  }
 }

 QGlobal::IOCOMMAND = "";
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
        AnsiString command1, command2;
	while (std::getline(file, str))
	{ 
		int ishash;
                int isComment;
		ishash = str.find('#');
                isComment = str.find('//');
              //WriteLog("Comment at: " + IntToStr(isComment));

		if ((str.length() > 5) && (ishash < 0))
		{// example line: ? 1 P3B1 "APPEXIT"

                 skeybind = split(str, ' ');
                 QGlobal::IOSET[cl].bit = trim(skeybind[0]);
                 QGlobal::IOSET[cl].ifbit1 = trim(skeybind[1]);
                 QGlobal::IOSET[cl].ifbit0 = trim(skeybind[2]);
                 QGlobal::IOSET[cl].bitid = trim(skeybind[3]);
                 QGlobal::IOSET[cl].command1 = trim(skeybind[4]);
                 QGlobal::iobindsnum++;

                 command1 = QGlobal::IOSET[cl].command1.c_str();
                 command1 = command1.Trim();
                 QGlobal::IOSET[cl].command1 = command1.c_str();

                 WriteLogSS("IOMAP:",  "[" + QGlobal::IOSET[cl].bitid + "]:[" + QGlobal::IOSET[cl].ifbit1 + "]:[" + QGlobal::IOSET[cl].ifbit0 + "]:[" + QGlobal::IOSET[cl].command1 + "]" );
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










