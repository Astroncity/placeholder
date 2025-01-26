#include "enemy.h"
#include "engine.h"
#include "mathEx.h"
#include "platform.h"
#include "player.h"
#include "uiFramework.h"
#include <raylib.h>
#include <stdio.h>

Texture2D bottomPanel;

void render_general(ecs_entity_t e) {
    const position_c* p = ecs_get(state.world, e, position_c);
    DrawRectangle(p->x, p->y, state.screenWidth, 256, GRUV_GREEN);
}

void draw_background(i32 h) {
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

void camera_follow(position_c* playerPos) {
    f32 curr_tar = state.camera.target.y;
    f32 tar = playerPos->y - state.screenHeight / 2;
    f32 t = GetFrameTime() * 3;

    state.camera.target = (Vector2){0, lerp(curr_tar, tar, t)};
}

void draw_ui() {}

int main(void) {
    engine_init();
    enemies_init();
    ECS_IMPORT(state.world, UIModule);
    RenderTexture2D target =
        LoadRenderTexture(state.screenWidth, state.screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    ecs_entity_t player = PlayerNew();
    position_c* playerPos = ecs_get_mut(state.world, player, position_c);
    PlatformRandom(50);
    DragonFlyRandom(50);
    DragonflyNew(100, 20);

    ecs_entity_t floor = ecs_entity(state.world, {.name = "floor"});
    ecs_set(state.world, floor, position_c, {0, state.screenHeight - 32});
    ecs_set(state.world, floor, Collider, {state.screenWidth, 32, NULL});
    ecs_set(state.world, floor, Renderable, {0, render_general});

    while (!WindowShouldClose()) {
        f32 scale = get_window_scale();
        update_mouse(scale);

        if (IsKeyPressed(KEY_F1)) {
            printf("==== Mouse Debug ====\n");
            printf("    Mouse (Screen): (%f, %f)\n", state.mouse->x, state.mouse->y);
            v2 real_mouse = GetScreenToWorld2D(*state.mouse, state.camera);
            printf("    Mouse (World): (%f, %f)\n", real_mouse.x, real_mouse.y);
        }

        BeginTextureMode(target);
        ClearBackground(BLACK);
        draw_background(abs((i32)playerPos->y - 240));
        DrawFPS(0, 0);
        camera_follow(playerPos);

        ecs_progress(state.world, GetFrameTime());

        EndMode2D();
        EndTextureMode();
        BeginDrawing();
        ClearBackground(BLACK);
        draw_window(target, scale);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
