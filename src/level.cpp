#include "jet.h"
#include "level.h"
#include "main.h"
#include "movement.h"
#include "render_level.h"



void debug_data(struct LevelInst * level, struct asset_data * asset, ALLEGRO_FONT * font)
{
    std::vector<JetInst>::iterator object = level->jet_q.begin();
        std::string buffer = "";

        for(int i = 0; i<6; i++)
        {
            switch(i)
            {
                case 0: buffer = std::to_string(object->alter.turn_speed); break;
                //case 1: if(level->bullet_q.size())   buffer = std::to_string(level->bullet_q.front().curr.speed)+" bul speed"; break;
                case 2: buffer = std::to_string(object->target_angle)+" target"; break;
                case 3: buffer = std::to_string(object->alter.turn_speed)+" turn speed"; break;
                case 4: buffer = std::to_string(object->curr.speed)+" speed"; break;
                case 5: buffer = std::to_string(object->weapon[0].engaged)+" shoot"; break;
            }
            al_draw_text(font,al_map_rgb(240,0,240),0,i*10,0,buffer.c_str());
        }
}


void destroy_prompt_screen(asset_data * asset, LevelInst * level, std::vector<prompt_screen>::iterator object)
{
if(object != level->prompt_q.end() && level->prompt_q.size()) object = level->prompt_q.end()-1;

switch(object->type)
{
case 0:
level->prompt_q.erase(object);
level->finished = 1;
break;

default:
break;
}




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
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++) object->decay--;
    for(std::vector<RadarNode>::iterator object = level->radar.node_q.begin(); object != level->radar.node_q.end(); object++) object->decay--;
    for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++) if(object->decay >= 0) object->decay--;

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
            destroy_prompt_screen(asset,level,object);
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

float rad_distance(state * current, state * target)
{
float new_angle = atan2(( target->y - current->y) ,(target->x - current->x));
return angle_difference(current->turn_angle,new_angle);
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

float distance(std::vector<ProjInst>::iterator shell, std::vector<JetInst>::iterator target)
{
    return sqrt( pow( target->curr.x - shell->curr.x ,2) +  pow(  target->curr.y - shell->curr.y ,2));
}


void collision(struct LevelInst * input, struct asset_data * asset)
{

#pragma omp parallel for
for(std::vector<ProjInst>::iterator shell = input->proj_q.begin(); shell != input->proj_q.end(); shell++)
{
	bool activated = 0;

    if(!shell->decay || shell->curr.x <= 0 || shell->curr.x >= asset->lvl_data[input->level_name].map_width || shell->curr.y <= 0 || shell->curr.y >= asset->lvl_data[input->level_name].map_height)
		{
		activated = 1;
		}

    for(std::vector<JetInst>::iterator target = input->jet_q.begin(); target != input->jet_q.end(); target++)
	{
		if( shell->decay + 3 <= asset->proj_data[shell->type].decay + shell->launcher->decay && 
        distance(shell,target) < asset->jet_data[target->type].hitbox + asset->proj_data[shell->type].radius && 
        (asset->proj_data[shell->type].trait.hitCircular || ( fabs(rad_distance(&shell->curr,&target->curr)) < PI/3   )  )    
        ) //target hit
		{
            if(shell->type != RAD_M || (shell->type == RAD_M && shell->isBotLaunched != target->isBot)) //self-enemy check
            {  
                activated = 1;
            }
            if(activated)
            {
                #pragma omp critical
                {
                    target->hp = (target->hp - shell->damage > 0 ? target->hp - shell->damage : 0);
                }
                if(!asset->proj_data[shell->type].trait.isAOE) break;
            }
		}
	}

    if(activated)
    {
        shell->decay = 0;
    }

    
}


}

void draw_pause_screen(struct LevelInst * level, struct asset_data * asset, struct allegro5_data * alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display); 

al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
al_draw_filled_rectangle(0,0,window_width,window_height,al_map_rgba(10,10,10,120));
al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending

al_draw_filled_rectangle(window_width/2-300,window_height/2-100,window_width/2+300,window_height/2+100,al_map_rgb(27,27,17));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),window_width/2,window_height/2-15,ALLEGRO_ALIGN_CENTER,"PAUSED");
std::string desc = "Press ESC to unpause.";


if(level->level_name < ENUM_LVL_TYPE_FIN) desc += "\nPress F to exit to menu.";
else desc += "\nPress F to reset mission.";
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),window_width/2,window_height/2,400,10,ALLEGRO_ALIGN_CENTER,desc.c_str());

}

