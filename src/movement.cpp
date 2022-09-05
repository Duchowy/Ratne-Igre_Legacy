#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"

enum OperationMode{SIMPLIFIED,ADVANCED};

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
object->alter.target_angle = getTargetAngle(&object->curr,&player->curr,16);
object->curr.turn_angle = object->alter.target_angle;

}



}





void dash(std::vector<JetInst>::iterator object,unsigned int map_width, unsigned int map_height)
{
    if(object->ability[DASH].duration)
    {
        move(&object->curr,map_width,map_height,10/object->curr.speed);
    }
        
}

void accelerate(struct state * pos, struct state_change * alt , state_change_limit * limit, float modification)
{

if(modification > 0) pos->speed = (pos->speed + modification > limit->speed_limit[1] ? limit->speed_limit[1] : pos->speed + modification);
else pos->speed = (pos->speed + modification < limit->speed_limit[0] ? limit->speed_limit[0] : pos->speed + modification);
}

void transform(struct state * pos,unsigned int map_width, unsigned int map_height,float distance, float angle)
{
    pos->x += cos(angle)*distance;
    if(pos->x < 0) pos->x = 0;
    if(pos->x > map_width) pos->x = map_width;
    pos->y += sin(angle)*distance;
    if(pos->y < 0) pos->y = 0;
    if(pos->y > map_height) pos->y = map_height;
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


float movement_coef_calculate(state_change_limit * limit, float speed)
{
return 1 - ( limit->mobility_coef * pow(speed-limit->default_speed,2) / speed * (0.0075/limit->speed_rate[1]));
}





void advance(struct state * pos, struct state_change * alt, state_change_limit * limit, unsigned short mode_of_operation)
{
// 0 brake, 1 stay, 2 accelerate
if(alt->acceleratable && limit)
{
    if(pos->speed != alt->target_speed)
    {
        if(pos->speed < alt->target_speed) pos->speed = (pos->speed + limit->speed_rate[1] > alt->target_speed ? alt->target_speed : pos->speed + limit->speed_rate[1]);
        if(pos->speed > alt->target_speed) pos->speed = (pos->speed - limit->speed_rate[0] < alt->target_speed ? alt->target_speed : pos->speed - limit->speed_rate[0]);
    }

}

//rotation section
if(alt->rotatable)
{

    if(limit)
    {
        
        float angle_diff = angle_difference(pos->turn_angle,alt->target_angle);
        float road = 0.5 * pow(alt->turn_speed,2) / limit->turn_rate; 
        float rotation_coef = 1;
        float speed_loss_coef = 0;
        if(mode_of_operation == ADVANCED) 
        {
            rotation_coef = movement_coef_calculate(limit,pos->speed);
            speed_loss_coef = fabs(alt->turn_speed) / limit->alter.turn_speed / rotation_coef   * (pos->speed  / limit->speed_limit[1])  ;
            accelerate(pos,alt,limit, -speed_loss_coef * 0.0115);
        }


            if(fabs(angle_diff)>road) //outer scope
            { //rough targeting, altering radial velocity
                if(angle_diff >= 0)  alt->turn_speed = (alt->turn_speed - limit->turn_rate > -limit->alter.turn_speed * rotation_coef ? alt->turn_speed - limit->turn_rate :  -limit->alter.turn_speed * rotation_coef);
                else alt->turn_speed = (alt->turn_speed + limit->turn_rate < limit->alter.turn_speed * rotation_coef ? alt->turn_speed + limit->turn_rate :  limit->alter.turn_speed * rotation_coef);
                
                
                
                //angle alteration part
                pos->turn_angle = angle_addition(pos->turn_angle,alt->turn_speed);
            }
            else //inner scope
            { //slowing down
                if(fabs(angle_diff)>fabs(alt->turn_speed))
                {
                    if(angle_diff >= 0) alt->turn_speed = (alt->turn_speed + limit->turn_rate < limit->alter.turn_speed * rotation_coef ? alt->turn_speed + limit->turn_rate :  limit->alter.turn_speed * rotation_coef); 
                    else alt->turn_speed = (alt->turn_speed - limit->turn_rate > -limit->alter.turn_speed * rotation_coef ? alt->turn_speed - limit->turn_rate :  -limit->alter.turn_speed * rotation_coef);
                        
                        
                    pos->turn_angle = angle_addition(pos->turn_angle,alt->turn_speed);
                }
                else
                {
                pos->turn_angle = alt->target_angle;
                alt->turn_speed = 0;
                }
            }

    }
    else
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
        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height,1);
        advance(&object->curr,&object->alter,    (object->overwrite_limit ? object->overwrite_limit : &asset->jet_data[object->type].alter_limit) ,ADVANCED);
    }

    #pragma omp parallel for
    for(std::vector<ProjInst>::iterator object = data->proj_q.begin(); object != data->proj_q.end(); object++)
    {
        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height,1);
        if(object->alter) advance(&object->curr,object->alter,&asset->proj_data[object->type].alter_limit,SIMPLIFIED);

    }

    

    #pragma omp parallel for
    for(std::vector<ParticleInst>::iterator object = data->prt_q.begin(); object != data->prt_q.end(); object++)
    {
        move(&object->curr,asset->lvl_data[data->level_name].map_width,asset->lvl_data[data->level_name].map_height,1);
        advance(&object->curr,&object->alter,NULL,SIMPLIFIED);
    }



}



