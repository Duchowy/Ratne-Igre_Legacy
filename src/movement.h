#ifndef MOVEMENT_H
#define MOVEMENT_H
void move(std::vector<missile> &input_vec, struct lvl_dat * limit);
void move(std::vector<bullet> &input_vec, struct lvl_dat * limit);
void move(std::vector<jet> &input_vec, struct lvl_dat * limit);
void rotation(std::vector<jet> &input_vec);
void rotation(std::vector<missile> &input_vec);
#endif