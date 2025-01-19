#include "state.h"

State state = {0};

void initState(u32 screenWidth, u32 screenHeight) {
    state.globalFont = LoadFontEx("assets/fonts/spaceMono.ttf", 512, 0, 0);
    state.world = ecs_init();
    state.mouse = malloc(sizeof(v2));
    state.screenWidth = screenWidth;
    state.screenHeight = screenHeight;
    state.camera = (Camera2D){0};
    state.camera.zoom = 1.0f;
}
