/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef ButtonH
#define ButtonH

#include "Model3d.h"
#include "QueryParserComp.hpp"

class TButton
{ // animacja dwustanowa, w��cza jeden z dw�ch submodeli (jednego z nich mo�e nie by�)
  private:
    TSubModel *pModelOn, *pModelOff; // submodel dla stanu za��czonego i wy��czonego

    int iFeedbackBit; // Ra: bit informacji zwrotnej, do wyprowadzenia na pulpit
    void Update();

  public:
    bool bOn;
    TButton();
    ~TButton();
    void Clear(int i = -1);
    inline void FeedbackBitSet(int i)
    {
        iFeedbackBit = 1 << i;
    };
    inline void Turn(bool to)
    {
        bOn = to;
        Update();
    };
    inline void TurnOn()
    {
        bOn = true;
        Update();
    };
    inline void TurnOff()
    {
        bOn = false;
        Update();
    };
    inline void Switch()
    {
        bOn = !bOn;
        Update();
    };
    inline bool Active()
    {
        return (pModelOn) || (pModelOff);
    };
    void Init(AnsiString asName, TModel3d *pModel, bool bNewOn = false);
    void Load(TQueryParserComp *Parser, TModel3d *pModel1, TModel3d *pModel2 = NULL);
};

//---------------------------------------------------------------------------
#endif
