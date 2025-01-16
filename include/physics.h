#pragma once
#include "defs.h"
#include "flecs.h"

typedef struct Collider {
    i32 width;
    i32 height;
    void (*onCollision)(ecs_entity_t self, ecs_entity_t other);
} Collider;

extern ECS_TAG_DECLARE(_physicsObj);
extern ECS_COMPONENT_DECLARE(Collider);
extern ECS_SYSTEM_DECLARE(PhysicsUpdate);
extern ECS_SYSTEM_DECLARE(ResolveCollisions);

void PhysicsImport(ecs_world_t* world);
