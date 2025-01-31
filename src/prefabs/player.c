#include "player.h"
#include "enemy.h"
#include "flecs.h"
#include "mathEx.h"
#include "physics.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include <math.h>

#define GRAPPLE_RADIUS 50
#define LOCK_RADIUS 48

static bool ready = false;
static Position* pos;
static Velocity* vel;
static PlayerController* controller;

static Position* mouse_collider_pos;
static ecs_entity_t mouse_obj_over = 0;

static const f32 base_grapple_speed = 200;
static v2 grapple_point = {0, 0};
static v2 locked_cursor = {0, 0};
static f32 grapple_speed = base_grapple_speed;

static void handle_mouse_collision(ecs_entity_t self, ecs_entity_t other) {
    (void)self;

    if (ecs_has(state.world, other, enemy)) {
        mouse_obj_over = other;
    }
}

static void on_grapple_finish(void) {
    controller->grappling = false;
    grapple_point = (v2){0, 0};
    grapple_speed = base_grapple_speed;

    vel->x /= 2;
    vel->y /= 2;
}

static void pre_grapple(void) {
    v2 m = GetScreenToWorld2D(*state.mouse, state.camera);
    *mouse_collider_pos = (v2){m.x - LOCK_RADIUS / 2, m.y - LOCK_RADIUS / 2};

    state.plr_dat.grapp_cool -= GetFrameTime();
    if (state.plr_dat.grapp_cool < 0) {
        state.plr_dat.grapp_cool = 0;
    }

    if (mouse_obj_over == 0 || !ecs_is_valid(state.world, mouse_obj_over)) {
        locked_cursor = v2Lerp(locked_cursor, m, 0.2);
        return;
    }

    const Position* p = ecs_get(state.world, mouse_obj_over, Position);
    v2 real_pos = {p->x + 12, p->y + 8};

    locked_cursor = v2Lerp(locked_cursor, real_pos, 0.2);
    DrawTexture(state.cursor_locked, locked_cursor.x - 16, locked_cursor.y - 16,
                WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !controller->grappling) {
        grapple_point = real_pos;
        controller->grappling = true;
        state.plr_dat.grapp_cool = state.plr_dat.grapp_cool_max;
    } else {
        mouse_obj_over = 0;
    }
}

static void during_grapple(void) {
    const v2* obj_pos = ecs_get(state.world, mouse_obj_over, Position);
    grapple_point = *obj_pos;

    f32 angle = atan2f(grapple_point.y - pos->y - 8, grapple_point.x - pos->x - 12);

    vel->x = cosf(angle) * grapple_speed * GetFrameTime() * 60;
    vel->y = sinf(angle) * grapple_speed * GetFrameTime() * 60;

    f32 dist = v2Dist((v2){pos->x + 12, pos->y + 8}, grapple_point);

    if (dist < 5 || grapple_speed >= 1000) {
        on_grapple_finish();
    }

    grapple_speed *= 1.05;
}

static void handle_grapple() {
    if (controller->grappling) {
        during_grapple();
    } else {
        pre_grapple();
    }
}

static void init(void) {
    ECS_SYSTEM(state.world, handle_grapple, OnCamera, 0);

    ecs_entity_t mc = ecs_entity(state.world, {.name = "mouse_collider"});
    ecs_set(state.world, mc, Position, {0, 0});
    ecs_set(state.world, mc, Collider,
            {LOCK_RADIUS, LOCK_RADIUS, handle_mouse_collision});
    mouse_collider_pos = ecs_get_mut(state.world, mc, Position);

    ready = true;
}

static void onCollision(ecs_entity_t self, ecs_entity_t other) {
    const Position* fp = ecs_get(state.world, other, Position);

    if (vel->y <= 0) {
        return;
    }

    PlayerController* cn = ecs_get_mut(state.world, self, PlayerController);

    if (ecs_has(state.world, other, _ground)) {
        cn->on_ground = true;

        if (pos->y + 16 > fp->y) {
            vel->y = 0;
        }
    }

    (void)other;
}

static void print_debug(void) {
    if (IsKeyPressed(KEY_F1)) {
        printf("==== Player Debug ====\n");
        printf("    Pos: (%f, %f)\n", pos->x, pos->y);
        printf("    Vel: (%f, %f)\n", vel->x, vel->y);
    }
}

static void render(ecs_entity_t e) {
    Position* p = pos;
    (void)e;
    DrawRectangle(p->x, p->y, 24, 16, GRUV_AQUA);
    DrawCircleLines(p->x + 12, p->y + 8, GRAPPLE_RADIUS, GRUV_AQUA);

    if (controller->grappling) {

        DrawLineEx((v2){pos->x + 12, pos->y + 8}, grapple_point, 3, BLACK);
    }

    print_debug();
}

ecs_entity_t PlayerNew(void) {
    if (!ready) {
        init();
    }

    ecs_entity_t player = ecs_entity(state.world, {.name = "player"});
    ecs_set(state.world, player, Position, {state.screenWidth / 2 - 16, 350});
    ecs_set(state.world, player, Velocity, {0, 0});
    ecs_set(state.world, player, PlayerController, {false});
    ecs_add(state.world, player, _physicsObj);
    ecs_set(state.world, player, Renderable, {1, render});
    ecs_set(state.world, player, Collider, {24, 16, onCollision});

    pos = ecs_get_mut(state.world, player, Position);
    vel = ecs_get_mut(state.world, player, Velocity);
    controller = ecs_get_mut(state.world, player, PlayerController);
    return player;
}