void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset, struct allegro5_data * alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);

{//jet section
    std::vector<JetInst>::iterator player = level->jet_q.begin();
    

    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
    float dist = distance(player,object);
    if(dist < 1000) 
    {
    float x_dist = object->curr.x - player->curr.x;
    float y_dist = object->curr.y - player->curr.y;

    float x_diff = asset->scale_factor * (x_dist) + window_width/2;
    float y_diff = asset->scale_factor * (y_dist) + window_height/2;

    int full_hp = asset->jet_data[object->type].hp;

    if(dist < 800)
    {
    al_draw_scaled_rotated_bitmap(asset->jet_texture[object->type],23,23,
    x_diff, y_diff ,asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0);
    }
    else
    {
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_tinted_scaled_rotated_bitmap(asset->jet_texture[object->type],al_map_rgba(255,255,255,255 - 255*(dist-600)/200),23,23,
        x_diff, y_diff ,asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0
        );
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
    }
    
    
    
    
    al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
        if(asset->jet_data[object->type].isBoss) 
        {
            al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(0,0,0));
            al_draw_filled_rectangle(x_diff-10,y_diff-9,x_diff+10,y_diff-6,al_map_rgb(255 *(1 - object->hp/full_hp),255*object->hp/full_hp,0));
        }
        else 
        {
            al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
            al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb(255 *(1 - object->hp/full_hp),255*object->hp/full_hp,0));
        }
    if(object->mode != PATROL)
    {
        al_draw_text(alleg5->font,al_map_rgb(240,240,0),x_diff+8,y_diff-9,0,"!");

    }
    #ifdef DEBUG
        al_draw_text(alleg5->font,al_map_rgb(240,140,0),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff-9,0,std::to_string(object->mode).c_str());
    #endif


    }
    


    }
}


