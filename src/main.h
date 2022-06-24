#ifndef ALLEG_5
#define ALLEG_5
#include<allegro5/allegro5.h>
#include<allegro5/allegro_primitives.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_image.h>
#endif

#ifndef LIBS
#define LIBS
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string>
#include<iostream>
#include<time.h>
#include<queue>
#include<vector>
#include<algorithm>
#include <omp.h>
#endif

#ifndef MAIN_H
#define MAIN_H

#include "enum.h"

#define FPS 60.0
#define window_width 1000
#define window_height 600
enum GMODE{MENU,SELECTION,MISSION,QUIT};

struct allegro5_data
{
ALLEGRO_DISPLAY * display;
ALLEGRO_EVENT event;
ALLEGRO_EVENT_QUEUE * queue;
ALLEGRO_TIMER * timer;
ALLEGRO_FONT * font;
};

struct asset_data
{
float scale_factor;
ALLEGRO_BITMAP * bkgr_texture [ENUM_BKGR_TYPE_FIN];
ALLEGRO_BITMAP * jet_texture [ENUM_JET_TYPE_FIN];
ALLEGRO_BITMAP * bullet_texture [ENUM_BULLET_TYPE_FIN][2];
ALLEGRO_BITMAP * msl_texture [ENUM_MSL_TYPE_FIN];
};



#endif