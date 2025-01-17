#include "physics.h"
#include "state.h"
#include "transform.h"

ECS_TAG_DECLARE(_physicsObj);
ECS_COMPONENT_DECLARE(Collider);
ECS_SYSTEM_DECLARE(PhysicsUpdate);
ECS_SYSTEM_DECLARE(ResolveCollisions);

void PhysicsUpdate(ecs_iter_t* it) { (void)it; }

void ResolveCollisions(ecs_iter_t* it) {
    i32 n = ecs_query_count(it->query).entities;
    i32 proc = 0;
    const Collider* colBuf[n];
    const position_c* posBuf[n];
    ecs_entity_t eBuf[n];

    while (ecs_query_next(it)) {
        const Collider* col = ecs_field(it, Collider, 0);
        const position_c* pos = ecs_field(it, position_c, 1);

        for (int i = 0; i < it->count; i++) {
            posBuf[proc] = &pos[i];
            colBuf[proc] = &col[i];
            eBuf[proc] = it->entities[i];
            proc++;
        }
    }

    for (int i = 0; i < n; i++) {
        if (colBuf[i]->onCollision == NULL) {
            continue;
        }

        Rect e1 = {posBuf[i]->x, posBuf[i]->y, colBuf[i]->width, colBuf[i]->height};
        for (int j = 0; j < n; j++) {
            if (i == j) {
                continue;
            }

            Rect e2 = {posBuf[j]->x, posBuf[j]->y, colBuf[j]->width,
                       colBuf[j]->height};

            if (CheckCollisionRecs(e1, e2)) {
                colBuf[i]->onCollision(eBuf[i], eBuf[j]);
            }
        }
    }
}

void PhysicsImport(ecs_world_t* world) {
    ECS_MODULE(world, Physics);
    ECS_TAG_DEFINE(world, _physicsObj);
    ECS_COMPONENT_DEFINE(world, Collider);
    ECS_SYSTEM_DEFINE(world, PhysicsUpdate, EcsPreUpdate,
                      _physicsObj, [out] transform.module.velocity_c);

    ecs_entity_t cols = ecs_system(
        world,
        {.entity = ecs_entity(world,
                              {.name = "ResolveCollisions", // Name of the system
                               .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
         .query.terms = {{.id = ecs_id(Collider), .inout = EcsIn, .src.id = EcsThis},
                         {.id = ecs_id(position_c),
                          .inout = EcsIn,
                          .src.id = EcsThis}},
         .run = ResolveCollisions});
    (void)cols;
}
