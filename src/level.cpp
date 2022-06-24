#include "main.h"
#include "jet.h"
#include "level.h"
#include "movement.h"



void debug_data(struct jet * object, ALLEGRO_FONT * font)
{
        std::string buffer = "";

        for(int i = 0; i<5; i++)
        {
            switch(i)
            {
                case 0: buffer = std::to_string(object->curr_angle_w); break;
                case 1: buffer = std::to_string(object->curr_angle)+" current"; break;
                case 2: buffer = std::to_string(object->target_angle)+" target"; break;
                case 3: buffer = std::to_string(object->weap_ammo[1][1])+" rkt ammo"; break;
                case 4: buffer = std::to_string(object->weap_ammo[1][0])+" rkt max ammo"; break;
            }
            al_draw_text(font,al_map_rgb(240,0,240),0,i*10,0,buffer.c_str());
        }
}

void cooldown(std::vector<jet> &input_vec)
{
    #pragma omp parallel for
for(std::vector<jet>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
    for(int i =0;  i<3; i++) //gun cooldown
    {
        if(object->weap_delay[i]) object->weap_delay[i]--;
    }
}


}

float angle_difference(float current, float target)
{
float diff = current-target;
    if(diff>PI && target < 0) diff = diff - 2*PI;
    if(diff<-PI && target >= 0) diff = 2*PI + diff;
return diff;
}

float rad_distance(std::vector<jet>::iterator current, std::vector<jet>::iterator target)
{
float new_angle = atan2(( target->y - current->y) ,(target->x - current->x));
return angle_difference(current->curr_angle,new_angle);
}

float distance(std::vector<jet>::iterator current, std::vector<jet>::iterator target)
{
    return sqrt( pow( target->x - current->x ,2) +  pow(  target->y - current->y ,2));
}

float distance(std::vector<missile>::iterator shell, std::vector<jet>::iterator target)
{
    return sqrt( pow( target->x - shell->x ,2) +  pow(  target->y - shell->y ,2));
}


void collision(std::vector<jet> &input_vec, std::vector<missile> &shell_vec, struct lvl_dat * limit)
{
for(std::vector<missile>::iterator shell = shell_vec.begin(); shell != shell_vec.end(); shell++)
{
	bool activated = 0;
    for(std::vector<jet>::iterator target = input_vec.begin(); target != input_vec.end(); target++)
	{
		if( shell->decay[1] + 5 <= shell->decay[0]  && distance(shell,target) < target->hitbox + shell->radius) //target hit
		{
		activated = 1;
        target->hp[1] -= shell->damage;
        
        if(target->hp[1] < 0)
        {
            if(target != input_vec.begin()) input_vec.erase(target); //to be moved somewhere else, destruction animation
        }
		break;
		}
	}
	if(!activated)
	{
        shell->decay[1]--;
	    if(!shell->decay[1] || shell->x <= 0 || shell->x >= limit->map_width || shell->y <= 0 || shell->y >= limit->map_height)
		{
		activated = 1;
        shell_vec.erase(shell);
		shell--;
		}
	}
    else
    {
        shell_vec.erase(shell);
		shell--;
    }
}


}

void collision(std::vector<jet> &input_vec, std::vector<bullet> &shell_vec, struct lvl_dat * limit)
{
for(std::vector<bullet>::iterator shell = shell_vec.begin(); shell != shell_vec.end(); shell++)
{
	bool destroyed = 0;
    if(!shell->decay[1] || shell->x <= 0 || shell->x >= limit->map_width || shell->y <= 0 || shell->y >= limit->map_height)
		{
		destroyed = 1;
		}
    else
    {
        for(std::vector<jet>::iterator target = input_vec.begin(); target != input_vec.end(); target++)
        {
            if( sqrt( pow( target->x - shell->x ,2) +  pow(  target->y - shell->y ,2)) < target->hitbox) //target hit
            {
                destroyed = 1;
                target->hp[1] -= shell->damage;
                if(target->hp[1] < 0)
                {
                    if(target != input_vec.begin()) input_vec.erase(target); //to be moved somewhere else, destruction animation
                }
                break;
            }
        }
    }
	if(!destroyed)
	{
        shell->decay[1]--;
	}
    else
    {
        shell_vec.erase(shell);
        shell--;
    }
}


}

