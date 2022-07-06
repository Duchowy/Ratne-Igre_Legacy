#ifndef LOAD_LEVEL_H
#define LOAD_LEVEL_H
#include "common.h"

struct curr_selection
{
struct selection item;
unsigned short selection;
};



struct button_scroll
{
    int x;
    int y;
    int width;
    int height;
    std::string name;
    std::string desc;
};


int level_select(struct LevelInst *, struct asset_data *,allegro5_data*);
int spawn_level(struct asset_data * asset,struct LevelInst * level);
void level_init(asset_data * asset);


#endif