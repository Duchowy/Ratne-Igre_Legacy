#ifndef PI
#define PI atan(1)*4
#endif


#ifndef LEVEL_H
#define LEVEL_H
#include "common.h"
struct Level{
unsigned short enemy_quality[ENUM_BOSS_TYPE_FIN];
unsigned int map_width;
unsigned int map_height;
short next_level;
bool isBoss;
};

struct RadarNode
{
float dist;
float rad_dist;
short decay;
};


struct RadarInst
{
bool mode;
float range_rad;
float range_dist;
float turn_angle;
float turn_speed;
std::vector<RadarNode> node_q;
};



struct ParticleInst{
    unsigned int type;
    ALLEGRO_COLOR color;
    struct state curr;
    float scale_x;
    float scale_y;
    struct state_change alter;
    int decay;
    bool isDecaying;
    unsigned short flip_img;
};



struct riven
{
bool engaged;
bool dualSided;
uint8_t type[4];
double value[4];
};




struct Player_Data
{
struct selection choice;
struct state_change_limit * custom_stat[ENUM_JET_TYPE_FIN];
struct riven mod[ENUM_JET_TYPE_FIN];
};





struct LevelInst{
unsigned short level_name;
unsigned short enemy_quality[ENUM_BOSS_TYPE_FIN];
std::vector<JetInst> jet_q;
std::vector<ProjInst> proj_q;
std::vector<ParticleInst> prt_q;
std::vector<prompt_screen> prompt_q;
float scale;
struct RadarInst radar;
struct Player_Data player;
short tick;
riven * gift;
bool pauseEngaged;
bool finished;
bool finalPromptEngaged;
};


struct Particle
{
    ALLEGRO_BITMAP * texture;
    int decay;
};





riven * spawn_riven();
state_change_limit process_riven(riven * mod, state_change_limit * base);
void refresh_riven(struct LevelInst * level,struct asset_data * asset);
int level(allegro5_data*,asset_data*,LevelInst*);
float angle_addition(float object, float addition);
float angle_difference(float current, float target);
float distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target);
float rad_distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target);

#endif