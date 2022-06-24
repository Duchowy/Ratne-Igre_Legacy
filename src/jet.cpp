#include "main.h"
#include "jet.h"
#include "level.h"


void shoot(std::vector<jet> &input_vec, std::vector<missile> &series, struct lvl_dat * lvl)
{
for(std::vector<jet>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
    if(object->weap_ammo[1][1] > 0 && object->will_shoot[1] && !object->weap_delay[1])
    {
        missile ap = lvl->msl_data[object->weap[1]];
        ap.speed[1] = object->speed[object->speed_mode];
        ap.curr_angle = ap.target_angle = object->curr_angle;
        ap.curr_angle_w = 0;
        ap.x=object->x + cos(object->curr_angle)*(object->hitbox+ap.radius+0.5);
        ap.y=object->y + sin(object->curr_angle)*(object->hitbox+ap.radius+0.5);
        series.push_back(ap);
        object->weap_delay[1] = 120;
        object->weap_ammo[1][1]--;
    }


}
}

void shoot(std::vector<jet> &input_vec, std::vector<bullet> &series, struct lvl_dat * lvl)
{
for(std::vector<jet>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
    if(object->weap_ammo[0][1] > 0 && object->will_shoot[0] && !object->weap_delay[0] ) ////weap_ammo tested!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        bullet ap;
        ap.color[0] = rand()%20+230;
        ap.color[1] = rand()%20+190;
        ap.color[2] = rand()%10+30;

        ap.x=object->x + cos(object->curr_angle)*(object->hitbox+0.5);
        ap.y=object->y + sin(object->curr_angle)*(object->hitbox+0.5);
        ap.angle=object->curr_angle;
        ap.angle += (float)rand()/RAND_MAX * 2 * lvl->gun_data[object->weap[0]].spread - lvl->gun_data[object->weap[0]].spread;


        bullet_init(&ap,lvl->gun_data[(object->weap[0])].ammo_type);
        ap.speed = lvl->gun_data[object->weap[0]].speed;
        ap.damage = lvl->gun_data[(object->weap[0])].damage;
        ap.type = lvl->gun_data[(object->weap[0])].ammo_type;
        series.push_back(ap);

        object->weap_delay[0] = lvl->gun_data[(object->weap[0])].weap_delay;
        object->weap_ammo[0][1]--;
    }
}
}

void target(std::vector<jet>::iterator object, std::vector<jet>::iterator target)
{
    float target_a[2];
    target_a[0] =  target->x + 16*cos(target->curr_angle)*target->speed[target->speed_mode];
    target_a[1] = target->y + 16*sin(target->curr_angle)*target->speed[target->speed_mode];

object->target_angle = atan2(( target_a[1] - object->y) ,(target_a[0] - object->x));
}

void target(std::vector<jet> &input_vec, std::vector<missile> &shell_vec)
{
for(std::vector<missile>::iterator shell = shell_vec.begin(); shell != shell_vec.end(); shell++)
{
    float new_target = shell->target_angle;
    float min_deviation = 2*PI;
    //float aot = 0;

    for(std::vector<jet>::iterator target = input_vec.begin(); target != input_vec.end(); target++)
    {
        float distance = sqrt( pow( target->x - shell->x ,2) +  pow(  target->y - shell->y ,2));
        float new_angle = atan2(( target->y - shell->y) ,(target->x - shell->x));
        float deviation = fabs(angle_difference(shell->curr_angle,new_angle));
        switch(shell->type)
        {
            case IR:
            {
            float angle_of_attack = fabs(angle_difference(shell->curr_angle,target->curr_angle)) ;
            if( angle_of_attack < PI/2  &&  distance < shell->decay[1] * shell->speed[0]  &&  deviation  < shell->targeting_angle )
            {
                

                //aot = angle_of_attack;
                if(deviation < min_deviation)
                {
                new_target = new_angle;
                min_deviation = deviation;
                }
            }
            }
            break;
            case RAD:
            {
                if(distance < shell->decay[1] * shell->speed[0]  &&  deviation  < shell->targeting_angle )
                {
                    

                    //aot = angle_of_attack;
                    if(deviation < min_deviation)
                    {
                    new_target = new_angle;
                    min_deviation = deviation;
                    }
                }
            }
            break;
        }
    }
    shell->target_angle = new_target;
}


}

