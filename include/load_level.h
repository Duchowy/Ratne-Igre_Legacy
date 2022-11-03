#ifndef LOAD_LEVEL_H
#define LOAD_LEVEL_H
#include "common.h"

int eq_select(struct LevelInst *, struct asset_data *,allegro5_data*);
int spawn_level(struct asset_data * asset,struct LevelInst * level);
void destroy_level(asset_data * asset, LevelInst * level);
void level_init(asset_data * asset);


#endif