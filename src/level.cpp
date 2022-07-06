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
}


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

float distance(std::vector<JetInst>::iterator current, std::vector<JetInst>::iterator target)
{
    return sqrt( pow( target->curr.x - current->curr.x ,2) +  pow(  target->curr.y - current->curr.y ,2));
}

float distance(std::vector<MslInst>::iterator shell, std::vector<JetInst>::iterator target)
{
    return sqrt( pow( target->curr.x - shell->curr.x ,2) +  pow(  target->curr.y - shell->curr.y ,2));
}


void collision(struct LevelInst * input, struct asset_data * asset)
{

//asset->lvl_data[input->level_name]
//bullet vs jet

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
                target->hp -= shell->damage;
                if(target->hp < 0)
                {
                    if(target != input->jet_q.begin()) input->jet_q.erase(target); //to be moved somewhere else, destruction animation
                }
                break;
            }
        }
    }
	if(!destroyed)
	{
        shell->decay--;
	}
    else
    {
        input->bullet_q.erase(shell);
        shell--;
    }
}

//msl vs jet


for(std::vector<MslInst>::iterator shell = input->msl_q.begin(); shell != input->msl_q.end(); shell++)
{
	bool activated = 0;
    for(std::vector<JetInst>::iterator target = input->jet_q.begin(); target != input->jet_q.end(); target++)
	{
		if( shell->decay + 5 <= asset->msl_data[shell->type].decay  && distance(shell,target) < asset->jet_data[target->item.player_jet].hitbox + asset->msl_data[shell->type].radius) //target hit
		{
		activated = 1;
        target->hp -= asset->msl_data[shell->type].damage;
        


        if(target->hp < 0)
        {
            if(target != input->jet_q.begin()) input->jet_q.erase(target); //to be moved somewhere else, destruction animation
        }
		break;
		}
	}
	if(!activated)
	{
        shell->decay--;
	    if(!shell->decay || shell->curr.x <= 0 || shell->curr.x >= asset->lvl_data[input->level_name].map_width || shell->curr.y <= 0 || shell->curr.y >= asset->lvl_data[input->level_name].map_height)
		{
		activated = 1;
        input->msl_q.erase(shell);
		shell--;
		}
	}
    else
    {
        input->msl_q.erase(shell);
		shell--;
    }
}





}

void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset)
{
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
    al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb(255 *(1 - object->hp/full_hp),255*object->hp/full_hp,0));
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




{ //bullet section
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
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
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
}

}



void draw_ui(struct LevelInst * level, struct asset_data * asset, ALLEGRO_FONT * font)
{
std::vector<JetInst>::iterator player = level->jet_q.begin();
al_draw_filled_rectangle(window_width-80,window_height-35,window_width,window_height,al_map_rgb(0,20,20));

/*########
### HP ###
########*/




float current_HP = (float) player->hp / asset->jet_data[player->item.player_jet].hp;
if(current_HP > 0.9) al_draw_text(font,al_map_rgb(0,240,0),window_width-al_get_text_width(font,"OK")-10,window_height-20,0,"OK");
else if ( current_HP > 0.7) al_draw_text(font,al_map_rgb(240,240,0),window_width-al_get_text_width(font,"OK")-10,window_height-20, 0,"OK");
else if( current_HP > 0.3) al_draw_text(font,al_map_rgb(240,240,0),window_width-al_get_text_width(font,"Damaged")-10,window_height-20,0,"Damaged");
else al_draw_text(font,al_map_rgb(240,0,0),window_width-al_get_text_width(font,"Damaged")-10,window_height-20,0,"Damaged");

/*########
## AMMO ##
########*/
al_draw_filled_rectangle(0,window_height,al_get_text_width(font,"GUN")+5 + al_get_text_width(font,"RKT")+5,window_height-20,al_map_rgb(0,20,20));                                                 //font theme

float ammo_percentage = (float) player->weap_ammo[0] / (asset->gun_data[player->item.player_gun].ammo_max * asset->jet_data[player->item.player_jet].gun_mult);
ALLEGRO_COLOR ammo_color;
if(ammo_percentage > 0.4) ammo_color = al_map_rgb(240,240,240);
else ammo_color = al_map_rgb(250,pow((ammo_percentage/0.4),3)*255,pow((ammo_percentage/0.4),3)*255);
al_draw_filled_rectangle(0,window_height-20, al_get_text_width(font,"GUN"), window_height -20 - 60*ammo_percentage,ammo_color);    //ammo bar
bool used = (player->weap_ammo[0] != 0);
al_draw_text(font,al_map_rgb(100+140*used,100+140*used,100+140*used),0,window_height-10,0,"GUN");

used = (player->weap_ammo[1] != 0);
al_draw_text(font,al_map_rgb(100+140*used,100+140*used,100+140*used),al_get_text_width(font,"GUN")+5,window_height-10,0,"RKT");    //rocket bar
for(int i = 0; i< player->weap_ammo[1]; i++) 
al_draw_line(al_get_text_width(font,"GUN")+5,window_height-20.5 -3*i ,al_get_text_width(font,"GUN")+5 + al_get_text_width(font,"RKT"),window_height-20.5 -3*i,
al_map_rgb(240,230,140),1);


/*########
## RADAR #
########*/

    for(std::vector<JetInst>::iterator object = level->jet_q.begin()+1; object != level->jet_q.end(); object++)
    {
        float rad_pointer = atan2(( object->curr.y - player->curr.y) ,(object->curr.x - player->curr.x));
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
        target(lvl,assets);
//transformation
        transform(lvl,assets);

        collision(lvl,assets);
//draw
        //al_draw_bitmap(assets->bkgr_texture[lvl.level_name],window_width/2 - jet_q.front().x,window_height/2 - jet_q.front().y,0);
        al_draw_scaled_rotated_bitmap(assets->bkgr_texture[lvl->level_name],0,0,
        window_width/2 - lvl->jet_q.front().curr.x * assets->scale_factor,window_height/2 - lvl->jet_q.front().curr.y * assets->scale_factor,assets->scale_factor,assets->scale_factor,0,0);

        

        draw(lvl,lvl->jet_q.begin(),assets);
        draw_ui(lvl,assets,alleg5->font);
//debug
        debug_data(lvl,assets,alleg5->font);
        
        al_flip_display();
        al_clear_to_color(al_map_rgb(27,27,27));
        redraw = 0;
        cooldown(lvl->jet_q);

        for(int i =1; i<3; i++) lvl->jet_q.front().will_shoot[i] = 0;
    }
}

return QUIT; //do not change that
}



