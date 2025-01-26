#include "transform.h"
#include "raylib.h"

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(PlayerController);

ECS_SYSTEM_DECLARE(Move);
ECS_SYSTEM_DECLARE(Controller);

void Move(ecs_iter_t* it) {
    Position* p = ecs_field(it, Position, 0);
    const Velocity* v = ecs_field(it, Velocity, 1);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * GetFrameTime();
        p[i].y += v[i].y * GetFrameTime();
    }
}

void Controller(ecs_iter_t* it) {
    Velocity* v = ecs_field(it, Velocity, 1);
    PlayerController* cn = ecs_field(it, PlayerController, 0);
    const f32 speedDef = 100;
    const f32 jumpPower = 200;

    for (int i = 0; i < it->count; i++) {
        v[i].x = 0;
        f32 delta = 1;

        if (!cn[i].on_ground && !cn[i].grappling) {
            v[i].y += 9.8 * 40 * GetFrameTime();
        }

        f32 speed = speedDef;

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            speed *= 1.5;
        }

        if (IsKeyDown(KEY_W) && cn[i].on_ground) {
            v[i].y -= jumpPower * delta;
        }
        if (IsKeyDown(KEY_A)) {
            v[i].x -= speed * delta;
        }
        if (IsKeyDown(KEY_D)) {
            v[i].x += speed * delta;
        }

        cn[i].on_ground = false;
    }
}

void TransformModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, TransformModule);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_COMPONENT_DEFINE(world, PlayerController);
    ECS_SYSTEM_DEFINE(world, Move, EcsOnUpdate, Position, Velocity);
    ECS_SYSTEM_DEFINE(world, Controller, EcsOnUpdate, PlayerController, Velocity);
}
