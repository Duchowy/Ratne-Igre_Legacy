#include "jet.h"
#include "level.h"
#include "main.h"
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
                case 1: buffer = std::to_string(object->alter.target_speed)+" target speed"; break;
                case 2: buffer = std::to_string(object->alter.target_angle)+" target"; break;
                case 3: buffer = std::to_string(object->alter.turn_speed)+" turn speed"; break;
                case 4: buffer = std::to_string(object->curr.speed)+" speed"; break;
                case 5: buffer = std::to_string(object->weapon[0].engaged)+" shoot"; break;
            }
            al_draw_text(font,al_map_rgb(240,0,240),0,i*10,0,buffer.c_str());
        }
}



void destroy_prompt_screen(asset_data * asset, LevelInst * level, std::vector<prompt_screen>::iterator object, bool decision)
{
if(object != level->prompt_q.end() && level->prompt_q.size()) object = level->prompt_q.end()-1;

switch(object->type)
{
case 0:
level->prompt_q.erase(object);
level->finished = 1;
break;
case 1:
if(decision == 1)
{
level->player.mod[level->player.choice.player_jet] = *level->gift;
}
level->prompt_q.erase(object);
level->finished = 1;
break;
default:
break;




}
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


std::string extract_riven(riven * mod, unsigned short trait)
{
    std::string text = "";
    if(!mod->type[trait]) return "None";

    if(mod->type[trait] <= 6) text += (mod->value[trait] >= 1. ? "+" : "") + std::to_string(   (mod->value[trait] >= 1. ? mod->value[trait] - 1. : 1. - mod->value[trait])  *  100.     ) + "% " ;
    else text += "+" + std::to_string(mod->value[trait]) + " ";



    switch(mod->type[trait])
        {
        case 1: text+= "Top speed"; break;
        case 2: text+= "Brake speed"; break;
        case 3: text+= "Speed bounds"; break;
        case 4: text+= "Engine power";break;
        case 5: text+= "Airbrake effect"; break;
        case 6: text+= "Rapidity"; break;
        case 7: text+= "Max turn speed"; break;
        case 8: text+= "Max responsivness"; break;
        case 9: text+= "Default speed"; break;
        case 10: text+= "Mobility coefficient"; break;
        }

return text;
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
int width = 400;
int height = 150;

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
            rander = rand()%12;
            switch(rander)
            {
                case 0: desc = "Tip: use scroll wheel to zoom in and out"; break;
                case 1: desc = "Tip: try not to die"; break;
                case 2: desc = "Tip: press R to change radar mode"; break;
                case 3: desc = "Tip: press W to speed up"; break;
                case 4: desc = "Tip: press S to slow down"; break;
                case 5: desc = "Tip: speed of your vehicle influences your agility"; break;
                case 6: desc = "Tip: press ESC to pause the game"; break;
                case 7: desc = "Tip: press Spacebar to launch a missile"; break;
                case 8: desc = "Tip: press LMB to shoot your gun"; break;
                case 9: desc = "Tip: press RMB to shoot your special weapon"; break;
                case 10: desc = "Tip: your mod data is saved in gamedata.sav file"; break;
                case 11: desc = "Tip: edit config.cfg to influence game operation"; break;
            }


        }
        else
        {
            title = "Mission accomplished. Return home.";
        }
        decay = 180;
        F_Action = 0;
        Z_Action = 0;
        height = 200;
    }
    break;
    case 1:
    {
        title = "Do you accept the new mod?\n";
        
        for(int i = 0; i< ((level->player.mod[level->player.choice.player_jet].engaged && (level->player.mod[level->player.choice.player_jet].dualSided) || level->gift->dualSided) ? 4 : 2); i++)
        {
            if(level->player.mod[level->player.choice.player_jet].engaged && (level->player.mod[level->player.choice.player_jet].type[i])) desc += extract_riven(&level->player.mod[level->player.choice.player_jet],i);
            else desc += "None";
            desc += " -> ";
            desc += extract_riven(level->gift,i);
            desc += "\n";
        }
        desc+= "\n";
        decay = -1;
        F_Action = 1;
        Z_Action = 1;
        width = 600;
    }

}
if(F_Action) desc += "Press F to confirm"; //F podmień
if(F_Action && Z_Action) desc += "\n";
if(Z_Action) desc += "Press Z to deny";


