#include "common.h"
#ifndef MAIN_H
#define MAIN_H



#define default_window_width 1000
#define default_window_height 600

struct config_data
{

unsigned int default_display_width;
unsigned int default_display_height;
bool particlesEnabled;
bool verticalSyncEnabled;
unsigned short MSAA;
bool oglEnabled;
bool autoUIscale;
float UIscale; 
float zoomUpperLimit;
float zoomLowerLimit;
short radarType;
bool additionalRadar;
float FPS;
float fadeDistance;
float fadingLength;
};




struct asset_data
{
float scale_factor;
struct config_data config;
sf::Texture bkgr_texture [ENUM_BKGR_TYPE_FIN];
sf::Texture jet_texture [ENUM_BOSS_TYPE_FIN];
sf::Texture proj_texture [ENUM_PROJECTILE_TYPE_FIN];
sf::Texture prt_texture[ENUM_PRT_TYPE_FIN];
sf::Texture ui_texture[1];
sf::Font font;

struct Boss boss_data[ENUM_BOSS_TYPE_FIN - ENUM_JET_TYPE_FIN];
struct Ability abl_data[BOSS_ABILITY::ENUM_BOSS_ABILITY_FIN];
struct Jet jet_data[ENUM_BOSS_TYPE_FIN];



struct Projectile proj_data[ENUM_PROJECTILE_TYPE_FIN];
struct Launcher laun_data[ENUM_LAUNCHER_TYPE_FIN];


struct Level lvl_data[ENUM_BKGR_TYPE_FIN];
struct Particle prt_data[ENUM_PRT_TYPE_FIN];
};


float calculateUIscale(float width, float height);
#endif