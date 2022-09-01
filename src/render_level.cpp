#include "jet.h"
#include "level.h"
#include "main.h"
#include "render_level.h"

void debug_data(struct LevelInst * level, struct asset_data * asset, sf::Window & display)
{/*
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
        }*/
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


void update_prompt_screen(sf::Window & display, LevelInst * level)
{
short num = 0;
for(std::vector<prompt_screen>::iterator object = level->prompt_q.begin(); object != level->prompt_q.end(); object++ , num++)
{
    object->body.x = display.getSize().x/2 + 25*num;
    object->body.y = display.getSize().y/2 + 15*num;
    
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
int window_width = display.getSize().x;
int window_height = display.getSize().y;

sf::RectangleShape shade(sf::Vector2f(window_width,window_height));
shade.setFillColor(sf::Color(10,10,10,120));
display.draw(shade);


sf::RectangleShape prompt(sf::Vector2f(600,200));
prompt.setPosition(sf::Vector2f(window_width/2,window_height/2));
display.draw(prompt);

//al_draw_filled_rectangle(window_width/2-300,window_height/2-100,window_width/2+300,window_height/2+100,al_map_rgb(27,27,17));
sf::Text title;
title.setFont(asset->font);
title.setString("PAUSED");
title.setPosition(sf::Vector2f(window_width/2,window_height/2-15));
display.draw(title);


std::string desc_text = "Press ESC to unpause.";
if(level->level_name < ENUM_LVL_TYPE_FIN) desc_text += "\nPress F to exit to menu.";
else desc_text += "\nPress F to reset mission.";


sf::Text desc;
desc.setFont(asset->font);
desc.setString(desc_text.c_str());
desc.setPosition(sf::Vector2f(window_width/2,window_height/2));
display.draw(desc);



}



void zoom(asset_data * assets, short direction)
{
if(direction > 0) assets->scale_factor+=0.1;
if(direction < 0) assets->scale_factor-=0.1;


if(assets->scale_factor < assets->config.zoomLowerLimit) assets->scale_factor = assets->config.zoomLowerLimit;
if(assets->scale_factor > assets->config.zoomUpperLimit) assets->scale_factor = assets->config.zoomUpperLimit;
//al_set_mouse_z(0); //ticker bound
}


void draw(struct LevelInst * level, std::vector<JetInst>::iterator reference, struct asset_data * asset, sf::RenderWindow & display)
{
int window_width = display.getSize().x;
int window_height = display.getSize().y;

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

        sf::Sprite obj_model;
        obj_model.setTexture(asset->jet_texture[object->type]);
        obj_model.setOrigin(sf::Vector2f(23,23));
        obj_model.setPosition(x_diff, y_diff);
        obj_model.setRotation(object->curr.turn_angle * 180);



        if(dist >= asset->config.fadeDistance)
        {
        obj_model.setColor(sf::Color(255,255,255,255 - 255*(dist-600)/200));
        }
        display.draw(obj_model);
        
        if(object->ID == player->botTarget && level->radar.mode == 2) 
        {
            sf::CircleShape circle(21);
            circle.setOutlineThickness(2);
            circle.setOutlineColor(sf::Color(240,10,10,255));
            circle.setFillColor(sf::Color(255,255,255,0));
            display.draw(circle);
        }

        
        /*
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
        to find elegant way*/ 

        if(object->mode != PATROL)
        {
            sf::Text text;
            text.setString("!");
            text.setFont(asset->font);
            text.setColor(sf::Color(240,240,0,255));
            text.setPosition(sf::Vector2f(x_diff+8,y_diff-9));
            display.draw(text);

        }
        #ifdef DEBUG
            //al_draw_textf(alleg5->font,al_map_rgb(240,140,0),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff-9,0,"%d",object->mode);
            //al_draw_textf(alleg5->font,al_map_rgb(240,0,240),x_diff+8+al_get_text_width(alleg5->font,"!"),y_diff+3,0,"%d",object->at_work);
        #endif


    }
    


    }
}


