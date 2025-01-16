#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct State {
    ecs_world_t* world;
    Font globalFont;
    v2* mouse;
    u32 screenWidth;
    u32 screenHeight;
} State;

extern State state;

void initState(u32 screenWidth, u32 screenHeight);
