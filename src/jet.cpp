#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"



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
        object.color = {(rand()%20+230), (rand()%20+190), (rand()%10+30), 255};
        object.alter = nullptr;
    }
    else
    {
        object.color = {255,255,255,255};
        object.alter = new state_change;
        object.alter->acceleratable = 1;
        object.alter->rotatable = 1;
        object.alter->target_angle = object.curr.turn_angle;
        object.alter->target_speed = gun->projectile->alter_limit.speed_limit[1];
        object.alter->turn_speed = 0;
        if(!gun->projectile->trait.DMGfall && object.target >= 0 && object.type == RAD_M)  object.decay +=50;
    }




return object;
}





void shoot(struct LevelInst * level, struct asset_data * asset)
{
    #pragma omp parallel for
    for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++)
    {
        for(int i = 0; i<3; i++) //weapon iteration
        {



            if(object->weapon[i].engaged && !object->weapon[i].cooldown)
            {
                int n = 0;
                for(n; n< object->weapon[i].launcher->multishot && object->weapon[i].ammo && object->weapon[i].magazine; n++)
                {
                        unsigned short proj_type = object->weapon[i].launcher->projectile - asset->proj_data;

                        ProjInst shell = spawn_projectile(proj_type,object->weapon[i].launcher,&object->curr,object->botTarget);
                        
                        if(asset->laun_data[object->weapon[i].type].wingMounted)
                        {
                            bool GenerateLeft = object->weapon[i].ammo%2;
                            transform(&shell.curr,asset->lvl_data[level->level_name].map_width,asset->lvl_data[level->level_name].map_height, 2.f + (float) rand()/RAND_MAX * 2.f, angle_addition(shell.curr.turn_angle,(GenerateLeft ?  -PI/2  :  PI/2  )));
                        }
                        /*
                        #pragma omp critical
                        {
                            if(n < 2)
                            {
                            if(shell.type >= ENUM_BULLET_TYPE_FIN) //missile
                            {
                                shell.isBotLaunched = object->isBot;
                                al_play_sample(asset->sound[3],1.0,0,1.0,ALLEGRO_PLAYMODE_ONCE,0);
                            }
                            else
                            {
                                ALLEGRO_SAMPLE * sound = nullptr;
                                switch(object->weapon[i].type)
                                {
                                    case SHVAK: sound = asset->sound[0]; break;
                                    case ADEN: sound = asset->sound[1]; break;
                                    case GATLING: sound = asset->sound[2]; break;
                                    case GUNPOD: sound = asset->sound[2]; break;
                                }
                                al_play_sample(sound,1.0,0,1.0,ALLEGRO_PLAYMODE_ONCE,0);


                            }
                            }
                        }*/
                        shell.curr.x += cos(shell.curr.turn_angle)*(asset->jet_data[object->type].hitbox + asset->proj_data[shell.type].activation_radius + 0.5);
                        shell.curr.y += sin(shell.curr.turn_angle)*(asset->jet_data[object->type].hitbox + asset->proj_data[shell.type].activation_radius + 0.5);
                        shell.curr.turn_angle += (float)rand()/RAND_MAX * 2 * asset->laun_data[object->weapon[0].type].spread - asset->laun_data[object->weapon[0].type].spread;
                        #pragma omp critical
                        level->proj_q.push_back(shell);
                        object->weapon[i].ammo-=1;
                        object->weapon[i].magazine -=1;
                    
                }
                if(n)
                {
                    object->weapon[i].cooldown = object->weapon[i].launcher->cooldown; //apply once
                    object->weapon[i].replenish_cooldown = object->weapon[i].launcher->replenish_cooldown; //apply once
                    if(asset->laun_data[object->weapon[i].type].recoil) //apply once
                    {
                        accelerate(&object->curr, &object->alter, (object->overwrite_limit ? object->overwrite_limit : &asset->jet_data[object->type].alter_limit   ),asset->laun_data[object->weapon[i].type].recoil);
                    }
                }

            }



        }
    }
    
    
}

