#include "jet.h"
#include "level.h"
#include "main.h"
#include "render_level.h"

void debug_data(struct LevelInst * level, struct asset_data * asset)
{/*
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
        }*/
}


void al_draw_filled_centered_rectangle(float dx, float dy, float dw, float dh,ALLEGRO_COLOR color)
{
    al_draw_filled_rectangle(dx-dw/2,dy-dh/2,dx+dw/2,dy+dh/2,color);
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


void update_prompt_screen(sf::RenderWindow & display, LevelInst * level)
{
for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++)
{
    object->body.x = display.getSize().x/2;
    object->body.y = display.getSize().y/2;
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



void spawn_prompt_screen(asset_data * asset, sf::RenderWindow & display, LevelInst * level, unsigned short type) 
{
int display_width = display.getSize().x;
int display_height = display.getSize().y; 


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












void draw_pause_screen(struct LevelInst * level, struct asset_data * asset, sf::RenderWindow & display)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display); 

//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
al_draw_filled_rectangle(0,0,window_width,window_height,al_map_rgba(10,10,10,120));
al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending

al_draw_filled_rectangle(window_width/2-300,window_height/2-100,window_width/2+300,window_height/2+100,al_map_rgb(27,27,17));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),window_width/2,window_height/2-15,ALLEGRO_ALIGN_CENTER,"PAUSED");
std::string desc = "Press ESC to unpause.";


desc += "\nPress F to exit to menu.";
desc += "\nPress Z to reset mission.";
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),window_width/2,window_height/2,400,10,ALLEGRO_ALIGN_CENTER,desc.c_str());

}



void zoom(asset_data * assets, short direction)
{
if(direction > 0) assets->scale_factor+=0.1;
if(direction < 0) assets->scale_factor-=0.1;


if(assets->scale_factor < assets->config.zoomLowerLimit) assets->scale_factor = assets->config.zoomLowerLimit;
if(assets->scale_factor > assets->config.zoomUpperLimit) assets->scale_factor = assets->config.zoomUpperLimit;

}


void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset, sf::RenderWindow & display)
{
int window_width = display.getSize().x;
int window_height = display.getSize().y;
std::vector<JetInst>::iterator player = level->jet_q.begin();
short nightColorCoef = 255 - asset->lvl_data[level->level_name].isNight * 76;
{//jet section
    
    

    for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++)
    {
    float dist = distance(&player->curr,&object->curr);
    if(dist < asset->config.fadeDistance + asset->config.fadingLength) 
    {

        int full_hp = asset->jet_data[object->type].hp;


        short visibilityCoef = (dist < asset->config.fadeDistance ? 255 : (1.f - (dist-asset->config.fadeDistance)/asset->config.fadingLength) * 255.f);

        sf::Sprite model;
        model.setTexture(asset->jet_texture[object->type]);
        model.setOrigin(sf::Vector2f(24,24));
        model.setPosition(object->curr.x, object->curr.y);
        model.setRotation(object->curr.turn_angle * 180);
        model.setColor(sf::Color(nightColorCoef,nightColorCoef,nightColorCoef,visibilityCoef));
        model.setScale(asset->scale_factor,asset->scale_factor);
        display.draw(model);



        if(object->curr.speed > 2.9f && object->curr.speed <= 3.1f)
        {
            short whichCone = 0;
            for(whichCone; whichCone < 5; whichCone++)
            {
                if(object->curr.speed < 2.9f + (float) whichCone * 0.04f) break;
            }
            //l_draw_tinted_scaled_rotated_bitmap_region(asset->prt_texture[SONIC_CONE],whichCone * al_get_bitmap_height(asset->prt_texture[SONIC_CONE]),0,al_get_bitmap_height(asset->prt_texture[SONIC_CONE]),al_get_bitmap_height(asset->prt_texture[SONIC_CONE]),
            //    al_map_rgba_f(1.f, 1.f, 1.f, 0.8f - fabs( (float) (whichCone-2)/5) - (float) rand()/RAND_MAX * 0.05f ),24,24,x_diff,y_diff,asset->scale_factor,asset->scale_factor,object->curr.turn_angle,0);
        }
            //al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending


        
        if(object->ID == player->botTarget && level->radar.mode == 2)
        {
            sf::CircleShape targetIndicator(21.f,36);
            targetIndicator.setPosition(sf::Vector2f(object->curr.x,object->curr.y));
            targetIndicator.setOutlineThickness(2);
            targetIndicator.setOutlineColor(sf::Color(240,10,10,255));
            display.draw(targetIndicator);
        }
        
         
        

        float indicator_fadeDistance = asset->config.fadeDistance * (1.0f - asset->lvl_data[level->level_name].isNight * 0.3f);

            if(dist < indicator_fadeDistance && object != player)
            {

                /*al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb_f(1,0,0));
                    if(asset->jet_data[object->type].isBoss) 
                    {
                        al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb_f(0,0,0));
                        al_draw_filled_rectangle(x_diff-10,y_diff-9,x_diff+10,y_diff-6,al_map_rgb_f((1 - object->hp/full_hp),object->hp/full_hp,0));
                    }
                    else 
                    {
                        al_draw_filled_triangle(x_diff-8,y_diff-9,   x_diff+8,y_diff-9, x_diff, y_diff-2,al_map_rgb_f(1,0,0));
                        al_draw_filled_rectangle(x_diff-7,y_diff-9,x_diff+7,y_diff-6,al_map_rgb_f((1 - object->hp/full_hp),object->hp/full_hp,0));
                    }*/
                if(object->mode != PATROL)
                {
                    sf::Text indicator("!",asset->font,8);
                    indicator.setPosition(sf::Vector2f(object->curr.x+8,object->curr.y-9));
                    display.draw(indicator);

                }
            }


        #ifdef NDEBUG
            if(object != player)
            {
                al_draw_textf(alleg5->font,al_map_rgb(240,140,0),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff-9,0,"%d",object->mode);
                al_draw_textf(alleg5->font,al_map_rgb(240,0,240),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff+3,0,"%d",object->at_work);
            }
        #endif


    }
    


    }
}


