#include "engine.h"
#include <math.h>
#include <time.h>

void engine_init(void) {
    init_state(270, 480);
    set_window_flags();
    ECS_IMPORT(state.world, TransformModule);
    ECS_IMPORT(state.world, RendererModule);
    ECS_IMPORT(state.world, Physics);

    srand(time(NULL));
}
