#pragma once

#include "defs.h"

#define MAG(v) sqrt(v.x* v.x + v.y * v.y)

void update_mouse(f32 scale);
void draw_window(RenderTexture2D target, f32 scale);
v2 v2Clamp(v2 vec, v2 min, v2 max);
void set_window_flags(void);
f32 get_window_scale();
