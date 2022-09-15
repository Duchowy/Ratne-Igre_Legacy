#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"
#include "render_level.h"
#include <chrono>

int comparator(const void * fa, const void * fb)
{
    return (*((uint8_t*)fa) - *((uint8_t*)fb));
}

state_change_limit process_riven(riven * mod, state_change_limit * base)
{
    state_change_limit overwrite = *base;
    for(int i = 0; i < (mod->dualSided ? 4 : 2 ); i++ )
    {
        //printf("%d: ",mod->type[i]);
        switch(mod->type[i])
        {
        case 1: overwrite.speed_limit[1] *= mod->value[i]; break; //uper speed limit
        case 2: overwrite.speed_limit[0] *= 2. - mod->value[i]; break; //lower speed limit
        case 3: overwrite.speed_limit[1] *= mod->value[i]; overwrite.speed_limit[0] *= 2. - mod->value[i]; break;
        case 4: overwrite.speed_rate[1] *= mod->value[i]; break;
        case 5: overwrite.speed_rate[0] *= mod->value[i]; break;
        case 6: overwrite.speed_rate[1] *= mod->value[i]; overwrite.speed_rate[0] *= mod->value[i]; break;
        case 7: overwrite.alter.turn_speed += mod->value[i]; break;
        case 8: overwrite.turn_rate += mod->value[i]; break;
        case 9: overwrite.default_speed += mod->value[i]; break;
        case 10: overwrite.mobility_coef -= mod->value[i]; break;
        }
        //printf("%f\n",mod->value[i]);
    }
    return overwrite;
}


void refresh_riven(struct LevelInst * level,struct asset_data * asset)
{
    for(int i = 0; i< ENUM_JET_TYPE_FIN; i++)
    {
        if(level->player.mod[i].engaged)
        {
            if(!level->player.custom_stat[i]) level->player.custom_stat[i] = new state_change_limit;
            *level->player.custom_stat[i] = process_riven(&level->player.mod[i],&asset->jet_data[i].alter_limit);
        }
        else
        {
            if(level->player.custom_stat[i]) delete level->player.custom_stat[i];
            level->player.custom_stat[i] = nullptr;
        }

    }


}


riven * spawn_riven()
{
    riven mod {
        .engaged = true,
        .dualSided = (rand()%4 == 0 ? true: false),
    };
    mod.type[2] = 0;
    mod.type[3] = 0;


        short pos = 0;
        do
        {
            mod.type[pos] = rand()%10+1;
            bool unique = 1;
            for(int i = 0; i< pos; i++)
            {
                if(
                    mod.type[i] == mod.type[pos] || 
                    ((mod.type[i]-1)/3 == 0 && (mod.type[pos]-1)/3 == 0) || //v limits
                    ((mod.type[i]-1)/3 == 1 && (mod.type[pos]-1)/3 == 1) //engine power limits
                )
                {
                    unique = 0;
                    break;
                } 
            }
            if(!unique) mod.type[pos] = (mod.type[pos] + 1 > 10 ? 1 : mod.type[pos] + 1);
            else pos+=1;
        } while (pos < (mod.dualSided ? 4 : 2));


        qsort(mod.type,(mod.dualSided ? 4 : 2 ),sizeof(uint8_t),comparator);


        for(int i = 0; i < (mod.dualSided ? 4 : 2 ); i++ )
        {

            if((mod.type[i] == 1 || mod.type[i] == 2 || mod.type[i] == 4 || mod.type[i] == 5)) //upper and lower bound, first pair velocity, second pair engine & airbrake power
            {
                mod.value[i] = (i == 3 ? -1.0 : 1.0) * pow((double)rand()/RAND_MAX,2) * 0.1 + 1.0;
            }
            else if(mod.type[i] == 3 || mod.type[i] == 6) //simultanous upper and lower bound, velocity and engine/airbrake respectively
            {
                mod.value[i] = (i == 3 ? -1.0 : 1.0) * pow((double)rand()/RAND_MAX,2) * 0.08 + 1.0;
            }
            else
            {
                switch(mod.type[i])
                {
                    case 7: //turn speed
                    mod.value[i] = (i == 3 ? -1.0 : 1.0) * pow((double)rand()/RAND_MAX,2) *0.007;
                    break;
                    case 8: //turn rate
                    mod.value[i] = (i == 3 ? -1.0 : 1.0) * pow((double)rand()/RAND_MAX,2) *0.005;
                    break;
                    case 9: //default speed
                    mod.value[i] = (i == 3 ? -1.0 : 1.0) * pow((double)rand()/RAND_MAX,2) * 0.2;
                    break;
                    case 10: //coefficient
                    mod.value[i] = (i == 3 ? -1.0 : 1.0) * pow((double)rand()/RAND_MAX,2) * 0.3;
                    break;
                }
            }




        }

riven * heap_mod = new riven;
*heap_mod = mod;
return heap_mod;
}





