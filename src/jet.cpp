#include "jet.h"
#include "level.h"
#include "main.h"


void shoot(struct LevelInst * level, struct asset_data * asset)
{
#pragma omp parallel for
for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++)
{
    if(object->weap_ammo[1] > 0 && object->will_shoot[1] && !object->weap_delay[1]) //missile
    {
        
        MslInst ap;
        ap.alter.acceleratable = 1;
        ap.alter.rotatable = 1;
        ap.type = object->item.player_msl;
        ap.curr.speed = object->curr.speed;
        ap.curr.turn_angle = ap.target_angle = object->curr.turn_angle;
        ap.alter.turn_speed = 0.;
        ap.alter.speed_mode = 2;
        ap.curr.x=object->curr.x + cos(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox + asset->msl_data[ap.type].radius + 0.5);
        ap.curr.y=object->curr.y + sin(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox + asset->msl_data[ap.type].radius + 0.5);
        ap.decay = asset->msl_data[ap.type].decay;
        for(int i = 0; i< ENUM_MSL_STATUS_FIN;i++) ap.status[i] = 0;
        ap.isBotLaunched = object->isBot;
        #pragma omp critical
        level->msl_q.push_back(ap);
        object->weap_delay[1] = 120;
        object->weap_ammo[1]--;
    }
    

    if(object->weap_ammo[0] > 0 && object->will_shoot[0] && !object->weap_delay[0] ) //bullet
    {
        BulInst ap;

        ap.curr.x=object->curr.x + cos(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox+0.5);
        ap.curr.y=object->curr.y + sin(object->curr.turn_angle)*(asset->jet_data[object->item.player_jet].hitbox+0.5);
        ap.curr.turn_angle=object->curr.turn_angle;
        ap.curr.turn_angle += (float)rand()/RAND_MAX * 2 * asset->gun_data[object->item.player_gun].spread - asset->gun_data[object->item.player_gun].spread;


        ap.curr.speed = asset->gun_data[object->item.player_gun].speed;
        ap.damage = asset->gun_data[object->item.player_gun].damage;
        ap.type = asset->gun_data[object->item.player_gun].ammo_type;
        ap.decay = asset->bul_data[asset->gun_data[object->item.player_gun].ammo_type].decay;
        #pragma omp critical
        {
        ap.color[0] = rand()%20+230; //to be moved when random function changed
        ap.color[1] = rand()%20+190;
        ap.color[2] = rand()%10+30;
        level->bullet_q.push_back(ap);
        }
        

        object->weap_delay[0] = asset->gun_data[object->item.player_gun].weap_delay;
        object->weap_ammo[0]--;
    }

}
}

void target(std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator target,float offset)
{
    float target_a[2];
    target_a[0] =  target->curr.x + offset*cos(target->curr.turn_angle)*target->curr.speed;
    target_a[1] = target->curr.y + offset*sin(target->curr.turn_angle)*target->curr.speed;

object->target_angle = atan2(( target_a[1] - object->curr.y) ,(target_a[0] - object->curr.x));
}

