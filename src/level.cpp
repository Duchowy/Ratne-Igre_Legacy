#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"



void debug_data(struct LevelInst * level, struct asset_data * asset, ALLEGRO_FONT * font)
{
    std::vector<JetInst>::iterator object = level->jet_q.begin();
        std::string buffer = "";

        for(int i = 0; i<6; i++)
        {
            switch(i)
            {
                case 0: buffer = std::to_string(object->alter.turn_speed); break;
                case 1: if(level->bullet_q.size())   buffer = std::to_string(level->bullet_q.front().curr.speed)+" bul speed"; break;
                case 2: buffer = std::to_string(object->target_angle)+" target"; break;
                case 3: buffer = std::to_string(level->bullet_q.size())+" num of bull"; break;
                case 4: buffer = std::to_string(object->curr.speed)+" speed"; break;
                case 5: buffer = std::to_string(object->will_shoot[0])+" shoot"; break;
            }
            al_draw_text(font,al_map_rgb(240,0,240),0,i*10,0,buffer.c_str());
        }
}

void cooldown(std::vector<JetInst> &input_vec)
{
    #pragma omp parallel for
for(std::vector<JetInst>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
    for(int i =0;  i<3; i++) //gun cooldown
    {
        if(object->weap_delay[i]) object->weap_delay[i]--;
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
    for(std::vector<BulInst>::iterator object = level->bullet_q.begin(); object != level->bullet_q.end(); object++) object->decay--;
    for(std::vector<MslInst>::iterator object = level->msl_q.begin(); object != level->msl_q.end(); object++) object->decay--;
    #pragma omp parallel for
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++) object->decay--;
}


void garbage_collect(asset_data * asset, LevelInst * level)
{
    
    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
    if(object->hp <= 0) 
    {
        level->enemy_quality[object->item.player_jet]--;
        if(object->ability) delete object->ability;
        level->jet_q.erase(object);
        object--;
    }
    }
    for(std::vector<BulInst>::iterator object = level->bullet_q.begin(); object != level->bullet_q.end(); object++)
    {
    if(object->decay <= 0) 
        {
            level->bullet_q.erase(object);
            object--;
        }
    }
    for(std::vector<MslInst>::iterator object = level->msl_q.begin(); object != level->msl_q.end(); object++)
    {
    if(object->decay <= 0) 
        {
            level->msl_q.erase(object);
            object--;
        }
    }
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        if(object->decay <= 0) 
        {
            level->prt_q.erase(object);
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

float rad_distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target)
{
float new_angle = atan2(( target->curr.y - current->curr.y) ,(target->curr.x - current->curr.x));
return angle_difference(current->curr.turn_angle,new_angle);
}

float distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target) //to be changed to use curr struct
{
    return sqrt( pow( target->curr.x - current->curr.x ,2) +  pow(  target->curr.y - current->curr.y ,2));
}

float distance(std::vector<MslInst>::iterator shell, std::vector<JetInst>::iterator target)
{
    return sqrt( pow( target->curr.x - shell->curr.x ,2) +  pow(  target->curr.y - shell->curr.y ,2));
}


void collision(struct LevelInst * input, struct asset_data * asset)
{

//bullet vs jet
#pragma omp parallel for
for(std::vector<BulInst>::iterator shell = input->bullet_q.begin(); shell != input->bullet_q.end(); shell++)
{
	bool destroyed = 0;
    if(!shell->decay || shell->curr.x <= 0 || shell->curr.x >= asset->lvl_data[input->level_name].map_width || shell->curr.y <= 0 || shell->curr.y >= asset->lvl_data[input->level_name].map_height)
		{
		destroyed = 1;
		}
    else
    {
        for(std::vector<JetInst>::iterator target = input->jet_q.begin(); target != input->jet_q.end(); target++)
        {
            if( sqrt( pow( target->curr.x - shell->curr.x ,2) +  pow(  target->curr.y - shell->curr.y ,2)) < asset->jet_data[target->item.player_jet].hitbox) //target hit
            {
                destroyed = 1;
                #pragma omp critical
                {
                target->hp -= shell->damage;
                }
                break;
                
            }
        }
    }
    if(destroyed) shell->decay = 0;
}

//msl vs jet

#pragma omp parallel for
for(std::vector<MslInst>::iterator shell = input->msl_q.begin(); shell != input->msl_q.end(); shell++)
{
	bool activated = 0;

    if(!shell->decay || shell->curr.x <= 0 || shell->curr.x >= asset->lvl_data[input->level_name].map_width || shell->curr.y <= 0 || shell->curr.y >= asset->lvl_data[input->level_name].map_height)
		{
		activated = 1;
		}

    for(std::vector<JetInst>::iterator target = input->jet_q.begin(); target != input->jet_q.end(); target++)
	{
		if( shell->decay + 5 <= asset->msl_data[shell->type].decay  && distance(shell,target) < asset->jet_data[target->item.player_jet].hitbox + asset->msl_data[shell->type].radius) //target hit
		{
            if(shell->type != RAD || (shell->type == RAD && shell->isBotLaunched != target->isBot)) //self-enemy check
            {  
                activated = 1;
            }
            if(activated)
            {
                #pragma omp critical
                target->hp -= asset->msl_data[shell->type].damage;
            }
		}
	}

    if(activated)
    {
        shell->decay = 0;
    }
}





}

