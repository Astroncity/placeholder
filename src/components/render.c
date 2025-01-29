#include "render.h"
#include "state.h"

ECS_COMPONENT_DECLARE(Renderable);
ECS_COMPONENT_DECLARE(RenderableStatic);
ECS_SYSTEM_DECLARE(render_s);
ECS_SYSTEM_DECLARE(renderStaticSystem);
ECS_SYSTEM_DECLARE(startCamera);
ECS_SYSTEM_DECLARE(endCamera);

ecs_entity_t OnCamera;
ecs_entity_t PostCamera;

static void render(ecs_iter_t* it) {

    const Renderable* s = ecs_field(it, Renderable, 0);

    for (int i = 0; i < it->count; i++) {
        s[i].render(it->entities[i]);
    }
}

static inline int compare(ecs_entity_t e1, const void* ptr1, ecs_entity_t e2,
                          const void* ptr2) {
    (void)e1;
    (void)e2;
    const Renderable* r1 = ptr1;
    const Renderable* r2 = ptr2;

    return r1->renderLayer - r2->renderLayer;
}

static void startCamera(ecs_iter_t* it) {
    (void)it;
    BeginMode2D(state.camera);
}

static void endCamera(ecs_iter_t* it) {
    (void)it;
    EndMode2D();
}

void RendererModuleImport(ecs_world_t* world) {
    ECS_MODULE(world, RendererModule);
    ECS_COMPONENT_DEFINE(world, Renderable);
    ECS_COMPONENT_DEFINE(world, RenderableStatic);

    OnCamera = ecs_new_w_id(world, EcsPhase);
    ecs_add_pair(world, OnCamera, EcsDependsOn, EcsOnUpdate);
    ECS_SYSTEM_DEFINE(world, startCamera, OnCamera, Renderable);

    PostCamera = ecs_new_w_id(world, EcsPhase);
    ecs_add_pair(world, PostCamera, EcsDependsOn, OnCamera);
    ECS_SYSTEM_DEFINE(world, endCamera, PostCamera, Renderable);

    ecs_entity_t render_s = ecs_system(
        world,
        {.entity = ecs_entity(world,
                              {.name = "render_s", // Name of the system
                               .add = ecs_ids(ecs_dependson(OnCamera))}),
         .query.terms =
             {
                 {.id = ecs_id(Renderable)} // Filter for entities with Renderable
             },
         .callback = render,
         .query.order_by = ecs_id(Renderable),
         .query.order_by_callback = compare});

    ecs_entity_t RenderStaticSystem = ecs_system(
        world,
        {.entity = ecs_entity(world,
                              {.name = "RenderStaticSystem", // Name of the system
                               .add = ecs_ids(ecs_dependson(PostCamera))}),
         .query.terms =
             {
                 {.id = ecs_id(
                      RenderableStatic)} // Filter for entities with Renderable
             },
         .callback = render,
         .query.order_by = ecs_id(RenderableStatic),
         .query.order_by_callback = compare});

    (void)render_s;
    (void)RenderStaticSystem;
}
