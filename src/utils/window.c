#include "window.h"
#include "state.h"

v2 v2Clamp(v2 vec, v2 min, v2 max) {
    return (v2){MIN(MAX(vec.x, min.x), max.x), MIN(MAX(vec.y, min.y), max.y)};
}

void updateScreenMousePos(f32 scale) {
    v2 mOld = GetMousePosition();
    f32 sw = (f32)state.screenWidth;
    f32 sh = (f32)state.screenHeight;

    state.mouse->x = (mOld.x - (GetScreenWidth() - (sw * scale)) * 0.5f) / scale;
    state.mouse->y = (mOld.y - (GetScreenHeight() - (sh * scale)) * 0.5f) / scale;
    *state.mouse = v2Clamp(*state.mouse, (v2){0, 0}, (v2){(f32)sw, (f32)sh});
}

void drawScaledWindow(RenderTexture2D target, f32 scale) {
    f32 tw = (f32)target.texture.width;
    f32 th = (f32)target.texture.height;
    f32 ssh = state.screenHeight * scale;
    f32 ssw = state.screenWidth * scale;

    Rect rect1 = {0.0f, 0.0f, tw, -th};
    f32 x = (GetScreenWidth() - ssw) * 0.5f;
    f32 y = (GetScreenHeight() - ssh) * 0.5f;

    Rect rect2 = {x, y, ssw, ssh};

    DrawTexturePro(target.texture, rect1, rect2, (v2){0, 0}, 0.0f, WHITE);
}

void validateState(void) {
    assert(state.screenWidth > 0 && "Screen width must be greater than 0");
    assert(state.screenHeight > 0 && "Screen height must be greater than 0");
    assert(state.world != NULL && "State not initialized");
    assert(state.mouse != NULL && "State not initialized");
}

void setWindowFlags(void) {
    validateState();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(state.screenWidth, state.screenHeight, "Planet Generation Test");
    InitAudioDevice();
    SetMasterVolume(1);
    SetTargetFPS(60);
    SetWindowSize(state.screenWidth * 2, state.screenHeight * 2);
}

f32 getWindowScale() {
    return MIN((f32)GetScreenWidth() / state.screenWidth,
               (float)GetScreenHeight() / state.screenHeight);
    return MIN((f32)GetScreenWidth() / state.screenWidth,
               (float)GetScreenHeight() / state.screenHeight);
}