void cooldown(std::vector<JetInst> &input_vec)
{
    #pragma omp parallel for
for(std::vector<JetInst>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
    for(int i =0;  i<3; i++) //gun cooldown
    {
        if(object->weapon[i].cooldown) object->weapon[i].cooldown--;
        if(object->weapon[i].replenish_cooldown) object->weapon[i].replenish_cooldown--;
    }
    if(object->ability)
    {
        for(int i = 0; i< ENUM_BOSS_ABILITY_FIN; i++)
        {
            if(object->ability[i].cooldown) object->ability[i].cooldown--;
            if(object->ability[i].duration) object->ability[i].duration--;
        }
    }

}


}

void decay(struct LevelInst * level, struct asset_data * asset)
{
    #pragma omp parallel for
    for(std::vector<ProjInst>::iterator object = level->proj_q.begin(); object != level->proj_q.end(); object++) object->decay = (object->decay - 1 > 0 ? object->decay - 1 : 0);
    #pragma omp parallel for
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++) object->decay = (object->decay - 1 > 0 ? object->decay - 1 : 0);
    for(std::vector<RadarNode>::iterator object = level->radar.node_q.begin(); object != level->radar.node_q.end(); object++) object->decay--;
    for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++) if(object->decay > 0) object->decay--;

}

void replenish(asset_data * asset, LevelInst * level)
{
#pragma omp parallel for
for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++)
{
    for(int i =0;  i<3; i++)
    {
        if(!object->weapon[i].replenish_cooldown && object->weapon[i].magazine < object->weapon[i].launcher->magazine && object->weapon[i].magazine < object->weapon[i].ammo)
        {
            object->weapon[i].magazine += 1;
            object->weapon[i].replenish_cooldown = object->weapon[i].launcher->replenish_cooldown;
        }
    }


}


}






void garbage_collect(asset_data * asset, LevelInst * level)
{
    
    
    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
    if(object->hp <= 0) 
    {
        if(asset->config.particlesEnabled)
        {
            ParticleInst temp1 = {
            .type = JET,
            .bitmap = asset->jet_texture[object->type],
            .color = nullptr,
            .curr = {
                .x = object->curr.x,
                .y = object->curr.y,
                .turn_angle = object->curr.turn_angle,
                .speed = object->curr.speed
            },
            .scale_x = 1,
            .scale_y = 1,
            .alter = {
                .turn_speed = (float) rand()/RAND_MAX * 0.01 - 0.005,
                .rotatable = 1,
                .acceleratable = 0,
            },
            .decay = 180,
            .isFading = true,
            .isFalling = true,
            .flip_img = 0
        };

        level->prt_q.push_back(temp1);

        }

        level->enemy_quality[object->type]--;
        if(object->ability) delete object->ability;
        level->jet_q.erase(object);
        object--;

        
    }
    }

    for(std::vector<ProjInst>::iterator object = level->proj_q.begin(); object != level->proj_q.end(); object++)
    {
    if(object->decay == 0) 
        {
            if(object->alter) delete object->alter;
            level->proj_q.erase(object);
            object--;
        }
    }


    
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        if(object->decay <= 0) 
        {
            if(object->color) delete object->color;
            level->prt_q.erase(object);
            object--;
        }
        
    }
    for(std::vector<RadarNode>::iterator object = level->radar.node_q.begin(); object != level->radar.node_q.end(); object++)
    {
        if(object->decay <= 0) 
        {
            level->radar.node_q.erase(object);
            object--;
        }
    }
    for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++)
    {
        if(object->decay == 0)
        {
            destroy_prompt_screen(asset,level,object,1);
            object--;
        }



    }
    

}