prompt_screen prompt = {.type=type,{.x = display_width/2 + 25 * level->prompt_q.size(), .y = display_height/2 + 15 * level->prompt_q.size(), .width = width, .height = height,.name = title, .desc = desc},.decay = decay, .F_Action = F_Action, .Z_Action = Z_Action};

level->prompt_q.push_back(prompt);





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



void zoom(asset_data * assets, short direction)
{
if(direction > 0) assets->scale_factor+=0.1;
if(direction < 0) assets->scale_factor-=0.1;


if(assets->scale_factor < assets->config.zoomLowerLimit) assets->scale_factor = assets->config.zoomLowerLimit;
if(assets->scale_factor > assets->config.zoomUpperLimit) assets->scale_factor = assets->config.zoomUpperLimit;
al_set_mouse_z(0); //ticker bound
}


void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset, struct allegro5_data * alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);
std::vector<JetInst>::iterator player = level->jet_q.begin();
{//jet section
    
    

    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
    float dist = distance(player,object);
    if(dist < asset->config.fadeDistance + asset->config.fadingLength) 
    {
        float x_dist = object->curr.x - player->curr.x;
        float y_dist = object->curr.y - player->curr.y;

        float x_diff = asset->scale_factor * (x_dist) + window_width/2;
        float y_diff = asset->scale_factor * (y_dist) + window_height/2;

        int full_hp = asset->jet_data[object->type].hp;

        if(dist < asset->config.fadeDistance)
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
        
        if(object->ID == player->botTarget && level->radar.mode == 2) al_draw_circle(x_diff,y_diff,21,al_map_rgb(240,10,10),2);
        

        
        
        al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
            if(asset->jet_data[object->type].isBoss) 
            {
                al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(0,0,0));
                al_draw_filled_rectangle(x_diff-10,y_diff-9,x_diff+10,y_diff-6,al_map_rgb_f((1 - object->hp/full_hp),object->hp/full_hp,0));
            }
            else 
            {
                al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
                al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb_f((1 - object->hp/full_hp),object->hp/full_hp,0));
            }
        if(object->mode != PATROL)
        {
            al_draw_text(alleg5->font,al_map_rgb(240,240,0),x_diff+8,y_diff-9,0,"!");

        }
        #ifdef DEBUG
            al_draw_textf(alleg5->font,al_map_rgb(240,140,0),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff-9,0,"%d",object->mode);
            al_draw_textf(alleg5->font,al_map_rgb(240,0,240),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff+3,0,"%d",object->at_work);
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

al_draw_scaled_rotated_bitmap(asset->jet_texture[player->type],23,23,
    window_width/2,window_height/2,asset->scale_factor,asset->scale_factor,player->curr.turn_angle,0);


