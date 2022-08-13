#include "jet.h"
#include "level.h"
#include "main.h"



ProjInst spawn_projectile(unsigned short type, Launcher * gun, struct state * pos, int target)
{
    ProjInst object {
            .type = type,
            .decay = gun->decay + gun->projectile->decay,
            .damage = gun->damage + gun->projectile->damage,
            .curr = {.x = pos->x, .y = pos->y, .turn_angle = pos->turn_angle, .speed = pos->speed + gun->velocity + gun->projectile->velocity },
            .status = {0},
            .launcher = gun,
            .target = target
        };


    if(type < ENUM_BULLET_TYPE_FIN)
    {
        object.color = {.r = (float)(rand()%20+230)/255, .g = (float)(rand()%20+190)/255, .b = (float)(rand()%10+30)/255, .a = 1};
        object.alter = nullptr;
    }
    else
    {
        object.color = {1,1,1,1};
        object.alter = new state_change;
        object.alter->acceleratable = 1;
        object.alter->rotatable = 1;
        object.alter->target_angle = object.curr.turn_angle;
        object.alter->speed_mode = 2;
        object.alter->turn_speed = 0;
        if(!gun->projectile->trait.DMGfall && object.target >= 0 && object.type == RAD_M)  object.decay +=50;
    }




return object;
}





void shoot(struct LevelInst * level, struct asset_data * asset)
{
    for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++)
    {
        for(int i = 0; i<3; i++)
        {
            if(object->weapon[i].ammo && object->weapon[i].magazine && object->weapon[i].engaged && !object->weapon[i].cooldown)
            {
                ProjInst shell = spawn_projectile(object->weapon[i].launcher->projectile - asset->proj_data,object->weapon[i].launcher,&object->curr,object->botTarget);
                if(shell.type >= ENUM_BULLET_TYPE_FIN)
                {
                    shell.isBotLaunched = object->isBot;
                }
                else
                {
                    shell.curr.turn_angle += (float)rand()/RAND_MAX * 2 * asset->laun_data[object->weapon[0].type].spread - asset->laun_data[object->weapon[0].type].spread;
                }
                shell.curr.x += cos(shell.curr.turn_angle)*(asset->jet_data[object->type].hitbox + asset->proj_data[shell.type].activation_radius + 0.5);
                shell.curr.y += sin(shell.curr.turn_angle)*(asset->jet_data[object->type].hitbox + asset->proj_data[shell.type].activation_radius + 0.5);


                level->proj_q.push_back(shell);
                object->weapon[i].ammo-=1;
                object->weapon[i].magazine -=1;
                object->weapon[i].cooldown = object->weapon[i].launcher->cooldown;
                object->weapon[i].replenish_cooldown = object->weapon[i].launcher->replenish_cooldown;
            }


        }
    }
    
    
}

void target(std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator target,float offset)
{
    float target_a[2];
    target_a[0] =  target->curr.x + offset*cos(target->curr.turn_angle)*target->curr.speed;
    target_a[1] = target->curr.y + offset*sin(target->curr.turn_angle)*target->curr.speed;

object->alter.target_angle = atan2(( target_a[1] - object->curr.y) ,(target_a[0] - object->curr.x));
}


std::vector<JetInst>::iterator findJet(std::vector<JetInst> & input_vec, int ID)
{
if(ID < 0) return input_vec.end();
for(std::vector<JetInst>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
if(object->ID == ID) return object;
}

return input_vec.end();
}



