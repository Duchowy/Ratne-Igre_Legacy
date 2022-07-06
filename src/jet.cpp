#include "jet.h"
#include "level.h"
#include "main.h"


void shoot(struct LevelInst * level, struct asset_data * asset)
{
for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++)
{
    if(object->weap_ammo[1] > 0 && object->will_shoot[1] && !object->weap_delay[1]) //missile
    {
        
        MslInst ap;
        ap.type = object->item.player_msl;
        ap.curr.speed = object->curr.speed;
        ap.curr.turn_angle = ap.target_angle = object->curr.turn_angle;
        ap.alter.turn_speed = 0.;
        ap.alter.speed_mode = 2;
        ap.curr.x=object->curr.x + cos(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox + asset->msl_data[ap.type].radius + 0.5);
        ap.curr.y=object->curr.y + sin(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox + asset->msl_data[ap.type].radius + 0.5);
        ap.decay = asset->msl_data[ap.type].decay;
        level->msl_q.push_back(ap);
        object->weap_delay[1] = 120;
        object->weap_ammo[1]--;
    }
    

    if(object->weap_ammo[0] > 0 && object->will_shoot[0] && !object->weap_delay[0] ) //bullet
    {
        BulInst ap;
        ap.color[0] = rand()%20+230;
        ap.color[1] = rand()%20+190;
        ap.color[2] = rand()%10+30;

        ap.curr.x=object->curr.x + cos(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox+0.5);
        ap.curr.y=object->curr.y + sin(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox+0.5);
        ap.curr.turn_angle=object->curr.turn_angle;
        ap.curr.turn_angle += (float)rand()/RAND_MAX * 2 * asset->gun_data[object->item.player_gun].spread - asset->gun_data[object->item.player_gun].spread;


        ap.curr.speed = asset->gun_data[object->item.player_gun].speed;
        ap.damage = asset->gun_data[object->item.player_gun].damage;
        ap.type = asset->gun_data[object->item.player_gun].ammo_type;
        ap.decay = asset->bul_data[asset->gun_data[object->item.player_gun].ammo_type].decay;
        level->bullet_q.push_back(ap);

        object->weap_delay[0] = asset->gun_data[object->item.player_gun].weap_delay;
        object->weap_ammo[0]--;
    }

}
}

void target(std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator target)
{
    float target_a[2];
    target_a[0] =  target->curr.x + 16*cos(target->curr.turn_angle)*target->curr.speed;
    target_a[1] = target->curr.y + 16*sin(target->curr.turn_angle)*target->curr.speed;

object->target_angle = atan2(( target_a[1] - object->curr.y) ,(target_a[0] - object->curr.x));
}