al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
//particle section
{
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        float base_scale = (object->isFalling ? (float) object->decay / asset->prt_data[object->type].decay : 1 );


        ALLEGRO_COLOR color = al_map_rgba_f(
            (object->color ? object->color->r : 1),(object->color ? object->color->g : 1),(object->color ? object->color->b : 1)
            , (object->isFading ? (float)object->decay / asset->prt_data[object->type].decay  : 1  ));

        if(object->type == JET)
        {
            al_draw_tinted_scaled_rotated_bitmap(object->bitmap,color,23,23,
            asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,asset->scale_factor *object->scale_x * base_scale,asset->scale_factor *object->scale_y * base_scale,
            object->curr.turn_angle,object->flip_img
            );
            
        }
        else if(object->type == PIXEL)
        {
            al_draw_filled_rectangle(asset->scale_factor * (object->curr.x - reference->curr.x -object->scale_x/2.) +window_width/2,
            asset->scale_factor * (object->curr.y - reference->curr.y -object->scale_y/2.) + window_height/2,
            asset->scale_factor * (object->curr.x - reference->curr.x +object->scale_x/2.) +window_width/2,
            asset->scale_factor * (object->curr.y - reference->curr.y +object->scale_y/2.) + window_height/2,
            color
            );
        }else
        {
            if(asset->prt_data[object->type].anim.isAnimated)
            {
                int which_region = ( (asset->prt_data[object->type].decay - object->decay) /asset->prt_data[object->type].anim.animationClock);



                al_draw_tinted_scaled_rotated_bitmap_region(object->bitmap,which_region*48,0,48,48,color,24,24,
                asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,
                asset->scale_factor *object->scale_x * base_scale,asset->scale_factor *object->scale_y * base_scale,
                object->curr.turn_angle,object->flip_img
                );
                
            }
            else
            {
                al_draw_tinted_scaled_rotated_bitmap(object->bitmap,color,23,23,
            asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,asset->scale_factor *object->scale_x*base_scale,asset->scale_factor *object->scale_y*base_scale,
            object->curr.turn_angle,object->flip_img
            );

            }

            
        }

    }
}
al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending


}



void draw_radar(asset_data * asset, std::vector<JetInst>::iterator reference ,  RadarInst * radar, float x, float y, float angle, ALLEGRO_COLOR bkgr_color)
{

    ALLEGRO_COLOR indicator;
    if(reference->type == MIG21) indicator = al_map_rgb(240,240,0);
    else indicator = al_map_rgb(0,240,0);
    float rad_pointer = angle_addition(angle,radar->turn_angle);


    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_pieslice(x,y,48 * asset->config.UIscale,angle+fabs(radar->range_rad),-fabs(2*radar->range_rad),bkgr_color);

    if(radar->mode == 2)
    {
    if(reference->botTarget != -1) al_draw_filled_circle(x + 30 * cos(angle-fabs(radar->range_rad) - 0.12), y + 30 * sin(angle-fabs(radar->range_rad)- 0.12),4,al_map_rgba(120,0,0,122));
    al_draw_circle(x + 30 * cos(angle-fabs(radar->range_rad) - 0.12), y + 30 * sin(angle-fabs(radar->range_rad)- 0.12),4,al_map_rgb(0,0,0), 1 );

    }

    for(std::vector<RadarNode>::iterator object = radar->node_q.begin(); object != radar->node_q.end(); object++)
    {
        float node_pointer = angle_addition(angle,object->rad_dist);
        float x_pos = x + cos(node_pointer) * (16 + (48 -2-16)* object->dist / radar->range_dist) * asset->config.UIscale;
        float y_pos = y +  sin(node_pointer) * (16 + (48 -2-16)* object->dist / radar->range_dist) * asset->config.UIscale;
        float opacity;
        
        switch(radar->mode)
        {
            case 0:
            opacity = 1 - pow(1 - (float) object->decay/48,4);
            break;
            case 1:
            opacity = 1 - pow(1 - (float) object->decay/24,2);
            break;
            case 2:
            opacity = 1 - pow(1 - (float) object->decay/24,2);
            break;
        }



            al_draw_filled_circle(x_pos, y_pos,
            2* asset->config.UIscale,(
            object->isTarget ? 
            al_map_rgba_f(0.98,0.04,0.04,opacity) :
            al_map_rgba_f(  indicator.r, indicator.g, indicator.b, opacity )
            )
            );
            al_draw_circle(x_pos,y_pos,2* asset->config.UIscale - 0.7,al_map_rgba_f(0,0,0,opacity),0.7);

    }
    al_draw_line(x + 18*cos(angle), y + 18*sin(angle),
                x + 48*cos(angle) * asset->config.UIscale, y + 48*sin(angle)* asset->config.UIscale,
                al_map_rgba_f(indicator.r, indicator.g, indicator.b, 0.25),0.8); //lead angle line

    al_draw_arc(x,y,  (16 +  30 *(asset->config.fadeDistance + asset->config.fadingLength) / radar->range_dist) * asset->config.UIscale,angle+fabs(radar->range_rad),-fabs(2*radar->range_rad),al_map_rgba_f(  indicator.r, indicator.g, indicator.b,0.2),0.8); //render distance radar arc reference

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
    al_draw_line(x + 18*cos(rad_pointer),y + 18*sin(rad_pointer),
                x + 48*cos(rad_pointer)* asset->config.UIscale,y + 48*sin(rad_pointer)* asset->config.UIscale,
                indicator,0.8); //radar seeker line

}