float angle_addition(float object, float addition)
{
    object += addition;
        if(object > PI) object = object - 2*PI;
        if(object < -PI) object = object + 2*PI;
return object;
}


float angle_difference(float current, float target)
{
float diff = current-target;
    if(diff>PI && target < 0) diff = diff - 2*PI;
    if(diff<-PI && target >= 0) diff = 2*PI + diff;
return diff;
}

float rad_distance(state * current, state * target) //returns radial distance to target. Turn angle is considered for *current
{
float new_angle = atan2(( target->y - current->y) ,(target->x - current->x));
return angle_difference(current->turn_angle,new_angle);
}


float distance(state * current, state * target )
{
return sqrt( pow( target->x - current->x ,2) +  pow(  target->y - current->y ,2));
}







int alive_enemy_jets(LevelInst * lvl)
{
    int sum = 0;
    for(int i = 0; i< ENUM_BOSS_TYPE_FIN; i++) sum+= lvl->enemy_quality[i];

    return sum;
}

void countermeasure(std::vector<JetInst>::iterator object, asset_data * asset, LevelInst * lvl) //to re-think the if statements (may be good, just think about it)
{
    bool activated = 0;
if(!object->ability[CMEASURE].cooldown || object->ability[CMEASURE].duration)
{
    for(std::vector<ProjInst>::iterator shell = lvl->proj_q.begin(); shell != lvl->proj_q.end(); shell++)
    {
        if(shell->type >= ENUM_BULLET_TYPE_FIN && distance(&shell->curr,&object->curr) < 350 && shell->isBotLaunched != object->isBot)
        {
            activated = 1;
            if(!shell->status[MSL_STATUS::CONTROLLED])
            {
                shell->status[MSL_STATUS::CONTROLLED] = 1;
                shell->alter->target_angle = (float) rand()/RAND_MAX *2*PI - PI;
            }
            
        }
    }

    if(activated && !object->ability[CMEASURE].cooldown)
    {
        object->ability[CMEASURE].cooldown = asset->abl_data[CMEASURE].cooldown;
        object->ability[CMEASURE].duration = asset->abl_data[CMEASURE].duration;
    }

    if(asset->config.particlesEnabled && object->ability[CMEASURE].duration && object->ability[CMEASURE].duration %20 == 0)
    {
    ParticleInst temp1, temp2;

    temp1 = {
        .type = FLARE,
        .bitmap = asset->prt_texture[FLARE],
        .color = nullptr,
        .curr = {
            .x = object->curr.x,
            .y = object->curr.y,
            .turn_angle = angle_addition(object->curr.turn_angle,PI/2),
            .speed = 0.25
        },
        .scale_x = 1,
        .scale_y = 1,
        .alter = {
            .turn_speed = 0.01,
            .rotatable = 1,
            .acceleratable = 0,
        },
        .decay = asset->prt_data[FLARE].decay,
        .isFading = true,
        .isFalling = false,
        .flip_img = 0
    };
    temp2 = {
        .type = FLARE,
        .bitmap = asset->prt_texture[FLARE],
        .color = nullptr,
        .curr = {
            .x = object->curr.x,
            .y = object->curr.y,
            .turn_angle = angle_difference(object->curr.turn_angle,PI/2),
            .speed = 0.25
        },
        .scale_x = 1,
        .scale_y = 1,
        .alter = {
            .turn_speed = -0.01,
            .rotatable = 1,
            .acceleratable = 0,
        },
        .decay = asset->prt_data[FLARE].decay,
        .isFading = true,
        .isFalling = false,
        .flip_img = ALLEGRO_FLIP_VERTICAL
    };
    move(&temp1.curr, asset->lvl_data[lvl->level_name].map_width, asset->lvl_data[lvl->level_name].map_height, 3/temp1.curr.speed);
    move(&temp2.curr, asset->lvl_data[lvl->level_name].map_width, asset->lvl_data[lvl->level_name].map_height, 3/temp2.curr.speed);

    lvl->prt_q.push_back(temp1);
    lvl->prt_q.push_back(temp2);



    };


}
    

}


