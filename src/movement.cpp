#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"

void dash(std::vector<JetInst>::iterator object,unsigned int map_width, unsigned int map_height)
{
        std::cout << "dashing\n";
        object->curr.x += cos(object->curr.turn_angle)*10;
        if(object->curr.x < 0) object->curr.x = 0;
        if(object->curr.x > map_width) object->curr.x = map_width;
        object->curr.y += sin(object->curr.turn_angle)*10;
        if(object->curr.y < 0) object->curr.y = 0;
        if(object->curr.y > map_height) object->curr.y = map_height;
}




void move(struct state * pos, unsigned int map_width, unsigned int map_height)
{
        pos->x += cos(pos->turn_angle)*pos->speed;
        if(pos->x < 0) pos->x = 0;
        if(pos->x > map_width) pos->x = map_width;
        pos->y += sin(pos->turn_angle)*pos->speed;
        if(pos->y < 0) pos->y = 0;
        if(pos->y > map_height) pos->y = map_height;
}

void advance(struct state * pos, struct state_change * alt, state_change_limit * limit, float target_angle)
{
// 0 brake, 1 stay, 2 accelerate
switch(alt->speed_mode)
{
    
    case 0:
    {
        if(pos->speed - limit->speed_rate[0] > limit->speed_limit[0]) pos->speed -= limit->speed_rate[0];
        else pos->speed = limit->speed_limit[0];
    }
    break;
    case 1: break;
    case 2:
    {
        if(pos->speed + limit->speed_rate[1] < limit->speed_limit[1]) pos->speed += limit->speed_rate[1];
        else pos->speed = limit->speed_limit[1];
    }
    break;
}

//rotation section

    float angle_diff = angle_difference(pos->turn_angle,target_angle);
    float road = 0.5 * pow(alt->turn_speed,2) / limit->turn_rate; 

    if(fabs(angle_diff)>road) //outer scope
    { //rough targeting, altering radial velocity
        if(angle_diff >= 0) 
        {
            if(alt->turn_speed > -limit->alter.turn_speed) alt->turn_speed -= limit->turn_rate;
        }
        else if(alt->turn_speed < limit->alter.turn_speed) alt->turn_speed += limit->turn_rate;
        //angle alteration part
        pos->turn_angle += alt->turn_speed;
        if(pos->turn_angle > PI) pos->turn_angle = pos->turn_angle - 2*PI;
        if(pos->turn_angle < -PI) pos->turn_angle = pos->turn_angle + 2*PI;
    }
    else //inner scope
    { //slowing down
     if(fabs(angle_diff)>fabs(alt->turn_speed))
     {
         if(angle_diff >= 0)
                {
               if(alt->turn_speed < limit->alter.turn_speed) alt->turn_speed += limit->turn_rate;
                }
        else if (alt->turn_speed > -limit->alter.turn_speed) alt->turn_speed -= limit->turn_rate;
            pos->turn_angle += alt->turn_speed;
            if(pos->turn_angle > PI) pos->turn_angle = pos->turn_angle - 2*PI;
            if(pos->turn_angle < -PI) pos->turn_angle = pos->turn_angle + 2*PI;
      }
      else //recently added
      {
      pos->turn_angle = target_angle;
      alt->turn_speed = 0;
      }
    }








}


void transform(struct LevelInst * data, struct asset_data * asset)
{


    #pragma omp parallel for
    for(std::vector<JetInst>::iterator object = data->jet_q.begin(); object != data->jet_q.end(); object++)
    {
        if(asset->jet_data[object->item.player_jet].isBoss && object->ability[BOSS_ABILITY::DASH].duration) dash(object,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height);

        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height);
        advance(&object->curr,&object->alter, &asset->jet_data[object->item.player_jet].alter_limit, object->target_angle);
    }
    for(std::vector<MslInst>::iterator object = data->msl_q.begin(); object != data->msl_q.end(); object++)
    {
        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height);
        advance(&object->curr,&object->alter,&asset->msl_data[object->type].alter_limit,object->target_angle);

    }
    for(std::vector<BulInst>::iterator object = data->bullet_q.begin(); object != data->bullet_q.end(); object++)
    {
        move(&object->curr, asset->lvl_data[data->level_name].map_width, asset->lvl_data[data->level_name].map_height);
    }



}



