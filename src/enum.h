#ifndef ENUM_H
#define ENUM_H

enum JET_TYPE{MIG21,F4,F104,HARRIER,ENUM_JET_TYPE_FIN};
enum BOSS_TYPE{MIG29 = ENUM_JET_TYPE_FIN,SR91,ENUM_BOSS_TYPE_FIN};

enum GUN_TYPE{SHVAK,ADEN,GATLING,ENUM_GUN_TYPE_FIN};
enum MSL_TYPE{INFRARED = ENUM_GUN_TYPE_FIN, RADAR, ENUM_MSL_TYPE_FIN};
enum SPC_TYPE{FLAK = ENUM_MSL_TYPE_FIN, ZUNI, RAILGUN, GUNPOD ,ENUM_LAUNCHER_TYPE_FIN};

enum BULLET_TYPE{SLUG,AIRBURST,ENUM_BULLET_TYPE_FIN};
enum MISSILE_TYPE{IR_M = ENUM_BULLET_TYPE_FIN, RAD_M, UNGUIDED, ENUM_PROJECTILE_TYPE_FIN};

enum ENEMY_MODE{PATROL,PURSUIT,DOGFIGHT,RETREAT};

enum BOSS_ABILITY{RAND_POS,DASH,CMEASURE,ENUM_BOSS_ABILITY_FIN};


enum MSL_STATUS{CONTROLLED,ENUM_MSL_STATUS_FIN};

enum SPEED{AIRBRAKE,STANDARD,AFTERBURNER};

enum LVL_TYPE{BERLIN,INDIA,PFERD,ENUM_LVL_TYPE_FIN};
enum PLVL_TYPE{DNEPR = ENUM_LVL_TYPE_FIN, ATLANTIC, ENUM_BKGR_TYPE_FIN};

enum GMODE{MENU,LVL_SELECTION,EQ_SELECTION,MISSION,MISSION_INIT,QUIT};

enum PRT_TYPE{FLARE, EXPLOSION, EXPLOSION_AIRBURST, SONIC_CONE, ENUM_PRT_TYPE_FIN};
enum NOTEXTURE_PRT_TYPE{JET = ENUM_PRT_TYPE_FIN, PIXEL, ENUM_NOTEXTURE_PRT_TYPE_FIN};
#endif