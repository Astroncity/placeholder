#include "dragonfly.h"
#include "enemy.h"
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
    const Position* p = ecs_get(state.world, e, Position);
    const Velocity* v = ecs_get(state.world, e, Velocity);

    Texture2D* s = &sprite;

    if (v->x < 0) {
        s = &spriteRev;
    }

    DrawTexture(*s, p->x, p->y, WHITE);
}

static void update(ecs_iter_t* it) {
    Position* p = ecs_field(it, Position, 0);
    Velocity* v = ecs_field(it, Velocity, 1);
    const dragonfly_c* d = ecs_field(it, dragonfly_c, 2);

    for (i32 i = 0; i < it->count; i++) {

        if (p[i].x < -SCREEN_BUF || p[i].x > state.screenWidth + SCREEN_BUF) {
            v[i].x *= -1;
        }

        p[i].y = d[i].initHeight + sinf(GetTime() * 5) * 7;
    }
}

static void init(void) {
    sprite = LoadTexture("assets/images/dragonfly.png");
    spriteRev = LoadTexture("assets/images/dragonflyRev.png");

    prefab = DECLARE_PREFAB("Dragonfly");
    ECS_COMPONENT_DEFINE(state.world, dragonfly_c);
    ECS_SYSTEM(state.world, update, EcsOnUpdate, [out] transform.module.Position,
               [out] transform.module.Velocity, [in] dragonfly_c);

    ecs_set(state.world, prefab, Renderable, {0, render});
    ecs_set(state.world, prefab, Collider, {24, 16, NULL});
    ecs_add(state.world, prefab, enemy);
    ready = true;
}

void DragonFlyRandom(usize n) {
    i32 left_bound = sprite.width;
    i32 right_bound = state.screenWidth - sprite.width;
    i32 offset = (sprite.height * -2.5) + 250;

    for (usize i = 0; i < n; i++) {
        i32 x = GetRandomValue(left_bound, right_bound);
        i32 y = offset;
        offset -= sprite.height * 2.2 + GetRandomValue(0, 50);
        DragonflyNew(x, y);
    }
}

ecs_entity_t DragonflyNew(i32 x, i32 y) {
    if (!ready) {
        init();
    }

    const f32 vel = GetRandomValue(300, 700) / 10.0;

    ecs_entity_t e = ecs_new_w_pair(state.world, EcsIsA, prefab);
    ecs_set(state.world, e, Position, {x, y});
    ecs_set(state.world, e, Velocity, {vel, 0});
    ecs_set(state.world, e, dragonfly_c, {y});

    return e;
}
