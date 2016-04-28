/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef DriverH
#define DriverH

#include "Classes.h"
#include "dumb3d.h"
#include <fstream>
using namespace Math3D;

enum TOrders
{ // rozkazy dla AI
    Wait_for_orders = 0, // czekanie na dostarczenie nast�pnych rozkaz�w
    // operacje tymczasowe
    Prepare_engine = 1, // w��czenie silnika
    Release_engine = 2, // wy��czenie silnika
    Change_direction = 4, // zmiana kierunku (bez skanowania sygnalizacji)
    Connect = 8, // pod��czanie wagon�w (z cz�ciowym skanowaniem sygnalizacji)
    Disconnect = 0x10, // od��czanie wagon�w (bez skanowania sygnalizacji)
    // jazda
    Shunt = 0x20, // tryb manewrowy
    Obey_train = 0x40, // tryb poci�gowy
    Jump_to_first_order = 0x60 // zap�lenie do pierwszej pozycji (po co?)
};

enum TMovementStatus
{ // flagi bitowe ruchu (iDrivigFlags)
    moveStopCloser = 1, // podjecha� blisko W4 (nie podje�d�a� na pocz�tku ani po zmianie czo�a)
    moveStopPoint = 2, // stawa� na W4 (wy��czone podczas zmiany czo�a)
    moveActive = 4, // pojazd jest za��czony i skanuje
    movePress = 8, // dociskanie przy od��czeniu (zamiast zmiennej Prepare2press)
    moveConnect = 0x10, // jest blisko innego pojazdu i mo�na pr�bowa� pod��czy�
    movePrimary = 0x20, // ma priorytet w sk�adzie (master)
    moveLate = 0x40, // flaga sp�nienia, w��czy bardziej
    moveStopHere = 0x80, // nie podje�d�a� do semafora, je�li droga nie jest wolna
    moveStartHorn = 0x100, // podawaj sygna� po podaniu wolnej drogi
    moveStartHornNow = 0x200, // podaj sygna� po odhamowaniu
    moveStartHornDone = 0x400, // podano sygna� po podaniu wolnej drogi
    moveOerlikons = 0x800, // sk�ad wy��cznie z zaworami? Oerlikona
    moveIncSpeed = 0x1000, // za��czenie jazdy (np. dla EZT)
    moveTrackEnd = 0x2000, // dalsza jazda do przodu trwale ograniczona (W5, koniec toru)
    moveSwitchFound = 0x4000, // na drodze skanowania do przodu jest rozjazd
    moveGuardSignal = 0x8000, // sygna� od kierownika (min�� czas postoju)
    moveVisibility = 0x10000, // jazda na widoczno�� po przejechaniu S1 na SBL
    moveDoorOpened = 0x20000, // drzwi zosta�y otwarte - doliczy� czas na zamkni�cie
    movePushPull = 0x40000, // zmiana czo�a przez zmian� kabiny - nie odczepia� przy zmianie kierunku
    moveSemaphorFound = 0x80000, // na drodze skanowania zosta� znaleziony semafor
    moveSemaphorWasElapsed = 0x100000, // mini�ty zosta� semafor
    moveTrainInsideStation = 0x200000, // poci�g mi�dzy semaforem a rozjazdami lub nast�pnym semaforem
    moveSpeedLimitFound = 0x400000 // poci�g w ograniczeniu z podan� jego d�ugo�ci�
};

enum TStopReason
{ // pow�d zatrzymania, dodawany do SetVelocity 0 - w zasadzie do usuni�cia
    stopNone, // nie ma powodu - powinien jecha�
    stopSleep, // nie zosta� odpalony, to nie pojedzie
    stopSem, // semafor zamkni�ty
    stopTime, // czekanie na godzin� odjazdu
    stopEnd, // brak dalszej cz�ci toru
    stopDir, // trzeba stan��, by zmieni� kierunek jazdy
    stopJoin, // stoi w celu po��czenia wagon�w
    stopBlock, // przeszkoda na drodze ruchu
    stopComm, // otrzymano tak� komend� (niewiadomego pochodzenia)
    stopOut, // komenda wyjazdu poza stacj� (raczej nie powinna zatrzymywa�!)
    stopRadio, // komunikat przekazany radiem (Radiostop)
    stopExt, // komenda z zewn�trz
    stopError // z powodu b��du w obliczeniu drogi hamowania
};

