/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak and others

*/

#include "system.hpp"
#include "classes.hpp"
#include "Texture.h"
#pragma hdrstop

#include "MdlMngr.h"
#include "Globals.h"
#include "qutils.h"
#define SeekFiles AnsiString("*.t3d")

TModel3d *__fastcall TMdlContainer::LoadModel(char *newName, bool dynamic)
{ // wczytanie modelu do kontenerka
    AnsiString full, path, fn, fnwe, fext, cmdfile, filetodel;
    SafeDeleteArray(Name);
    SafeDelete(Model);
    Name = new char[strlen(newName) + 1];
    strcpy(Name, newName);
    Model = new TModel3d();
    Model->iTYPE = QGlobal::iINCLUDETYPE; //Q 010116: tutaj obiektowi przypisujemy typ pobrany w parserze

    full = chartostdstr(Name).c_str();
    full = StringReplace( full.c_str(), "/", "\\", TReplaceFlags() << rfReplaceAll );
    path = ExtractFilePath(full);
    fn = ExtractFileName(full);
    fnwe = fn.SubString(1, fn.Length()-4);
    cmdfile = path + "recompile-" + fnwe + ".txt";
    filetodel = path + fnwe + ".e3d";

    if (FileExists(cmdfile))
     {
      DeleteFile(filetodel);   
      DeleteFile(cmdfile);
      WriteLog("FILE DELETED: " + filetodel + ", RECOMPILATION." );
     }

    //WriteLog("MODEL: " + IntToStr(Model->iTYPE));

    if (!Model->LoadFromFile(full.c_str(), dynamic)) // np. "models\\pkp/head1-y.t3d"
        SafeDelete(Model);
    return Model;
};

TMdlContainer *TModelsManager::Models;
int TModelsManager::Count;
const MAX_MODELS = 1000;

void TModelsManager::Init()
{
    Models = new TMdlContainer[MAX_MODELS];
    Count = 0;
}
/*
__fastcall TModelsManager::TModelsManager()
{
//    Models= NULL;
    Models= new TMdlContainer[MAX_MODELS];
    Count= 0;
};

__fastcall TModelsManager::~TModelsManager()
{
    Free();
};
  */
void TModelsManager::Free()
{
    SafeDeleteArray(Models);
}

TModel3d *__fastcall TModelsManager::LoadModel(char *Name, bool dynamic)
{ // wczytanie modelu do tablicy
    TModel3d *mdl = NULL;
    if (Count >= MAX_MODELS)
        Error("FIXME: Too many models, program will now crash :)");
    else
    {
        mdl = Models[Count].LoadModel(Name, dynamic);
        if (mdl)
            Count++; // je�li b��d wczytania modelu, to go nie wliczamy
    }
    return mdl;
}

