#include "defs.h"
#include "flecs.h"

typedef struct RollingCounter {
    u16 value;
    u16 digit_masks[4];
} RollingCounter;

extern ECS_COMPONENT_DECLARE(RollingCounter);

ecs_entity_t RollingCounterNew(v2 pos);