void action(std::vector<jet> &input_vec, std::vector<bullet> &shell_vec, struct lvl_dat * limit)
{

for(std::vector<jet>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
{
for(int i =0; i< 3; i++) object->will_shoot[i] = 0;
switch(object->mode)
{
    case PATROL:
    {
        object->speed_mode = AIRBRAKE;
        if(object->at_work)
        {
            if(object->curr_angle == object->target_angle) object->at_work = 0;
            else break;
        }
        else
        {
            float r = object->speed[object->speed_mode]/object->max_angle_w;
            if(object->x <= 3*r || object->x >= limit->map_width - 3*r || object->y <= 3*r || object->y >= limit->map_height-3*r)
            {
                int target_c[2] = {0,0};
                target_c[0] = limit->map_width/2 + rand()%240-120;
                target_c[1] = limit->map_height/14 * (rand()%13 + 1);
                object->target_angle  = atan2(( target_c[1] - object->y) ,(target_c[0] - object->x));
            }
            object->at_work = 1;
        }
        break;
    }
    case PURSUIT:
    {
        target(object,input_vec.begin());
        if(fabs(angle_difference(object->curr_angle,object->target_angle)) < object->angle_a && rand()%600 == 0) object->will_shoot[1] = 1; //rocket
        object->speed_mode = AFTERBURNER;
        break;
    }
    case DOGFIGHT:
    {
        target(object,input_vec.begin());
        float rad_dist = angle_difference(object->curr_angle,object->target_angle);
        if(fabs(rad_dist) < object->max_angle_w )  object->will_shoot[0] = 1; //gun
        if(fabs(rad_dist) > PI/2) object->speed_mode = AFTERBURNER;
        else object->speed_mode = STANDARD;
        break;
    }


}



}


}

void decision(std::vector<jet> &input_vec, struct lvl_dat * limit)
{
std::vector<jet>::iterator player = input_vec.begin();
for(std::vector<jet>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
{
float dist = distance(object,player);


bool triggered = 0;
if(object->mode == PURSUIT &&  dist < 350) //at pursuit
{
triggered = 1;
}
else if(  object->mode == PATROL   &&  ((dist < 350 && fabs(rad_distance(object,player)) < PI/6)  ||  dist < 160 + (1-(object->hp[1] / object->hp[0])) * 100 ))
    {
        triggered = 1;
    }
else object->mode = PATROL;

if(triggered)
{
    if(dist < 160) object->mode = DOGFIGHT;
    else object->mode = PURSUIT;
}


}
}



/*
###############################
###############################
######## JET INITIALIZE #######
###############################
###############################
*/
void bullet_init(struct bullet * object, unsigned short type)
{
    switch (type)
    {
        case SLUG:
        {
            object->type = type;
            object->decay[0] = 90;
            object->decay[1] = 90;
            object->width = 0.7;
            object->height = 1.2;
            break;
        }
    
    }
}

void gun_init(struct lvl_dat * lvl)
{
for(int i =0; i< ENUM_GUN_TYPE_FIN; i++)
{
      switch (i)
        {
        case SHVAK:
        {
            lvl->gun_data[i].damage = 60;
            lvl->gun_data[i].ammo_max = 180;
            lvl->gun_data[i].ammo_type = SLUG;
            lvl->gun_data[i].weap_delay = 8;
            lvl->gun_data[i].spread = 0.05;
            lvl->gun_data[i].speed = 6;
            break;
        }
        case ADEN:
        {
            lvl->gun_data[i].damage = 35;
            lvl->gun_data[i].ammo_max = 240;
            lvl->gun_data[i].ammo_type = SLUG;
            lvl->gun_data[i].weap_delay = 4;
            lvl->gun_data[i].spread = 0.075;
            lvl->gun_data[i].speed = 7;
            break;
        }
        case GATLING:
        {
            lvl->gun_data[i].damage = 18;
            lvl->gun_data[i].ammo_max = 400;
            lvl->gun_data[i].ammo_type = SLUG;
            lvl->gun_data[i].weap_delay = 3;
            lvl->gun_data[i].spread = 0.025;
            lvl->gun_data[i].speed = 8;
            break;
        }


    }
}
}

void msl_init(struct lvl_dat * lvl)
{
for(int i =0; i< ENUM_MSL_TYPE_FIN; i++)
{
      switch (i)
        {
            case IR:
            lvl->msl_data[i].type = IR;
            lvl->msl_data[i].angle_a = 0.005;
            lvl->msl_data[i].max_angle_w = 0.02;
            lvl->msl_data[i].speed[0] = 6.5;
            lvl->msl_data[i].speed_a = 0.2;
            lvl->msl_data[i].radius = 3;
            lvl->msl_data[i].targeting_angle = 1;
            lvl->msl_data[i].damage = 100;
            lvl->msl_data[i].ammo_max = 14;
            lvl->msl_data[i].decay[0] = lvl->msl_data[i].decay[1] = 130;
            break;
            case RAD:
            lvl->msl_data[i].type = RAD;
            lvl->msl_data[i].angle_a = 0.005;
            lvl->msl_data[i].max_angle_w = 0.035;
            lvl->msl_data[i].speed[0] = 5.5;
            lvl->msl_data[i].speed_a = 0.2;
            lvl->msl_data[i].radius = 3;
            lvl->msl_data[i].targeting_angle = 0.7;
            lvl->msl_data[i].damage = 100;
            lvl->msl_data[i].ammo_max = 10;
            lvl->msl_data[i].decay[0] = lvl->msl_data[i].decay[1] = 180;
            break;
        }
}
}


void jet_init(struct jet * object, struct lvl_dat * data,unsigned short type,bool bot)
{
    switch (type)
    {
        case MIG21:
        {
        object->type = MIG21;
        object->max_angle_w = 0.03;
        object->angle_a = 0.0075;
        object->speed[0] = 1.8;
        object->speed[1] = 2.6;
        object->speed[2] = 2.9;
        object->hp[1] = 100;
        object->hitbox = 6;
        if(bot) object->weap[0] = SHVAK;
        if(bot) object->weap[1] = IR;
        object->weap_ammo[0][0] = object->weap_ammo[0][1] = data->gun_data[object->weap[0]].ammo_max * 0.8;
        object->weap_ammo[1][0] = object->weap_ammo[1][1] = data->msl_data[object->weap[1]].ammo_max * 1;
        break;
        }
        case F4:
        {
        object->type = F4;
        object->max_angle_w = 0.026;
        object->angle_a = 0.009;
        object->speed[0] = 2.0;
        object->speed[1] = 2.4;
        object->speed[2] = 3.0;
        object->hp[1] = 110;
        object->hitbox = 6;
        if(bot) object->weap[0] = GATLING;
        if(bot) object->weap[1] = IR;
        object->weap_ammo[0][0] = object->weap_ammo[0][1] = data->gun_data[object->weap[0]].ammo_max * 1.1;
        object->weap_ammo[1][0] = object->weap_ammo[1][1] = data->msl_data[object->weap[1]].ammo_max * 1.1;
        break;
        }
        case F104:
        {
        object->type = F104;
        object->max_angle_w = 0.017;
        object->angle_a = 0.009;
        object->speed[0] = 2.4;
        object->speed[1] = 3.0;
        object->speed[2] = 3.4;
        object->hp[1] = 90;
        object->hitbox = 5;
        if(bot) object->weap[0] = GATLING;
        if(bot) object->weap[1] = IR;
        object->weap_ammo[0][0] = object->weap_ammo[0][1] = data->gun_data[object->weap[0]].ammo_max * 1;
        object->weap_ammo[1][0] = object->weap_ammo[1][1] = data->msl_data[object->weap[1]].ammo_max * 0.8;
        break;
        }
        case HARRIER:
        {
        object->type = HARRIER;
        object->max_angle_w = 0.017;
        object->angle_a = 0.009;
        object->speed[0] = 1.5;
        object->speed[1] = 1.9;
        object->speed[2] = 2.2;
        object->hp[1] = 110;
        object->hitbox = 6;
        if(bot) object->weap[0] = ADEN;
        if(bot) object->weap[1] = IR;
        object->weap_ammo[0][0] = object->weap_ammo[0][1] = data->gun_data[object->weap[0]].ammo_max * 1.1;
        object->weap_ammo[1][0] = object->weap_ammo[1][1] = data->msl_data[object->weap[1]].ammo_max * 1.4;
        break;
        }

    }
    
    if(bot)
    {
        if(rand()%2) object->curr_angle = (float)rand()/(RAND_MAX) *1.8 + 1;
        else object->curr_angle = (float)rand()/(RAND_MAX) *(-1.8) - 1.2;
    }
    else object->curr_angle = 0.0;
    if(bot) object->weap_ammo[1][0] = object->weap_ammo[1][1] = object->weap_ammo[1][0]*0.3;
    object->hp[0] = object->hp[1];
    object->target_angle = 0.0;
    object->curr_angle_w = 0.0;
    for(int i =0; i < 3; i++) object->weap_delay[i] = 0;
    object->speed_mode = STANDARD;



}

void enemy_init(std::vector<jet> &object, struct lvl_dat * lvl)
{
float x = lvl->map_width*0.7, y = lvl->map_height/(lvl->enemy_amount+1);
std::copy(lvl->enemy_quality[0],lvl->enemy_quality[0]+ENUM_JET_TYPE_FIN,lvl->enemy_quality[1]);

for(int i = 0; i<ENUM_JET_TYPE_FIN;  i++)
{
    for(int q = 0; q< lvl->enemy_quality[0][i]; q++)
    {
        jet temp;
        jet_init(&temp,lvl,i,1);
        temp.x = x + rand()%20-10;
        temp.y = y;
        object.push_back(temp);
        y += lvl->map_height/(lvl->enemy_amount+1);

    }
}



}

