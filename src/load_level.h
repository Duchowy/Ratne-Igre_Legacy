#ifndef LOAD_LEVEL_H
#define LOAD_LEVEL_H
#include<array>

struct curr_selection
{
unsigned short player_jet;
unsigned short player_gun;
unsigned short player_rkt;
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


int level_select(struct lvl_dat *, struct asset_data *,allegro5_data*);




#endif