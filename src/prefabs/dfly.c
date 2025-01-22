#include "dfly.h"
#include "engine.h"

static Texture2D sprite;
static ecs_entity_t prefab;
static bool ready = false;

static void render(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawTexture(sprite, p->x, p->y, WHITE);
}

void init_dragonfly(void) {
    sprite = LoadTexture("assets/images/dragonfly.png");

    prefab = DECLARE_PREFAB("Dragonfly");
    ecs_set(state.world, prefab, Renderable, {0, render});
    ecs_set(state.world, prefab, Collider, {24, 16, NULL});
    ready = true;
}

ecs_entity_t DragonflyNew(i32 x, i32 y) {
    if (!ready) {
        init_dragonfly();
    }

    ecs_entity_t e = ecs_new_w_pair(state.world, EcsIsA, prefab);
    ecs_set(state.world, e, position_c, {x, y});
    return e;
}