float getTargetAngle(state * object, state * target,float offset)
{
    float target_x =  target->x + offset*cos(target->turn_angle)*target->speed;
    float target_y = target->y + offset*sin(target->turn_angle)*target->speed;

return atan2(( target_y - object->y) ,(target_x - object->x));
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

//returns true if is safe to shoot
bool firendly_fire_check(std::vector<JetInst> & input_vec, std::vector<JetInst>::iterator reference, int slot)
{
    bool check = true;
    float range_check =  reference->weapon[slot].launcher->projectile->trait.targeting_angle/2 + reference->weapon[slot].launcher->spread;


    if(!reference->weapon[slot].launcher->projectile->trait.isRadarGuided)
    {
        for(int i = 0; i< input_vec.size(); i++)
        {
            auto object = input_vec.begin() + i;
            if(object != reference && object->isBot == reference->isBot)
            {
                float distance_between = distance(&reference->curr,&object->curr);

                if( distance_between > 10 && distance_between < 350 && fabs(rad_distance(&reference->curr,&object->curr)) < range_check) 
                {
                    check = false;
                }
                
                
            }
        }
    }
    return check;
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
        object->alter.target_speed = asset->jet_data[object->type].alter_limit.speed_limit[0];
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
        object->alter.target_angle = getTargetAngle(&object->curr,&player->curr,16);
        float radial_distance = rad_distance(&object->curr,&player->curr);

        if(fabs(radial_distance) < object->weapon[1].launcher->projectile->trait.targeting_angle/2.f) 
        {
            if(asset->jet_data[object->type].isBoss)
            {
                if(rand()%100 == 0 && distance(&object->curr,&player->curr) < (asset->proj_data[object->weapon[1].type].decay + asset->laun_data[object->weapon[1].type].decay) * asset->proj_data[object->weapon[1].type].alter_limit.speed_limit[1] && firendly_fire_check(level->jet_q,object,1)) object->weapon[1].engaged = 1; //boss launch missile
            }
            else
            {
                if(rand()%600 == 0 && firendly_fire_check(level->jet_q,object,1))
                {
                    object->weapon[1].engaged = 1; //launch missile
                }
                

            }
            
            
        }
        if(fabs(radial_distance) < object->weapon[2].launcher->spread)
        {
            if(object->weapon[2].type == RAILGUN)
            {
                if(distance(&object->curr,&player->curr) < asset->config.fadeDistance + asset->config.fadingLength  && firendly_fire_check(level->jet_q,object,2)) object->weapon[2].engaged = 1;
            }
        }






        
        
        object->alter.target_speed = asset->jet_data[object->type].alter_limit.speed_limit[1];
    }
    break;
    case DOGFIGHT:
    {
        object->alter.target_angle = getTargetAngle(&object->curr,&level->jet_q.begin()->curr,8);
        float rad_dist = angle_difference(object->curr.turn_angle,object->alter.target_angle); //radial distance between object and target
        if(fabs(rad_dist) < asset->laun_data[object->weapon[0].type].spread && firendly_fire_check(level->jet_q,object,0)) 
        {
            object->weapon[0].engaged = 1; //gun
        }

        



        if(fabs(rad_dist) > PI/2)
        {
            if(object->curr.speed > player->curr.speed) object->alter.target_speed = asset->jet_data[object->type].alter_limit.speed_limit[1];
            else object->alter.target_speed = asset->jet_data[object->type].alter_limit.default_speed;
        }
        else 
        {
            if(object->curr.speed < player->curr.speed) object->alter.target_speed = asset->jet_data[object->type].alter_limit.speed_limit[0];
            else object->alter.target_speed = asset->jet_data[object->type].alter_limit.default_speed;
        }
    }
    break;
    case RETREAT:
    {
    object->alter.target_speed = asset->jet_data[object->type].alter_limit.speed_limit[1];
    std::vector<JetInst>::iterator follow_target = findJet(level->jet_q,object->botTarget);
    object->alter.target_angle = getTargetAngle(&object->curr,&follow_target->curr,8);
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
        if(found < input_vec.end() && found != reference && distance(&object->curr,&((findJet(input_vec,object->botTarget))->curr)) > 150 && distance(&object->curr,&reference->curr) < 900) //ally exists and is retreating
        {
            //printf("Keeping retreat up!\n");
            elegible = 1;
        }
        else if(
            object->botTarget != -1   &&  //start retreating, had a target before but is disengaging now
            distance(&object->curr,&reference->curr) < 450 && fabs(rad_distance(&object->curr,&reference->curr)) > 2. * PI / 3. 
            && (turn_angle_difference > 3. * PI / 4. 
                || (turn_angle_difference > 2. * PI / 3. && asset->jet_data[object->type].alter_limit.speed_limit[1] > asset->jet_data[reference->type].alter_limit.speed_limit[1]   )   
                )
        )
        {
                float min_distance = 1000;
            for(std::vector<JetInst>::iterator ally = input_vec.begin(); ally != input_vec.end(); ally++)
            {
                if(ally == reference || ally == object || ally->mode != PATROL) continue;
                float ally_distance = distance(&object->curr,&ally->curr);
                if(fabs(rad_distance(&object->curr,&ally->curr)) < PI/4. && ally_distance < min_distance)
                {
                    min_distance = ally_distance;
                    object->botTarget = ally->ID;
                    elegible = 1;
                }
            }
            //if(elegible) printf("Retreat engaged. My ID: %d, Target ID: %d, At work: %d\n", object->ID, object->botTarget,object->at_work);
        }
    }
    if(elegible && 
    fabs(rad_distance(&object->curr,&reference->curr)) > (float) PI / 2.f && fabs(angle_difference(object->curr.turn_angle,reference->curr.turn_angle)) > (float) PI / 2.f
    ) object->at_work = true;
    else object->at_work = false;

     

    return elegible;
}


