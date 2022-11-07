#include "common.h"


sf::Vector2f operator/(sf::Vector2f & vec, float num)
{
    return sf::Vector2f(vec.x/num,vec.y/num);
}



sf::Vector2u operator/(sf::Vector2u & vec, float num)
{
    return sf::Vector2u(vec.x/num,vec.y/num);
}