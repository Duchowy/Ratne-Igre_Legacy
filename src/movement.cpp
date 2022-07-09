#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"

void randomize_position(std::vector<JetInst>::iterator object, asset_data * asset, LevelInst * lvl)
{
if(object->ability[RAND_POS].duration)
{

std::vector<JetInst>::iterator player = lvl->jet_q.begin();
int map_width = asset->lvl_data[lvl->level_name].map_width;
int map_height = asset->lvl_data[lvl->level_name].map_height;



if(rand()%2) //x is edge
{
    object->curr.y = (float) rand()/RAND_MAX * map_height;
    if(rand()%2) //left
    {
        object->curr.x = 0;
    }
    else //right
    {
        object->curr.x = map_width;
    }
}
else //y is edge
{
    object->curr.x = (float) rand()/RAND_MAX * map_width;
    if(rand()%2) //up
    {
        object->curr.y = 0;
    }
    else //down
    {
        object->curr.y = map_height;
    }

}
target(object,player);
object->curr.turn_angle = object->target_angle;

}



}





void dash(std::vector<JetInst>::iterator object,unsigned int map_width, unsigned int map_height)
{
        object->curr.x += cos(object->curr.turn_angle)*10;
        if(object->curr.x < 0) object->curr.x = 0;
        if(object->curr.x > map_width) object->curr.x = map_width;
        object->curr.y += sin(object->curr.turn_angle)*10;
        if(object->curr.y < 0) object->curr.y = 0;
        if(object->curr.y > map_height) object->curr.y = map_height;
}




void move(struct state * pos, unsigned int map_width, unsigned int map_height, int num)
{
        pos->x += cos(pos->turn_angle)*pos->speed*num;
        if(pos->x < 0) pos->x = 0;
        if(pos->x > map_width) pos->x = map_width;
        pos->y += sin(pos->turn_angle)*pos->speed*num;
        if(pos->y < 0) pos->y = 0;
        if(pos->y > map_height) pos->y = map_height;
}

void advance(struct state * pos, struct state_change * alt, state_change_limit * limit, float target_angle)
{
// 0 brake, 1 stay, 2 accelerate
if(alt->acceleratable)
{
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
}

//rotation section
if(alt->rotatable)
{

    if(limit)
    {
            
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
            pos->turn_angle = angle_addition(pos->turn_angle,alt->turn_speed);
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
                
                
                pos->turn_angle = angle_addition(pos->turn_angle,alt->turn_speed);
        }
        else //recently added
        {
        pos->turn_angle = target_angle;
        alt->turn_speed = 0;
        }
        }



    }else
    {

        pos->turn_angle = angle_addition(pos->turn_angle,alt->turn_speed);

    }



}



}


void transform(struct LevelInst * data, struct asset_data * asset)
{


    #pragma omp parallel for
    for(std::vector<JetInst>::iterator object = data->jet_q.begin(); object != data->jet_q.end(); object++)
    {
        if(asset->jet_data[object->item.player_jet].isBoss && object->ability[BOSS_ABILITY::DASH].duration) dash(object,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height);

        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height,1);
        advance(&object->curr,&object->alter, &asset->jet_data[object->item.player_jet].alter_limit, object->target_angle);
    }
    for(std::vector<MslInst>::iterator object = data->msl_q.begin(); object != data->msl_q.end(); object++)
    {
        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height,1);
        advance(&object->curr,&object->alter,&asset->msl_data[object->type].alter_limit,object->target_angle);

    }
    for(std::vector<BulInst>::iterator object = data->bullet_q.begin(); object != data->bullet_q.end(); object++)
    {
        move(&object->curr, asset->lvl_data[data->level_name].map_width, asset->lvl_data[data->level_name].map_height,1);
    }
    for(std::vector<ParticleInst>::iterator object = data->prt_q.begin(); object != data->prt_q.end(); object++)
    {
        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height,1);
        advance(&object->curr,&object->alter,NULL,0);
    }



}



