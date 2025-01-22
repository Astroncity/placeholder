#include "defs.h"
#include "physics.h"
#include "render.h"
#include "state.h"
#include "transform.h"
#include "window.h"

#define DECLARE_PREFAB(nameStr)                                                     \
    ecs_entity(state.world, {.name = #nameStr, .add = ecs_ids(EcsPrefab)});

void engine_init(void);
