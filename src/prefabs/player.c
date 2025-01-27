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
#define LOCK_RADIUS 32

bool ready = false;
Position* player_pos;
Velocity* player_vel;
PlayerController* player_cn;

Position* mouse_collider_pos;
ecs_entity_t mouse_obj_over = 0;

const f32 base_grapple_speed = 200;
v2 grapple_point = {0, 0};
v2 locked_cursor = {0, 0};
f32 grapple_speed = base_grapple_speed;

void handle_mouse_collision(ecs_entity_t self, ecs_entity_t other) {
    (void)self;

    if (ecs_has(state.world, other, enemy)) {
        mouse_obj_over = other;
    }
}

void on_grapple_finish() {
    player_cn->grappling = false;
    grapple_point = (v2){0, 0};
    grapple_speed = base_grapple_speed;

    player_vel->x /= 2;
    player_vel->y /= 2;
}

void pre_grapple() {
    v2 m = GetScreenToWorld2D(*state.mouse, state.camera);
    *mouse_collider_pos = (v2){m.x - LOCK_RADIUS / 2, m.y - LOCK_RADIUS / 2};

    state.playerData.grapple_cooldown -= GetFrameTime();
    if (state.playerData.grapple_cooldown < 0) {
        state.playerData.grapple_cooldown = 0;
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

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player_cn->grappling) {
        grapple_point = real_pos;
        player_cn->grappling = true;
        state.playerData.grapple_cooldown = state.playerData.grapple_cooldown_max;
    } else {
        mouse_obj_over = 0;
    }
}

void during_grapple() {
    const v2* obj_pos = ecs_get(state.world, mouse_obj_over, Position);
    grapple_point = *obj_pos;

    f32 angle = atan2f(grapple_point.y - player_pos->y - 8,
                       grapple_point.x - player_pos->x - 12);

    player_vel->x = cosf(angle) * grapple_speed * GetFrameTime() * 60;
    player_vel->y = sinf(angle) * grapple_speed * GetFrameTime() * 60;

    f32 dist = v2Dist((v2){player_pos->x + 12, player_pos->y + 8}, grapple_point);

    if (dist < 5 || grapple_speed >= 1000) {
        on_grapple_finish();
    }

    grapple_speed *= 1.05;
}

void handle_grapple() {
    if (player_cn->grappling) {
        during_grapple();
    } else {
        pre_grapple();
    }
}

void init_player(void) {
    ECS_SYSTEM(state.world, handle_grapple, OnCamera);

    ecs_entity_t mc = ecs_entity(state.world, {.name = "mouse_collider"});
    ecs_set(state.world, mc, Position, {0, 0});
    ecs_set(state.world, mc, Collider,
            {LOCK_RADIUS, LOCK_RADIUS, handle_mouse_collision});
    mouse_collider_pos = ecs_get_mut(state.world, mc, Position);

    ready = true;
}

void resolve_player_collision(ecs_entity_t self, ecs_entity_t other) {
    Velocity* v = ecs_get_mut(state.world, self, Velocity);

    if (v->y <= 0) {
        return;
    }

    PlayerController* cn = ecs_get_mut(state.world, self, PlayerController);

    if (ecs_get_name(state.world, other), "floor") {
        cn->on_ground = true;
    }

    v->y = 0;
    (void)other;
}

void handle_debug(void) {
    if (IsKeyPressed(KEY_F1)) {
        printf("==== Player Debug ====\n");
        printf("    Pos: (%f, %f)\n", player_pos->x, player_pos->y);
        printf("    Vel: (%f, %f)\n", player_vel->x, player_vel->y);
    }
}

void render_player(ecs_entity_t e) {
    Position* p = player_pos;
    (void)e;
    DrawRectangle(p->x, p->y, 24, 16, GRUV_AQUA);
    DrawCircleLines(p->x + 12, p->y + 8, GRAPPLE_RADIUS, GRUV_AQUA);

    if (player_cn->grappling) {

        DrawLineEx((v2){player_pos->x + 12, player_pos->y + 8}, grapple_point, 3,
                   BLACK);
    }

    handle_debug();
}

ecs_entity_t PlayerNew(void) {
    if (!ready) {
        init_player();
    }

    ecs_entity_t player = ecs_entity(state.world, {.name = "player"});
    ecs_set(state.world, player, Position, {state.screenWidth / 2 - 16, 350});
    ecs_set(state.world, player, Velocity, {0, 0});
    ecs_set(state.world, player, PlayerController, {false});
    ecs_add(state.world, player, _physicsObj);
    ecs_set(state.world, player, Renderable, {1, render_player});
    ecs_set(state.world, player, Collider, {24, 16, resolve_player_collision});

    player_pos = ecs_get_mut(state.world, player, Position);
    player_vel = ecs_get_mut(state.world, player, Velocity);
    player_cn = ecs_get_mut(state.world, player, PlayerController);
    return player;
}