void target(struct LevelInst * level, struct asset_data * asset)
{
for(std::vector<MslInst>::iterator shell = level->msl_q.begin(); shell != level->msl_q.end(); shell++)
{
    float new_target = shell->target_angle;
    float min_deviation = 2*PI;
    //float aot = 0;

    for(std::vector<JetInst>::iterator target = level->jet_q.begin(); target != level->jet_q.end(); target++)
    {
        float distance = sqrt( pow( target->curr.x - shell->curr.x ,2) +  pow(  target->curr.y - shell->curr.y ,2));
        float new_angle = atan2(( target->curr.y - shell->curr.y) ,(target->curr.x - shell->curr.x));
        float deviation = fabs(angle_difference(shell->curr.turn_angle,new_angle));
        switch(shell->type)
        {
            case IR:
            {
            float angle_of_attack = fabs(angle_difference(shell->curr.turn_angle,target->curr.turn_angle)) ;
            if( angle_of_attack < PI/2  &&  distance < shell->decay * shell->curr.speed  &&  deviation  < asset->msl_data[shell->type].targeting_angle )
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
                if(distance < shell->decay * shell->curr.speed  &&  deviation  < asset->msl_data[shell->type].targeting_angle )
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
    }//eof jet iteration
    shell->target_angle = new_target;
}


}

void action(struct LevelInst * level, struct asset_data * asset)
{

for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
{
for(int i =0; i< 3; i++) object->will_shoot[i] = 0;
switch(object->mode)
{
    case PATROL:
    {
        object->alter.speed_mode = AIRBRAKE;
        if(object->at_work)
        {
            if(object->curr.turn_angle == object->target_angle) object->at_work = 0;
            else break;
        }
        else
        { 
            float r = object->curr.speed/asset->jet_data[object->item.player_jet].alter_limit.alter.turn_speed;
            if(object->curr.x <= 3*r || object->curr.x >= asset->lvl_data[level->level_name].map_width - 3*r || object->curr.y <= 3*r || object->curr.y >= asset->lvl_data[level->level_name].map_height-3*r)
            {
                int target_c[2] = {0,0};
                target_c[0] = asset->lvl_data[level->level_name].map_width/2 + rand()%240-120;
                target_c[1] = asset->lvl_data[level->level_name].map_height/14 * (rand()%13 + 1);
                object->target_angle  = atan2(( target_c[1] - object->curr.y) ,(target_c[0] - object->curr.x));
            }
            object->at_work = 1;
        }
        break;
    }
    case PURSUIT:
    {
        target(object,level->jet_q.begin());
        if(fabs(angle_difference(object->curr.turn_angle,object->target_angle)) < asset->jet_data[object->item.player_jet].alter_limit.turn_rate && rand()%600 == 0) object->will_shoot[1] = 1; //rocket
        object->alter.speed_mode = AFTERBURNER;
        break;
    }
    case DOGFIGHT:
    {
        target(object,level->jet_q.begin());
        float rad_dist = angle_difference(object->curr.turn_angle,object->target_angle);
        if(fabs(rad_dist) < asset->jet_data[object->item.player_jet].alter_limit.alter.turn_speed )  object->will_shoot[0] = 1; //gun
        if(fabs(rad_dist) > PI/2) object->alter.speed_mode = AFTERBURNER;
        else object->alter.speed_mode = STANDARD;
        break;
    }


}



}


}

void decision(std::vector<JetInst> &input_vec, struct asset_data * limit)
{
std::vector<JetInst>::iterator player = input_vec.begin();
for(std::vector<JetInst>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
{
float dist = distance(object,player);



bool triggered = 0;
if(object->mode == PURSUIT &&  dist < 350) //at pursuit
{
triggered = 1;
}
else if(  object->mode == PATROL   &&  ((dist < 350 && fabs(rad_distance(object,player)) < PI/6)  ||  dist < 160 + (1-(object->hp / limit->jet_data[object->item.player_jet].hp)) * 100 ))
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
void bullet_init(struct asset_data * lvl)
{
    for(int i = 0; i< ENUM_BULLET_TYPE_FIN;i++)
    {
        switch (i)
        {
            case SLUG:
            {
                lvl->bul_data[i].decay = 90;
                lvl->bul_data[i].width = 0.7;
                lvl->bul_data[i].height = 1.2;
                break;
            }
        
        }
    }
        
}

void gun_init(struct asset_data * lvl)
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

void msl_init(struct asset_data * lvl)
{
for(int i =0; i< ENUM_MSL_TYPE_FIN; i++)
{
      switch (i)
        {
            case IR:
            lvl->msl_data[i].alter_limit.turn_rate = 0.005;
            lvl->msl_data[i].alter_limit.alter.turn_speed = 0.02;
            lvl->msl_data[i].alter_limit.speed_limit[1] = 6.5;
            lvl->msl_data[i].alter_limit.speed_rate[1] = 0.2;
            lvl->msl_data[i].radius = 3;
            lvl->msl_data[i].targeting_angle = 1;
            lvl->msl_data[i].damage = 100;
            lvl->msl_data[i].ammo_max = 14;
            lvl->msl_data[i].decay = 130;
            break;
            case RAD:
            lvl->msl_data[i].alter_limit.turn_rate = 0.005;
            lvl->msl_data[i].alter_limit.alter.turn_speed = 0.035;
            lvl->msl_data[i].alter_limit.speed_limit[1] = 5.5;
            lvl->msl_data[i].alter_limit.speed_rate[1] = 0.2;
            lvl->msl_data[i].radius = 3;
            lvl->msl_data[i].targeting_angle = 0.7;
            lvl->msl_data[i].damage = 100;
            lvl->msl_data[i].ammo_max = 10;
            lvl->msl_data[i].decay = 180;
            break;
        }
}
}

void jet_init(struct asset_data * data)
{
    for(int i = 0; i< ENUM_JET_TYPE_FIN; i++)
    {
        switch(i)
        {
            case MIG21:
            {
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0125;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0075;
            data->jet_data[i].alter_limit.alter.turn_speed = 0.03;
            data->jet_data[i].alter_limit.turn_rate = 0.0075;
            data->jet_data[i].alter_limit.speed_limit[0] = 1.8;
            data->jet_data[i].default_speed = 2.6;
            data->jet_data[i].alter_limit.speed_limit[1] = 2.9;
            data->jet_data[i].hp = 100;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 0.8;
            data->jet_data[i].msl_mult = 1;
            data->jet_data[i].spc_mult = 0.9;
            break;
            }
            case F4:
            {
            data->jet_data[i].alter_limit.alter.turn_speed = 0.026;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.speed_limit[0] = 2.0;
            data->jet_data[i].default_speed = 2.4;
            data->jet_data[i].alter_limit.speed_limit[1] = 3.0;
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0125;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0075;
            data->jet_data[i].hp = 110;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 1.1;
            data->jet_data[i].msl_mult = 1.1;
            data->jet_data[i].spc_mult = 1.1;
            break;
            }
            case F104:
            {
            data->jet_data[i].alter_limit.alter.turn_speed = 0.017;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.speed_limit[0] = 2.4;
            data->jet_data[i].default_speed = 3.0;
            data->jet_data[i].alter_limit.speed_limit[1] = 3.4;
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0100;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0085;
            data->jet_data[i].hp = 90;
            data->jet_data[i].hitbox = 5;
            data->jet_data[i].gun_mult = 1;
            data->jet_data[i].msl_mult = 0.8;
            data->jet_data[i].spc_mult = 0.8;
            break;
            }
            case HARRIER:
            {
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0100;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0050;
            data->jet_data[i].alter_limit.alter.turn_speed = 0.017;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.speed_limit[0] = 1.5;
            data->jet_data[i].default_speed = 1.9;
            data->jet_data[i].alter_limit.speed_limit[1] = 2.2;
            data->jet_data[i].hp = 110;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 1.1;
            data->jet_data[i].msl_mult = 1.4;
            data->jet_data[i].spc_mult = 1.4;
            break;
            }

        }

    }
}







JetInst jet_spawn(struct asset_data * asset, struct selection* selected,bool bot)
{
    JetInst object;
    object.item = *selected;
    object.hp = asset->jet_data[selected->player_jet].hp;
    if(bot)
    {
        if(rand()%2) object.curr.turn_angle = (float)rand()/(RAND_MAX) *1.8 + 1;
        else object.curr.turn_angle = (float)rand()/(RAND_MAX) *(-1.8) - 1.2;
       
    }
    else
    {
        object.curr.turn_angle = 0.0;

    }
    object.target_angle = object.curr.turn_angle;
    object.alter.speed_mode = STANDARD;
    object.alter.turn_speed = 0.;
    object.curr.speed = asset->jet_data[selected->player_jet].default_speed;
    object.at_work = 0;
    object.weap_ammo[0] = (float) asset->jet_data[selected->player_jet].gun_mult * asset->gun_data[selected->player_gun].ammo_max;
    object.weap_ammo[1] = (float) asset->jet_data[selected->player_jet].msl_mult * asset->msl_data[selected->player_msl].ammo_max;
    object.curr.x = -1;
    object.curr.y = -1;

    for(int i =0; i < 3; i++) object.will_shoot[i] = 0;
    for(int i =0; i < 3; i++) object.weap_delay[i] = 0;


    return object;
}

void enemy_spawn(struct LevelInst * level, struct asset_data * asset)
{
int enemy_amount = 0;

for(int i = 0; i<ENUM_JET_TYPE_FIN;  i++) enemy_amount += asset->lvl_data[level->level_name].enemy_quality[i];
float x = (float) asset->lvl_data[level->level_name].map_width*0.7, y = (float) asset->lvl_data[level->level_name].map_height/(enemy_amount+1);

struct selection templat[ENUM_JET_TYPE_FIN] = {
{.player_jet = MIG21,.player_gun=SHVAK,.player_msl=IR},
{.player_jet = F4, .player_gun = GATLING, .player_msl = RAD},
{.player_jet = F104, .player_gun = GATLING, .player_msl = IR},
{.player_jet = HARRIER, .player_gun = ADEN, .player_msl = IR}
};





std::copy(asset->lvl_data[level->level_name].enemy_quality,asset->lvl_data[level->level_name].enemy_quality+ENUM_JET_TYPE_FIN,level->enemy_quality);


for(int i = 0; i<ENUM_JET_TYPE_FIN;  i++)
{
    for(int q = 0; q< level->enemy_quality[i]; q++)
    {
        JetInst temp = jet_spawn(asset,templat+i,1);
        temp.curr.x = x + rand()%20-10;
        temp.curr.y = y;
        level->jet_q.push_back(temp);
        y += (float) asset->lvl_data[level->level_name].map_height/(enemy_amount+1);

    }
}



}