enum TAction
{ // przechowanie aktualnego stanu AI od poprzedniego przeb�ysku �wiadomo�ci
    actUnknown, // stan nieznany (domy�lny na pocz�tku)
    actPantUp, // podnie� pantograf (info dla u�ytkownika)
    actConv, // za��cz przetwornic� (info dla u�ytkownika)
    actCompr, // za��cz spr�ark� (info dla u�ytkownika)
    actSleep, //�pi (wygaszony)
    actDrive, // jazda
    actGo, // ruszanie z miejsca
    actSlow, // przyhamowanie przed ograniczeniem
    sctStop, // hamowanie w celu precyzyjnego zatrzymania
    actIdle, // luzowanie sk�adu przed odjazdem
    actRelease, // luzowanie sk�adu po zmniejszeniu pr�dko�ci
    actConnect, // dojazd w celu podczepienia
    actWait, // czekanie na przystanku
    actReady, // zg�oszona gotowo�� do odjazdu od kierownika
    actEmergency, // hamowanie awaryjne
    actGoUphill, // ruszanie pod g�r�
    actTest, // hamowanie kontrolne (podczas jazdy)
    actTrial // pr�ba hamulca (na postoju)
};

enum TSpeedPosFlag
{ // warto�ci dla iFlag w TSpeedPos
    spEnabled = 0x1, // pozycja brana pod uwag�
    spTrack = 0x2, // to jest tor
    spReverse = 0x4, // odwrotnie
    spSwitch = 0x8, // to zwrotnica
    spSwitchStatus = 0x10, // stan zwrotnicy
    spElapsed = 0x20, // pozycja mini�ta przez pojazd
    spEnd = 0x40, // koniec
    spCurve = 0x80, // �uk
    spEvent = 0x100, // event
    spShuntSemaphor = 0x200, // tarcza manewrowa
    spPassengerStopPoint = 0x400, // przystanek osobowy (wska�nik W4)
    spStopOnSBL = 0x800, // zatrzymanie na SBL
    spCommandSent = 0x1000, // komenda wys�ana
    spOutsideStation = 0x2000, // wska�nik ko�ca manewr�w
    spSemaphor = 0x4000, // semafor poci�gowy
    spRoadVel = 0x8000, // zadanie pr�dko�ci drogowej
    spSectionVel = 0x20000, // odcinek z ograniczeniem
    spProximityVelocity = 0x40000, // odcinek z ograniczeniem i podan� jego d�ugo�cia
    spEndOfTable = 0x10000 // zatkanie tabelki
};

class TSpeedPos
{ // pozycja tabeli pr�dko�ci dla AI
  public:
    double fDist; // aktualna odleg�o�� (ujemna gdy mini�te)
    double fVelNext; // pr�dko�� obowi�zuj�ca od tego miejsca
    double fSectionVelocityDist; //d�ugo�� ograniczenia pr�dko�ci
    // double fAcc;
    int iFlags; //flagi typu wpisu do tabelki
    // 1=istotny,2=tor,4=odwrotnie,8-zwrotnica (mo�e si� zmieni�),16-stan
    // zwrotnicy,32-mini�ty,64=koniec,128=�uk
    // 0x100=event,0x200=manewrowa,0x400=przystanek,0x800=SBL,0x1000=wys�ana komenda,0x2000=W5
    // 0x4000=semafor,0x10000=zatkanie
    vector3 vPos; // wsp�rz�dne XYZ do liczenia odleg�o�ci
    struct
    {
        TTrack *trTrack; // wska�nik na tor o zmiennej pr�dko�ci (zwrotnica, obrotnica)
        TEvent *evEvent; // po��czenie z eventem albo kom�rk� pami�ci
    };
    void CommandCheck();

  public:
    void Clear();
    bool Update(vector3 *p, vector3 *dir, double &len);
    bool Set(TEvent *e, double d, TOrders order = Wait_for_orders);
    void Set(TTrack *t, double d, int f);
    AnsiString TableText();
	AnsiString GetName();
	bool IsProperSemaphor(TOrders order = Wait_for_orders);
};

//----------------------------------------------------------------------------
static const bool Aggressive = true;
static const bool Easyman = false;
static const bool AIdriver = true;
static const bool Humandriver = false;
static const int maxorders = 32; // ilo�� rozkaz�w w tabelce
static const int maxdriverfails = 4; // ile b��d�w mo�e zrobi� AI zanim zmieni nastawienie
extern bool WriteLogFlag; // logowanie parametr�w fizycznych
//----------------------------------------------------------------------------

