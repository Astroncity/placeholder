#include "engine.h"
#include <math.h>
#include <time.h>

void engine_init(void) {
    init_state(270, 480);
    set_window_flags();
    state.cursor = LoadTexture("assets/images/cursor.png");
    state.cursor_locked = LoadTexture("assets/images/cursor_locked.png");
    ECS_IMPORT(state.world, TransformModule);
    ECS_IMPORT(state.world, RendererModule);
    ECS_IMPORT(state.world, Physics);

    srand(time(NULL));
}
