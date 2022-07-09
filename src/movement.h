#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "common.h"
void dash(std::vector<JetInst>::iterator object,unsigned int map_width, unsigned int map_height);
void transform(struct LevelInst * , struct asset_data * );
void randomize_position(std::vector<JetInst>::iterator, asset_data *, LevelInst * );
void move(struct state * pos, unsigned int map_width, unsigned int map_height, int);
#endif