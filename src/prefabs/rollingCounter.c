#include "rollingCounter.h"
#include "mathEx.h"
#include "render.h"
#include "state.h"
#include "transform.h"

ECS_COMPONENT_DECLARE(RollingCounter);

#define NUMBER_W 13
#define NUMBER_H 20
#define COUNTER_MAX 9999

static Texture2D numbers;
static bool ready = false;

static void init(void) {
    numbers = LoadTexture("assets/images/numbers.png");
    ECS_COMPONENT_DEFINE(state.world, RollingCounter);
    ready = true;
}

static inline u16 digit_to_height(u16 digit) { return (NUMBER_H * (10 - digit)); }

static void get_digits(u16 v, u16* digits) {
    for (u8 i = 0; i < 4; i++) {
        digits[i] = v % 10;
        v /= 10;
    }
}

static void render(ecs_entity_t self) {
    const Position* p = ecs_get(state.world, self, Position);
    RollingCounter* rc = ecs_get_mut(state.world, self, RollingCounter);

    u16 v = rc->value;
    if (v > COUNTER_MAX) {
        v = COUNTER_MAX;
    }

    u16 digits[4];
    get_digits(v, digits);

    for (u8 i = 0; i < 2; i++) {
        u16 temp = digits[i];
        digits[i] = digits[3 - i];
        digits[3 - i] = temp;
    }

    for (u8 i = 0; i < 4; i++) {
        u16 h_target = digit_to_height(digits[i]);
        rc->digit_masks[i] = lerp(rc->digit_masks[i], h_target, 0.2);

        if (abs(rc->digit_masks[i] - h_target) < 5) {
            rc->digit_masks[i] = h_target;
        }

        u16 y = rc->digit_masks[i] - NUMBER_H;

        Rectangle src = {0, y, NUMBER_W, rc->digit_masks[i]};
        Rectangle dst = {p->x + (i * NUMBER_W + i * 3), p->y, NUMBER_W, NUMBER_H};
        DrawTextureRec(numbers, src, (v2){dst.x, dst.y}, WHITE);
    }
}

ecs_entity_t RollingCounterNew(v2 pos) {
    if (!ready) {
        init();
    }

    ecs_entity_t e = ecs_new(state.world);
    ecs_set(state.world, e, Position, {pos.x, pos.y});

    u16 h = digit_to_height(0);
    ecs_set(state.world, e, RollingCounter, {0, {h, h, h, h}});

    ecs_set(state.world, e, RenderableStatic, {10, render});

    return e;
}