for(std::vector<ProjInst>::iterator object = level->proj_q.begin(); object != level->proj_q.end(); object++)
{
    sf::Sprite model(asset->proj_texture[object->type]);
    model.setPosition(object->curr.x,object->curr.y);
    
    model.setOrigin(24,24);
    model.setRotation(object->curr.turn_angle * 180);
    model.setScale(asset->scale_factor,asset->scale_factor);


    if(asset->proj_data[object->type].trait.DMGfall)
    {
        model.setColor(sf::Color(object->color.r,object->color.g,object->color.b,sqrt( 255.f * object->decay / (asset->proj_data[object->type].decay + object->launcher->decay))));
    }
    else
    {
        model.setColor(object->color);
    }

    display.draw(model);


}

//al_draw_tinted_scaled_rotated_bitmap(asset->jet_texture[player->type],al_map_rgb_f(nightColorCoef,nightColorCoef,nightColorCoef),24,24,
//    window_width/2,window_height/2,asset->scale_factor,asset->scale_factor,player->curr.turn_angle,0);

nightColorCoef = 255 - asset->lvl_data[level->level_name].isNight * 38;

//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
//particle section
{
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        float base_scale = (object->isFalling ? (float) object->decay / asset->prt_data[object->type].decay : 1 );


        sf::Color color = sf::Color(
            (object->color ? object->color->r : 1.f),(object->color ? object->color->g : 1.f),(object->color ? object->color->b : 1.f)
            , (object->isFading ? (float)object->decay / asset->prt_data[object->type].decay  : 1.f  ));

        if(object->type == JET)
        {
            sf::Sprite model;
            model.setTexture(asset->jet_texture[object->type]);
            model.setOrigin(sf::Vector2f(24,24));
            model.setPosition(object->curr.x, object->curr.y);
            model.setRotation(object->curr.turn_angle * 180);
            model.setColor(sf::Color(nightColorCoef,nightColorCoef,nightColorCoef,color.a));
            model.setScale(asset->scale_factor*object->scale_x * base_scale,asset->scale_factor*object->scale_y * base_scale);
            display.draw(model);
        }
        else if(object->type == PIXEL)
        {
            sf::RectangleShape pixel(sf::Vector2f(object->scale_x,object->scale_y));
            pixel.setPosition(object->curr.x,object->curr.y);
            pixel.setFillColor(color);
            pixel.setScale(sf::Vector2f(asset->scale_factor,asset->scale_factor));
            display.draw(pixel);
        }else
        {
            /*if(asset->prt_data[object->type].anim.isAnimated)
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
                al_draw_tinted_scaled_rotated_bitmap(object->bitmap,color,24,24,
            asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2,asset->scale_factor *object->scale_x*base_scale,asset->scale_factor *object->scale_y*base_scale,
            object->curr.turn_angle,object->flip_img
            );

            }*/

            
        }

    }
}
//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending

