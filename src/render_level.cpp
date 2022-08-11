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
                //case 1: if(level->bullet_q.size())   buffer = std::to_string(level->bullet_q.front().curr.speed)+" bul speed"; break;
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
            rander = rand()%10;
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


if(assets->scale_factor < 0.8) assets->scale_factor = 0.8;
if(assets->scale_factor > 1.0) assets->scale_factor = 1.0;
al_set_mouse_z(0); //ticker bound
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
        al_draw_textf(alleg5->font,al_map_rgb(240,140,0),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff-9,0,"%d",object->mode);
        al_draw_textf(alleg5->font,al_map_rgb(240,0,240),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff+3,0,"%d",object->ID);
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