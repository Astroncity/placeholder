#pragma once
#include "defs.h"
#include "flecs.h"

typedef v2 Position, Velocity;

typedef struct {
    bool on_ground;
    bool grappling;
} PlayerController;

extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(PlayerController);
extern ECS_SYSTEM_DECLARE(Move);
extern ECS_SYSTEM_DECLARE(Controller);

void TransformModuleImport(ecs_world_t* world);
