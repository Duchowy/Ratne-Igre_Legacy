#ifndef RENDER_LEVEL_H
#define RENDER_LEVEL_H
#include "common.h"



void debug_data(struct LevelInst * level, struct asset_data * asset, ALLEGRO_FONT * font);
void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset, struct allegro5_data * alleg5);
void draw_ui(struct LevelInst * level, struct asset_data * asset, struct allegro5_data * alleg5);
void zoom(asset_data * assets, short direction);

std::string extract_riven(riven * mod, unsigned short trait);

void spawn_prompt_screen(asset_data * asset, allegro5_data * alleg5, LevelInst * level, unsigned short type);
void update_prompt_screen(allegro5_data * alleg5, LevelInst * level);
void destroy_prompt_screen(asset_data * asset, LevelInst * level, std::vector<prompt_screen>::iterator object, bool decision);

#endif