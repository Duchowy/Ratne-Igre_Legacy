#ifndef ALLEG_5
#define ALLEG_5
#include <SFML/Graphics.hpp>


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
#include<array>
#include <omp.h>
#include "enum.h"
#endif

#ifndef COMMON_H
#define COMMON_H

struct box_string
{
    int x;
    int y;
    int width;
    int height;
    std::string name;
    std::string desc;
};


struct prompt_screen{
unsigned short type;
struct box_string body;
short decay;
bool F_Action;
bool Z_Action;
};

#endif