void draw_ui(struct LevelInst * level, struct asset_data * asset, struct allegro5_data * alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);

std::vector<JetInst>::iterator player = level->jet_q.begin();
al_draw_filled_rectangle(window_width- asset->config.UIscale * 80,window_height - asset->config.UIscale*35,window_width,window_height,al_map_rgb(0,20,20));

/*########
### HUD ##
########*/




/*########
### HP ###
########*/
{
char const * HP_text;
ALLEGRO_COLOR HP_color;
float current_HP = (float) player->hp / asset->jet_data[player->type].hp;
if(current_HP > 0.9)
{
    HP_text = "OK";
    HP_color = al_map_rgb(0,240,0);
}
else if ( current_HP > 0.7)
{
    HP_text = "OK";
    HP_color = al_map_rgb(240,240,0);

}
else if( current_HP > 0.3) 
{
    HP_text = "Damaged";
    HP_color = al_map_rgb(240,240,0);
}
else
{
    HP_text = "Damaged";
    HP_color = al_map_rgb(240,0,0);
}

al_draw_text(alleg5->font,HP_color,window_width-al_get_text_width(alleg5->font,HP_text)-10 * asset->config.UIscale,window_height-20*asset->config.UIscale,0,HP_text);
}


/*########
## AMMO ##
########*/
{
    bool SPCisGun = (player->weapon[2].launcher->ammo * player->weapon[2].multiplier > 20 );
//asset->proj_data[player->weapon[2].launcher->projectile - asset->proj_data].trait.isCountable

    al_draw_filled_rectangle(0,window_height,(al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5 + al_get_text_width(alleg5->font,"SPC")+5) * asset->config.UIscale
    ,window_height- 20 * asset->config.UIscale,al_map_rgb(0,20,20));                                                 //font theme

    float ammo_percentage = (float) player->weapon[0].ammo / (asset->laun_data[player->weapon[0].type].ammo *  player->weapon[0].multiplier);
    float mag_percentage = (float) player->weapon[0].magazine / player->weapon[0].launcher->magazine;
    ALLEGRO_COLOR ammo_color;
    int rectangle_height = 60;
    
//ammo indicator
    ammo_color = (ammo_percentage > 0.4 ? al_map_rgb(240,240,240) : al_map_rgb_f(0.98,pow((ammo_percentage/0.4)*0.98,3),pow((ammo_percentage/0.4)*0.98,3)) );
    al_draw_filled_rectangle(0,window_height-20 * asset->config.UIscale, al_get_text_width(alleg5->font,"GUN") * asset->config.UIscale, window_height -20* asset->config.UIscale - rectangle_height*ammo_percentage* asset->config.UIscale,ammo_color);    
    
    //mag cooler
    if(mag_percentage <= 0.4)
    {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_rectangle(window_width/2 - 120, window_height/2 + 40,window_width/2 - 100, window_height/2 - 40 + ( 80.0 * mag_percentage), mag_percentage ? al_map_rgba_f(0.98,0.98,0.98,0.6 * (float)(0.4 - mag_percentage) / 0.4) : al_map_rgba(240,120,60,153));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
    }





    bool used;
    used = (player->weapon[0].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),0,window_height-12* asset->config.UIscale,0,"GUN");
    used = (player->weapon[1].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),(al_get_text_width(alleg5->font,"GUN")+5)* asset->config.UIscale,window_height-12* asset->config.UIscale,0,"MSL");    //rocket bar
    used = (player->weapon[2].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),(al_get_text_width(alleg5->font,"GUN")+5+al_get_text_width(alleg5->font,"MSL")+5)* asset->config.UIscale,window_height-12* asset->config.UIscale,0,"SPC");    //rocket bar