void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset, struct allegro5_data * alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);

{//jet section
    std::vector<JetInst>::iterator player = level->jet_q.begin();
    al_draw_scaled_rotated_bitmap(asset->jet_texture[player->item.player_jet],23,23,
    window_width/2,window_height/2,asset->scale_factor,asset->scale_factor,player->curr.turn_angle,0);

    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {

    if(distance(player,object) < 800) 
    {
    float x_dist = object->curr.x - player->curr.x;
    float y_dist = object->curr.y - player->curr.y;

    float x_diff = asset->scale_factor * (x_dist) + window_width/2;
    float y_diff = asset->scale_factor * (y_dist) + window_height/2;

    int full_hp = asset->jet_data[object->item.player_jet].hp;

    al_draw_scaled_rotated_bitmap(asset->jet_texture[object->item.player_jet],23,23,
    x_diff, y_diff ,asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0);
    al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
        if(asset->jet_data[object->item.player_jet].isBoss) 
        {
            al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(0,0,0));
            al_draw_filled_rectangle(x_diff-10,y_diff-9,x_diff+10,y_diff-6,al_map_rgb(255 *(1 - object->hp/full_hp),255*object->hp/full_hp,0));
        }
        else 
        {
            al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
            al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb(255 *(1 - object->hp/full_hp),255*object->hp/full_hp,0));
        }
    }
    


    }
}



{//msl section
    for(std::vector<MslInst>::iterator object = level->msl_q.begin(); object != level->msl_q.end(); object++)
    {
    al_draw_scaled_rotated_bitmap(asset->msl_texture[object->type],23,23,
    asset->scale_factor *(object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,
    asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0);
    }
}



al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
{ //bullet section
    
    for(std::vector<BulInst>::iterator object = level->bullet_q.begin(); object != level->bullet_q.end(); object++)
    {
        
        ALLEGRO_COLOR col = al_map_rgba(object->color[0],object->color[1],object->color[2],255*object->decay / asset->bul_data[object->type].decay);
        al_draw_tinted_scaled_rotated_bitmap(asset->bullet_texture[object->type][1],col,23,23,
        asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,
        asset->bul_data[object->type].height,asset->bul_data[object->type].width,object->curr.turn_angle,0);
        

        //asset->bul_data[object->type].

        col = al_map_rgba(255,255,255,255*object->decay / asset->bul_data[object->type].decay);
        al_draw_tinted_scaled_rotated_bitmap(asset->bullet_texture[object->type][0],col,23,23,
        asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,
        asset->bul_data[object->type].height,asset->bul_data[object->type].width,object->curr.turn_angle,0);
        
    }
    
}

//particle section
{
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        if(object->isDecaying)
        al_draw_tinted_scaled_rotated_bitmap(asset->prt_data[object->type].texture,al_map_rgba(255,255,255,255*object->decay / asset->prt_data[object->type].decay),23,23,
        asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,object->scale_x,object->scale_y,
        object->curr.turn_angle,object->flip_img
        );
        else
        al_draw_tinted_scaled_rotated_bitmap(asset->prt_data[object->type].texture,object->color,23,23,
        asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,object->scale_x,object->scale_y,
        object->curr.turn_angle,object->flip_img
        );

    }




}
al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending


}



void draw_ui(struct LevelInst * level, struct asset_data * asset, struct allegro5_data * alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);

std::vector<JetInst>::iterator player = level->jet_q.begin();
al_draw_filled_rectangle(window_width-80,window_height-35,window_width,window_height,al_map_rgb(0,20,20));

/*########
### HP ###
########*/


float current_HP = (float) player->hp / asset->jet_data[player->item.player_jet].hp;
if(current_HP > 0.9) al_draw_text(alleg5->font,al_map_rgb(0,240,0),window_width-al_get_text_width(alleg5->font,"OK")-10,window_height-20,0,"OK");
else if ( current_HP > 0.7) al_draw_text(alleg5->font,al_map_rgb(240,240,0),window_width-al_get_text_width(alleg5->font,"OK")-10,window_height-20, 0,"OK");
else if( current_HP > 0.3) al_draw_text(alleg5->font,al_map_rgb(240,240,0),window_width-al_get_text_width(alleg5->font,"Damaged")-10,window_height-20,0,"Damaged");
else al_draw_text(alleg5->font,al_map_rgb(240,0,0),window_width-al_get_text_width(alleg5->font,"Damaged")-10,window_height-20,0,"Damaged");




