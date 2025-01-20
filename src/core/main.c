#include "mathEx.h"
#include "physics.h"
#include "platform.h"
#include "player.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include "uiFramework.h"
#include "window.h"
#include <raylib.h>
#include <stdio.h>

void renderGen(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawRectangle(p->x, p->y, state.screenWidth, 256, GRUV_GREEN);
}

void drawBackground(i32 h) {
    // TODO: use a shader for this

    // WARNING: temp value
    const i32 maxHeight = 10000;
    const i32 height = abs(h);

    const Color init = {164, 221, 219, 255};
    const Color final = {23, 32, 56, 255};

    const f32 r = (f32)height / maxHeight;
    const Color bg = {
        (u8)lerp(init.r, final.r, r),
        (u8)lerp(init.g, final.g, r),
        (u8)lerp(init.b, final.b, r),
        (u8)lerp(init.a, final.a, r),
    };

    DrawRectangle(0, 0, state.screenWidth, state.screenHeight, bg);
}

void cameraFollow(position_c* playerPos) {
    f32 curr_tar = state.camera.target.y;
    f32 tar = playerPos->y - state.screenHeight / 2;
    f32 t = GetFrameTime() * 3;

    state.camera.target = (Vector2){0, lerp(curr_tar, tar, t)};
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

    ecs_entity_t player = PlayerNew();
    position_c* playerPos = ecs_get_mut(state.world, player, position_c);
    PlatformRandom(50);

    ecs_entity_t floor = ecs_entity(state.world, {.name = "floor"});
    ecs_set(state.world, floor, position_c, {0, state.screenHeight - 32});
    ecs_set(state.world, floor, Collider, {state.screenWidth, 32, NULL});
    ecs_set(state.world, floor, Renderable, {0, renderGen});

    while (!WindowShouldClose()) {
        f32 scale = getWindowScale();
        updateScreenMousePos(scale);

        BeginTextureMode(target);
        ClearBackground(BLACK);
        drawBackground(abs((i32)playerPos->y - 240));
        cameraFollow(playerPos);

        BeginMode2D(state.camera);

        ecs_progress(state.world, GetFrameTime());

        EndMode2D();
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        drawScaledWindow(target, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