void boss_ability_use(asset_data * asset, LevelInst * level)
{
for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
{
    if(asset->jet_data[object->type].isBoss)
    {
        if(asset->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[RAND_POS]) randomize_position(object,asset, level);
        if(asset->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[DASH]) dash(object,asset->lvl_data[level->level_name].map_width,asset->lvl_data[level->level_name].map_height);
        if(asset->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[CMEASURE]) countermeasure(object,asset,level);
    }

}


}

void execute_status(asset_data * asset, LevelInst * level)
{
#pragma omp parallel for
for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object < level->jet_q.end(); object++)
{
if(object->status[BURNING])
{
    object->status[BURNING] -= 1;
    float hp_to_deduct;
    #pragma omp critical
    hp_to_deduct = (float) rand() / RAND_MAX * 0.2;
    object->hp =  (object->hp - hp_to_deduct > 0.f ? object->hp - hp_to_deduct : 0.f);
}
}


}





void move_radar(LevelInst * level)
{
    level->radar.turn_angle+= level->radar.turn_speed;
    if(fabs(level->radar.turn_angle) > level->radar.range_rad)
    {
        level->radar.turn_speed = -level->radar.turn_speed;
        level->radar.turn_angle+= level->radar.turn_speed;
    }
}

void radarCycleTarget(std::vector<JetInst> & input_vec, std::vector<JetInst>::iterator reference, RadarInst * radar)
{
    std::vector<JetInst>::iterator follow_target = findJet(input_vec,reference->botTarget);
    if(follow_target == input_vec.end()) follow_target = input_vec.begin();
    else follow_target +=1;
bool finished = 0;
do
{
    if(follow_target == input_vec.end())
    {
        reference->botTarget = -1;
        finished = 1;
    } 
    else if(follow_target != reference && fabs(rad_distance(&reference->curr,&follow_target->curr)) < radar->range_rad && distance(&reference->curr,&follow_target->curr) < radar->range_dist )
    {
        reference->botTarget = follow_target->ID;
        finished = 1;
    }
    follow_target+=1;
} while (!finished);




}