/*if(player->status[OBSCURE] >= 0)
{
    std::vector<JetInst>::iterator object = findJet(level->jet_q,player->status[OBSCURE]);
    if(object < level->jet_q.end())
    {
        float dist = distance(&player->curr,&object->curr);
        float termination_length = 2 * asset->config.fadeDistance + asset->config.fadingLength;

            float influence_scale = ( dist < termination_length ? 
            (dist > asset->config.fadeDistance ? (0.4f) * (termination_length - dist) / (termination_length - asset->config.fadeDistance) + 0.3f   : 0.7f)
            :
            0.3f   
            );


            float circle_scale = asset->scale_factor * (1.f - influence_scale) *( (float) asset->config.fadeDistance / (al_get_bitmap_width(asset->ui_texture[1])/2));

        ALLEGRO_BITMAP * buffer = al_get_backbuffer(alleg5->display);
        ALLEGRO_BITMAP * shadow = al_create_bitmap(window_width,window_height);
        al_set_target_bitmap(shadow);
        al_clear_to_color(al_map_rgba_f(0.f,0.f,0.f,1.f * influence_scale));
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO); //copy blending
        al_draw_tinted_scaled_bitmap(asset->ui_texture[1], al_map_rgba_f(1.f,1.f,1.f,1.f * influence_scale), 
        0,0,al_get_bitmap_width(asset->ui_texture[1]),al_get_bitmap_height(asset->ui_texture[1]),
        (window_width - al_get_bitmap_width(asset->ui_texture[1])* circle_scale)/2,(window_height - al_get_bitmap_height(asset->ui_texture[1])* circle_scale)/2,al_get_bitmap_width(asset->ui_texture[1]) * circle_scale, al_get_bitmap_height(asset->ui_texture[1]) * circle_scale, 0);
        
        
        
        //(window_width - al_get_bitmap_width(asset->ui_texture[1]) * asset->scale_factor )/2, (window_height - al_get_bitmap_height(asset->ui_texture[1])* asset->scale_factor)/2,0);
        al_set_target_bitmap(buffer);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_bitmap(shadow,0,0,0);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
        al_destroy_bitmap(shadow);
    }
}*/
}






void draw_radar(asset_data * asset, std::vector<JetInst>::iterator reference ,  RadarInst * radar, float x, float y, float angle, ALLEGRO_COLOR bkgr_color)
{
/*
    sf::Color indicator;
    if(reference->type == MIG21) indicator = sf::Color(240,240,0);
    else indicator = sf::Color(0,240,0);
    float rad_pointer = angle_addition(angle,radar->turn_angle);


    //al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);


    al_draw_arc(x,y,28*asset->config.UIscale,angle+fabs(radar->range_rad),-fabs(2*radar->range_rad),bkgr_color,40*asset->config.UIscale);
    //al_draw_filled_pieslice(x,y,48 * asset->config.UIscale,angle+fabs(radar->range_rad),-fabs(2*radar->range_rad),bkgr_color);

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

    //al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
    al_draw_line(x + 18*cos(rad_pointer),y + 18*sin(rad_pointer),
                x + 48*cos(rad_pointer)* asset->config.UIscale,y + 48*sin(rad_pointer)* asset->config.UIscale,
                indicator,0.8); //radar seeker line
*/
}






