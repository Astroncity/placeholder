#include "mathEx.h"
#include <math.h>

f32 lerp(f32 a, f32 b, f32 t) { return a + (b - a) * t; }
v2 v2Lerp(v2 a, v2 b, f32 t) { return (v2){lerp(a.x, b.x, t), lerp(a.y, b.y, t)}; }
f32 v2Dist(v2 a, v2 b) {
    return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}
