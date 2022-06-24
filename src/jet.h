#ifndef JET_H
#define JET_H
#include "enum.h"

struct jet{
unsigned short type;
//location, movement
float x;
float y;
float curr_angle;
float target_angle;
float max_angle_w;
float curr_angle_w;
float angle_a;
float speed[3];
short speed_mode;
//endof
bool will_shoot[3];
unsigned short weap[3];
unsigned short weap_delay[3];
//[x][0] for cooldown limit, [x][1] for modification
unsigned short weap_ammo[3][2];
int hp[2];
//unsigned short ability[ENUM_JET_ABILITY_FIN][2];
//unsigned short status[ENUM_JET_STATUS_FIN][2]; //65535 for unlimited
//endof
unsigned short hitbox;
unsigned short mode;
bool at_work;
};


struct missile{
int type;
float x;
float y;
float curr_angle;
float target_angle;
float max_angle_w;
float curr_angle_w;
float angle_a;
float speed[2];
float speed_a;
float targeting_angle;
short decay[2]; //[x][0] for cooldown limit (reference value), [x][1] for modified
unsigned short ammo_max;
int damage;
int radius;
};

struct gun{
unsigned short ammo_max;
unsigned short ammo_type;
unsigned int damage;
unsigned short weap_delay;
float spread;
float speed;
};


struct bullet{
unsigned short type;
float x;
float y;
float angle;
float speed;
short decay[2]; //[x][0] for cooldown limit, [x][1] for modified
unsigned int damage;
float height;
float width;
int color[3];
};

void shoot(std::vector<jet> &input_vec, std::vector<missile> &series, struct lvl_dat * lvl);
void shoot(std::vector<jet> &input_vec, std::vector<bullet> &series, struct lvl_dat * lvl);
void target(std::vector<jet>::iterator object, std::vector<jet>::iterator target);
void target(std::vector<jet> &input_vec, std::vector<missile> &shell_vec);
void action(std::vector<jet> &input_vec, std::vector<bullet> &shell_vec, struct lvl_dat * limit);
void decision(std::vector<jet> &input_vec, struct lvl_dat * limit);

float angle_difference(float current, float target);

void gun_init(struct lvl_dat*);
void msl_init(struct lvl_dat*);
void bullet_init(struct bullet *, unsigned short);
void jet_init(struct jet *,struct lvl_dat*,unsigned short,bool);
void enemy_init(std::vector<jet> &object, struct lvl_dat *);



#endif