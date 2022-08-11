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
float target_angle;
bool rotatable;
bool acceleratable;
};
struct state_change_limit{
struct state_change alter;
float turn_rate;
float speed_rate[2]; //0 for brake action, 1 for acceleration
float speed_limit[2];
float default_speed;
float mobility_coef;
};





struct Projectile
{
unsigned short decay;
unsigned short damage;
float velocity;
state_change_limit alter_limit;
float radius;
struct
{
    float targeting_angle;
    float draw_width;
    float draw_height;
    bool hitCircular; //if 0, in a 60 deg cone
    bool isAOE;
    bool DMGfall;
} trait;
};

struct Launcher
{
unsigned short decay;
unsigned short damage;
float velocity;
unsigned short cooldown;
unsigned short replenish_cooldown;
unsigned short ammo;
unsigned short magazine;
float spread;
struct Projectile * projectile;
};


struct ProjInst
{
    unsigned short type;
    unsigned short decay;
    unsigned short damage;
    struct state curr;
    struct state_change * alter;
    bool status[ENUM_MSL_STATUS_FIN];
    ALLEGRO_COLOR color;
    struct Launcher * launcher;
    bool isBotLaunched;
    int target;
};

struct LaunInst
{
    bool engaged;
    unsigned short type;
    unsigned short ammo;
    unsigned short magazine;
    unsigned short cooldown;
    unsigned short replenish_cooldown;
    struct Launcher * launcher;
};








struct selection{
unsigned short player_jet;
unsigned short weapon[3];
};


struct Ability
{
    int duration;
    int cooldown;
};



struct JetInst{
unsigned short ID;
unsigned short type;
int hp;
struct state curr; //position and speed
struct state_change alter; //speed alteration
unsigned short mode;

struct LaunInst weapon[3];
bool at_work;
bool status[ENUM_JET_STATUS_FIN];
bool isBot;
int botTarget;
struct Ability * ability;
struct state_change_limit * overwrite_limit;
};



struct Jet{
int hp;
float weapon_mult[3];
struct state_change_limit alter_limit;
float hitbox;
bool isBoss;
};



struct Boss{
    bool ability[ENUM_BOSS_ABILITY_FIN];
};





void shoot(struct LevelInst * level, struct asset_data * asset);
void target(std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator target,float offset);
void target(struct LevelInst * level, struct asset_data * asset);
void action(struct LevelInst * level, struct asset_data * asset);
void decision(std::vector<JetInst> &input_vec, struct asset_data * limit);

float angle_difference(float current, float target);
void launcher_init(struct asset_data * asset);
void projectile_init(struct asset_data * asset);
void jet_init(struct asset_data * );
void boss_init(struct asset_data * );
void abl_init(struct asset_data * );
std::vector<JetInst>::iterator findJet(std::vector<JetInst> & input_vec, int ID);
JetInst jet_spawn(struct asset_data * asset, struct selection* selected,struct state_change_limit * overwrite,bool bot,unsigned short ID);
void enemy_spawn(struct LevelInst * level, struct asset_data * asset);



#endif