/*########
## AMMO ##
########*/
al_draw_filled_rectangle(0,window_height,al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"RKT")+5,window_height-20,al_map_rgb(0,20,20));                                                 //font theme

float ammo_percentage = (float) player->weap_ammo[0] / (asset->gun_data[player->item.player_gun].ammo_max * asset->jet_data[player->item.player_jet].gun_mult);
ALLEGRO_COLOR ammo_color;
if(ammo_percentage > 0.4) ammo_color = al_map_rgb(240,240,240);
else ammo_color = al_map_rgb(250,pow((ammo_percentage/0.4),3)*255,pow((ammo_percentage/0.4),3)*255);
al_draw_filled_rectangle(0,window_height-20, al_get_text_width(alleg5->font,"GUN"), window_height -20 - 60*ammo_percentage,ammo_color);    //ammo bar
bool used = (player->weap_ammo[0] != 0);
al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),0,window_height-10,0,"GUN");

used = (player->weap_ammo[1] != 0);
al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),al_get_text_width(alleg5->font,"GUN")+5,window_height-10,0,"RKT");    //rocket bar
for(int i = 0; i< player->weap_ammo[1]; i++) 
al_draw_line(al_get_text_width(alleg5->font,"GUN")+5,window_height-20.5 -3*i ,al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"RKT"),window_height-20.5 -3*i,
al_map_rgb(240,230,140),1);


/*########
## RADAR #
########*/



    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
        float rad_pointer = atan2(( object->curr.y - player->curr.y) ,(object->curr.x - player->curr.x));
        float rad_dist = rad_distance(player,object);
        if(distance(player,object) < 1800 && fabs(rad_dist) < PI/6 && !asset->jet_data[object->item.player_jet].isBoss) 
        {
            ALLEGRO_COLOR indicator;
            if(distance(player,object) < 800) indicator = al_map_rgb(240,240,0);
            else indicator = al_map_rgb(0,240,0);
            al_draw_line(window_width/2 + 14*cos(rad_pointer),window_height/2 + 14*sin(rad_pointer),
            window_width/2 + 28*cos(rad_pointer),window_height/2 + 28*sin(rad_pointer),
            indicator,0.8);
        }
        

    }

}

void zoom(asset_data * assets, short direction)
{
if(direction > 0) assets->scale_factor+=0.1;
if(direction < 0) assets->scale_factor-=0.1;


if(assets->scale_factor < 0.8) assets->scale_factor = 0.8;
if(assets->scale_factor > 1.0) assets->scale_factor = 1.0;
al_set_mouse_z(0); //ticker bound
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
    for(std::vector<MslInst>::iterator shell = lvl->msl_q.begin(); shell != lvl->msl_q.end(); shell++)
    {
        if(distance(shell,object) < 350 && shell->isBotLaunched != object->isBot)
        {
            activated = 1;
            if(!shell->status[MSL_STATUS::CONTROLLED])
            {
                shell->status[MSL_STATUS::CONTROLLED] = 1;
                shell->target_angle = (float) rand()/RAND_MAX *2*PI - PI;
            }
            
        }
    }

    if(activated && !object->ability[CMEASURE].cooldown)
    {
        object->ability[CMEASURE].cooldown = asset->abl_data[CMEASURE].cooldown;
        object->ability[CMEASURE].duration = asset->abl_data[CMEASURE].duration;
    }

    if(particlesEnabled && object->ability[CMEASURE].duration && object->ability[CMEASURE].duration %20 == 0)
    {
    ParticleInst temp1, temp2;
    temp1.type = temp2.type = FLARE;
    temp1.decay = temp2.decay = asset->prt_data[FLARE].decay;
    temp1.curr.turn_angle = angle_addition(object->curr.turn_angle,PI/2);
    temp2.curr.turn_angle = angle_addition(object->curr.turn_angle,-PI/2);
    temp1.alter.rotatable = temp2.alter.rotatable = 1;
    temp1.alter.acceleratable = temp2.alter.acceleratable = 0;
    temp1.alter.turn_speed =  0.01;
    temp2.alter.turn_speed = -temp1.alter.turn_speed;
    temp1.curr.speed = temp2.curr.speed = 0.25;
    temp1.curr.x = temp2.curr.x = object->curr.x; 
    temp1.curr.y = temp2.curr.y = object->curr.y; 
    temp1.flip_img = 0;
    temp2.flip_img = ALLEGRO_FLIP_VERTICAL;
    temp1.scale_x = temp2.scale_x = temp1.scale_y = temp2.scale_y = 1;
    temp1.isDecaying = temp2.isDecaying = 1;
    move(&temp1.curr, asset->lvl_data[lvl->level_name].map_width, asset->lvl_data[lvl->level_name].map_height, 3/temp1.curr.speed);
    move(&temp2.curr, asset->lvl_data[lvl->level_name].map_width, asset->lvl_data[lvl->level_name].map_height, 3/temp2.curr.speed);

    lvl->prt_q.push_back(temp1);
    lvl->prt_q.push_back(temp2);

    }


}
    

}


