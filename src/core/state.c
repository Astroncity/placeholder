#include "state.h"

State state = {0};

void init_state(u32 screenWidth, u32 screenHeight) {
    state.globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 512, 0, 0);
    state.world = ecs_init();
    state.mouse = malloc(sizeof(v2));
    state.screenWidth = screenWidth;
    state.screenHeight = screenHeight;
    state.camera = (Camera2D){0};
    state.camera.zoom = 1.0f;
    state.plr_dat = (PlayerData){.lives = 3,
                                 .lives_max = 3,
                                 .score = 0,
                                 .grapp_cool_max = 1.0f,
                                 .grapp_cool = 0.0f};
    state.draw_cursor = true;
}
