#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "common.h"
void transform(struct LevelInst * , struct asset_data * );
void randomize_position(std::vector<JetInst>::iterator, asset_data *, LevelInst * );
void move(struct state * pos, unsigned int map_width, unsigned int map_height, int);
#endif