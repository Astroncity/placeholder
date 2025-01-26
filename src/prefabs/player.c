#include "player.h"
#include "flecs.h"
#include "mathEx.h"
#include "physics.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include <math.h>

#define GRAPPLE_RADIUS 50

bool ready = false;
v2 grapple_point = {0, 0};
position_c* player_pos;
velocity_c* player_vel;
PlayerController* player_cn;

void handle_grapple(); // NOTE: For system definition

void init_player(void) {
    ECS_SYSTEM(state.world, handle_grapple, EcsOnUpdate);
    ready = true;
}

void resolve_player_collision(ecs_entity_t self, ecs_entity_t other) {
    velocity_c* v = ecs_get_mut(state.world, self, velocity_c);

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

void on_grapple_finish() {
    player_cn->grappling = false;
    grapple_point = (v2){0, 0};
}

void pre_grapple() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !player_cn->grappling) {
        grapple_point = GetScreenToWorld2D(*state.mouse, state.camera);
        player_cn->grappling = true;
    }
}

void during_grapple() {
    printf("Grappling to %f, %f\n", grapple_point.x, grapple_point.y);
    static const f32 grapple_speed = 5;
    f32 angle = atan2f(grapple_point.y - player_pos->y - 8,
                       grapple_point.x - player_pos->x - 12);

    player_vel->x = cosf(angle) * grapple_speed * GetFrameTime() * 60;
    player_vel->y = sinf(angle) * grapple_speed * GetFrameTime() * 60;

    if (v2Dist((v2){player_pos->x + 12, player_pos->y + 8}, grapple_point) < 5) {
        on_grapple_finish();
    }
    return;
}

void handle_grapple() {
    if (player_cn->grappling) {
        during_grapple();
    } else {
        pre_grapple();
    }
}

void handle_debug(void) {
    if (IsKeyPressed(KEY_F1)) {
        printf("==== Player Debug ====\n");
        printf("    Pos: (%f, %f)\n", player_pos->x, player_pos->y);
        printf("    Vel: (%f, %f)\n", player_vel->x, player_vel->y);
    }
}

void render_player(ecs_entity_t e) {
    position_c* p = player_pos;
    (void)e;
    DrawRectangle(p->x, p->y, 24, 16, GRUV_AQUA);
    DrawCircleLines(p->x + 12, p->y + 8, GRAPPLE_RADIUS, GRUV_AQUA);

    if (player_cn->grappling) {

        DrawLineEx((v2){player_pos->x + 12, player_pos->y + 8},
                   (v2){grapple_point.x, grapple_point.y}, 3, BLACK);
    }

    handle_debug();
}

ecs_entity_t PlayerNew(void) {
    if (!ready) {
        init_player();
    }

    ecs_entity_t player = ecs_entity(state.world, {.name = "player"});
    ecs_set(state.world, player, position_c, {state.screenWidth / 2 - 16, 350});
    ecs_set(state.world, player, velocity_c, {0, 0});
    ecs_set(state.world, player, PlayerController, {false});
    ecs_add(state.world, player, _physicsObj);
    ecs_set(state.world, player, Renderable, {1, render_player});
    ecs_set(state.world, player, Collider, {24, 16, resolve_player_collision});

    player_pos = ecs_get_mut(state.world, player, position_c);
    player_vel = ecs_get_mut(state.world, player, velocity_c);
    player_cn = ecs_get_mut(state.world, player, PlayerController);
    return player;
}