TModel3d *__fastcall TModelsManager::GetModel(const char *Name, bool dynamic)
{ // model mo�e by� we wpisie "node...model" albo "node...dynamic", a tak�e by� dodatkowym w dynamic
    // (kabina, wn�trze, �adunek)
    // dla "node...dynamic" mamy podan� �cie�k� w "\dynamic\" i musi by� co najmniej 1 poziom, zwkle
    // s� 2
    // dla "node...model" mo�e by� typowy model statyczny ze �cie�k�, domy�lnie w "\scenery\" albo
    // "\models"
    // albo mo�e by� model z "\dynamic\", je�li potrzebujemy wstawi� auto czy wagon nieruchomo
    // - ze �cie�ki z kt�rej jest wywo�any, np. dir="scenery\bud\" albo dir="dynamic\pkp\st44_v1\"
    // plus name="model.t3d"
    // - z domy�lnej �cie�ki dla modeli, np. "scenery\" albo "models\" plus name="bud\dombale.t3d"
    // (dir="")
    // - konkretnie podanej �cie�ki np. name="\scenery\bud\dombale.t3d" (dir="")
    // wywo�ania:
    // - konwersja wszystkiego do E3D, podawana dok�adna �cie�ka, tekstury tam, gdzie plik
    // - wczytanie kabiny, dok�adna �cie�ka, tekstury z katalogu modelu
    // - wczytanie �adunku, �cie�ka dok�adna, tekstury z katalogu modelu
    // - wczytanie modelu, �cie�ka dok�adna, tekstury z katalogu modelu
    // - wczytanie przedsionk�w, �cie�ka dok�adna, tekstury z katalogu modelu
    // - wczytanie uproszczonego wn�trza, �cie�ka dok�adna, tekstury z katalogu modelu
    // - niebo animowane, �cie�ka brana ze wpisu, tekstury nieokre�lone
    // - wczytanie modelu animowanego - Init() - sprawdzi�
    char buf[255];
    AnsiString buftp = Global::asCurrentTexturePath; // zapami�tanie aktualnej �cie�ki do tekstur,
    // bo b�dzie tyczmasowo zmieniana
    /*
    // Ra: niby tak jest lepiej, ale dzia�a gorzej, wi�c przywr�cone jest oryginalne
     //nawet je�li model b�dzie pobrany z tablicy, to trzeba ustali� �cie�k� dla tekstur
     if (dynamic) //na razie tak, bo nie wiadomo, jaki mo�e mie� wp�yw na pozosta�e modele
     {//dla pojazd�w podana jest zawsze pe�na �cie�ka do modelu
      strcpy(buf,Name);
      if (strchr(Name,'/')!=NULL)
      {//pobieranie tekstur z katalogu, w kt�rym jest model
       Global::asCurrentTexturePath=Global::asCurrentTexturePath+AnsiString(Name);
       Global::asCurrentTexturePath.Delete(Global::asCurrentTexturePath.Pos("/")+1,Global::asCurrentTexturePath.Length());
      }
     }
     else
     {//dla modeli scenerii trzeba ustali� �cie�k�
      if (strchr(Name,'\\')==NULL)
      {//je�li nie ma lewego uko�nika w �cie�ce, a jest prawy, to zmieni� �cie�k� dla tekstur na t�
    z modelem
       strcpy(buf,"models\\"); //Ra: by�o by lepiej katalog doda� w parserze
       //strcpy(buf,"scenery\\"); //Ra: by�o by lepiej katalog doda� w parserze
       strcat(buf,Name);
       if (strchr(Name,'/')!=NULL)
       {//jeszcze musi by� prawy uko�nik
        Global::asCurrentTexturePath=Global::asCurrentTexturePath+AnsiString(Name);
        Global::asCurrentTexturePath.Delete(Global::asCurrentTexturePath.Pos("/")+1,Global::asCurrentTexturePath.Length());
       }
      }
      else
      {//je�li jest lewy uko�nik, to �cie�k� do tekstur zmieni� tylko dla pojazd�w
       strcpy(buf,Name);
      }
     }
     StrLower(buf);
     for (int i=0;i<Count;i++)
     {//bezsensowne przeszukanie tabeli na okoliczno�� wyst�pienia modelu
      if (strcmp(buf,Models[i].Name)==0)
      {
       Global::asCurrentTexturePath=buftp; //odtworzenie �cie�ki do tekstur
       return (Models[i].Model); //model znaleziony
      }
     };
    */
    if (strchr(Name, '\\') == NULL)
    {
        strcpy(buf, "models\\"); // Ra: by�o by lepiej katalog doda� w parserze
        strcat(buf, Name);
        if (strchr(Name, '/') != NULL)
        {
            Global::asCurrentTexturePath = Global::asCurrentTexturePath + AnsiString(Name);
            Global::asCurrentTexturePath.Delete(Global::asCurrentTexturePath.Pos("/") + 1, Global::asCurrentTexturePath.Length());

        }
    }
    else
    {
        strcpy(buf, Name);
        if (dynamic) // na razie tak, bo nie wiadomo, jaki mo�e mie� wp�yw na pozosta�e modele
            if (strchr(Name, '/') != NULL)
            { // pobieranie tekstur z katalogu, w kt�rym jest model
                Global::asCurrentTexturePath = Global::asCurrentTexturePath + AnsiString(Name);
                Global::asCurrentTexturePath.Delete(Global::asCurrentTexturePath.Pos("/") + 1, Global::asCurrentTexturePath.Length());
            }
    }
    StrLower(buf);
    for (int i = 0; i < Count; i++)
    {
        if (strcmp(buf, Models[i].Name) == 0)
        {
            Global::asCurrentTexturePath = buftp;

            return (Models[i].Model);
        }
    };
    TModel3d *tmpModel = LoadModel(buf, dynamic); // model nie znaleziony, to wczyta�
    Global::asCurrentTexturePath = buftp; // odtworzenie �cie�ki do tekstur

    return (tmpModel); // NULL je�li b��d
};

/*
TModel3d TModelsManager::GetModel(char *Name, AnsiString asReplacableTexture)
{
    GLuint ReplacableTextureID= 0;
    TModel3d NewModel;

    NewModel= *GetNextModel(Name);

    if (asReplacableTexture!=AnsiString("none"))
      ReplacableTextureID= TTexturesManager::GetTextureID(asReplacableTexture.c_str());

    NewModel.ReplacableSkinID=ReplacableTextureID;

    return NewModel;
};
*/

//---------------------------------------------------------------------------
#pragma package(smart_init)
