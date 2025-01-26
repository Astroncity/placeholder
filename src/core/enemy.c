#include "enemy.h"
#include "state.h"

ECS_TAG_DECLARE(enemy);

void enemies_init(void) { ECS_TAG_DEFINE(state.world, enemy); }
