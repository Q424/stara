/*
This Source Code Form is subject to the
terms of the Mozilla Public License, v.
2.0. If a copy of the MPL was not
distributed with this file, You can
obtain one at
http://mozilla.org/MPL/2.0/.
*/

#ifndef effects2dH
#define effects2dH
#include <system.hpp>
#include "model3d.h"

void generatenoisetex();
void drawNoise(int bnoise, float alpha);
void DrawCircle(float cx, float cy, float r, Color4 color);
void DrawCircle(float cx, float cy, float r, int segments);
void Disk( float x, float y, float r );
void DrawRoundRectF( int x, int y, int width, int height, int radius, int resolution, Color4 color);
void DrawRoundRectB( float x, float y, float width, float height, float radius, Color4 color);
//---------------------------------------------------------------------------
#endif