void draw(std::vector<bullet> &input_vec, std::vector<jet>::iterator reference, struct asset_data * assets)
{
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
for(std::vector<bullet>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
{
    
    ALLEGRO_COLOR col = al_map_rgba(object->color[0],object->color[1],object->color[2],255*object->decay[1] / object->decay[0]);
    al_draw_tinted_scaled_rotated_bitmap(assets->bullet_texture[object->type][1],col,23,23,
    assets->scale_factor * (object->x - reference->x) +window_width/2, assets->scale_factor * (object->y - reference->y) + window_height/2,
    object->height,object->width,object->angle,0);
    
    col = al_map_rgba(255,255,255,255*object->decay[1] / object->decay[0]);
    al_draw_tinted_scaled_rotated_bitmap(assets->bullet_texture[object->type][0],col,23,23,
    assets->scale_factor * (object->x - reference->x) +window_width/2, assets->scale_factor * (object->y - reference->y) + window_height/2,
    object->height,object->width,object->angle,0);
    
}
al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
}

void draw(std::vector<missile> &input_vec, std::vector<jet>::iterator reference, struct asset_data * assets)
{
for(std::vector<missile>::iterator object = input_vec.begin(); object != input_vec.end(); object++)
    {
    al_draw_scaled_rotated_bitmap(assets->msl_texture[object->type],23,23,
    assets->scale_factor *(object->x - reference->x) +window_width/2, assets->scale_factor * (object->y - reference->y) + window_height/2,
    assets->scale_factor,assets->scale_factor,object->curr_angle,0);
    }
}

void draw(std::vector<jet> &input_vec, struct asset_data * assets)
{
std::vector<jet>::iterator player = input_vec.begin();
al_draw_scaled_rotated_bitmap(assets->jet_texture[player->type],23,23,
window_width/2,window_height/2,assets->scale_factor,assets->scale_factor,player->curr_angle,0);

for(std::vector<jet>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
{
/*
float x_diff = object->x - player->x + window_width/2;
float y_diff = object->y - player->y + window_height/2;

al_draw_rotated_bitmap(assets->jet_texture[object->type],23,23,x_diff, y_diff ,object->curr_angle,0);

al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb(255 *(1 - object->hp[1]/object->hp[0]),255*object->hp[1]/object->hp[0],0));
*/


if(distance(player,object) < 800) 
{
float x_dist = object->x - player->x;
float y_dist = object->y - player->y;

float x_diff = assets->scale_factor * (x_dist) + window_width/2;
float y_diff = assets->scale_factor * (y_dist) + window_height/2;



al_draw_scaled_rotated_bitmap(assets->jet_texture[object->type],23,23,
x_diff, y_diff ,assets->scale_factor,assets->scale_factor,object->curr_angle,0);
al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb(255,0,0));
al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb(255 *(1 - object->hp[1]/object->hp[0]),255*object->hp[1]/object->hp[0],0));
}



}
}

