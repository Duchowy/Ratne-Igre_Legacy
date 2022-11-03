#ifndef MOVEMENT_H
#define MOVEMENT_H
#include "common.h"
float movement_coef_calculate(state_change_limit * limit, float speed);
void dash(std::vector<JetInst>::iterator object,unsigned int map_width, unsigned int map_height);
void transform(struct state * pos,unsigned int map_width, unsigned int map_height,float distance, float angle);
void transform(struct LevelInst * , struct asset_data * );
void randomize_position(std::vector<JetInst>::iterator, asset_data *, LevelInst * );
void accelerate(struct state * pos, struct state_change * alt , state_change_limit * limit, float modification);
void move(struct state * pos, unsigned int map_width, unsigned int map_height, int);
#endif