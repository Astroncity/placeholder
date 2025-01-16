#pragma once

#include "defs.h"

#define MAG(v) sqrt(v.x* v.x + v.y * v.y)

void updateScreenMousePos(f32 scale);
void drawScaledWindow(RenderTexture2D target, f32 scale);
v2 v2Clamp(v2 vec, v2 min, v2 max);
void setWindowFlags(void);
f32 getWindowScale();
