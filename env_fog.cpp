/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#include "system.hpp"
#include "classes.hpp"
#pragma hdrstop

#include "env_fog.h"
#include "Globals.h"

//---------------------------------------------------------------------------

TFog::~TFog(){};

TFog::TFog(){};

void TFog::Init()
{
//
};

void TFog::Render()
{
    // przypadek A: gdy aktualny koniec mgly mniejszy niz docelowy
    if (QGlobal::bchangingfoga) { if (Global::fFogEnd < QGlobal::fdestfogend) Global::fFogEnd = Global::fFogEnd + QGlobal::fogchangef; else QGlobal::bchangingfoga = false; }
    // przypadek B: gdy koniec wiekszy niz docelowy
    if (QGlobal::bchangingfogb) { if (Global::fFogEnd > QGlobal::fdestfogend) Global::fFogEnd = Global::fFogEnd - QGlobal::fogchangef; else QGlobal::bchangingfogb = false; }
    if (QGlobal::bchangingfogsa) { if (Global::fFogStart < QGlobal::fdestfogstart) Global::fFogStart += QGlobal::fogchangef; else QGlobal::bchangingfogsa = false; }
    // przypadek B: gdy koniec wiekszy niz docelowy
    if (QGlobal::bchangingfogsb) { if (Global::fFogStart > QGlobal::fdestfogstart) Global::fFogStart -= QGlobal::fogchangef; else QGlobal::bchangingfogsb = false; }

    if (QGlobal::bchangingfoga || QGlobal::bchangingfogb || QGlobal::bchangingfogsa || QGlobal::bchangingfogsb)
      {
        glFogf(GL_FOG_START, Global::fFogStart);
        glFogf(GL_FOG_END, Global::fFogEnd);
      }

};

//---------------------------------------------------------------------------

#pragma package(smart_init)