class TController
{
  private: // obs�uga tabelki pr�dko�ci (musi mie� mo�liwo�� odhaczania stacji w rozk�adzie)
    TSpeedPos *sSpeedTable; // najbli�sze zmiany pr�dko�ci
    int iSpeedTableSize; // wielko�� tabelki
    int iFirst; // aktualna pozycja w tabeli (modulo iSpeedTableSize)
    int iLast; // ostatnia wype�niona pozycja w tabeli <iFirst (modulo iSpeedTableSize)
    int iTableDirection; // kierunek zape�nienia tabelki wzgl�dem pojazdu z AI
    double fLastVel; // pr�dko�� na poprzednio sprawdzonym torze
    TTrack *tLast; // ostatni analizowany tor
    TEvent *eSignSkip; // mo�na pomin�� ten SBL po zatrzymaniu
	TSpeedPos *sSemNext; // nast�pny semafor na drodze zale�ny od trybu jazdy
	TSpeedPos *sSemNextStop; // nast�pny semafor na drodze zale�ny od trybu jazdy i na st�j
  private: // parametry aktualnego sk�adu
    double fLength; // d�ugo�� sk�adu (do wyci�gania z ogranicze�)
    double fMass; // ca�kowita masa do liczenia stycznej sk�adowej grawitacji
    double fAccGravity; // przyspieszenie sk�adowej stycznej grawitacji
  public:
    TEvent *eSignNext; // sygna� zmieniaj�cy pr�dko��, do pokazania na [F2]
    AnsiString asNextStop; // nazwa nast�pnego punktu zatrzymania wg rozk�adu
    int iStationStart; // numer pierwszej stacji pokazywanej na podgl�dzie rozk�adu
  private: // parametry sterowania pojazdem (stan, hamowanie)
    double fShuntVelocity; // maksymalna pr�dko�� manewrowania, zale�y m.in. od sk�adu
    int iVehicles; // ilo�� pojazd�w w sk�adzie
    int iEngineActive; // ABu: Czy silnik byl juz zalaczony; Ra: post�p w za��czaniu
    // vector3 vMechLoc; //pozycja pojazdu do liczenia odleg�o�ci od semafora (?)
    bool Psyche;
    int iDrivigFlags; // flagi bitowe ruchu
    double fDriverBraking; // po pomno�eniu przez v^2 [km/h] daje ~drog� hamowania [m]
    double fDriverDist; // dopuszczalna odleg�o�� podjechania do przeszkody
    double fVelMax; // maksymalna pr�dko�� sk�adu (sprawdzany ka�dy pojazd)
    double fBrakeDist; // przybli�ona droga hamowania
    double fAccThreshold; // pr�g op�nienia dla zadzia�ania hamulca
  public:
    double fLastStopExpDist; // odleg�o�� wygasania ostateniego przystanku
    double ReactionTime; // czas reakcji Ra: czego i na co? �wiadomo�ci AI
    double fBrakeTime; // wpisana warto�� jest zmniejszana do 0, gdy ujemna nale�y zmieni� nastaw�
    // hamulca
  private:
    double fReady; // poziom odhamowania wagon�w
    bool Ready; // ABu: stan gotowosci do odjazdu - sprawdzenie odhamowania wagonow
    double LastUpdatedTime; // czas od ostatniego logu
    double ElapsedTime; // czas od poczatku logu
    double deltalog; // przyrost czasu
    double LastReactionTime;
    double fActionTime; // czas u�ywany przy regulacji pr�dko�ci i zamykaniu drzwi
    TAction eAction; // aktualny stan
    bool HelpMeFlag; // wystawiane True jesli cos niedobrego sie dzieje
  public:
    inline TAction GetAction()
    {
        return eAction;
    }
    bool AIControllFlag; // rzeczywisty/wirtualny maszynista
    int iRouteWanted; // oczekiwany kierunek jazdy (0-stop,1-lewo,2-prawo,3-prosto) np. odpala
    // migacz lub czeka na stan zwrotnicy
  private:
    TDynamicObject *pVehicle; // pojazd w kt�rym siedzi steruj�cy
    TDynamicObject *
        pVehicles[2]; // skrajne pojazdy w sk�adzie (niekoniecznie bezpo�rednio sterowane)
    TMoverParameters *mvControlling; // jakim pojazdem steruje (mo�e silnikowym w EZT)
    TMoverParameters *mvOccupied; // jakim pojazdem hamuje
    Mtable::TTrainParameters *TrainParams; // rozk�ad jazdy zawsze jest, nawet je�li pusty
    // int TrainNumber; //numer rozkladowy tego pociagu
    // AnsiString OrderCommand; //komenda pobierana z pojazdu
    // double OrderValue; //argument komendy
    int iRadioChannel; // numer aktualnego kana�u radiowego
    TTextSound *tsGuardSignal; // komunikat od kierownika
    int iGuardRadio; // numer kana�u radiowego kierownika (0, gdy nie u�ywa radia)
  public:
    double AccPreferred; // preferowane przyspieszenie (wg psychiki kieruj�cego, zmniejszana przy
    // wykryciu kolizji)
    double AccDesired; // przyspieszenie, jakie ma utrzymywa� (<0:nie przyspieszaj,<-0.1:hamuj)
    double VelDesired; // predko��, z jak� ma jecha�, wynikaj�ca z analizy tableki; <=VelSignal
    double fAccDesiredAv; // u�rednione przyspieszenie z kolejnych przeb�ysk�w �wiadomo�ci, �eby
    // ograniczy� migotanie
  public:
    double VelforDriver; // pr�dko��, u�ywana przy zmianie kierunku (ograniczenie przy nieznajmo�ci
    // szlaku?)
    double VelSignal; // ograniczenie pr�dko�ci z kompilacji znak�w i sygna��w
    double VelLimit; // predko�� zadawana przez event jednokierunkowego ograniczenia pr�dko�ci
  public:
    double VelSignalLast; // pr�dko�� zadana na ostatnim semaforze
    double VelLimitLast; // pr�dko�� zadana przez ograniczenie
    double VelRoad; // aktualna pr�dko�� drogowa (ze znaku W27)
    // (PutValues albo komend�)
  public:
    double VelNext; // pr�dko��, jaka ma by� po przejechaniu d�ugo�ci ProximityDist
  private:
     double fProximityDist; //odleglosc podawana w SetProximityVelocity(); >0:przelicza� do
    // punktu, <0:podana warto��
	 double FirstSemaphorDist; // odleg�o�� do pierwszego znalezionego semafora
  public:
    double
        ActualProximityDist; // odleg�o�� brana pod uwag� przy wyliczaniu pr�dko�ci i przyspieszenia
  private:
    vector3 vCommandLocation; // polozenie wskaznika, sygnalizatora lub innego obiektu do ktorego
    // odnosi sie komenda
    TOrders OrderList[maxorders]; // lista rozkaz�w
    int OrderPos, OrderTop; // rozkaz aktualny oraz wolne miejsce do wstawiania nowych
    std::ofstream LogFile; // zapis parametrow fizycznych
    std::ofstream AILogFile; // log AI
    bool MaxVelFlag;
    bool MinVelFlag;
    int iDirection; // kierunek jazdy wzgl�dem sprz�g�w pojazdu, w kt�rym siedzi AI (1=prz�d,-1=ty�)
    int iDirectionOrder; //�adany kierunek jazdy (s�u�y do zmiany kierunku)
    int iVehicleCount; // ilo�� pojazd�w do od��czenia albo zabrania ze sk�adu (-1=wszystkie)
    int iCoupler; // maska sprz�gu, jak� nale�y u�y� przy ��czeniu (po osi�gni�ciu trybu Connect), 0
    // gdy jazda bez ��czenia
    int iDriverFailCount; // licznik b��d�w AI
    bool Need_TryAgain; // true, je�li druga pozycja w elektryku nie za�apa�a
    bool Need_BrakeRelease;

