#include "dragonfly.h"
#include "engine.h"
#include <math.h>

#define SCREEN_BUF 20

static Texture2D sprite;
static Texture2D spriteRev;
static ecs_entity_t prefab;
static bool ready = false;

typedef struct dragonfly_c {
    i32 initHeight;
} dragonfly_c;

ECS_COMPONENT_DECLARE(dragonfly_c);

static void render(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    const velocity_c* v = ecs_get(state.world, e, velocity_c);

    Texture2D* s = &sprite;

    if (v->x < 0) {
        s = &spriteRev;
    }

    DrawTexture(*s, p->x, p->y, WHITE);
}

void dragonfly_update(ecs_iter_t* it) {
    position_c* p = ecs_field(it, position_c, 0);
    velocity_c* v = ecs_field(it, velocity_c, 1);
    const dragonfly_c* d = ecs_field(it, dragonfly_c, 2);

    if (p->x < -SCREEN_BUF || p->x > state.screenWidth + SCREEN_BUF) {
        v->x *= -1;
    }

    p->y = d->initHeight + sin(GetTime() * 5) * 5;
}

void init_dragonfly(void) {
    sprite = LoadTexture("assets/images/dragonfly.png");
    spriteRev = LoadTexture("assets/images/dragonflyRev.png");

    prefab = DECLARE_PREFAB("Dragonfly");
    ECS_COMPONENT_DEFINE(state.world, dragonfly_c);
    ECS_SYSTEM(state.world, dragonfly_update,
               EcsOnUpdate, [out] transform.module.position_c,
               [out] transform.module.velocity_c, [in] dragonfly_c);

    ecs_set(state.world, prefab, Renderable, {0, render});
    ecs_set(state.world, prefab, Collider, {24, 16, NULL});
    ready = true;
}

ecs_entity_t DragonflyNew(i32 x, i32 y) {
    if (!ready) {
        init_dragonfly();
    }

    const f32 vel = GetRandomValue(500, 500) / 10.0;

    ecs_entity_t e = ecs_new_w_pair(state.world, EcsIsA, prefab);
    ecs_set(state.world, e, position_c, {x, y});
    ecs_set(state.world, e, velocity_c, {vel, 0});
    ecs_set(state.world, e, dragonfly_c, {y});

    return e;
}