void boss_ability_use(asset_data * asset, LevelInst * level)
{
for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
{
    if(asset->jet_data[object->item.player_jet].isBoss)
    {
        if(asset->boss_data[object->item.player_jet-ENUM_JET_TYPE_FIN].ability[RAND_POS]) randomize_position(object,asset, level);
        if(asset->boss_data[object->item.player_jet-ENUM_JET_TYPE_FIN].ability[DASH]) dash(object,asset->lvl_data[level->level_name].map_width,asset->lvl_data[level->level_name].map_height);
        if(asset->boss_data[object->item.player_jet-ENUM_JET_TYPE_FIN].ability[CMEASURE]) countermeasure(object,asset,level);
    }

}


}





int level(allegro5_data*alleg5, asset_data * assets, LevelInst * lvl)
{
bool kill = 0;
bool redraw = 1;



ALLEGRO_MOUSE_STATE mouse;

while(!kill)
{
    lvl->jet_q.front().will_shoot[0] = 0; //temporary solution
    al_wait_for_event(alleg5->queue,&alleg5->event);
    switch (alleg5->event.type)
    {
        case ALLEGRO_EVENT_DISPLAY_RESIZE: al_acknowledge_resize(alleg5->display); break;
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
                case ALLEGRO_KEY_W:
                lvl->jet_q.front().alter.speed_mode = AFTERBURNER;
                break;
                case ALLEGRO_KEY_S:
                lvl->jet_q.front().alter.speed_mode = AIRBRAKE;
                break;
                case ALLEGRO_KEY_SPACE:
                lvl->jet_q.front().will_shoot[1] = 1;
                break;
                case ALLEGRO_KEY_ESCAPE:
                if(lvl->level_name < ENUM_LVL_TYPE_FIN) return SELECTION;
                else return MISSION_INIT;
                break;
            }
        break;
        }
        case ALLEGRO_EVENT_KEY_UP:
        {
            switch(alleg5->event.keyboard.keycode)
            {
                case ALLEGRO_KEY_W:
                lvl->jet_q.front().alter.speed_mode = STANDARD;
                break;
                case ALLEGRO_KEY_S:
                lvl->jet_q.front().alter.speed_mode = STANDARD;
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
        if(mouse.buttons & 1) lvl->jet_q.front().will_shoot[0] = 1; //left mouse button
        lvl->jet_q.front().target_angle = atan2((mouse.y-window_height/2) ,(mouse.x - window_width/2));
        }
        if(lvl->jet_q.front().hp < 0) return MISSION_INIT;
//temporary solution
        shoot(lvl,assets);
//decisions
        decision(lvl->jet_q,assets);
        action(lvl,assets);
        boss_ability_use(assets,lvl);
        target(lvl,assets);
//transformation
        transform(lvl,assets);

        collision(lvl,assets);
//draw
        //al_draw_bitmap(assets->bkgr_texture[lvl.level_name],window_width/2 - jet_q.front().x,window_height/2 - jet_q.front().y,0);
        al_draw_scaled_rotated_bitmap(assets->bkgr_texture[lvl->level_name],0,0,
        window_width/2 - lvl->jet_q.front().curr.x * assets->scale_factor,window_height/2 - lvl->jet_q.front().curr.y * assets->scale_factor,assets->scale_factor,assets->scale_factor,0,0);

        

        draw(lvl,lvl->jet_q.begin(),assets,alleg5);
        draw_ui(lvl,assets,alleg5);
//debug
        //debug_data(lvl,assets,alleg5->font);
        
        al_flip_display();
        al_clear_to_color(al_map_rgb(27,27,27));
        redraw = 0;
        garbage_collect(assets,lvl);
        cooldown(lvl->jet_q);
        decay(lvl,assets);

        for(int i =1; i<3; i++) lvl->jet_q.front().will_shoot[i] = 0;

        if(!alive_enemy_jets(lvl) && assets->lvl_data[lvl->level_name].next_level != ENUM_BKGR_TYPE_FIN)
        {
            lvl->level_name = assets->lvl_data[lvl->level_name].next_level;
            return MISSION_INIT;
        }


    }
}

return QUIT; //do not change that
}



