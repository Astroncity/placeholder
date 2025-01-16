#include "flecs.h"
#include "physics.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include "uiFramework.h"
#include "window.h"
#include <raylib.h>
#include <stdio.h>

void renderPlayer(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawRectangle(p->x, p->y, 32, 32, GRUV_AQUA);
}

void renderGen(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawRectangle(p->x, p->y, state.screenWidth, 32, GRUV_GREEN);
}

void resolve_player_collision(ecs_entity_t self, ecs_entity_t other) {
    position_c* p = ecs_get_mut(state.world, self, position_c);
    velocity_c* v = ecs_get_mut(state.world, self, velocity_c);

    p->y -= v->y * GetFrameTime();
    v->y = 0;
    (void)other;
    printf("Collided with something\n");
}

int main(void) {
    initState(270, 480);
    setWindowFlags();
    RenderTexture2D target =
        LoadRenderTexture(state.screenWidth, state.screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    ECS_IMPORT(state.world, TransformModule);
    ECS_IMPORT(state.world, RendererModule);
    ECS_IMPORT(state.world, UIModule);
    ECS_IMPORT(state.world, Physics);

    ecs_entity_t player = ecs_entity(state.world, {.name = "player"});
    ecs_set(state.world, player, position_c, {0, 0});
    ecs_set(state.world, player, velocity_c, {0, 0});
    ecs_add(state.world, player, _controllable);
    ecs_add(state.world, player, _physicsObj);
    ecs_set(state.world, player, Renderable, {1, renderPlayer});
    ecs_set(state.world, player, Collider, {32, 32, resolve_player_collision});

    ecs_entity_t floor = ecs_entity(state.world, {.name = "floor"});
    ecs_set(state.world, floor, position_c, {0, state.screenHeight - 32});
    ecs_set(state.world, floor, velocity_c, {0, 0});
    ecs_set(state.world, floor, Collider, {state.screenWidth, 32, NULL});
    ecs_set(state.world, floor, Renderable, {0, renderGen});

    Texture debugGrid = LoadTexture("assets/images/grid.png");

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        updateScreenMousePos(scale);

        BeginTextureMode(target);
        ClearBackground(BLACK);
        DrawTexture(debugGrid, 0, 0, WHITE);

        ecs_progress(state.world, GetFrameTime());

        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