void draw_ui(struct LevelInst * level, struct asset_data * asset, sf::RenderWindow & display)
{
int window_width = display.getSize().x;
int window_height = display.getSize().y;

std::vector<JetInst>::iterator player = level->jet_q.begin();


sf::RectangleShape HP_background(sf::Vector2f(asset->config.UIscale * 80,asset->config.UIscale*35));
HP_background.setPosition(sf::Vector2f(window_width- asset->config.UIscale * 80,window_height - asset->config.UIscale*35));
HP_background.setFillColor(sf::Color(0,20,20,255));
display.draw(HP_background);

/*########
### HUD ##
########*/




/*########
### HP ###
########*/
{
char const * HP_text;
sf::Color HP_color;
float current_HP = (float) player->hp / asset->jet_data[player->type].hp;
if(current_HP > 0.9)
{
    HP_text = "OK";
    HP_color = sf::Color(0,240,0,255);
}
else if ( current_HP > 0.7)
{
    HP_text = "OK";
    HP_color = sf::Color(240,240,0,255);

}
else if( current_HP > 0.3) 
{
    HP_text = "Damaged";
    HP_color = sf::Color(240,240,0,255);
}
else
{
    HP_text = "Damaged";
    HP_color = sf::Color(240,0,255);
}
sf::Text draw_HP_text(HP_text,asset->font,12);
draw_HP_text.setPosition(window_width- draw_HP_text.getGlobalBounds().width  -10 * asset->config.UIscale,window_height-20*asset->config.UIscale);
draw_HP_text.setColor(HP_color);
display.draw(draw_HP_text);
}


/*########
## BAR ###
########*/
{


    bool used;
    sf::Text GunText("GUN",asset->font,12);
    used = (player->weapon[0].ammo != 0);
    GunText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    GunText.setPosition(sf::Vector2f(window_height-12,0));

    sf::Text MslText("MSL",asset->font,12);
    used = (player->weapon[1].ammo != 0);
    MslText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    MslText.setPosition(sf::Vector2f(window_height-12,GunText.getLocalBounds().width + 5));

    sf::Text SpcText("SPC",asset->font,12);
    used = (player->weapon[2].ammo != 0);
    SpcText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    SpcText.setPosition(MslText.getPosition() + sf::Vector2f(MslText.getGlobalBounds().width + 5,0));

    sf::Text ThrText("THR",asset->font,12);
    used = (player->weapon[2].ammo != 0);
    ThrText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    ThrText.setPosition(SpcText.getPosition() + sf::Vector2f(SpcText.getGlobalBounds().width + 5,0));
    








    bool SPCisGun = (player->weapon[2].launcher->ammo * player->weapon[2].multiplier > 20 );

    float themeLength = GunText.getGlobalBounds().width +5 + MslText.getGlobalBounds().width + 5 + SpcText.getGlobalBounds().width +5 + ThrText.getGlobalBounds().width + 5;
    
    sf::RectangleShape theme(sf::Vector2f(themeLength,20 * asset->config.UIscale));
    theme.setPosition(0,window_height- 20 * asset->config.UIscale);
    theme.setFillColor(sf::Color(0,20,20));
    //font theme; not to confuse wuth gun ammo indicator


    float ammo_percentage = (float) player->weapon[0].ammo / (asset->laun_data[player->weapon[0].type].ammo *  player->weapon[0].multiplier);
    float mag_percentage = (float) player->weapon[0].magazine / player->weapon[0].launcher->magazine;
    sf::Color ammo_color;
    int rectangle_height = 60;
    
    
    
    
    
    //ammo indicator
    sf::RectangleShape ammoBar(sf::Vector2f(GunText.getGlobalBounds().width,rectangle_height*ammo_percentage* asset->config.UIscale));
    ammo_color = (ammo_percentage > 0.4 ? sf::Color(250,250,250) : sf::Color(250,pow((ammo_percentage/0.4),3) * 250.f,pow((ammo_percentage/0.4),3) * 250.f) );
    ammoBar.setFillColor(ammo_color);
    ammoBar.setOrigin(0,ammoBar.getSize().y);
    ammoBar.setPosition(GunText.getPosition());


    //mag cooler
    if(mag_percentage <= 0.4)
    {
    sf::RectangleShape GunCool(sf::Vector2f(80.f * mag_percentage,20));
    GunCool.setOrigin(10,GunCool.getSize().y - 40);
    GunCool.setFillColor(mag_percentage ? sf::Color(250,250,250,  153.f * (float)(0.4 - mag_percentage) / 0.4) : sf::Color(240,120,60,153));
    GunCool.setPosition(display.getSize()/2.f - sf::Vector2f(100,0) );
    display.draw(GunCool);
    }





    

 //missile indicator
    for(int i = 0; i< player->weapon[1].ammo; i++)
    {
    sf::RectangleShape bar(sf::Vector2f(MslText.getGlobalBounds().width,2));
    bar.setFillColor(i+player->weapon[1].magazine >= player->weapon[1].ammo ?  sf::Color(240,120,60)  :  sf::Color (200,180,100)  );
    bar.setPosition((GunText.getGlobalBounds().width +5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*(i+1));
    }
//special indicator


    if(SPCisGun)
    {
        float ammo_percentage = (float) player->weapon[2].ammo / (asset->laun_data[player->weapon[2].type].ammo * player->weapon[2].multiplier);
        float mag_percentage = (float) player->weapon[2].magazine / player->weapon[2].launcher->magazine;
        if(mag_percentage <= 0.4)
        {

            sf::RectangleShape SpcCool(sf::Vector2f(80.f * mag_percentage,20));
            SpcCool.setOrigin(10,SpcCool.getSize().y - 40);
            SpcCool.setFillColor(mag_percentage ? sf::Color(250,250,250,  153.f * (float)(0.4 - mag_percentage) / 0.4) : sf::Color(240,120,60,153));
            SpcCool.setPosition(display.getSize()/2.f + sf::Vector2f(100,0) );
            display.draw(SpcCool);
        }

    sf::RectangleShape ammoBar(sf::Vector2f(SpcText.getGlobalBounds().width,rectangle_height*ammo_percentage* asset->config.UIscale));
    ammo_color = (ammo_percentage > 0.4 ? sf::Color(250,250,250) : sf::Color(250,pow((ammo_percentage/0.4),3) * 250.f,pow((ammo_percentage/0.4),3) * 250.f) );
    ammoBar.setFillColor(ammo_color);
    ammoBar.setOrigin(0,ammoBar.getSize().y);
    ammoBar.setPosition(SpcText.getPosition());
    }
    else
    {

        for(int i = 0; i< player->weapon[2].ammo; i++)
        {
            sf::RectangleShape bar(sf::Vector2f(SpcText.getGlobalBounds().width,2));
            bar.setFillColor(i+player->weapon[2].magazine >= player->weapon[2].ammo ?  sf::Color(200,180,100)  :  sf::Color (250,250,250)  );
            bar.setPosition(
                (GunText.getGlobalBounds().width +5 + MslText.getGlobalBounds().width + 5)* asset->config.UIscale,
                window_height-20.5* asset->config.UIscale -3*(i+1));
        }

    }
//throttle
/*
al_draw_filled_rectangle((al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5 +al_get_text_width(alleg5->font,"SPC")+5)*asset->config.UIscale,
window_height-20*asset->config.UIscale,
(al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5+ al_get_text_width(alleg5->font,"SPC")+5 +al_get_text_width(alleg5->font,"THR")+5)*asset->config.UIscale,
window_height - (20+16)*asset->config.UIscale,al_map_rgb_f(0.1,0.1,0.1));

state_change_limit * player_limit = (player->overwrite_limit ? player->overwrite_limit : &asset->jet_data[player->type].alter_limit);
float throttleFactor = (player->alter.target_speed - player_limit->speed_limit[0]) / (player_limit->speed_limit[1] - player_limit->speed_limit[0])* 0.6f + 0.4f;
al_draw_textf(alleg5->font,al_map_rgb(240,240,240),(al_get_text_width(alleg5->font,"GUN")+5 + al_get_text_width(alleg5->font,"MSL")+5 +al_get_text_width(alleg5->font,"SPC")+5),window_height - (20+16)*asset->config.UIscale,0,"%.0f%%",throttleFactor*100);
*/


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
        if(distance(&player->curr,&object->curr) < level->radar.range_dist && fabs(rad_dist) < level->radar.range_rad && !asset->jet_data[object->type].isBoss) 
        {
            ALLEGRO_COLOR indicator;
            if(distance(&player->curr,&object->curr) < asset->config.fadeDistance + asset->config.fadingLength) indicator = al_map_rgb(240,240,0);
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