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
#define MAX_GRAPPLE_SPEED 50

static bool ready = false;
static Position* pos;
static Velocity* vel;
static PlayerController* controller;

static Position* mouse_collider_pos;
static ecs_entity_t mouse_obj_over = 0;

static const f32 base_grapple_speed = 1;
static v2 grapple_point = {0, 0};
static v2 locked_cursor = {0, 0};
static f32 grapple_speed = base_grapple_speed;

static i32 set_mouse_obj(ecs_entity_t e) {
    if (controller->grappling) return -1;
    mouse_obj_over = e;
    return 0;
}

static void handle_mouse_collision(ecs_entity_t self, ecs_entity_t other) {
    (void)self;

    if (ecs_has(state.world, other, enemy)) {
        set_mouse_obj(other);
    }
}

static void on_grapple_finish(void) {
    controller->grappling = false;
    grapple_point = (v2){0, 0};
    grapple_speed = base_grapple_speed;
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
    f32 dt = GetFrameTime();

    *pos = v2Lerp(*pos, grapple_point, grapple_speed * dt);

    if (grapple_speed > MAX_GRAPPLE_SPEED) {
        grapple_speed = MAX_GRAPPLE_SPEED;
    }

    f32 dist = v2Dist((v2){pos->x + 12, pos->y + 8}, grapple_point);

    printf("grapple_speed: %f\n", grapple_speed);
    printf("dist: %f\n", dist);
    if (dist < 20) on_grapple_finish();

    grapple_speed += 25 * dt;
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

static void handle_enemy_collision(ecs_entity_t other) {
    if (ecs_has(state.world, other, enemy)) {
        ecs_delete(state.world, other);

        controller->grappling = false;

        if (!controller->grappling && state.plr_dat.lives > 0) {
            state.plr_dat.lives--;
        }
    }
}

static void handle_ground_collision(const Position* p) {
    if (controller->grappling) return;
    controller->on_ground = true;
    f32 dist = p->y - pos->y;
    f32 dist_alt = p->y - (pos->y - vel->y); // buffer for fast moving player

    if (dist >= 14 || dist_alt >= 14) {
        vel->y = 0;
    }
}

static void onCollision(ecs_entity_t self, ecs_entity_t other) {
    (void)self;
    const Position* fp = ecs_get(state.world, other, Position);

    if (vel->y <= 0) {
        return;
    }

    if (ecs_has(state.world, other, enemy)) {
        handle_enemy_collision(other);
    }

    if (ecs_has(state.world, other, _ground)) {
        handle_ground_collision(fp);
    }
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