  public:
    double fMinProximityDist; // minimalna oleg�o�� do przeszkody, jak� nale�y zachowa�
    double fOverhead1; // informacja o napi�ciu w sieci trakcyjnej (0=brak drutu, zatrzymaj!)
    double fOverhead2; // informacja o sposobie jazdy (-1=normalnie, 0=bez pr�du, >0=z opuszczonym i
    // ograniczeniem pr�dko�ci)
    int iOverheadZero; // suma bitowa jezdy bezpr�dowej, bity ustawiane przez pojazdy z
    // podniesionymi pantografami
    int iOverheadDown; // suma bitowa opuszczenia pantograf�w, bity ustawiane przez pojazdy z
    // podniesionymi pantografami
    double fVoltage; // u�rednione napi�cie sieci: przy spadku poni�ej warto�ci minimalnej op�ni�
    // rozruch o losowy czas
  private:
    double fMaxProximityDist; // akceptowalna odleg�o�� stani�cia przed przeszkod�
    TStopReason eStopReason; // pow�d zatrzymania przy ustawieniu zerowej pr�dko�ci
    AnsiString VehicleName;
    double fVelPlus; // dopuszczalne przekroczenie pr�dko�ci na ograniczeniu bez hamowania
    double fVelMinus; // margines obni�enia pr�dko�ci, powoduj�cy za��czenie nap�du
    double fWarningDuration; // ile czasu jeszcze tr�bi�
    double fStopTime; // czas postoju przed dalsz� jazd� (np. na przystanku)
    double WaitingTime; // zliczany czas oczekiwania do samoistnego ruszenia
    double WaitingExpireTime; // maksymlany czas oczekiwania do samoistnego ruszenia
    // TEvent* eSignLast; //ostatnio znaleziony sygna�, o ile nie mini�ty
  private: //---//---//---//---// koniec zmiennych, poni�ej metody //---//---//---//---//
    void SetDriverPsyche();
    bool PrepareEngine();
    bool ReleaseEngine();
    bool IncBrake();
    bool DecBrake();
    bool IncSpeed();
    bool DecSpeed(bool force = false);
    void SpeedSet();
    void Doors(bool what);
    void RecognizeCommand(); // odczytuje komende przekazana lokomotywie
    void Activation(); // umieszczenie obsady w odpowiednim cz�onie
    void ControllingSet(); // znajduje cz�on do sterowania
    void AutoRewident(); // ustawia hamulce w sk�adzie
  public:
    Mtable::TTrainParameters *__fastcall Timetable()
    {
        return TrainParams;
    };
    void PutCommand(AnsiString NewCommand, double NewValue1, double NewValue2,
                    const _mover::TLocation &NewLocation, TStopReason reason = stopComm);
    bool PutCommand(AnsiString NewCommand, double NewValue1, double NewValue2,
                    const vector3 *NewLocation, TStopReason reason = stopComm);
    bool UpdateSituation(double dt); // uruchamiac przynajmniej raz na sekund�
    // procedury dotyczace rozkazow dla maszynisty
    void SetVelocity(double NewVel, double NewVelNext,
                     TStopReason r = stopNone); // uaktualnia informacje o pr�dko�ci
    bool SetProximityVelocity(
        double NewDist,
        double NewVelNext); // uaktualnia informacje o pr�dko�ci przy nastepnym semaforze
  public:
    void JumpToNextOrder();
    void JumpToFirstOrder();
    void OrderPush(TOrders NewOrder);
    void OrderNext(TOrders NewOrder);
    inline TOrders OrderCurrentGet();
    inline TOrders OrderNextGet();
    bool CheckVehicles(TOrders user = Wait_for_orders);

