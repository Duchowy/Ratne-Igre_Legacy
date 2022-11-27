#ifndef SFML
#define SFML
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"



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

struct Button{
    std::vector<sf::RectangleShape> box; //the first one is always theme
    std::vector<sf::Text> text; //the first one is always a name

};


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


sf::Vector2u operator/(const sf::Vector2u & vec, int num);
sf::Vector2f operator/(const sf::Vector2f & vec, double num);








#endif