void spawn_particle_pixel(asset_data * asset,LevelInst * level)
{

    
    if(asset->config.particlesEnabled)
    {
        for(int i = 0; i< level->prt_q.size(); i++)
        {
            auto object = level->prt_q.at(i);

            switch(object.type)
            {
                case JET:
                {
                    int num = 2 + rand()%6;
                    if(  !(object.decay%2) && object.decay > asset->prt_data[PIXEL].decay/1.5)
                    {
                        for(int i = 0; i< num; i++)
                        {
                            bool GenerateLeft = i%2;

                            ParticleInst temp1 = {
                            .type = PIXEL,
                            .bitmap = nullptr,
                            .color = new ALLEGRO_COLOR,
                            .curr = {
                                .x = object.curr.x,
                                .y = object.curr.y,
                                .turn_angle = angle_addition(object.curr.turn_angle, (float)rand()/RAND_MAX * (GenerateLeft ?  -PI/30  : PI/30  )) ,
                                .speed = object.curr.speed /2
                            },
                            .scale_x = 2,
                            .scale_y = 2,
                            .alter = {
                                .turn_speed = (float) rand()/RAND_MAX * 0.01 - 0.005,
                                .rotatable = 1,
                                .acceleratable = 0,
                            },
                            .decay = (  object.decay > asset->prt_data[JET].decay ?   asset->prt_data[PIXEL].decay : object.decay ),
                            .isFading = true,
                            .isFalling = true,
                            .flip_img = 0
                            };

                            short gray_coef = rand()%30 + 50;
                            ALLEGRO_COLOR grey = {.r = (float)gray_coef/255, .g = (float)gray_coef/255, .b = (float)gray_coef/255, .a = 1};
                            ALLEGRO_COLOR orange = {.r = (float)(rand()%20+230)/255, .g = (float)(rand()%20+115)/255, .b = (float)(rand()%10+10)/255, .a = 1};
                            (*temp1.color) = (rand()%5 ? grey : orange );



                            transform(&temp1.curr,asset->lvl_data[level->level_name].map_width,asset->lvl_data[level->level_name].map_height, 0.5 + (float) rand()/RAND_MAX * 1.5, angle_addition(temp1.curr.turn_angle,(GenerateLeft ?  -PI/2  :  PI/2  )));
                            level->prt_q.push_back(temp1);
                        }



                    }
                }
                break;



            }
        }

        for(int i = 0; i< level->jet_q.size(); i++)
        {
            auto object = level->jet_q.at(i);
            if(object.status[BURNING])
            {
                    int num = 2 + rand()%6;
                    for(int i = 0; i< num; i++)
                        {
                            bool GenerateLeft = i%2;

                            ParticleInst temp1 = {
                            .type = PIXEL,
                            .bitmap = nullptr,
                            .color = new ALLEGRO_COLOR,
                            .curr = {
                                .x = object.curr.x,
                                .y = object.curr.y,
                                .turn_angle = angle_addition(object.curr.turn_angle, (float)rand()/RAND_MAX * (GenerateLeft ?  -PI/30  : PI/30  )) ,
                                .speed = object.curr.speed /2
                            },
                            .scale_x = 2,
                            .scale_y = 2,
                            .alter = {
                                .turn_speed = (float) rand()/RAND_MAX * 0.01 - 0.005,
                                .rotatable = 1,
                                .acceleratable = 0,
                            },
                            .decay = asset->prt_data[PIXEL].decay ,
                            .isFading = true,
                            .isFalling = true,
                            .flip_img = 0
                            };

                            short gray_coef = rand()%30 + 50;
                            ALLEGRO_COLOR grey = {.r = (float)gray_coef/255, .g = (float)gray_coef/255, .b = (float)gray_coef/255, .a = 1};
                            ALLEGRO_COLOR orange = {.r = (float)(rand()%20+230)/255, .g = (float)(rand()%20+115)/255, .b = (float)(rand()%10+10)/255, .a = 1};
                            (*temp1.color) = (rand()%5 ? grey : orange );



                            transform(&temp1.curr,asset->lvl_data[level->level_name].map_width,asset->lvl_data[level->level_name].map_height, 0.5 + (float) rand()/RAND_MAX * 1.5, angle_addition(temp1.curr.turn_angle,(GenerateLeft ?  -PI/2  :  PI/2  )));
                            level->prt_q.push_back(temp1);
                        }
            }

        }
















    }




}







void process_radar(LevelInst * level)
{



    
    short decay;
    if(!level->radar.mode) decay = 48;
    else decay = 24;

    std::vector<JetInst>::iterator player = level->jet_q.begin();

    if(level->radar.mode != 2) player->botTarget = -1;
    std::vector<JetInst>::iterator follow_target = findJet(level->jet_q,player->botTarget);
    if(follow_target == level->jet_q.end()) player->botTarget = -1;
    if(fabs(rad_distance(&player->curr,&follow_target->curr)) > level->radar.range_rad || distance(&player->curr,&follow_target->curr) > level->radar.range_dist) player->botTarget = -1;


    float tied_radar_pos = angle_addition(player->curr.turn_angle, level->radar.turn_angle);
    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
            float rad_pointer = atan2(( object->curr.y - player->curr.y) ,(object->curr.x - player->curr.x));
            float range_obj_player = distance(&object->curr,&player->curr);
        if(fabs(angle_difference(tied_radar_pos,rad_pointer)) < fabs(level->radar.turn_speed/2) && range_obj_player < level->radar.range_dist && range_obj_player > 450)
        {
            struct RadarNode newNode = { .dist = distance(&player->curr,&object->curr), .rad_dist = level->radar.turn_angle, .decay = decay, .isTarget = (object->ID == player->botTarget ? 1 : 0)};
            level->radar.node_q.push_back(newNode);
        }

    }



}