void target(struct LevelInst * level, struct asset_data * asset)
{
#pragma omp parallel for
for(std::vector<MslInst>::iterator shell = level->msl_q.begin(); shell != level->msl_q.end(); shell++)
{
    if(shell->status[CONTROLLED] == 1) continue;
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
                if(distance < shell->decay * shell->curr.speed  &&  deviation  < asset->msl_data[shell->type].targeting_angle && shell->isBotLaunched != target->isBot)
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
std::vector<JetInst>::iterator player = level->jet_q.begin();
//#pragma omp parallel for //blocked by rand
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
            if(!object->at_work)
            {
                if(rand()%900 == 0) object->target_angle = (object->target_angle, (float)0.6 *rand()/RAND_MAX - 0.3);
            }

        }
    }
    break;
    case PURSUIT:
    {
        target(object,player,16);
        if(fabs(rad_distance(object,player)) < asset->gun_data[object->item.player_gun].spread) 
        {
            if(asset->jet_data[object->item.player_jet].isBoss)
            {
                if(rand()%100 == 0 && distance(object,player) < asset->msl_data[object->item.player_msl].decay * asset->msl_data[object->item.player_msl].alter_limit.speed_limit[1] ) object->will_shoot[1] = 1; //boss launch missile
            }
            else
            {
                if(rand()%600 == 0) object->will_shoot[1] = 1; //launch missile
            }

            
        }
        
        
        object->alter.speed_mode = AFTERBURNER;
    }
    break;
    case DOGFIGHT:
    {
        target(object,level->jet_q.begin(),8);
        float rad_dist = angle_difference(object->curr.turn_angle,object->target_angle); //radial distance between object and target
        if(fabs(rad_dist) < asset->gun_data[object->item.player_gun].spread )  object->will_shoot[0] = 1; //gun
        if(fabs(rad_dist) > PI/2)
        {
            if(object->curr.speed > player->curr.speed) object->alter.speed_mode = AFTERBURNER;
            else object->alter.speed_mode = STANDARD;
        }
        else 
        {
            if(object->curr.speed < player->curr.speed) object->alter.speed_mode = AIRBRAKE;
            else object->alter.speed_mode = STANDARD;
        }
    }
    break;


}



}


}