void decision_graph(std::vector<JetInst> &input_vec, struct asset_data * limit)
{
std::vector<JetInst>::iterator player = input_vec.begin();
#pragma omp parallel for
for(std::vector<JetInst>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
{
    float dist = distance(&object->curr,&player->curr);
    switch (object->mode)
    {
        case PURSUIT:
            if(dist < 160) object->mode = DOGFIGHT;
            if(dist > 400) object->mode = PATROL;
        break;
        case PATROL:
            float conditionlessSpot = 160 + (1-(object->hp / limit->jet_data[object->type].hp)) * 140;
            if( dist < conditionlessSpot || (dist < 350 && fabs(rad_distance(&object->curr,&player->curr)) < PI/6 ) )
            object->mode = DOGFIGHT;
        break;
        case DOGFIGHT:
            if(dist >= 160) object->mode = DOGFIGHT;
            if(object->hp <= 30 && elegibleForRetreat(input_vec,object,player,limit)) object->mode = RETREAT;
        break;
        case RETREAT:
            if(fabs(rad_distance(&object->curr,&player->curr)) < PI / 2.
            && 
            fabs(angle_difference(object->curr.turn_angle,player->curr.turn_angle)) < (float) PI / 2.f
            )
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
    float dist = distance(&object->curr,&player->curr);

    bool triggered = 0;
    bool retreating = 0;



    if(limit->jet_data[object->type].isBoss)
    {
        triggered = 1;
        if(
            limit->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[BOSS_ABILITY::DASH] &&
            object->ability[BOSS_ABILITY::DASH].cooldown == 0 &&
            (
            (dist > 350 && fabs(rad_distance(&object->curr,&player->curr)) < PI/6) || (dist < 350 && fabs(rad_distance(&object->curr,&player->curr)) > 2*PI/3)
            )
        )
        {
            object->ability[BOSS_ABILITY::DASH].cooldown = limit->abl_data[BOSS_ABILITY::DASH].cooldown;
            object->ability[BOSS_ABILITY::DASH].duration = limit->abl_data[BOSS_ABILITY::DASH].duration;
        };

        if(limit->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[BOSS_ABILITY::RAND_POS])
        {
            player->status[OBSCURE] = object->ID;
            if(
                object->ability[BOSS_ABILITY::RAND_POS].cooldown == 0 &&
                (dist < 350 && dist > 150 && fabs(rad_distance(&object->curr,&player->curr)) > 3*PI/5)
            )
            {
                object->ability[BOSS_ABILITY::RAND_POS].cooldown = limit->abl_data[BOSS_ABILITY::RAND_POS].cooldown;
                object->ability[BOSS_ABILITY::RAND_POS].duration = limit->abl_data[BOSS_ABILITY::RAND_POS].duration;
            };
        }



    }
    else
    {
        float conditionlessSpot = 160 + (1-(object->hp / limit->jet_data[object->type].hp)) * 140;
        
        if(
            (object->mode == PATROL   &&  ( dist < conditionlessSpot || (dist < 350 && fabs(rad_distance(&object->curr,&player->curr)) < PI/6 ) ) ) ||
            (object->mode == DOGFIGHT) ||
            (object->mode == PURSUIT &&  dist < 400) ||
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
                        if(dist < conditionlessSpot || (dist < 350 && fabs(rad_distance(&object->curr,&player->curr)) > 5*PI/6 ) ) 
                        {
                            //printf("Our radial distance: %.2f, our angle difference: %.2f\n",fabs(rad_distance(&object->curr,&player->curr)),fabs(angle_difference(object->curr.turn_angle,player->curr.turn_angle)));
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
        float obj_dist = distance(&object->curr,&player->curr);
        for(std::vector<JetInst>::iterator ally = object+1; ally != input_vec.end(); ally++)
                {
                    if( (object->mode != PATROL) == (ally->mode != PATROL)) continue;
                    float dist_between = distance(&object->curr,&ally->curr);
                    float aly_dist = distance(&ally->curr,&player->curr);
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

LaunInst launcher_spawn( Launcher * launcher, unsigned short type, float multiplier)
{
LaunInst object {
.engaged = 0,
.multiplier = multiplier,
.type = type,
.ammo = multiplier * static_cast<float>(launcher->ammo),
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
                .target_angle = (bot ? ( rand()%2 ?  (float)rand()/RAND_MAX *1.5 + PI/2  : (float) rand()/RAND_MAX *(-1.5) - PI/2  )    :  0.0     ),
                .target_speed = asset->jet_data[selected->player_jet].alter_limit.default_speed,
                .rotatable = true,
                .acceleratable = true
        },
        .mode = STANDARD,
        .at_work = false,
        .status = {0,-1},
        .isBot = bot,
        .botTarget = -1,
        .ability = nullptr,
        .overwrite_limit = (overwrite ? overwrite : nullptr)
    };

    for(int i = 0; i<3; i++) object.weapon[i] = launcher_spawn(&asset->laun_data[selected->weapon[i]],selected->weapon[i],asset->jet_data[selected->player_jet].weapon_mult[i] * selected->multiplier[i]);
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
{.player_jet = MIG21,.weapon = {SHVAK, INFRARED,FLAK}, .multiplier = {0.7,0.7,0.f} },
{.player_jet = F4, .weapon= {GATLING,RADAR,FLAK},.multiplier = {0.7,0.7,0.f}},
{.player_jet = F104, .weapon= {GATLING,INFRARED,FLAK}, .multiplier = {0.7,0.7,0.f}},
{.player_jet = HARRIER, .weapon= {ADEN,INFRARED,ZUNI}, .multiplier = {0.7,0.7,0.2}},
{.player_jet = MIG29, .weapon= {SHVAK,RADAR,ZUNI}, .multiplier = {1.0,1.0,0.f}},
{.player_jet = SR91, .weapon= {GATLING,RADAR,RAILGUN}, .multiplier = {1.0,1.0,1.7}}
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
    .recoil = -0.06,
    .spread = 0.05,
    .multishot = 1,
    .wingMounted = false,
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
    .recoil = -0.05,
    .spread = 0.075,
    .multishot = 2,
    .wingMounted = false,
    .projectile = asset->proj_data + SLUG
},
{ //GATLING
    .decay = 30,
    .damage = 9,
    .velocity = 1.75,
    .cooldown = 3,
    .replenish_cooldown = 18,
    .ammo = 400,
    .magazine = 180,
    .recoil = -0.03,
    .spread = 0.03,
    .multishot = 1,
    .wingMounted = false,
    .projectile = asset->proj_data + SLUG
},
{ //Infrared
    .decay = 30,
    .damage = 0,
    .velocity = 0,
    .cooldown = 40,
    .replenish_cooldown = 120,
    .ammo = 14,
    .magazine = 2,
    .recoil = 0.,
    .spread = 0.03,
    .multishot = 1,
    .wingMounted = true,
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
    .recoil = 0.,
    .spread = 0.03,
    .multishot = 1,
    .wingMounted = true,
    .projectile = asset->proj_data + RAD_M
},
{ //FLAK
    .decay = 35,
    .damage = 45,
    .velocity = 1.75,
    .cooldown = 20,
    .replenish_cooldown = 30,
    .ammo = 30,
    .magazine = 4,
    .recoil = -0.25,
    .spread = 0.05,
    .multishot = 1,
    .wingMounted = false,
    .projectile = asset->proj_data + AIRBURST
},
{ //ZUNI
    .decay = 30,
    .damage = 0,
    .velocity = 0,
    .cooldown = 8,
    .replenish_cooldown = 12,
    .ammo = 32,
    .magazine = 24,
    .recoil = 0.,
    .spread = 0.08,
    .multishot = 2,
    .wingMounted = true,
    .projectile = asset->proj_data + UNGUIDED
},
{ //RAILGUN
    .decay = 60,
    .damage = 80,
    .velocity = 14.75,
    .cooldown = 90,
    .replenish_cooldown = 180,
    .ammo = 14,
    .magazine = 5,
    .recoil = -0.25,
    .spread = 0.005,
    .multishot = 1,
    .wingMounted = false,
    .projectile = asset->proj_data + SLUG
},
{ //GUNPOD
    .decay = 30,
    .damage = 2,
    .velocity = 1.75,
    .cooldown = 6,
    .replenish_cooldown = 22,
    .ammo = 200,
    .magazine = 120,
    .recoil = -0.15,
    .spread = 0.03,
    .multishot = 2,
    .wingMounted = true,
    .projectile = asset->proj_data + SLUG
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
            .alter = {.turn_speed = 0,.rotatable = 0, .acceleratable = 0},
            .turn_rate = 0,.speed_rate = {0,0},.speed_limit = {0,10},1
            },
        .radius = 1,
        .activation_radius = 1,
        .trait = {.targeting_angle = 0, .draw_width = 0.7, .draw_height = 1.2, .hitCircular = 1, .isAOE = 0, .DMGfall = 1, .isRadarGuided = false},
    },
    { //airburst
        .decay = 60,
        .damage = 40,
        .velocity = 2,
        .alter_limit = {
            .alter = {.turn_speed = 0,.rotatable = 0, .acceleratable = 0},
            .turn_rate = 0,.speed_rate = {0,0},.speed_limit = {0,10},1
            },
        .radius = 20,
        .activation_radius = 8,
        .trait = {.targeting_angle = 0, .draw_width = 1.2, .draw_height = 0.7, .hitCircular = 0, .isAOE = 1, .DMGfall = 0,.isRadarGuided = false},
    },
    { //infrared
        .decay = 100,
        .damage = 100,
        .velocity = 0,
        .alter_limit = {
            .alter = {.turn_speed = 0.025,.rotatable = 1, .acceleratable = 1},
            .turn_rate = 0.005,.speed_rate = {0,0.2},.speed_limit = {0,6.5},1
            },
        .radius = 7,
        .activation_radius = 3,
        .trait = {.targeting_angle = 1, .draw_width = 1.0, .draw_height = 1.0, .hitCircular = 1, .isAOE = 1, .DMGfall = 0,.isRadarGuided = false},
    },
    { //radar
        .decay = 150,
        .damage = 100,
        .velocity = 0,
        .alter_limit = {
            .alter = {.turn_speed = 0.03,.rotatable = 1, .acceleratable = 1},
            .turn_rate = 0.006,.speed_rate = {0,0.2},.speed_limit = {0,5.5},1
            },
        .radius = 7,
        .activation_radius = 3,
        .trait = {.targeting_angle = 0.7, .draw_width = 1.0, .draw_height = 1.0, .hitCircular = 1, .isAOE = 1, .DMGfall = 0,.isRadarGuided = true},
    },
    { //zuni
        .decay = 100,
        .damage = 90,
        .velocity = 0,
        .alter_limit = {
            .alter = {.turn_speed = 0.f,.rotatable = 0, .acceleratable = 1},
            .turn_rate = 0.f,.speed_rate = {0,0.1},.speed_limit = {0,5.5}, .mobility_coef = 1.f
            },
        .radius = 7,
        .activation_radius = 3,
        .trait = {.targeting_angle = 0.f, .draw_width = 1.0, .draw_height = 1.0, .hitCircular = 1, .isAOE = 1, .DMGfall = 0,.isRadarGuided = false},
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