for(std::vector<ProjInst>::iterator object = level->proj_q.begin(); object != level->proj_q.end(); object++)
{
if(asset->proj_data[object->type].trait.DMGfall)
{
al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
al_draw_tinted_scaled_rotated_bitmap(asset->proj_texture[object->type],al_map_rgba_f(object->color.r,  object->color.g,  object->color.b,  sqrt((float) object->decay / (asset->proj_data[object->type].decay + object->launcher->decay))),23,23,
    asset->scale_factor *(object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,
    asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0);
al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
}else
{

al_draw_scaled_rotated_bitmap(asset->proj_texture[object->type],23,23,
    asset->scale_factor *(object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,
    asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0);


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
### HUD ##
########*/




/*########
### HP ###
########*/


float current_HP = (float) player->hp / asset->jet_data[player->type].hp;
if(current_HP > 0.9) al_draw_text(alleg5->font,al_map_rgb(0,240,0),window_width-al_get_text_width(alleg5->font,"OK")-10,window_height-20,0,"OK");
else if ( current_HP > 0.7) al_draw_text(alleg5->font,al_map_rgb(240,240,0),window_width-al_get_text_width(alleg5->font,"OK")-10,window_height-20, 0,"OK");
else if( current_HP > 0.3) al_draw_text(alleg5->font,al_map_rgb(240,240,0),window_width-al_get_text_width(alleg5->font,"Damaged")-10,window_height-20,0,"Damaged");
else al_draw_text(alleg5->font,al_map_rgb(240,0,0),window_width-al_get_text_width(alleg5->font,"Damaged")-10,window_height-20,0,"Damaged");




/*########
## AMMO ##
########*/
{
    bool SPCisGun = (player->weapon[2].launcher->projectile - asset->proj_data < ENUM_BULLET_TYPE_FIN)  ;


    al_draw_filled_rectangle(0,window_height,al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5 + al_get_text_width(alleg5->font,"SPC")+5,window_height-20,al_map_rgb(0,20,20));                                                 //font theme

    float ammo_percentage = (float) player->weapon[0].ammo / (asset->laun_data[player->weapon[0].type].ammo * asset->jet_data[player->type].weapon_mult[0]);
    float mag_percentage = (float) player->weapon[0].magazine / player->weapon[0].launcher->magazine;
    ALLEGRO_COLOR ammo_color;
    int rectangle_height = 60;
    
//ammo indicator
    ammo_color = (ammo_percentage > 0.4 ? al_map_rgb(240,240,240) : al_map_rgb_f(0.98,pow((ammo_percentage/0.4)*0.98,3),pow((ammo_percentage/0.4)*0.98,3)) );
    al_draw_filled_rectangle(0,window_height-20, al_get_text_width(alleg5->font,"GUN"), window_height -20 - rectangle_height*ammo_percentage,ammo_color);    
    
    
    if(mag_percentage <= 0.4)
    {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_rectangle(window_width/2 - 120, window_height/2 + 40,window_width/2 - 100, window_height/2 - 40 + ( 80.0 * mag_percentage), mag_percentage ? al_map_rgba_f(0.98,0.98,0.98,0.6 * (float)(0.4 - mag_percentage) / 0.4) : al_map_rgba(240,120,60,153));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
    }





    bool used;
    used = (player->weapon[0].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),0,window_height-10,0,"GUN");
    used = (player->weapon[1].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),al_get_text_width(alleg5->font,"GUN")+5,window_height-10,0,"MSL");    //rocket bar
    used = (player->weapon[2].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),al_get_text_width(alleg5->font,"GUN")+5+al_get_text_width(alleg5->font,"MSL")+5,window_height-10,0,"SPC");    //rocket bar

//missile indicator
    for(int i = 0; i< player->weapon[1].ammo; i++)
    {
    ALLEGRO_COLOR bar_color = al_map_rgb(200,180,100);//al_map_rgb(240,230,140);
    if(i+player->weapon[1].magazine >= player->weapon[1].ammo) bar_color = al_map_rgb(240,120,60);
    al_draw_line(al_get_text_width(alleg5->font,"GUN")+5,window_height-20.5 -3*i ,al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL"),window_height-20.5 -3*i,
    bar_color,1);
    }
//special indicator


    if(SPCisGun)
    {
        float ammo_percentage = (float) player->weapon[2].ammo / (asset->laun_data[player->weapon[2].type].ammo * asset->jet_data[player->type].weapon_mult[2]);
        float mag_percentage = (float) player->weapon[2].magazine / player->weapon[2].launcher->magazine;
        ALLEGRO_COLOR ammo_color = (ammo_percentage > 0.4 ? al_map_rgb(240,240,240) : al_map_rgb_f(0.98,pow((ammo_percentage/0.4)*0.98,3),pow((ammo_percentage/0.4)*0.98,3)) );;
        if(mag_percentage <= 0.4)
        {
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_filled_rectangle(window_width/2 + 120, window_height/2 + 40,window_width/2 + 100, window_height/2 - 40 + ( 80.0 * mag_percentage),  mag_percentage ? al_map_rgba_f(0.98,0.98,0.98,0.6 * (float)(0.4 - mag_percentage) / 0.4) : al_map_rgba(240,120,60,153));
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
        }
        
        al_draw_filled_rectangle(
        al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5,window_height-20,
        al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL") + al_get_text_width(alleg5->font,"SPC")+5, window_height -20 - rectangle_height*ammo_percentage,ammo_color
        );    

    }
    else
    {

        for(int i = 0; i< player->weapon[2].ammo; i++)
    {
    ALLEGRO_COLOR bar_color = al_map_rgb(250,250,250);
    if(i+player->weapon[2].magazine >= player->weapon[2].ammo) bar_color = al_map_rgb(200,180,100);
    al_draw_line(
        al_get_text_width(alleg5->font,"GUN")+5+al_get_text_width(alleg5->font,"MSL")+5,window_height-20.5 -3*i ,
        al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL") + al_get_text_width(alleg5->font,"SPC")+5,window_height-20.5 -3*i,
    bar_color,1);
    }




    }





}



/*########
## RADAR #
########*/


/*
    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
        float rad_pointer = atan2(( object->curr.y - player->curr.y) ,(object->curr.x - player->curr.x));
        float rad_dist = rad_distance(player,object);
        if(distance(player,object) < 1800 && fabs(rad_dist) < PI/6 && !asset->jet_data[object->type].isBoss) 
        {
            ALLEGRO_COLOR indicator;
            if(distance(player,object) < 800) indicator = al_map_rgb(240,240,0);
            else indicator = al_map_rgb(0,240,0);
            al_draw_line(window_width/2 + 14*cos(rad_pointer),window_height/2 + 14*sin(rad_pointer),
            window_width/2 + 28*cos(rad_pointer),window_height/2 + 28*sin(rad_pointer),
            indicator,0.8);
        }
        

    }
*/

{
ALLEGRO_COLOR indicator;
if(player->type == MIG21) indicator = al_map_rgb(240,240,0);
else indicator = al_map_rgb(0,240,0);
float rad_pointer = angle_addition(player->curr.turn_angle,level->radar.turn_angle);


al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
al_draw_filled_pieslice(window_width/2,window_height/2,48,player->curr.turn_angle+fabs(level->radar.range_rad),-fabs(2*level->radar.range_rad),al_map_rgba_f(  indicator.r, indicator.g, indicator.b,0.2));

for(std::vector<RadarNode>::iterator object = level->radar.node_q.begin(); object != level->radar.node_q.end(); object++)
{
    float node_pointer = angle_addition(player->curr.turn_angle,object->rad_dist);
    float x_pos = window_width/2 + cos(node_pointer) * (16 + (48 -2-16)* object->dist / level->radar.range_dist);
    float y_pos = window_height/2 +  sin(node_pointer) * (16 + (48 -2-16)* object->dist / level->radar.range_dist);
    float opacity;
    
    switch(level->radar.mode)
    {
        case 0:
        opacity = 1 - pow(1 - (float) object->decay/48,4);
        break;
        case 1:
        opacity = 1 - pow(1 - (float) object->decay/24,2);
        break;
    }



        al_draw_filled_circle(x_pos, y_pos,
        2,al_map_rgba_f(  indicator.r, indicator.g, indicator.b, opacity )
        );
        al_draw_circle(x_pos,y_pos,1,al_map_rgba_f(0,0,0,opacity),0.5);

}
al_draw_line(window_width/2 + 18*cos(player->curr.turn_angle),window_height/2 + 18*sin(player->curr.turn_angle),
            window_width/2 + 48*cos(player->curr.turn_angle),window_height/2 + 48*sin(player->curr.turn_angle),
            al_map_rgba_f(indicator.r, indicator.g, indicator.b, 0.25),0.8); //lead angle line

al_draw_arc(window_width/2,window_height/2,30,player->curr.turn_angle+fabs(level->radar.range_rad),-fabs(2*level->radar.range_rad),al_map_rgba_f(  indicator.r, indicator.g, indicator.b,0.2),0.8);

al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
al_draw_line(window_width/2 + 18*cos(rad_pointer),window_height/2 + 18*sin(rad_pointer),
            window_width/2 + 48*cos(rad_pointer),window_height/2 + 48*sin(rad_pointer),
            indicator,0.8); //radar seeker line

}

al_draw_scaled_rotated_bitmap(asset->jet_texture[player->type],23,23,
    window_width/2,window_height/2,asset->scale_factor,asset->scale_factor,player->curr.turn_angle,0);



/*########
# PROMPT #
########*/
{
for(std::vector<prompt_screen>::iterator prompt = level->prompt_q.begin(); prompt != level->prompt_q.end(); prompt++)
{
al_draw_filled_rectangle(prompt->body.x-prompt->body.width/2,prompt->body.y-prompt->body.height/2,prompt->body.x+prompt->body.width/2,prompt->body.y+prompt->body.height/2,al_map_rgb(27,27,17));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),prompt->body.x,prompt->body.y,ALLEGRO_ALIGN_CENTER,prompt->body.name.c_str());
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),prompt->body.x,prompt->body.y,300,10,ALLEGRO_ALIGN_CENTER,prompt->body.desc.c_str());
}
}