//all boss abilities used & standard bot actions
void decision(std::vector<JetInst> &input_vec, struct asset_data * limit)
{
std::vector<JetInst>::iterator player = input_vec.begin();
#pragma omp parallel for
for(std::vector<JetInst>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
{
    float dist = distance(object,player);

    bool triggered = 0;
    if(limit->jet_data[object->item.player_jet].isBoss)
    {
        triggered = 1;
        if(
            limit->boss_data[object->item.player_jet-ENUM_JET_TYPE_FIN].ability[BOSS_ABILITY::DASH] &&
            object->ability[BOSS_ABILITY::DASH].cooldown == 0 &&
            (
            (dist > 350 && fabs(rad_distance(object,player)) < PI/6) || (dist < 350 && fabs(rad_distance(object,player)) > 2*PI/3)
            )
        )
        {
            object->ability[BOSS_ABILITY::DASH].cooldown = limit->abl_data[BOSS_ABILITY::DASH].cooldown;
            object->ability[BOSS_ABILITY::DASH].duration = limit->abl_data[BOSS_ABILITY::DASH].duration;
        };

        if(
            limit->boss_data[object->item.player_jet-ENUM_JET_TYPE_FIN].ability[BOSS_ABILITY::RAND_POS] &&
            object->ability[BOSS_ABILITY::RAND_POS].cooldown == 0 &&
            (dist < 350 && dist > 150 && fabs(rad_distance(object,player)) > 3*PI/5)
        )
        {
            object->ability[BOSS_ABILITY::RAND_POS].cooldown = limit->abl_data[BOSS_ABILITY::RAND_POS].cooldown;
            object->ability[BOSS_ABILITY::RAND_POS].duration = limit->abl_data[BOSS_ABILITY::RAND_POS].duration;
        };




    }
    else
    {
        if(object->mode == PURSUIT &&  dist < 350) //at pursuit
        {
            triggered = 1;
        }
        if(  object->mode == PATROL   &&  ((dist < 350 && fabs(rad_distance(object,player)) < PI/6)  ||  dist < 160 + (1-(object->hp / limit->jet_data[object->item.player_jet].hp)) * 120 ))
        {
            triggered = 1;
        }
        if(object->mode == DOGFIGHT) triggered = 1;
    }
    if(triggered)
        {
            if(dist < 160) object->mode = DOGFIGHT;
            else object->mode = PURSUIT;
        }
    else object->mode = PATROL;


}

if(input_vec.size() > 1)
{
    #pragma omp parallel for
    for(std::vector<JetInst>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
    {
        if(object->mode != PATROL) continue;
        float obj_dist = distance(object,player);
        for(std::vector<JetInst>::iterator ally = object+1; ally != input_vec.end(); ally++)
                {
                    if( (object->mode != PATROL) == (ally->mode != PATROL)) continue;
                    float dist_between = distance(object,ally);
                    float aly_dist = distance(ally,player);
                    #pragma omp critical
                    {
                        if(dist_between < 550)
                        {
                            if(object->mode != PATROL)
                            {
                                if(aly_dist < 450)
                                {
                                    if(aly_dist < 160) ally->mode = DOGFIGHT;
                                    else ally->mode = PURSUIT;
                                }
                            }
                            else
                            {
                                if(obj_dist < 450)
                                {
                                    if(obj_dist < 160) object->mode = DOGFIGHT;
                                    else object->mode = PURSUIT;
                                }
                            }
                        }
                    }
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
    object.curr.speed = asset->jet_data[selected->player_jet].alter_limit.default_speed;
    object.at_work = 0;
    object.weap_ammo[0] = (float) asset->jet_data[selected->player_jet].gun_mult * asset->gun_data[selected->player_gun].ammo_max;
    object.weap_ammo[1] = (float) asset->jet_data[selected->player_jet].msl_mult * asset->msl_data[selected->player_msl].ammo_max;
    object.curr.x = -1;
    object.curr.y = -1;
    object.alter.acceleratable = 1;
    object.alter.rotatable = 1;
    object.ability = nullptr;
    object.isBot = bot;
    object.overwrite_limit = nullptr;


    for(int i =0; i < 3; i++) object.will_shoot[i] = 0;
    for(int i =0; i < 3; i++) object.weap_delay[i] = 0;

    for(int i = 0; i< ENUM_JET_STATUS_FIN;i++) object.status[i] = 0;


    return object;
}

void enemy_spawn(struct LevelInst * level, struct asset_data * asset)
{
int enemy_amount = 0;

float map_width = asset->lvl_data[level->level_name].map_width;
float map_height = asset->lvl_data[level->level_name].map_height;

for(int i = 0; i<ENUM_BOSS_TYPE_FIN;  i++) enemy_amount += asset->lvl_data[level->level_name].enemy_quality[i];
float x = (float) map_width*0.7, y = (float) map_height/(enemy_amount+1);

struct selection templat[ENUM_BOSS_TYPE_FIN] = {
{.player_jet = MIG21,.player_gun=SHVAK,.player_msl=IR},
{.player_jet = F4, .player_gun = GATLING, .player_msl = RAD},
{.player_jet = F104, .player_gun = GATLING, .player_msl = IR},
{.player_jet = HARRIER, .player_gun = ADEN, .player_msl = IR},
{.player_jet = MIG29, .player_gun = SHVAK, .player_msl = RAD},
{.player_jet = SR91, .player_gun = GATLING, .player_msl = RAD}
};





std::copy(asset->lvl_data[level->level_name].enemy_quality,asset->lvl_data[level->level_name].enemy_quality+ENUM_BOSS_TYPE_FIN,level->enemy_quality);


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
for(int i = ENUM_JET_TYPE_FIN; i< ENUM_BOSS_TYPE_FIN; i++)
{
    for(int q = 0; q< level->enemy_quality[i]; q++)
    {
        JetInst temp = jet_spawn(asset,templat+i,1);
        temp.curr.x = x + (float)rand()/RAND_MAX*0.4*map_width - 0.2*map_width;
        temp.curr.y = map_height*(float)rand()/RAND_MAX;
        temp.ability = new struct Ability[ENUM_BOSS_ABILITY_FIN];
        for(int m = 0 ; m< ENUM_BOSS_ABILITY_FIN; m++)
        {
            temp.ability[m].cooldown = 0;
            temp.ability[m].duration = 0;
        }

        level->jet_q.push_back(temp);

    }


}





}




/*#############################
###############################
######## JET INITIALIZE #######
###############################
#############################*/

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
            lvl->gun_data[i].speed = 6.25;
            break;
        }
        case ADEN:
        {
            lvl->gun_data[i].damage = 35;
            lvl->gun_data[i].ammo_max = 240;
            lvl->gun_data[i].ammo_type = SLUG;
            lvl->gun_data[i].weap_delay = 5;
            lvl->gun_data[i].spread = 0.075;
            lvl->gun_data[i].speed = 6.75;
            break;
        }
        case GATLING:
        {
            lvl->gun_data[i].damage = 22;
            lvl->gun_data[i].ammo_max = 400;
            lvl->gun_data[i].ammo_type = SLUG;
            lvl->gun_data[i].weap_delay = 3;
            lvl->gun_data[i].spread = 0.03;
            lvl->gun_data[i].speed = 7.75;
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
            lvl->msl_data[i].alter_limit.alter.turn_speed = 0.025;
            lvl->msl_data[i].alter_limit.speed_limit[1] = 6.5;
            lvl->msl_data[i].alter_limit.speed_rate[1] = 0.2;
            lvl->msl_data[i].radius = 3;
            lvl->msl_data[i].targeting_angle = 1;
            lvl->msl_data[i].damage = 100;
            lvl->msl_data[i].ammo_max = 14;
            lvl->msl_data[i].decay = 130;
            break;
            case RAD:
            lvl->msl_data[i].alter_limit.turn_rate = 0.006;
            lvl->msl_data[i].alter_limit.alter.turn_speed = 0.03;
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
    for(int i = 0; i< ENUM_BOSS_TYPE_FIN; i++)
    {
        switch(i)
        {
            case MIG21:
            {
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0125;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0075;
            data->jet_data[i].alter_limit.alter.turn_speed = 0.03;
            data->jet_data[i].alter_limit.turn_rate = 0.0075;
            data->jet_data[i].alter_limit.mobility_coef = 1.0;
            data->jet_data[i].alter_limit.speed_limit[0] = 1.8;
            data->jet_data[i].alter_limit.default_speed = 2.6;
            data->jet_data[i].alter_limit.speed_limit[1] = 2.9;
            data->jet_data[i].hp = 100;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 0.8;
            data->jet_data[i].msl_mult = 1;
            data->jet_data[i].spc_mult = 0.9;
            data->jet_data[i].isBoss = 0;
            break;
            }
            case F4:
            {
            data->jet_data[i].alter_limit.alter.turn_speed = 0.026;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.mobility_coef = 1.0;
            data->jet_data[i].alter_limit.speed_limit[0] = 2.0;
            data->jet_data[i].alter_limit.default_speed = 2.4;
            data->jet_data[i].alter_limit.speed_limit[1] = 3.0;
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0125;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0075;
            data->jet_data[i].hp = 110;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 1.1;
            data->jet_data[i].msl_mult = 1.1;
            data->jet_data[i].spc_mult = 1.1;
            data->jet_data[i].isBoss = 0;
            break;
            }
            case F104:
            {
            data->jet_data[i].alter_limit.alter.turn_speed = 0.017;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.mobility_coef = 1.0;
            data->jet_data[i].alter_limit.speed_limit[0] = 2.4;
            data->jet_data[i].alter_limit.default_speed = 3.0;
            data->jet_data[i].alter_limit.speed_limit[1] = 3.4;
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0100;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0085;
            data->jet_data[i].hp = 90;
            data->jet_data[i].hitbox = 5;
            data->jet_data[i].gun_mult = 1;
            data->jet_data[i].msl_mult = 0.8;
            data->jet_data[i].spc_mult = 0.8;
            data->jet_data[i].isBoss = 0;
            break;
            }
            case HARRIER:
            {
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0100;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0150;
            data->jet_data[i].alter_limit.alter.turn_speed = 0.019;
            data->jet_data[i].alter_limit.turn_rate = 0.003;
            data->jet_data[i].alter_limit.mobility_coef = 1.0;
            data->jet_data[i].alter_limit.speed_limit[0] = 1.2;
            data->jet_data[i].alter_limit.default_speed = 1.9;
            data->jet_data[i].alter_limit.speed_limit[1] = 2.2;
            data->jet_data[i].hp = 110;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 1.1;
            data->jet_data[i].msl_mult = 1.4;
            data->jet_data[i].spc_mult = 1.4;
            data->jet_data[i].isBoss = 0;
            break;
            }
            case MIG29:
            {
            data->jet_data[i].alter_limit.alter.turn_speed = 0.026;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.mobility_coef = 1.0;
            data->jet_data[i].alter_limit.speed_limit[0] = 2.2;
            data->jet_data[i].alter_limit.default_speed = 2.6;
            data->jet_data[i].alter_limit.speed_limit[1] = 3.4;
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0125;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0095;
            data->jet_data[i].hp = 250;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 0.8;
            data->jet_data[i].msl_mult = 1.1;
            data->jet_data[i].spc_mult = 1.1;
            data->jet_data[i].isBoss = 1;
            break;
            }
            case SR91:
            {
            data->jet_data[i].alter_limit.alter.turn_speed = 0.017;
            data->jet_data[i].alter_limit.turn_rate = 0.009;
            data->jet_data[i].alter_limit.mobility_coef = 1.0;
            data->jet_data[i].alter_limit.speed_limit[0] = 3.0;
            data->jet_data[i].alter_limit.default_speed = 3.6;
            data->jet_data[i].alter_limit.speed_limit[1] = 4.4;
            data->jet_data[i].alter_limit.speed_rate[0] = 0.0100;
            data->jet_data[i].alter_limit.speed_rate[1] = 0.0085;
            data->jet_data[i].hp = 300;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].gun_mult = 1.1;
            data->jet_data[i].msl_mult = 1.1;
            data->jet_data[i].spc_mult = 1.1;
            data->jet_data[i].isBoss = 1;
            break;
            }


        }

    }
}

void boss_init(struct asset_data * data)
{
    for(int i = 0; i< ENUM_BOSS_TYPE_FIN-ENUM_JET_TYPE_FIN; i++)
    {
        switch(i)
        {
            case MIG29 - ENUM_JET_TYPE_FIN:
            {
            data->boss_data[i].ability[BOSS_ABILITY::RAND_POS] = 0;
            data->boss_data[i].ability[BOSS_ABILITY::DASH] = 1;
            data->boss_data[i].ability[BOSS_ABILITY::CMEASURE] = 1;
            }
            break;
            case SR91 - ENUM_JET_TYPE_FIN:
            {
            data->boss_data[i].ability[BOSS_ABILITY::RAND_POS] = 1;
            data->boss_data[i].ability[BOSS_ABILITY::DASH] = 1;
            data->boss_data[i].ability[BOSS_ABILITY::CMEASURE] = 1;
            }
            break;
        }
    }
    
}

void abl_init(struct asset_data * data)
{
    for(int i = 0; i< ENUM_BOSS_ABILITY_FIN; i++)
    {
        switch(i)
        {
            case BOSS_ABILITY::RAND_POS:
            {
            data->abl_data[i].cooldown = 60 * 7;
            data->abl_data[i].duration = 1;
            }
            break;
            case BOSS_ABILITY::DASH:
            {
            data->abl_data[i].cooldown = 60 * 3;
            data->abl_data[i].duration = 20;
            }
            break;
            case BOSS_ABILITY::CMEASURE:
            {
            data->abl_data[i].cooldown = 60 * 6;
            data->abl_data[i].duration = 60;
            }
            break;
        }
    }





}












