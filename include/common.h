#ifndef SFML
#define SFML
#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>



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
    std::vector<sf::RectangleShape> box;
    std::vector<sf::Text> text;

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


sf::Vector2u operator/(sf::Vector2u & vec, float num);
sf::Vector2f operator/(sf::Vector2f & vec, float num);


struct Button
{
std::vector<sf::RectangleShape> box; //the first one is always theme
std::vector<sf::Text> text; //the first one is always a name
};

#endif