/*########
## PAUSE #
########*/


if(level->pauseEngaged)
{
    draw_pause_screen(level,asset,alleg5);
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
    for(std::vector<ProjInst>::iterator shell = lvl->proj_q.begin(); shell != lvl->proj_q.end(); shell++)
    {
        if(shell->type >= ENUM_BULLET_TYPE_FIN && distance(shell,object) < 350 && shell->isBotLaunched != object->isBot)
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
    if(asset->jet_data[object->type].isBoss)
    {
        if(asset->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[RAND_POS]) randomize_position(object,asset, level);
        if(asset->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[DASH]) dash(object,asset->lvl_data[level->level_name].map_width,asset->lvl_data[level->level_name].map_height);
        if(asset->boss_data[object->type-ENUM_JET_TYPE_FIN].ability[CMEASURE]) countermeasure(object,asset,level);
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

void process_radar(LevelInst * level)
{
    short decay;
    if(!level->radar.mode) decay = 48;
    else decay = 24;

    std::vector<JetInst>::iterator player = level->jet_q.begin();
    float tied_radar_pos = angle_addition(player->curr.turn_angle, level->radar.turn_angle);
    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
        float rad_pointer = atan2(( object->curr.y - player->curr.y) ,(object->curr.x - player->curr.x));
        float range_obj_player = distance(object,player);
    if(fabs(angle_difference(tied_radar_pos,rad_pointer)) < fabs(level->radar.turn_speed/2) && range_obj_player < level->radar.range_dist && range_obj_player > 450)
    {
        struct RadarNode newNode = { .dist = distance(player,object), .rad_dist = level->radar.turn_angle, .decay = decay};
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

void update_prompt_screen(allegro5_data * alleg5, LevelInst * level)
{
short num = 0;
for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++ , num++)
{
    object->body.x = al_get_display_width(alleg5->display)/2 + 25*num;
    object->body.y = al_get_display_height(alleg5->display)/2 + 15*num;
}

}


void spawn_prompt_screen(asset_data * asset, allegro5_data * alleg5, LevelInst * level, unsigned short type) 
{
int display_width = al_get_display_width(alleg5->display);
int display_height = al_get_display_height(alleg5->display); 


for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++) if(object->type == type) return;

std::string title;
std::string desc;
short decay;
bool F_Action;
bool Z_Action;

switch(type)
{
    case 0:
    {
        
        if(asset->lvl_data[level->level_name].next_level != ENUM_BKGR_TYPE_FIN)
        {
            unsigned short rander = rand()%4;
            switch(rander)
            {
                case 0: title = "Ready for more? You'd better be."; break;
                case 1: title = "Don't let your guard down just jet."; break;
                case 2: title = "Move on, more hostiles on the way."; break;
                case 3: title = "Brave enough for more? Hostiles incoming!"; break;
            }
        }
        else
        {
            title = "Mission accomplished. Return home.";
        }
        decay = 180;
        F_Action = 0;
        Z_Action = 0;
    }
    break;

}
if(F_Action) desc + "Press F to confirm"; //F podmień
if(F_Action && Z_Action) desc + "\n";
if(Z_Action) desc + "Press Z to deny";


prompt_screen prompt = {.type=type,{.x = display_width/2 + 25 * level->prompt_q.size(), .y = display_height/2 + 15 * level->prompt_q.size(), .width = 400, .height = 150,.name = title, .desc = desc},.decay = decay, .F_Action = F_Action, .Z_Action = Z_Action};

level->prompt_q.push_back(prompt);





}





int level(allegro5_data*alleg5, asset_data * assets, LevelInst * lvl)
{
bool kill = 0;
bool redraw = 1;



ALLEGRO_MOUSE_STATE mouse;

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
                case ALLEGRO_KEY_W:
                lvl->jet_q.front().alter.speed_mode = AFTERBURNER;
                break;
                case ALLEGRO_KEY_S:
                lvl->jet_q.front().alter.speed_mode = AIRBRAKE;
                break;
                case ALLEGRO_KEY_SPACE:
                lvl->jet_q.front().weapon[1].engaged = 1;
                break;
                case ALLEGRO_KEY_F:
                if(lvl->pauseEngaged)
                {
                    al_resume_timer(alleg5->timer);


                    if(lvl->level_name < ENUM_LVL_TYPE_FIN) return EQ_SELECTION;
                    else return MISSION_INIT;
                }
                else if(lvl->prompt_q.back().F_Action) 
                {
                    
                }
                break;
                case ALLEGRO_KEY_Z:
                if(lvl->prompt_q.back().Z_Action)
                {

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
                lvl->radar.mode = !lvl->radar.mode;
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
        if(mouse.buttons & 1) lvl->jet_q.front().weapon[0].engaged = 1; //left mouse button
        if(mouse.buttons & 2) lvl->jet_q.front().weapon[2].engaged = 1; //left mouse button
        lvl->jet_q.front().target_angle = atan2((mouse.y-window_height/2) ,(mouse.x - window_width/2));
        }
        if(lvl->jet_q.front().hp == 0) return MISSION_INIT;
//temporary solution
        shoot(lvl,assets);
//decisions
        decision(lvl->jet_q,assets);
        action(lvl,assets);
        boss_ability_use(assets,lvl);
        target(lvl,assets);
//transformation
        transform(lvl,assets);
        move_radar(lvl);

        collision(lvl,assets);
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
            spawn_prompt_screen(assets,alleg5,lvl,0);
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

        lvl->tick = (lvl->tick >= FPS ? 0 : lvl->tick+1);
    }
}

return QUIT; //do not change that
}



