#include "player.h"
#include "flecs.h"
#include "physics.h"
#include "render.h"
#include "state.h"
#include "transform.h"

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

void renderPlayer(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawRectangle(p->x, p->y, 32, 32, GRUV_AQUA);
}

ecs_entity_t PlayerNew(void) {
    ecs_entity_t player = ecs_entity(state.world, {.name = "player"});
    ecs_set(state.world, player, position_c, {0, 0});
    ecs_set(state.world, player, velocity_c, {0, 0});
    ecs_set(state.world, player, PlayerController, {false});
    ecs_add(state.world, player, _physicsObj);
    ecs_set(state.world, player, Renderable, {1, renderPlayer});
    ecs_set(state.world, player, Collider, {32, 32, resolve_player_collision});
    return player;
}