void render(struct LevelInst * level, struct asset_data * asset, struct allegro5_data * alleg5)
{
    int window_width = al_get_display_width(alleg5->display);
    int window_height = al_get_display_height(alleg5->display);
    
    al_draw_scaled_rotated_bitmap(asset->bkgr_texture[level->level_name],0,0,
        window_width/2 - level->jet_q.front().curr.x * asset->scale_factor,window_height/2 - level->jet_q.front().curr.y * asset->scale_factor,asset->scale_factor,asset->scale_factor,0,0);
        update_prompt_screen(alleg5,level);
        

        draw(level,level->jet_q.begin(),asset,alleg5);
        process_radar(level);
        draw_ui(level,asset,alleg5);
//debug
        #ifdef DEBUG
        debug_data(level,asset,alleg5->font);
        #endif
        al_flip_display();
        al_clear_to_color(al_map_rgb(27,27,27));



}





int level(allegro5_data*alleg5, asset_data * assets, LevelInst * lvl)
{

bool kill = 0;
bool redraw = 1;


ALLEGRO_MOUSE_STATE mouse;
ALLEGRO_KEYBOARD_STATE keyboard;

while(!kill)
{
    

    lvl->jet_q.front().weapon[0].engaged = false; //temporary solution
    lvl->jet_q.front().weapon[2].engaged = false; //temporary solution
    al_wait_for_event(alleg5->queue,&alleg5->event);
    switch (alleg5->event.type)
    {
        case ALLEGRO_EVENT_DISPLAY_RESIZE: 
        {
        al_acknowledge_resize(alleg5->display);
        if(assets->config.autoUIscale) assets->config.UIscale = calculateUIscale(al_get_display_width(alleg5->display), al_get_display_height(alleg5->display));
        render(lvl,assets,alleg5);
        }
        break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE: kill = 1; break;
        case ALLEGRO_EVENT_TIMER: redraw = 1; break;
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            al_get_mouse_state(&mouse);
            if(al_get_mouse_state_axis(&mouse, 2) != 0) zoom(assets,al_get_mouse_state_axis(&mouse, 2));
            break;
        }
        case ALLEGRO_EVENT_KEY_DOWN:
        {
            switch(alleg5->event.keyboard.keycode)
            {
                case ALLEGRO_KEY_SPACE:
                lvl->jet_q.front().weapon[1].engaged = 1;
                break;
                case ALLEGRO_KEY_F:
                if(lvl->pauseEngaged)
                {
                    al_resume_timer(alleg5->timer);
                    if(lvl->level_name < ENUM_LVL_TYPE_FIN) return EQ_SELECTION;
                    else 
                    {
                        lvl->level_name = ENUM_LVL_TYPE_FIN;
                        return LVL_SELECTION;
                    }
                }
                else if(lvl->prompt_q.size() && lvl->prompt_q.back().F_Action) 
                {
                    if(lvl->prompt_q.back().type == 1) destroy_prompt_screen(assets,lvl,lvl->prompt_q.end()-1,1);
                }
                break;
                case ALLEGRO_KEY_Z:
                if(lvl->pauseEngaged)
                {
                    al_resume_timer(alleg5->timer);
                    return MISSION_INIT;
                }
                else if(lvl->prompt_q.size() && lvl->prompt_q.back().Z_Action)
                {
                    if(lvl->prompt_q.back().type == 1) destroy_prompt_screen(assets,lvl,lvl->prompt_q.end()-1,0);
                }
                break;
                case ALLEGRO_KEY_ESCAPE:
                    lvl->pauseEngaged = !lvl->pauseEngaged;
                    if(lvl->pauseEngaged)
                    {
                        render(lvl,assets,alleg5);
                        al_stop_timer(alleg5->timer);
                    } 
                    else al_resume_timer(alleg5->timer);
                break;
                case ALLEGRO_KEY_R:
                {
                lvl->radar.mode = (lvl->radar.mode + 1 > 2 ? 0 : lvl->radar.mode + 1);
                float new_range_rad;
                    if(!lvl->radar.mode)
                    {
                        new_range_rad = PI/6;
                    }
                    else
                    {
                        new_range_rad = PI/12;
                    }
                    if(lvl->radar.range_rad > 0) lvl->radar.range_rad = new_range_rad;
                    else lvl->radar.range_rad = - new_range_rad;


                    if(fabs(lvl->radar.turn_angle) > new_range_rad )
                    {
                        if(lvl->radar.turn_angle > 0) lvl->radar.turn_angle = new_range_rad;
                        else lvl->radar.turn_angle = -new_range_rad;
                    }
                    lvl->radar.node_q.clear();
                }
                break;
                case ALLEGRO_KEY_TAB:
                {
                    if(lvl->radar.mode == 2) radarCycleTarget(lvl->jet_q,lvl->jet_q.begin(),&lvl->radar);
                }
                break;
            }
        break;
        }
    }
    if(redraw && al_is_event_queue_empty(alleg5->queue))
    {
        int window_width = al_get_display_width(alleg5->display);
        int window_height = al_get_display_height(alleg5->display);
        { //player actions
            al_get_mouse_state(&mouse);
            if(mouse.buttons & 1) lvl->jet_q.front().weapon[0].engaged = 1; //left mouse button
            if(mouse.buttons & 2) lvl->jet_q.front().weapon[2].engaged = 1; //left mouse button
            lvl->jet_q.front().alter.target_angle = atan2((mouse.y-window_height/2) ,(mouse.x - window_width/2));
            
            al_get_keyboard_state(&keyboard);
            bool accelerate = al_key_down(&keyboard,ALLEGRO_KEY_W);
            bool decelerate = al_key_down(&keyboard,ALLEGRO_KEY_S);

            if(accelerate ^ decelerate)
            {
                
                state_change_limit * player_limit = (lvl->jet_q.front().overwrite_limit ? lvl->jet_q.front().overwrite_limit : &assets->jet_data[lvl->jet_q.front().type].alter_limit);
                double step = (player_limit->speed_limit[1] - player_limit->speed_limit[0]) / 100.;

                if(accelerate) //accelerate
                {
                    lvl->jet_q.front().alter.target_speed = ( lvl->jet_q.front().alter.target_speed + step >= player_limit->speed_limit[1] ? player_limit->speed_limit[1] : lvl->jet_q.front().alter.target_speed + step );

                }
                if(decelerate) //accelerate
                {
                    lvl->jet_q.front().alter.target_speed = ( lvl->jet_q.front().alter.target_speed - step <= player_limit->speed_limit[0] ? player_limit->speed_limit[0] : lvl->jet_q.front().alter.target_speed - step );

                }

                
            }
        }
        if(lvl->jet_q.front().hp == 0) return MISSION_INIT;
//temporary solution
        shoot(lvl,assets);
//decisions
        decision(lvl->jet_q,assets);
        action(lvl,assets);
        boss_ability_use(assets,lvl);
        target(lvl,assets);
//transformation sequence
        {
        transform(lvl,assets);
        //collision(lvl,assets);
        move_radar(lvl);
        }
        execute_status(assets,lvl);
        spawn_particle_pixel(assets,lvl);
//draw
        render(lvl,assets,alleg5);
        redraw = 0;
        garbage_collect(assets,lvl);
        cooldown(lvl->jet_q);
        replenish(assets,lvl);
        decay(lvl,assets);

        for(int i =1; i<3; i++) lvl->jet_q.front().weapon[i].engaged = 0;

        if(!alive_enemy_jets(lvl) && !lvl->finalPromptEngaged)
        {
            lvl->finalPromptEngaged = true;
            spawn_prompt_screen(assets,alleg5,lvl,(assets->lvl_data[lvl->level_name].next_level == ENUM_BKGR_TYPE_FIN && assets->lvl_data[lvl->level_name].isBoss ? 1 : 0 ));
        }
        if(lvl->finished)
        {
            if(assets->lvl_data[lvl->level_name].next_level != ENUM_BKGR_TYPE_FIN)
            {
            lvl->level_name = assets->lvl_data[lvl->level_name].next_level;
            return MISSION_INIT;
            }
            else
            {
                lvl->level_name = ENUM_LVL_TYPE_FIN;
                return LVL_SELECTION;
            }

            
        }

        lvl->tick = (lvl->tick >= assets->config.FPS ? 0 : lvl->tick+1);
    }
}

return QUIT; //do not change that
}



