#include "main.h"
#include "jet.h"
#include "level.h"
#include "movement.h"


void move(std::vector<missile> &input_vec, struct lvl_dat * limit)
{
    for(std::vector<missile>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
    {
        object->x += cos(object->curr_angle)*object->speed[1];
        if(object->x < 0) object->x = 0;
        if(object->x > limit->map_width) object->x = limit->map_width;
        object->y += sin(object->curr_angle)*object->speed[1];
        if(object->y < 0) object->y = 0;
        if(object->y > limit->map_height) object->y = limit->map_height;

        if(object->speed[1] + object->speed_a < object->speed[0]) object->speed[1] += object->speed_a;
        else object->speed[1] = object->speed[0];
    }
}

void move(std::vector<bullet> &input_vec, struct lvl_dat * limit)
{
    #pragma omp parallel for
    for(std::vector<bullet>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
    {
        object->x += cos(object->angle)*object->speed;
        if(object->x < 0) object->x = 0;
        if(object->x > limit->map_width) object->x = limit->map_width;
        object->y += sin(object->angle)*object->speed;
        if(object->y < 0) object->y = 0;
        if(object->y > limit->map_height) object->y = limit->map_height;
    }
}

void move(std::vector<jet> &input_vec, struct lvl_dat * limit)
{
    #pragma omp parallel for
for(std::vector<jet>::iterator object = input_vec.begin(); object != input_vec.end(); object++) //input_vec.end()
{
    object->x += cos(object->curr_angle)*object->speed[object->speed_mode];
    if(object->x < 0) object->x = 0;
    if(object->x > limit->map_width) object->x = limit->map_width;
    object->y += sin(object->curr_angle)*object->speed[object->speed_mode];
    if(object->y < 0) object->y = 0;
    if(object->y > limit->map_height) object->y = limit->map_height;
}
}

void rotation(std::vector<jet> &input_vec)
{
    for(std::vector<jet>::iterator object = input_vec.begin(); object != input_vec.end(); object++) //input_vec.end()
    {
    float angle_diff = angle_difference(object->curr_angle,object->target_angle);

    float road = 0.5 * pow(object->curr_angle_w,2) / object->angle_a;


    if(fabs(angle_diff)>road) //outer scope
    { //rough targeting, altering radial velocity
        if(angle_diff >= 0) 
        {
            if(object->curr_angle_w > -object->max_angle_w) object->curr_angle_w -= object->angle_a;
        }
        else if(object->curr_angle_w < object->max_angle_w) object->curr_angle_w += object->angle_a;
        //angle alteration part
        object->curr_angle += object->curr_angle_w;
        if(object->curr_angle > PI) object->curr_angle = object->curr_angle - 2*PI;
        if(object->curr_angle < -PI) object->curr_angle = object->curr_angle + 2*PI;
    }
    else //inner scope
    { //slowing down
     if(fabs(angle_diff)>fabs(object->curr_angle_w))
     {
         if(angle_diff >= 0)
                {
               if(object->curr_angle_w < object->max_angle_w) object->curr_angle_w += object->angle_a;
                }
        else if (object->curr_angle_w > -object->max_angle_w) object->curr_angle_w -= object->angle_a;
            object->curr_angle += object->curr_angle_w;
      }
      else //recently added
      {
      object->curr_angle = object->target_angle;
      object->curr_angle_w = 0;
      }
    }
    }
}

void rotation(std::vector<missile> &input_vec)
{
    for(std::vector<missile>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
    {
    float angle_diff = angle_difference(object->curr_angle,object->target_angle);

    float road = 0.5 * pow(object->curr_angle_w,2) / object->angle_a;


    if(fabs(angle_diff)>road) //outer scope
    { //rough targeting, altering radial velocity
        if(angle_diff >= 0) 
        {
            if(object->curr_angle_w > -object->max_angle_w) object->curr_angle_w -= object->angle_a;
        }
        else if(object->curr_angle_w < object->max_angle_w) object->curr_angle_w += object->angle_a;
        //angle alteration part
        object->curr_angle += object->curr_angle_w;
        if(object->curr_angle > PI) object->curr_angle = object->curr_angle - 2*PI;
        if(object->curr_angle < -PI) object->curr_angle = object->curr_angle + 2*PI;
    }
    else //inner scope
    { //slowing down
     if(fabs(angle_diff)>fabs(object->curr_angle_w))
     {
         if(angle_diff >= 0)
                {
               if(object->curr_angle_w < object->max_angle_w) object->curr_angle_w += object->angle_a;
                }
        else if (object->curr_angle_w > -object->max_angle_w) object->curr_angle_w -= object->angle_a;
            object->curr_angle += object->curr_angle_w;
      }
      else //recently added
      {
      object->curr_angle = object->target_angle;
      object->curr_angle_w = 0;
      }
    }
    }
}