/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef EventH
#define EventH

#include "Classes.h"
#include "dumb3d.h"
using namespace Math3D;

typedef enum
{
    tp_Unknown,
    tp_Sound,
    tp_SoundPos,
    tp_Exit,
    tp_Disable,
    tp_Velocity,
    tp_Animation,
    tp_Lights,
    tp_UpdateValues,
    tp_GetValues,
    tp_PutValues,
    tp_Switch,
    tp_DynVel,
    tp_TrackVel,
    tp_Multiple,
    tp_AddValues,
    tp_Ignored,
    tp_CopyValues,
    tp_WhoIs,
    tp_LogValues,
    tp_Visible,
    tp_Voltage,
    tp_Message,
    tp_Friction,
    tp_setfog,
    tp_dynevent
} TEventType;

const int update_memstring = 0x0000001; // zmodyfikowa� tekst (UpdateValues)
const int update_memval1 = 0x0000002; // zmodyfikowa� pierwsz� wartos�
const int update_memval2 = 0x0000004; // zmodyfikowa� drug� wartos�
const int update_memadd = 0x0000008; // doda� do poprzedniej zawarto�ci
const int update_load = 0x0000010; // odczyta� �adunek
const int update_only = 0x00000FF; // warto�� graniczna
const int conditional_memstring = 0x0000100; // por�wnanie tekstu
const int conditional_memval1 = 0x0000200; // por�wnanie pierwszej warto�ci liczbowej
const int conditional_memval2 = 0x0000400; // por�wnanie drugiej warto�ci
const int conditional_else = 0x0010000; // flaga odwr�cenia warunku (przesuwana bitowo)
const int conditional_anyelse = 0x0FF0000; // do sprawdzania, czy s� odwr�cone warunki
const int conditional_trackoccupied = 0x1000000; // je�li tor zaj�ty
const int conditional_trackfree = 0x2000000; // je�li tor wolny
const int conditional_propability = 0x4000000; // zale�nie od generatora lizcb losowych
const int conditional_memcompare = 0x8000000; // por�wnanie zawarto�ci

union TParam
{
    void *asPointer;
    TMemCell *asMemCell;
    TGroundNode *nGroundNode;
    TTrack *asTrack;
    TAnimModel *asModel;
    TAnimContainer *asAnimContainer;
    TTrain *asTrain;
    TDynamicObject *asDynamic;
    TEvent *asEvent;
    bool asBool;
    double asdouble;
    int asInt;
    TTextSound *tsTextSound;
    char *asText;
    TCommandType asCommand;
    TTractionPowerSource *psPower;
};

class TEvent // zmienne: ev*
{ // zdarzenie
  private:
    void Conditions(cParser *parser, AnsiString s);

  public:
    AnsiString asName;
    bool bEnabled; // false gdy ma nie by� dodawany do kolejki (skanowanie sygna��w)
    int iQueued; // ile razy dodany do kolejki
    // bool bIsHistory;
    TEvent *evNext; // nast�pny w kolejce
    TEvent *evNext2;
    TEventType Type;
    double fStartTime;
    double fDelay;
    TDynamicObject *Activator;
    TParam Params[13]; // McZapkie-070502 //Ra: zamieni� to na union/struct
    unsigned int iFlags; // zamiast Params[8] z flagami warunku
    AnsiString asNodeName; // McZapkie-100302 - dodalem zeby zapamietac nazwe toru
    TEvent *evJoined; // kolejny event z t� sam� nazw� - od wersji 378
    double fRandomDelay; // zakres dodatkowego op�nienia
    AnsiString asctime1;
    AnsiString asctime2;
    AnsiString asfog_s;
    AnsiString asfog_e;
    AnsiString asfog_f;
  public: // metody
    TEvent(AnsiString m = "");
    ~TEvent();
    void Init();
    void Load(cParser *parser, vector3 *org);
    void AddToQuery(TEvent *e);
    AnsiString CommandGet();
    TCommandType Command();
    double ValueGet(int n);
    vector3 PositionGet();
    bool StopCommand();
    void StopCommandSent();
    void Append(TEvent *e);
};

//---------------------------------------------------------------------------
#endif