for(std::vector<ProjInst>::iterator object = level->proj_q.begin(); object != level->proj_q.end(); object++)
{


    sf::Sprite obj_model;
    obj_model.setTexture(asset->jet_texture[object->type]);
    obj_model.setOrigin(sf::Vector2f(23,23));
    obj_model.setPosition(asset->scale_factor *(object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2);
    obj_model.setRotation(object->curr.turn_angle * 180);
    obj_model.setColor(sf::Color(object->color.r,  object->color.g,  object->color.b,(asset->proj_data[object->type].trait.DMGfall ? sqrt((float) object->decay / (asset->proj_data[object->type].decay + object->launcher->decay)) * 255 : 255)));
    display.draw(obj_model);

}


//particle section
{
    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        sf::Color color(
            (object->color ? object->color->r : 255),(object->color ? object->color->g : 255),(object->color ? object->color->b : 255)
            , (object->isDecaying ? (float)object->decay / asset->prt_data[object->type].decay  : 255  ));


        sf::Sprite obj_model;
        

        obj_model.setTexture(asset->jet_texture[object->type]);
        obj_model.setOrigin(sf::Vector2f(24,24));
        obj_model.setPosition(sf::Vector2f(asset->scale_factor * (object->curr.x - reference->curr.x) +window_width/2, asset->scale_factor * (object->curr.y - reference->curr.y) + window_height/2));
        obj_model.setRotation(object->curr.turn_angle * 180);
        

        /*if(object->type == PIXEL) //not used
        {
            al_draw_filled_rectangle(asset->scale_factor * (object->curr.x - reference->curr.x -object->scale_x/2.) +window_width/2,
            asset->scale_factor * (object->curr.y - reference->curr.y -object->scale_y/2.) + window_height/2,
            asset->scale_factor * (object->curr.x - reference->curr.x +object->scale_x/2.) +window_width/2,
            asset->scale_factor * (object->curr.y - reference->curr.y +object->scale_y/2.) + window_height/2,
            color
            );
        }else*/
        {
            if(asset->prt_data[object->type].anim.isAnimated)
            {
                int which_region = ( (asset->prt_data[object->type].decay - object->decay) /asset->prt_data[object->type].anim.animationClock);
                
                obj_model.setTextureRect(sf::IntRect(which_region*48,0,48,48));
                //address flip
                
            }
            

            
        }
        display.draw(obj_model);
    }
}



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
sf::Text draw_HP_text;
draw_HP_text.setFont(asset->font);
draw_HP_text.setString(HP_text);
draw_HP_text.setPosition(window_width- draw_HP_text.getGlobalBounds().width  -10 * asset->config.UIscale,window_height-20*asset->config.UIscale);
draw_HP_text.setColor(HP_color);
display.draw(draw_HP_text);
}


