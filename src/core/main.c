#include "enemy.h"
#include "engine.h"
#include "mathEx.h"
#include "platform.h"
#include "player.h"
#include "uiFramework.h"
#include <raylib.h>
#include <stdio.h>

Texture2D grapple_cooldown;

void render_general(ecs_entity_t e) {
    const Position* p = ecs_get(state.world, e, Position);
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

void camera_follow(Position* playerPos) {
    f32 curr_tar = state.camera.target.y;
    f32 tar = playerPos->y - state.screenHeight / 2;
    f32 t = GetFrameTime() * 3;

    state.camera.target = (Vector2){0, lerp(curr_tar, tar, t)};
}

void init_ui(void) {
    grapple_cooldown = LoadTexture("assets/images/grapple_cooldown.png");
    return;
}

void draw_ui() {
    i32 h = 96;
    Rect bottom = {0, state.screenHeight - h, state.screenWidth, h};
    DrawRectangleRounded(bottom, 0.25, 10, WHITE);

    // cooldown  NOTE: should be extracted

    f32 r =
        state.playerData.grapple_cooldown / state.playerData.grapple_cooldown_max;

    f32 len = grapple_cooldown.width * r;

    DrawTexture(grapple_cooldown, 100, state.screenHeight - h + 40, WHITE);
    DrawRectangle(100, state.screenHeight - h + 40, len, 32,
                  (Color){255, 255, 255, 128});
}

void draw_cursor(void) {
    if (!state.draw_cursor) {
        return;
    }

    v2 loc = {state.mouse->x - 16, state.mouse->y - 16};
    DrawTextureV(state.cursor, loc, WHITE);
}

int main(void) {
    engine_init();
    enemies_init();
    init_ui();
    ECS_IMPORT(state.world, UIModule);
    RenderTexture2D target =
        LoadRenderTexture(state.screenWidth, state.screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    HideCursor();

    ecs_entity_t player = PlayerNew();
    Position* playerPos = ecs_get_mut(state.world, player, Position);
    PlatformRandom(50);
    DragonFlyRandom(150);
    DragonflyNew(100, 30);
    DragonflyNew(150, 50);
    DragonflyNew(200, 80);

    ecs_entity_t floor = ecs_entity(state.world, {.name = "floor"});
    ecs_set(state.world, floor, Position, {0, state.screenHeight - 32});
    ecs_set(state.world, floor, Collider, {state.screenWidth, 32, NULL});
    ecs_set(state.world, floor, Renderable, {0, render_general});

    ECS_SYSTEM(state.world, draw_ui, PostCamera);

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
        draw_cursor();
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
