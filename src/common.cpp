#include "common.h"


sf::Vector2f operator/(const sf::Vector2f & vec, double num)
{
    return sf::Vector2f(vec.x/num,vec.y/num);
}



sf::Vector2u operator/(const sf::Vector2u & vec, int num)
{
    return sf::Vector2u(vec.x/num,vec.y/num);
}