  private:
    void CloseLog();
    void OrderCheck();

  public:
    void OrdersInit(double fVel);
    void OrdersClear();
    void OrdersDump();
    TController(bool AI, TDynamicObject *NewControll, bool InitPsyche,
                bool primary = true // czy ma aktywnie prowadzi�?
                );
    AnsiString OrderCurrent();
    void WaitingSet(double Seconds);

  private:
    AnsiString Order2Str(TOrders Order);
    void DirectionForward(bool forward);
    int OrderDirectionChange(int newdir, TMoverParameters *Vehicle);
    void Lights(int head, int rear);
    double Distance(vector3 &p1, vector3 &n, vector3 &p2);

  private: // Ra: metody obs�uguj�ce skanowanie toru
    TEvent *__fastcall CheckTrackEvent(double fDirection, TTrack *Track);
    bool TableCheckEvent(TEvent *e);
    bool TableAddNew();
    bool TableNotFound(TEvent *e);
    void TableClear();
    TEvent *__fastcall TableCheckTrackEvent(double fDirection, TTrack *Track);
    void TableTraceRoute(double fDistance, TDynamicObject *pVehicle = NULL);
    void TableCheck(double fDistance);
    TCommandType TableUpdate(double &fVelDes, double &fDist, double &fNext, double &fAcc);
    void TablePurger();

  private: // Ra: stare funkcje skanuj�ce, u�ywane do szukania sygnalizatora z ty�u
    bool BackwardTrackBusy(TTrack *Track);
    TEvent *__fastcall CheckTrackEventBackward(double fDirection, TTrack *Track);
    TTrack *__fastcall BackwardTraceRoute(double &fDistance, double &fDirection, TTrack *Track,
                                          TEvent *&Event);
    void SetProximityVelocity(double dist, double vel, const vector3 *pos);
    TCommandType BackwardScan();

  public:
    void PhysicsLog();
    AnsiString StopReasonText();
    ~TController();
    AnsiString NextStop();
    void TakeControl(bool yes);
    AnsiString Relation();
    AnsiString TrainName();
    int StationCount();
    int StationIndex();
    bool IsStop();
    bool Primary()
    {
        return this ? bool(iDrivigFlags & movePrimary) : false;
    };
    int inline DrivigFlags()
    {
        return iDrivigFlags;
    };
    void MoveTo(TDynamicObject *to);
    void DirectionInitial();
    AnsiString TableText(int i);
    int CrossRoute(TTrack *tr);
    void RouteSwitch(int d);
    AnsiString OwnerName();
};

#endif
