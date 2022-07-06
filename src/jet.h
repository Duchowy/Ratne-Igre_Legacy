#ifndef JET_H
#define JET_H
#include "common.h"

struct state{
float x;
float y;
float turn_angle;
float speed;
};
struct state_change{
float turn_speed;
unsigned short speed_mode;
bool rotatable;
bool acceleratable;
};
struct state_change_limit{
struct state_change alter;
float turn_rate;
float speed_rate[2];
float speed_limit[2];
};
struct selection{
unsigned short player_jet;
unsigned short player_gun;
unsigned short player_msl;
unsigned short player_spc;
};
struct Ability
{
    int duration;
    int cooldown;
};



struct JetInst{
struct selection item; //items selected
struct state curr; //position and speed
struct state_change alter; //speed alteration
float target_angle;
unsigned short mode;
int hp;
unsigned short weap_delay[3];
unsigned short weap_ammo[3];
bool will_shoot[3];
bool at_work;
bool status[ENUM_JET_STATUS_FIN];
bool isBot;
struct Ability * ability;
};




struct MslInst{
unsigned short type;
struct state curr;
struct state_change alter;
float target_angle;
short decay;
bool status[ENUM_MSL_STATUS_FIN];
bool isBotLaunched;
};

struct BulInst{
unsigned short type;
struct state curr;
short decay;
unsigned int damage;
int color[3];
};

struct Bullet{
float height;
float width;
short decay;
};


struct Jet{
int hp;
float gun_mult;
float msl_mult;
float spc_mult;
struct state_change_limit alter_limit;
float default_speed;
float hitbox;
bool isBoss;
};



struct Boss{
    bool ability[ENUM_BOSS_ABILITY_FIN];
};






struct Missile{
struct state_change_limit alter_limit;
short decay;
unsigned short ammo_max;
float targeting_angle;
int damage;
int radius;
float default_speed;
};


struct Gun{
unsigned short ammo_max;
unsigned short ammo_type;
unsigned int damage;
unsigned short weap_delay;
float spread;
float speed;
};




void shoot(struct LevelInst * level, struct asset_data * asset);
void target(std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator target);
void target(struct LevelInst * level, struct asset_data * asset);
void action(struct LevelInst * level, struct asset_data * asset);
void decision(std::vector<JetInst> &input_vec, struct asset_data * limit);

float angle_difference(float current, float target);

void gun_init(struct asset_data *);
void msl_init(struct asset_data *);
void bullet_init(struct asset_data *);
void jet_init(struct asset_data * );
void boss_init(struct asset_data * );
void abl_init(struct asset_data * );
JetInst jet_spawn(struct asset_data * asset, struct selection* selected,bool bot);
void enemy_spawn(struct LevelInst * level, struct asset_data * asset);



#endif