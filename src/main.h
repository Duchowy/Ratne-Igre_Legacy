#include "common.h"
#ifndef MAIN_H
#define MAIN_H



#define default_window_width 1000
#define default_window_height 600

struct config_data
{
float scaleUI; 
unsigned int default_display_width;
unsigned int default_display_height;
bool particlesEnabled;
bool verticalSyncEnabled;
unsigned short MSAA;
bool oglEnabled;
float FPS;
float fadeDistance;
float fadingLength;
};




struct asset_data
{
float scale_factor;
struct config_data config;
ALLEGRO_BITMAP * bkgr_texture [ENUM_BKGR_TYPE_FIN];
ALLEGRO_BITMAP * jet_texture [ENUM_BOSS_TYPE_FIN];
ALLEGRO_BITMAP * proj_texture [ENUM_PROJECTILE_TYPE_FIN];
ALLEGRO_BITMAP * prt_texture[ENUM_PRT_TYPE_FIN];
ALLEGRO_BITMAP * ui_texture[1];

struct Boss boss_data[ENUM_BOSS_TYPE_FIN - ENUM_JET_TYPE_FIN];
struct Ability abl_data[BOSS_ABILITY::ENUM_BOSS_ABILITY_FIN];
struct Jet jet_data[ENUM_BOSS_TYPE_FIN];



struct Projectile proj_data[ENUM_PROJECTILE_TYPE_FIN];
struct Launcher laun_data[ENUM_LAUNCHER_TYPE_FIN];


struct Level lvl_data[ENUM_BKGR_TYPE_FIN];
struct Particle prt_data[ENUM_PRT_TYPE_FIN];
};



#endif