/*########
## AMMO ##
########*/
{
    

    bool used;
    sf::Text GunText;
    used = (player->weapon[0].ammo != 0);
    GunText.setFont(asset->font);
    GunText.setString("GUN");
    GunText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    GunText.setPosition(sf::Vector2f(window_height-12,0));
    sf::Text MslText;
    used = (player->weapon[1].ammo != 0);
    MslText.setFont(asset->font);
    MslText.setString("MSL");
    MslText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    MslText.setPosition(sf::Vector2f(window_height-12,GunText.getLocalBounds().width + 5));
    sf::Text SpcText;
    used = (player->weapon[2].ammo != 0);
    SpcText.setFont(asset->font);
    SpcText.setString("SPC");
    SpcText.setColor(sf::Color(100+140*used,100+140*used,100+140*used,255));
    SpcText.setPosition(sf::Vector2f(window_height-12,GunText.getLocalBounds().width + 5 + MslText.getLocalBounds().width + 5));

    sf::RectangleShape ammo_theme(sf::Vector2f((GunText.getLocalBounds().width + 5 + MslText.getLocalBounds().width + 5 + SpcText.getLocalBounds().width + 5) * asset->config.UIscale,20 * asset->config.UIscale));
    ammo_theme.setFillColor(sf::Color(0,20,20,255));
    ammo_theme.setPosition(sf::Vector2f(0,window_height- 20 * asset->config.UIscale));

    display.draw(ammo_theme);
    display.draw(GunText);
    display.draw(MslText);
    display.draw(SpcText);



    bool SPCisGun = (player->weapon[2].launcher->projectile - asset->proj_data < ENUM_BULLET_TYPE_FIN)  ;


    float ammo_percentage = (float) player->weapon[0].ammo / (asset->laun_data[player->weapon[0].type].ammo * asset->jet_data[player->type].weapon_mult[0]);
    float mag_percentage = (float) player->weapon[0].magazine / player->weapon[0].launcher->magazine;
    int rectangle_height = 60;
    
//ammo indicator
    sf::Color ammo_color = (ammo_percentage > 0.4 ? sf::Color(250,250,250,255) : sf::Color(250,pow((ammo_percentage/0.4)*250,3),pow((ammo_percentage/0.4)*250,3),255) );
    sf::RectangleShape GunBox(sf::Vector2f(GunText.getLocalBounds().width , rectangle_height*ammo_percentage* asset->config.UIscale));
    GunBox.setPosition(sf::Vector2f(0,window_height - 20 * asset->config.UIscale));
    GunBox.setFillColor(ammo_color);
    display.draw(GunBox);

    //mag cooler
    if(mag_percentage <= 0.4)
    {
        sf::RectangleShape GunCool(sf::Vector2f(20,80.0 * mag_percentage));
        GunCool.setOrigin(10,40);
        GunCool.setPosition(window_width/2-110,window_height/2);
        GunCool.setFillColor(mag_percentage ? sf::Color(250,250,250,153 * (float)(0.4 - mag_percentage) / 0.4) : sf::Color(240,120,60,153));
        display.draw(GunCool);
    }




/*
    bool used;
    used = (player->weapon[0].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),0,window_height-12* asset->config.UIscale,0,"GUN");
    used = (player->weapon[1].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),(GunText.getLocalBounds().width+5)* asset->config.UIscale,window_height-12* asset->config.UIscale,0,"MSL");    //rocket bar
    used = (player->weapon[2].ammo != 0);
    al_draw_text(alleg5->font,al_map_rgb(100+140*used,100+140*used,100+140*used),(GunText.getLocalBounds().width+5+al_get_text_width(alleg5->font,"MSL")+5)* asset->config.UIscale,window_height-12* asset->config.UIscale,0,"SPC");    //rocket bar
*/



//missile indicator
    /* rewrite
    for(int i = 0; i< player->weapon[1].ammo; i++)
    {
    ALLEGRO_COLOR bar_color = al_map_rgb(200,180,100);//al_map_rgb(240,230,140);
    if(i+player->weapon[1].magazine >= player->weapon[1].ammo) bar_color = al_map_rgb(240,120,60);
    al_draw_line((GunText.getLocalBounds().width+5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*i ,
    
    (GunText.getLocalBounds().width+5 + al_get_text_width(alleg5->font,"MSL"))* asset->config.UIscale ,window_height-20.5* asset->config.UIscale -3*i,
    bar_color,1);
    }
    */
//special indicator


    if(SPCisGun)
    {
        float ammo_percentage = (float) player->weapon[2].ammo / (asset->laun_data[player->weapon[2].type].ammo * asset->jet_data[player->type].weapon_mult[2]);
        float mag_percentage = (float) player->weapon[2].magazine / player->weapon[2].launcher->magazine;

        sf::Color ammo_color = (ammo_percentage > 0.4 ? sf::Color(250,250,250,255) : sf::Color(250,pow((ammo_percentage/0.4)*250,3),pow((ammo_percentage/0.4)*250,3),255) );
        sf::RectangleShape SpcBox(sf::Vector2f(SpcText.getLocalBounds().width , rectangle_height*ammo_percentage* asset->config.UIscale));
        SpcBox.setPosition(sf::Vector2f(GunText.getLocalBounds().width + 5 + MslText.getLocalBounds().width + 5,window_height - 20 * asset->config.UIscale));
        SpcBox.setFillColor(ammo_color);
        display.draw(SpcBox);

        //mag cooler
        if(mag_percentage <= 0.4)
        {
            sf::RectangleShape SpcCool(sf::Vector2f(20,80.0 * mag_percentage));
            SpcCool.setOrigin(10,40);
            SpcCool.setPosition(window_width/2+110,window_height/2);
            SpcCool.setFillColor(mag_percentage ? sf::Color(250,250,250,153 * (float)(0.4 - mag_percentage) / 0.4) : sf::Color(240,120,60,153));
            display.draw(SpcCool);
        }    

    }
    else
    {
/*rewrite
        for(int i = 0; i< player->weapon[2].ammo; i++)
    {
    ALLEGRO_COLOR bar_color = al_map_rgb(250,250,250);
    if(i+player->weapon[2].magazine >= player->weapon[2].ammo) bar_color = al_map_rgb(200,180,100);
    al_draw_line(
        (GunText.getLocalBounds().width+5+al_get_text_width(alleg5->font,"MSL")+5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*i ,
        (GunText.getLocalBounds().width+5 + al_get_text_width(alleg5->font,"MSL") + al_get_text_width(alleg5->font,"SPC")+5)* asset->config.UIscale,window_height-20.5* asset->config.UIscale -3*i,
    bar_color,1);
    }


*/

    }





}