void target(struct LevelInst * level, struct asset_data * asset)
{
#pragma omp parallel for
for(std::vector<ProjInst>::iterator shell = level->proj_q.begin(); shell != level->proj_q.end(); shell++)
{
    if(shell->status[CONTROLLED] == 1 || asset->proj_data[shell->type].trait.targeting_angle == (float) 0.) continue;
    bool search_for_new_target = true;
    float new_target = shell->alter->target_angle;

    
    if(shell->type == RAD_M )
    {
        std::vector<JetInst>::iterator follow_target = findJet(level->jet_q,shell->target);
        if(follow_target != level->jet_q.end()) 
        {
            search_for_new_target = false;
            new_target = atan2(( follow_target->curr.y - shell->curr.y) ,(follow_target->curr.x - shell->curr.x));
        }
        else 
        {
            search_for_new_target = true;
        }
    }

    if(search_for_new_target == true)
    {
        float min_deviation = 2*PI;

        for(std::vector<JetInst>::iterator target = level->jet_q.begin(); target != level->jet_q.end(); target++)
        {
            float distance = sqrt( pow( target->curr.x - shell->curr.x ,2) +  pow(  target->curr.y - shell->curr.y ,2));
            float new_angle = atan2(( target->curr.y - shell->curr.y) ,(target->curr.x - shell->curr.x));
            float deviation = fabs(angle_difference(shell->curr.turn_angle,new_angle));
            switch(shell->type)
            {
                case IR_M: 
                {
                float angle_of_attack = fabs(angle_difference(shell->curr.turn_angle,target->curr.turn_angle)) ;
                if( angle_of_attack < PI/2  &&  distance < shell->decay * shell->curr.speed  &&  deviation  < asset->proj_data[shell->type].trait.targeting_angle )
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
                case RAD_M:
                {
                    if(distance < shell->decay * shell->curr.speed  &&  deviation  < asset->proj_data[shell->type].trait.targeting_angle && shell->isBotLaunched != target->isBot)
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
    }
    shell->alter->target_angle = new_target;
}


}








void action(struct LevelInst * level, struct asset_data * asset)
{
std::vector<JetInst>::iterator player = level->jet_q.begin();
//#pragma omp parallel for //blocked by rand
for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
{
for(int i =0; i< 3; i++) object->weapon[i].engaged = 0;
switch(object->mode)
{
    case PATROL:
    {
        object->alter.speed_mode = AIRBRAKE;
        if(object->at_work)
        {
            if(object->curr.turn_angle == object->alter.target_angle) object->at_work = 0;
            else break;
        }
        else
        { 
            float r = object->curr.speed/asset->jet_data[object->type].alter_limit.alter.turn_speed;
            if(object->curr.x <= 3*r || object->curr.x >= asset->lvl_data[level->level_name].map_width - 3*r || object->curr.y <= 3*r || object->curr.y >= asset->lvl_data[level->level_name].map_height-3*r)
            {
                int target_c[2] = {0,0};
                target_c[0] = asset->lvl_data[level->level_name].map_width/2 + rand()%240-120;
                target_c[1] = asset->lvl_data[level->level_name].map_height/14 * (rand()%13 + 1);
                object->alter.target_angle  = atan2(( target_c[1] - object->curr.y) ,(target_c[0] - object->curr.x));
                object->at_work = 1;
            }
            if(!object->at_work)
            {
                if(rand()%900 == 0) object->alter.target_angle = angle_addition(object->alter.target_angle, (float)0.6 *rand()/RAND_MAX - 0.3);
            }

        }
    }
    break;
    case PURSUIT:
    {
        target(object,player,16);
        if(fabs(rad_distance(object,player)) < asset->laun_data[object->weapon[0].type].spread) 
        {
            if(asset->jet_data[object->type].isBoss)
            {
                if(rand()%100 == 0 && distance(object,player) < (asset->proj_data[object->weapon[1].type].decay + asset->laun_data[object->weapon[1].type].decay) * asset->proj_data[object->weapon[1].type].alter_limit.speed_limit[1] ) object->weapon[1].engaged = 1; //boss launch missile
            }
            else
            {
                if(rand()%600 == 0) object->weapon[1].engaged = 1; //launch missile
            }

            
        }
        
        
        object->alter.speed_mode = AFTERBURNER;
    }
    break;
    case DOGFIGHT:
    {
        target(object,level->jet_q.begin(),8);
        float rad_dist = angle_difference(object->curr.turn_angle,object->alter.target_angle); //radial distance between object and target
        if(fabs(rad_dist) < asset->laun_data[object->weapon[0].type].spread )  object->weapon[0].engaged = 1; //gun
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
    case RETREAT:
    {
    object->alter.speed_mode = AFTERBURNER;
    std::vector<JetInst>::iterator follow_target = findJet(level->jet_q,object->botTarget);
    target(object,follow_target,8);
    }
    break;


}



}


}


bool elegibleForRetreat(std::vector<JetInst> & input_vec, std::vector<JetInst>::iterator object, std::vector<JetInst>::iterator reference, asset_data * asset)
{
    bool elegible = 0;
    if(input_vec.size() > 2)
    {
        float turn_angle_difference = fabs(angle_difference(object->curr.turn_angle,reference->curr.turn_angle));
        std::vector<JetInst>::iterator found = findJet(input_vec,object->botTarget);
        if(found != input_vec.end() && found != reference && distance(object,findJet(input_vec,object->botTarget)) > 150 && distance(object,reference) < 900) //ally exists and is retreating
        {
            elegible = 1;
        }
        else if(
            object->botTarget != -1   &&  //start retreating, had a target before but is disengaging now
            distance(object,reference) < 450 && fabs(rad_distance(object,reference)) > 2. * PI / 3. 
            && (turn_angle_difference > 3. * PI / 4. 
                || (turn_angle_difference > 2. * PI / 3. && asset->jet_data[object->type].alter_limit.speed_limit[1] > asset->jet_data[reference->type].alter_limit.speed_limit[1]   )   
                )
        )
        {
                float min_distance = 1400;
            for(std::vector<JetInst>::iterator ally = input_vec.begin(); ally != input_vec.end(); ally++)
            {
                if(ally == reference || ally == object || ally->mode != PATROL) continue;
                float ally_distance = distance(object,ally);
                if(fabs(rad_distance(object,ally)) < PI/3. && ally_distance < min_distance)
                {
                    min_distance = ally_distance;
                    object->botTarget = ally->ID;
                    elegible = 1;
                }
            }
        }
    }
    if(elegible && 
    fabs(rad_distance(object,reference)) > (float) PI / 2. && fabs(angle_difference(object->curr.turn_angle,reference->curr.turn_angle)) > (float) PI / .2
    ) object->at_work = true;
    else object->at_work = false;

     

    return elegible;
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
    bool retreating = 0;



    if(limit->jet_data[object->type].isBoss)
    {
        triggered = 1;
        if(
            limit->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[BOSS_ABILITY::DASH] &&
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
            limit->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[BOSS_ABILITY::RAND_POS] &&
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
        float conditionlessSpot = 160 + (1-(object->hp / limit->jet_data[object->type].hp)) * 120;
        
        if(
            (object->mode == PATROL   &&  (dist < 350 && fabs(rad_distance(object,player)) < PI/6 || dist < conditionlessSpot) ) ||
            (object->mode == DOGFIGHT) ||
            (object->mode == PURSUIT &&  dist < 350) ||
            (object->mode == RETREAT)
        )
        {
            if(object->hp > 30)
            {
                triggered = 1;
            }else
            {
                if(elegibleForRetreat(input_vec,object,player,limit))
                {
                    if(object->at_work) object->mode = RETREAT;
                    else //surroundings check
                    {
                        if(dist<conditionlessSpot) 
                        {
                            //printf("Our angle difference: %.2f\n",fabs(angle_difference(object->curr.turn_angle,player->curr.turn_angle)));
                            triggered = 1;
                        }
                        
                        else object->mode = RETREAT;
                    }
                }
                else
                {
                    triggered = 1;
                }
            }
        }
        else
        {
            object->mode = PATROL;
            object->botTarget = -1;
            object->at_work = 0;
        }

    }






    if(triggered)
    {
        if(dist < 160) object->mode = DOGFIGHT;
        else object->mode = PURSUIT;
        object->at_work = 0;
        object->botTarget = 0;
    }

}

if(input_vec.size() > 2)
{
    #pragma omp parallel for
    for(std::vector<JetInst>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
        {
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
                                if(aly_dist < 450 && ally->mode != RETREAT)
                                {
                                    if(aly_dist < 160) ally->mode = DOGFIGHT;
                                    else ally->mode = PURSUIT;
                                    ally->botTarget = 0;
                                    ally->at_work = 0;
                                }
                            }
                            else
                            {
                                if(obj_dist < 450 && object->mode != RETREAT)
                                {
                                    if(obj_dist < 160) object->mode = DOGFIGHT;
                                    else object->mode = PURSUIT;
                                    object->botTarget = 0;
                                    object->at_work = 0;
                                }
                            }
                        }
                    }
                }

    }
}






}

LaunInst launcher_spawn(Jet * jet, Launcher * launcher, unsigned short type, unsigned short slot)
{
LaunInst object {
.engaged = 0,
.type = type,
.ammo = jet->weapon_mult[slot] * launcher->ammo,
.magazine = launcher->magazine,
.cooldown = 0,
.replenish_cooldown = 0,
.launcher = launcher
};
return object;


}




JetInst jet_spawn(struct asset_data * asset, struct selection* selected,state_change_limit * overwrite,bool bot,unsigned short ID)
{
    JetInst object = {
        .ID = ID,
        .type = selected->player_jet,
        .hp = asset->jet_data[selected->player_jet].hp,
        .curr = {.x = -1, 
                    .y = -1, 
                    .turn_angle = (bot ? PI/2 : 0),
                    .speed = asset->jet_data[selected->player_jet].alter_limit.default_speed},
        .alter = {
                .turn_speed = 0.,
                .speed_mode = STANDARD,
                .target_angle = (bot ? ( rand()%2 ?  (float)rand()/RAND_MAX *1.5 + PI/2  : (float) rand()/RAND_MAX *(-1.5) - PI/2  )    :  0.0     ),
                .rotatable = true,
                .acceleratable = true
        },
        .mode = STANDARD,
        .at_work = false,
        .status = {0,0},
        .isBot = bot,
        .botTarget = -1,
        .ability = nullptr,
        .overwrite_limit = (overwrite ? overwrite : nullptr)
    };

    for(int i = 0; i<3; i++) object.weapon[i] = launcher_spawn(&asset->jet_data[selected->player_jet],&asset->laun_data[selected->weapon[i]],selected->weapon[i],i);
    return object;
}

void enemy_spawn(struct LevelInst * level, struct asset_data * asset)
{
int enemy_amount = 0;

float map_width = asset->lvl_data[level->level_name].map_width;
float map_height = asset->lvl_data[level->level_name].map_height;

for(int i = 0; i<ENUM_BOSS_TYPE_FIN;  i++) enemy_amount += asset->lvl_data[level->level_name].enemy_quality[i];
float x = (float) map_width*0.8, y = (float) map_height/(enemy_amount+1);

struct selection templat[ENUM_BOSS_TYPE_FIN] = {
{.player_jet = MIG21,.weapon = {SHVAK, INFRARED,FLAK} },
{.player_jet = F4, .weapon= {GATLING,RADAR,FLAK}},
{.player_jet = F104, .weapon= {GATLING,INFRARED,FLAK}},
{.player_jet = HARRIER, .weapon= {ADEN,INFRARED,FLAK}},
{.player_jet = MIG29, .weapon= {SHVAK,RADAR,FLAK}},
{.player_jet = SR91, .weapon= {GATLING,RADAR,FLAK}}
};





std::copy(asset->lvl_data[level->level_name].enemy_quality,asset->lvl_data[level->level_name].enemy_quality+ENUM_BOSS_TYPE_FIN,level->enemy_quality);


for(int i = 0; i<ENUM_JET_TYPE_FIN;  i++)
{
    for(int q = 0; q< level->enemy_quality[i]; q++, level->nextID++)
    {
        JetInst temp = jet_spawn(asset,templat+i,nullptr,1,level->nextID);
        temp.curr.x = x + rand()%30-15;
        temp.curr.y = y;
        level->jet_q.push_back(temp);
        y += (float) asset->lvl_data[level->level_name].map_height/(enemy_amount+1);

    }
}
for(int i = ENUM_JET_TYPE_FIN; i< ENUM_BOSS_TYPE_FIN; i++, level->nextID++)
{
    for(int q = 0; q< level->enemy_quality[i]; q++)
    {
        JetInst temp = jet_spawn(asset,templat+i,nullptr,1,level->nextID);
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

void launcher_init(struct asset_data * asset)
{
Launcher object[ENUM_LAUNCHER_TYPE_FIN]{
{ //SHVAK
.decay = 35,
.damage = 45,
.velocity = 1,
.cooldown = 8,
.replenish_cooldown = 14,
.ammo = 180,
.magazine = 25,
.spread = 0.05,
.projectile = asset->proj_data + SLUG
},
{ //ADEN
.decay = 30,
.damage = 15,
.velocity = 1.5,
.cooldown = 5,
.replenish_cooldown = 16,
.ammo = 240,
.magazine = 40,
.spread = 0.075,
.projectile = asset->proj_data + SLUG
},
{ //GATLING
.decay = 30,
.damage = 2,
.velocity = 1.75,
.cooldown = 3,
.replenish_cooldown = 18,
.ammo = 400,
.magazine = 180,
.spread = 0.03,
.projectile = asset->proj_data + SLUG
},
{ //Infrared
.decay = 30,
.damage = 0,
.velocity = 0,
.cooldown = 60,
.replenish_cooldown = 120,
.ammo = 14,
.magazine = 2,
.spread = 0.03,
.projectile = asset->proj_data + IR_M
},
{ //Radar
.decay = 30,
.damage = 0,
.velocity = 0,
.cooldown = 120,
.replenish_cooldown = 120,
.ammo = 10,
.magazine = 1,
.spread = 0.03,
.projectile = asset->proj_data + RAD_M
},
{ //FLAK
.decay = 35,
.damage = 45,
.velocity = 1.75,
.cooldown = 15,
.replenish_cooldown = 30,
.ammo = 30,
.magazine = 5,
.spread = 0.05,
.projectile = asset->proj_data + AIRBURST
},


};


std::copy(object,object+ENUM_LAUNCHER_TYPE_FIN,asset->laun_data);

}





void projectile_init(struct asset_data * asset)
{

Projectile object[ENUM_PROJECTILE_TYPE_FIN] { 
    
    { //bullet
        .decay = 60,
        .damage = 20,
        .velocity = 2,
        .alter_limit = {
            .alter = {.turn_speed = 0,.speed_mode = 1,.rotatable = 0, .acceleratable = 0},
            .turn_rate = 0,.speed_rate = {0,0},.speed_limit = {0,10},1
            },
        .radius = 0,
        .activation_radius = 0,
        .trait = {.targeting_angle = 0, .draw_width = 0.7, .draw_height = 1.2, .hitCircular = 1, .isAOE = 0, .DMGfall = 1},
    },
    { //airburst
        .decay = 60,
        .damage = 40,
        .velocity = 2,
        .alter_limit = {
            .alter = {.turn_speed = 0,.speed_mode = 1,.rotatable = 0, .acceleratable = 0},
            .turn_rate = 0,.speed_rate = {0,0},.speed_limit = {0,10},1
            },
        .radius = 24,
        .activation_radius = 6,
        .trait = {.targeting_angle = 0, .draw_width = 0.7, .draw_height = 1.2, .hitCircular = 0, .isAOE = 1, .DMGfall = 0},
    },
    { //infrared
        .decay = 100,
        .damage = 100,
        .velocity = 0,
        .alter_limit = {
            .alter = {.turn_speed = 0.025,.speed_mode = 2,.rotatable = 1, .acceleratable = 1},
            .turn_rate = 0.005,.speed_rate = {0,0.2},.speed_limit = {0,6.5},1
            },
        .radius = 5,
        .activation_radius = 3,
        .trait = {.targeting_angle = 1, .draw_width = 1.0, .draw_height = 1.0, .hitCircular = 1, .isAOE = 1, .DMGfall = 0},
    },
    { //radar
        .decay = 150,
        .damage = 100,
        .velocity = 0,
        .alter_limit = {
            .alter = {.turn_speed = 0.03,.speed_mode = 2,.rotatable = 1, .acceleratable = 1},
            .turn_rate = 0.006,.speed_rate = {0,0.2},.speed_limit = {0,5.5},1
            },
        .radius = 5,
        .activation_radius = 3,
        .trait = {.targeting_angle = 0.7, .draw_width = 1.0, .draw_height = 1.0, .hitCircular = 1, .isAOE = 1, .DMGfall = 0},
    },





};


std::copy(object,object+ENUM_PROJECTILE_TYPE_FIN,asset->proj_data);

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
            data->jet_data[i].weapon_mult[0] = 0.8;
            data->jet_data[i].weapon_mult[1] = 1;
            data->jet_data[i].weapon_mult[2] = 0.9;
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
            data->jet_data[i].weapon_mult[0] = 1.1;
            data->jet_data[i].weapon_mult[1] = 1.1;
            data->jet_data[i].weapon_mult[2] = 1.1;
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
            data->jet_data[i].weapon_mult[0] = 1;
            data->jet_data[i].weapon_mult[1] = 0.8;
            data->jet_data[i].weapon_mult[2] = 0.8;
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
            data->jet_data[i].alter_limit.speed_limit[1] = 2.3;
            data->jet_data[i].hp = 110;
            data->jet_data[i].hitbox = 6;
            data->jet_data[i].weapon_mult[0] = 1.1;
            data->jet_data[i].weapon_mult[1] = 1.4;
            data->jet_data[i].weapon_mult[2] = 1.4;
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
            data->jet_data[i].weapon_mult[0] = 0.8;
            data->jet_data[i].weapon_mult[1] = 1.1;
            data->jet_data[i].weapon_mult[2] = 1.1;
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
            data->jet_data[i].weapon_mult[0] = 1.1;
            data->jet_data[i].weapon_mult[1] = 1.1;
            data->jet_data[i].weapon_mult[2] = 1.1;
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












