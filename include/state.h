#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct PlayerData {
    u32 lives;
    u32 lives_max;
    u32 score;
    f32 grapp_cool_max;
    f32 grapp_cool;
} PlayerData;

typedef struct State {
    ecs_world_t* world;
    Camera2D camera;
    Font globalFont;
    v2* mouse;
    u32 screenWidth;
    u32 screenHeight;
    PlayerData plr_dat;
    Texture2D cursor;
    Texture2D cursor_locked;
    bool draw_cursor;
} State;

extern State state;

void init_state(u32 screenWidth, u32 screenHeight);
