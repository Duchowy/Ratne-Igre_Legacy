#ifndef PI
#define PI atan(1)*4
#endif


#ifndef LEVEL_H
#define LEVEL_H
struct lvl_dat{
unsigned short level_name;
unsigned short enemy_amount;
unsigned short enemy_quality[2][ENUM_JET_TYPE_FIN]; //[0][X] for reference, [1][x] for editing
float scale;
unsigned int map_width;
unsigned int map_height;
struct gun gun_data[ENUM_GUN_TYPE_FIN];
struct missile msl_data[ENUM_MSL_TYPE_FIN];
struct jet player;
};

int level(allegro5_data*,asset_data*,lvl_dat*);
float angle_difference(float current, float target);
float distance(std::vector<jet>::iterator current, std::vector<jet>::iterator target);
float rad_distance(std::vector<jet>::iterator current, std::vector<jet>::iterator target);

#endif