#include "common.h"
#ifndef MAIN_H
#define MAIN_H



#define FPS 60.0
#define window_width 1000
#define window_height 600


struct asset_data
{
float scale_factor;
ALLEGRO_BITMAP * bkgr_texture [ENUM_BKGR_TYPE_FIN];
ALLEGRO_BITMAP * jet_texture [ENUM_BOSS_TYPE_FIN];
ALLEGRO_BITMAP * bullet_texture [ENUM_BULLET_TYPE_FIN][2];
ALLEGRO_BITMAP * msl_texture [ENUM_MSL_TYPE_FIN];
struct Boss boss_data[ENUM_BOSS_TYPE_FIN - ENUM_JET_TYPE_FIN];
struct Ability abl_data[BOSS_ABILITY::ENUM_BOSS_ABILITY_FIN];
struct Jet jet_data[ENUM_BOSS_TYPE_FIN];
struct Gun gun_data[ENUM_GUN_TYPE_FIN];
struct Bullet bul_data[ENUM_BULLET_TYPE_FIN];
struct Missile msl_data[ENUM_MSL_TYPE_FIN];
struct Level lvl_data[ENUM_BKGR_TYPE_FIN];
};



#endif