//missile indicator
    for(int i = 0; i< player->weapon[1].ammo; i++)
    {
    ALLEGRO_COLOR bar_color = al_map_rgb(200,180,100);//al_map_rgb(240,230,140);
    if(i+player->weapon[1].magazine >= player->weapon[1].ammo) bar_color = al_map_rgb(240,120,60);
    al_draw_line((al_get_text_width(alleg5->font,"GUN")+5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*i ,
    
    (al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL"))* asset->config.UIscale ,window_height-20.5* asset->config.UIscale -3*i,
    bar_color,1);
    }
//special indicator


    if(SPCisGun)
    {
        float ammo_percentage = (float) player->weapon[2].ammo / (asset->laun_data[player->weapon[2].type].ammo * player->weapon[2].multiplier);
        float mag_percentage = (float) player->weapon[2].magazine / player->weapon[2].launcher->magazine;
        ALLEGRO_COLOR ammo_color = (ammo_percentage > 0.4 ? al_map_rgb(240,240,240) : al_map_rgb_f(0.98,pow((ammo_percentage/0.4)*0.98,3),pow((ammo_percentage/0.4)*0.98,3)) );;
        if(mag_percentage <= 0.4)
        {
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_filled_rectangle(window_width/2 + 120, window_height/2 + 40,window_width/2 + 100, window_height/2 - 40 + ( 80.0 * mag_percentage),  mag_percentage ? al_map_rgba_f(0.98,0.98,0.98,0.6 * (float)(0.4 - mag_percentage) / 0.4) : al_map_rgba(240,120,60,153));
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
        }
        
        al_draw_filled_rectangle(
        (al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5)* asset->config.UIscale,window_height-20* asset->config.UIscale,
        (al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL") + al_get_text_width(alleg5->font,"SPC")+5)* asset->config.UIscale, window_height -20* asset->config.UIscale - rectangle_height*ammo_percentage* asset->config.UIscale,ammo_color
        );    

    }
    else
    {

        for(int i = 0; i< player->weapon[2].ammo; i++)
    {
    ALLEGRO_COLOR bar_color = al_map_rgb(250,250,250);
    if(i+player->weapon[2].magazine >= player->weapon[2].ammo) bar_color = al_map_rgb(200,180,100);
    al_draw_line(
        (al_get_text_width(alleg5->font,"GUN")+5+al_get_text_width(alleg5->font,"MSL")+5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*i ,
        (al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL") + al_get_text_width(alleg5->font,"SPC")+5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*i,
    bar_color,1);
    }




    }





}



/*########
## RADAR #
########*/

switch(asset->config.radarType)
{
case 1:
{
    float rad_range_delimiter = player->curr.turn_angle+fabs(level->radar.range_rad);
    al_draw_line(window_width/2 + 20*cos(rad_range_delimiter), window_height/2 + 20*sin(rad_range_delimiter),
            window_width/2 + (20 + 16 * asset->config.UIscale )*cos(rad_range_delimiter) ,window_height/2 + (20 + 16* asset->config.UIscale)*sin(rad_range_delimiter) ,
            al_map_rgb(240,240,240),0.8);
    rad_range_delimiter = player->curr.turn_angle-fabs(level->radar.range_rad);
    al_draw_line(window_width/2 + 20*cos(rad_range_delimiter), window_height/2 + 20*sin(rad_range_delimiter),
            window_width/2 + (20 + 16 * asset->config.UIscale )*cos(rad_range_delimiter) ,window_height/2 + (20 + 16* asset->config.UIscale)*sin(rad_range_delimiter) ,
            al_map_rgb(240,240,240),0.8);


    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
        float rad_pointer = atan2(( object->curr.y - player->curr.y) ,(object->curr.x - player->curr.x));
        float rad_dist = rad_distance(&player->curr,&object->curr);
        if(distance(player,object) < level->radar.range_dist && fabs(rad_dist) < level->radar.range_rad && !asset->jet_data[object->type].isBoss) 
        {
            ALLEGRO_COLOR indicator;
            if(distance(player,object) < asset->config.fadeDistance + asset->config.fadingLength) indicator = al_map_rgb(240,240,0);
            else indicator = al_map_rgb(0,240,0);
            al_draw_line(window_width/2 + 14*cos(rad_pointer), window_height/2 + 14*sin(rad_pointer),
            window_width/2 + (14 + 30 * asset->config.UIscale)*cos(rad_pointer) ,window_height/2 + (14 + 30 * asset->config.UIscale)*sin(rad_pointer) ,
            indicator,0.8);

            if(player->botTarget == object->ID)
            {
                indicator = al_map_rgb(240,0,0);
                al_draw_line(window_width/2 + (14 + 18 * asset->config.UIscale)*cos(rad_pointer), window_height/2 + (14 + 18 * asset->config.UIscale)*sin(rad_pointer),
                window_width/2 + (14 + 30 * asset->config.UIscale)*cos(rad_pointer),window_height/2 + (14 + 30 * asset->config.UIscale)*sin(rad_pointer) ,
                indicator,0.8);
            }
        }
        

    }
    if(level->radar.mode == 2)
    {
    if(player->botTarget != -1) al_draw_filled_circle(window_width/2 + 30 * cos(player->curr.turn_angle-fabs(level->radar.range_rad) - 0.12), window_height/2 + 30 * sin(player->curr.turn_angle-fabs(level->radar.range_rad)- 0.12),4,al_map_rgba(120,0,0,122));
    al_draw_circle(window_width/2 + 30 * cos(player->curr.turn_angle-fabs(level->radar.range_rad) - 0.12), window_height/2 + 30 * sin(player->curr.turn_angle-fabs(level->radar.range_rad)- 0.12),4,al_map_rgb(0,0,0), 1 );

    }
}
break;
case 2:
{
ALLEGRO_COLOR indicator = (player->type == MIG21 ? al_map_rgba(240,240,0,51) : al_map_rgba(0,240,0,51));
draw_radar(asset,player,&level->radar,window_width/2,window_height/2,player->curr.turn_angle,indicator);
}
break;
}


if(asset->config.additionalRadar)
{
draw_radar(asset,player,&level->radar,window_width / 6,window_height/2,-PI/2,al_map_rgba_f( 0.2,0.2,0.2,0.7));

}















/*########
# PROMPT #
########*/
{
for(std::vector<prompt_screen>::iterator prompt = level->prompt_q.begin(); prompt != level->prompt_q.end(); prompt++)
{
al_draw_filled_rectangle(prompt->body.x-prompt->body.width/2,prompt->body.y-prompt->body.height/2,prompt->body.x+prompt->body.width/2,prompt->body.y+prompt->body.height/2,al_map_rgb(27,27,17));
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),prompt->body.x ,prompt->body.y- prompt->body.height/2 + 10,prompt->body.width-100,10,ALLEGRO_ALIGN_CENTER,prompt->body.name.c_str());
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),prompt->body.x ,prompt->body.y - prompt->body.height/3,prompt->body.width-50,10,ALLEGRO_ALIGN_CENTER,prompt->body.desc.c_str());
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