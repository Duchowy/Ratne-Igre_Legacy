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
    ALLEGRO_COLOR color;
    struct Launcher * launcher;
};

struct LaunInst
{
    bool engaged;
    unsigned short ammo;
    unsigned short magazine;
    unsigned short cooldown;
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
struct state_change_limit * overwrite_limit;
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
ALLEGRO_COLOR color;
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
void target(std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator target,float offset);
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