#include "uiFramework.h"
#include "defs.h"
#include "raylib.h"
#include "render.h"
#include "state.h"
#include "transform.h"

#define RENDER_PRIORITY 100
#define UI_MAIN_COLOR GRUV_GREEN

ECS_COMPONENT_DECLARE(label_c);

typedef struct {
    usize size;
    i32 maxLen;
    i32 minLen;
} textbox_c;
ECS_COMPONENT_DECLARE(textbox_c);

void render_label(ecs_entity_t e) {
    const label_c* l = ecs_get(state.world, e, label_c);
    const Position* pos = ecs_get(state.world, e, Position);
    i32 iconOffset = 0;

    if (IsTextureValid(l->icon)) {
        DrawTexture(l->icon, pos->x + l->offset.x,
                    pos->y + l->offset.y - l->icon.height / 2.0, WHITE);
        iconOffset = l->icon.width * 1.2f;
    }

    i32 yoff = MeasureTextEx(state.globalFont, l->text, l->fontSize, 1).y / 2;

    DrawTextEx(state.globalFont, l->text,
               (v2){pos->x + l->offset.x + iconOffset, pos->y + l->offset.y - yoff},
               l->fontSize, 1, WHITE);
}
void render_textbox(ecs_entity_t e) {
    const Position* pos = ecs_get(state.world, e, Position);
    const textbox_c* box = ecs_get(state.world, e, textbox_c);

    const f32 width = MAX(box->maxLen, box->minLen);

    DrawRectangleRounded((Rect){pos->x, pos->y, width, 20 * box->size}, 0.3, 2,
                         UI_MAIN_COLOR);
}
textbox_e create_textbox(const char* title, v2 pos) {
    textbox_e e = ecs_new(state.world);
    ecs_set(state.world, e, Position, {pos.x, pos.y});
    ecs_set(state.world, e, RenderableStatic, {RENDER_PRIORITY, render_textbox});
    ecs_set(state.world, e, textbox_c, {.size = 0, .maxLen = 0, .minLen = 100});

    TextboxPush(e, title, 20, (Texture2D){});
    TextboxPush(e, "", 20, (Texture2D){});
    return e;
}

ecs_entity_t TextboxPush(textbox_e e, const char* text, f32 fontSize,
                         Texture2D icon) {
    const Position* boxPos = ecs_get(state.world, e, Position);
    u32 priority = ecs_get(state.world, e, RenderableStatic)->renderLayer + 1;
    textbox_c* box = ecs_get_mut(state.world, e, textbox_c);

    const v2 measure = MeasureTextEx(state.globalFont, text, fontSize, 1);

    const i16 padx = 5;
    i32 pady = measure.y / 1.7;

    box->maxLen = MAX(box->maxLen, measure.x + padx + icon.width * (2.5f) + 10);

    ecs_entity_t label = ecs_entity(state.world, {.parent = e});
    ecs_set(state.world, label, label_c,
            {.text = text,
             .offset = {padx, pady + 5},
             .icon = icon,
             .fontSize = fontSize});
    ecs_set(state.world, label, Position,
            {boxPos->x, boxPos->y + (pady * 2 * box->size)});
    ecs_set(state.world, label, RenderableStatic, {priority, render_label});
    ++box->size;

    return label;
}

void UIModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, UIModule);
    ECS_COMPONENT_DEFINE(world, label_c);
    ECS_COMPONENT_DEFINE(world, textbox_c);
}