void draw_ui(std::vector<jet> &input_vec, struct lvl_dat * limit, ALLEGRO_FONT * font)
{
std::vector<jet>::iterator player = input_vec.begin();
al_draw_filled_rectangle(window_width-80,window_height-35,window_width,window_height,al_map_rgb(0,20,20));

/*########
### HP ###
########*/
float current_HP = (float) player->hp[1]/player->hp[0];
if(current_HP > 0.9) al_draw_text(font,al_map_rgb(0,240,0),window_width-al_get_text_width(font,"OK")-10,window_height-20,0,"OK");
else if ( current_HP > 0.7) al_draw_text(font,al_map_rgb(240,240,0),window_width-al_get_text_width(font,"OK")-10,window_height-20, 0,"OK");
else if( current_HP > 0.3) al_draw_text(font,al_map_rgb(240,240,0),window_width-al_get_text_width(font,"Damaged")-10,window_height-20,0,"Damaged");
else al_draw_text(font,al_map_rgb(240,0,0),window_width-al_get_text_width(font,"Damaged")-10,window_height-20,0,"Damaged");

/*########
## AMMO ##
########*/
al_draw_filled_rectangle(0,window_height,al_get_text_width(font,"GUN")+5 + al_get_text_width(font,"RKT")+5,window_height-20,al_map_rgb(0,20,20));                                                 //font theme

float ammo_percentage = (float) player->weap_ammo[0][1] / player->weap_ammo[0][0];
ALLEGRO_COLOR ammo_color;
if(ammo_percentage > 0.4) ammo_color = al_map_rgb(240,240,240);
else ammo_color = al_map_rgb(250,pow((ammo_percentage/0.4),3)*255,pow((ammo_percentage/0.4),3)*255);
al_draw_filled_rectangle(0,window_height-20, al_get_text_width(font,"GUN"), window_height -20 - 60*ammo_percentage,ammo_color);    //ammo bar
bool used = (player->weap_ammo[0][1] != 0);
al_draw_text(font,al_map_rgb(100+140*used,100+140*used,100+140*used),0,window_height-10,0,"GUN");

used = (player->weap_ammo[1][1] != 0);
al_draw_text(font,al_map_rgb(100+140*used,100+140*used,100+140*used),al_get_text_width(font,"GUN")+5,window_height-10,0,"RKT");    //rocket bar
for(int i = 0; i< player->weap_ammo[1][1]; i++) 
al_draw_line(al_get_text_width(font,"GUN")+5,window_height-20.5 -3*i ,al_get_text_width(font,"GUN")+5 + al_get_text_width(font,"RKT"),window_height-20.5 -3*i,
al_map_rgb(240,230,140),1);


/*########
## RADAR #
########*/

    for(std::vector<jet>::iterator object = input_vec.begin()+1; object != input_vec.end(); object++)
    {
        float rad_pointer = atan2(( object->y - player->y) ,(object->x - player->x));
        float rad_dist = rad_distance(player,object);
        if(distance(player,object) < 1800 && fabs(rad_dist) < PI/6) 
        {
            ALLEGRO_COLOR indicator;
            if(distance(player,object) < 800) indicator = al_map_rgb(240,240,0);
            else indicator = al_map_rgb(0,240,0);
            al_draw_line(window_width/2 + 12*cos(rad_pointer),window_height/2 + 12*sin(rad_pointer),
            window_width/2 + 28*cos(rad_pointer),window_height/2 + 28*sin(rad_pointer),
            indicator,0.6);
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


int level(allegro5_data*alleg5, asset_data * assets, lvl_dat * lvl)
{
bool kill = 0;
bool redraw = 1;
assets->scale_factor = 1.0;



std::vector<bullet> bullet_q;
std::vector<jet> jet_q;
std::vector<missile> msl_q;


jet_q.push_back(lvl->player);


enemy_init(jet_q,lvl);



ALLEGRO_MOUSE_STATE mouse;

while(!kill)
{
    jet_q.front().will_shoot[0] = 0; //temporary solution
    al_wait_for_event(alleg5->queue,&alleg5->event);
    switch (alleg5->event.type)
    {
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
                jet_q.front().speed_mode = AFTERBURNER;
                break;
                case ALLEGRO_KEY_S:
                jet_q.front().speed_mode = AIRBRAKE;
                break;
                case ALLEGRO_KEY_SPACE:
                jet_q.front().will_shoot[1] = 1;
                break;
                case ALLEGRO_KEY_ESCAPE:
                return SELECTION;
                break;
            }
        break;
        }
        case ALLEGRO_EVENT_KEY_UP:
        {
            switch(alleg5->event.keyboard.keycode)
            {
                case ALLEGRO_KEY_W:
                jet_q.front().speed_mode = STANDARD;
                break;
                case ALLEGRO_KEY_S:
                jet_q.front().speed_mode = STANDARD;
                break;
            }
        break;
        }
    }
    if(redraw && al_is_event_queue_empty(alleg5->queue))
    {
        { //player actions
        al_get_mouse_state(&mouse);
        if(mouse.buttons & 1) jet_q.front().will_shoot[0] = 1; //left mouse button
        jet_q.front().target_angle = atan2((mouse.y-window_height/2) ,(mouse.x - window_width/2));
        }
        if(jet_q.front().hp[1] < 0) return MISSION;
//temporary solution
        shoot(jet_q,bullet_q,lvl);
        shoot(jet_q,msl_q,lvl);
//decisions
        decision(jet_q,lvl);
        action(jet_q,bullet_q,lvl);
        target(jet_q,msl_q);
//movement
        move(msl_q,lvl);
        move(jet_q, lvl);
        move(bullet_q, lvl);
//rotation
        rotation(jet_q);
        rotation(msl_q);
        collision(jet_q,bullet_q,lvl);
        collision(jet_q,msl_q,lvl);
//draw
        //al_draw_bitmap(assets->bkgr_texture[lvl.level_name],window_width/2 - jet_q.front().x,window_height/2 - jet_q.front().y,0);
        al_draw_scaled_rotated_bitmap(assets->bkgr_texture[lvl->level_name],0,0,
        window_width/2 - jet_q.front().x * assets->scale_factor,window_height/2 - jet_q.front().y * assets->scale_factor,assets->scale_factor,assets->scale_factor,0,0);

        
        draw(jet_q,assets);
        draw(bullet_q,jet_q.begin(),assets);
        draw(msl_q,jet_q.begin(),assets);
        draw_ui(jet_q,lvl,alleg5->font);
//debug
        //debug_data(&jet_q.front(),alleg5->font);
        
        al_flip_display();
        al_clear_to_color(al_map_rgb(27,27,27));
        redraw = 0;
        cooldown(jet_q);

        for(int i =1; i<3; i++) jet_q.front().will_shoot[i] = 0;
    }
}

return QUIT; //do not change that
}



