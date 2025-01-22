#include "platform.h"
#include "engine.h"

static ecs_entity_t prefab;
static Texture2D sprite;
static bool ready = false;

const i32 width = 64;
const i32 height = 16;

void render_tile(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawTexture(sprite, p->x, p->y, WHITE);
}

void init_platform(void) {
    // TODO: Ensure dependent modules are loaded

    prefab = DECLARE_PREFAB("Platform");

    ecs_set(state.world, prefab, Renderable, {0, render_tile});
    ecs_set(state.world, prefab, Collider, {width, height, NULL});
    sprite = LoadTexture("assets/images/platform.png");
    ready = true;
}

void PlatformNew(i32 x, i32 y) {
    if (!ready) {
        init_platform();
    }

    ecs_entity_t e = ecs_new_w_pair(state.world, EcsIsA, prefab);
    ecs_set(state.world, e, position_c, {x, y});
}

void PlatformRandom(u32 n) {
    const i32 offset = height * 3.5;
    const i32 leftBound = width;
    const i32 rightBound = state.screenWidth - width;

    for (u32 i = 0; i < n; i++) {
        i32 x = GetRandomValue(leftBound, rightBound);
        i32 y = i * -offset + 480;
        PlatformNew(x, y);
    }
}