/*########
## RADAR #
########*/
/*
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
ALLEGRO_COLOR indicator;
if(player->type == MIG21) indicator = al_map_rgb(240,240,0);
else indicator = al_map_rgb(0,240,0);
float rad_pointer = angle_addition(player->curr.turn_angle,level->radar.turn_angle);


//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
al_draw_filled_pieslice(window_width/2,window_height/2,48 * asset->config.UIscale,player->curr.turn_angle+fabs(level->radar.range_rad),-fabs(2*level->radar.range_rad),al_map_rgba_f(  indicator.r, indicator.g, indicator.b,0.2));

if(level->radar.mode == 2)
{
if(player->botTarget != -1) al_draw_filled_circle(window_width/2 + 30 * cos(player->curr.turn_angle-fabs(level->radar.range_rad) - 0.12), window_height/2 + 30 * sin(player->curr.turn_angle-fabs(level->radar.range_rad)- 0.12),4,al_map_rgba(120,0,0,122));
al_draw_circle(window_width/2 + 30 * cos(player->curr.turn_angle-fabs(level->radar.range_rad) - 0.12), window_height/2 + 30 * sin(player->curr.turn_angle-fabs(level->radar.range_rad)- 0.12),4,al_map_rgb(0,0,0), 1 );

}

for(std::vector<RadarNode>::iterator object = level->radar.node_q.begin(); object != level->radar.node_q.end(); object++)
{
    float node_pointer = angle_addition(player->curr.turn_angle,object->rad_dist);
    float x_pos = window_width/2 + cos(node_pointer) * (16 + (48 -2-16)* object->dist / level->radar.range_dist) * asset->config.UIscale;
    float y_pos = window_height/2 +  sin(node_pointer) * (16 + (48 -2-16)* object->dist / level->radar.range_dist) * asset->config.UIscale;
    float opacity;
    
    switch(level->radar.mode)
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
al_draw_line(window_width/2 + 18*cos(player->curr.turn_angle), window_height/2 + 18*sin(player->curr.turn_angle),
            window_width/2 + 48*cos(player->curr.turn_angle) * asset->config.UIscale, window_height/2 + 48*sin(player->curr.turn_angle)* asset->config.UIscale,
            al_map_rgba_f(indicator.r, indicator.g, indicator.b, 0.25),0.8); //lead angle line

al_draw_arc(window_width/2,window_height/2,  (16 +  30 *(asset->config.fadeDistance + asset->config.fadingLength) / level->radar.range_dist) * asset->config.UIscale,player->curr.turn_angle+fabs(level->radar.range_rad),-fabs(2*level->radar.range_rad),al_map_rgba_f(  indicator.r, indicator.g, indicator.b,0.2),0.8); //render distance radar arc reference

//al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
al_draw_line(window_width/2 + 18*cos(rad_pointer),window_height/2 + 18*sin(rad_pointer),
            window_width/2 + 48*cos(rad_pointer)* asset->config.UIscale,window_height/2 + 48*sin(rad_pointer)* asset->config.UIscale,
            indicator,0.8); //radar seeker line

}
break;
}


if(asset->config.additionalRadar)
{
	float radar_x_offset = window_width / 6;
	float radar_y_offset = window_height/2;

    ALLEGRO_COLOR indicator;
    if(player->type == MIG21) indicator = al_map_rgb(240,240,0);
    else indicator = al_map_rgb(0,240,0);
    float rad_pointer = angle_addition(-PI/2,level->radar.turn_angle);


    //al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_filled_pieslice(radar_x_offset,radar_y_offset,48 * asset->config.UIscale,-PI/2+fabs(level->radar.range_rad),-fabs(2*level->radar.range_rad),al_map_rgba_f( 0.2,0.2,0.2,0.7));

    if(level->radar.mode == 2)
    {
    if(player->botTarget != -1) al_draw_filled_circle(radar_x_offset + 30 * cos(-PI/2-fabs(level->radar.range_rad) - 0.12), radar_y_offset + 30 * sin(-PI/2-fabs(level->radar.range_rad)- 0.12),4,al_map_rgba(120,0,0,122));
    al_draw_circle(radar_x_offset + 30 * cos(-PI/2-fabs(level->radar.range_rad) - 0.12), radar_y_offset + 30 * sin(-PI/2-fabs(level->radar.range_rad)- 0.12),4,al_map_rgb(0,0,0), 1 );

    }

    for(std::vector<RadarNode>::iterator object = level->radar.node_q.begin(); object != level->radar.node_q.end(); object++)
    {
        float node_pointer = angle_addition(-PI/2,object->rad_dist);
        float x_pos = radar_x_offset + cos(node_pointer) * (16 + (48 -2-16)* object->dist / level->radar.range_dist) * asset->config.UIscale;
        float y_pos = radar_y_offset +  sin(node_pointer) * (16 + (48 -2-16)* object->dist / level->radar.range_dist) * asset->config.UIscale;
        float opacity;
        
        switch(level->radar.mode)
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
    al_draw_line(radar_x_offset + 18*cos(-PI/2), radar_y_offset + 18*sin(-PI/2),
                radar_x_offset + 48*cos(-PI/2) * asset->config.UIscale, radar_y_offset + 48*sin(-PI/2)* asset->config.UIscale,
                al_map_rgba_f(indicator.r, indicator.g, indicator.b, 0.25),0.8); //lead angle line

    al_draw_arc(radar_x_offset,radar_y_offset,  (16 +  30 *(asset->config.fadeDistance + asset->config.fadingLength) / level->radar.range_dist) * asset->config.UIscale,-PI/2+fabs(level->radar.range_rad),-fabs(2*level->radar.range_rad),al_map_rgba_f(  indicator.r, indicator.g, indicator.b,0.2),0.8); //render distance radar arc reference

    //al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
    al_draw_line(radar_x_offset + 18*cos(rad_pointer),radar_y_offset + 18*sin(rad_pointer),
                radar_x_offset + 48*cos(rad_pointer)* asset->config.UIscale,radar_y_offset + 48*sin(rad_pointer)* asset->config.UIscale,
                indicator,0.8); //radar seeker line

}











al_draw_scaled_rotated_bitmap(asset->jet_texture[player->type],23,23,
    window_width/2,window_height/2,asset->scale_factor,asset->scale_factor,player->curr.turn_angle,0);
*/


/*########
# PROMPT #
########*/

{
for(std::vector<prompt_screen>::iterator prompt = level->prompt_q.begin(); prompt != level->prompt_q.end(); prompt++)
{
//al_draw_filled_rectangle(prompt->body.x-prompt->body.width/2,prompt->body.y-prompt->body.height/2,prompt->body.x+prompt->body.width/2,prompt->body.y+prompt->body.height/2,al_map_rgb(27,27,17));
//al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),prompt->body.x ,prompt->body.y- prompt->body.height/2 + 10,prompt->body.width-100,10,ALLEGRO_ALIGN_CENTER,prompt->body.name.c_str());
//al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),prompt->body.x ,prompt->body.y - prompt->body.height/3,prompt->body.width-50,10,ALLEGRO_ALIGN_CENTER,prompt->body.desc.c_str());
}
}

/*########
## PAUSE #
########*/


if(level->pauseEngaged)
{
    draw_pause_screen(level,asset,display);
}


}