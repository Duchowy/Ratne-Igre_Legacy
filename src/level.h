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
};

struct RadarNode
{
float dist;
float rad_dist;
short decay;
};


struct RadarInst
{
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


struct LevelInst{
unsigned short level_name;
unsigned short enemy_quality[ENUM_BOSS_TYPE_FIN];
std::vector<BulInst> bullet_q;
std::vector<JetInst> jet_q;
std::vector<MslInst> msl_q;
std::vector<ParticleInst> prt_q;
float scale;
struct RadarInst radar;
struct JetInst player;
};


struct Particle
{
    ALLEGRO_BITMAP * texture;
    int decay;
};






int level(allegro5_data*,asset_data*,LevelInst*);
float angle_addition(float object, float addition);
float angle_difference(float current, float target);
float distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target);
float rad_distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target);

#endif