#include "common.h"

void Vector2f::move(Vector2f & offset)
{
this->x += offset.x;
this->y += offset.y;
}

void Vector2f::setPosition(Vector2f & position)
{
*this = position;
}


Vector2f Vector2f::operator+(Vector2f & vector)
{
    Vector2f obj;
    obj.x = this->x + vector.x;
    obj.y = this->y + vector.y;
    return obj;
}

Vector2f Vector2f::operator-(Vector2f & vector)
{
    Vector2f obj;
    obj.x = this->x - vector.x;
    obj.y = this->y - vector.y;
    return obj;
}