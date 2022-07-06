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

struct LevelInst{
unsigned short level_name;
unsigned short enemy_quality[ENUM_BOSS_TYPE_FIN];
std::vector<BulInst> bullet_q;
std::vector<JetInst> jet_q;
std::vector<MslInst> msl_q;
std::vector<ParticleInst> prt_q;
float scale;

struct JetInst player;
};


struct Particle
{
    int decay;
};





struct ParticleInst{
    unsigned int type;
    ALLEGRO_COLOR color;
    struct state curr;
    struct state_change alter;
    int decay;
    bool isDecaying;
};


int level(allegro5_data*,asset_data*,LevelInst*);
float angle_difference(float current, float target);
float distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target);
float rad_distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target);

#endif