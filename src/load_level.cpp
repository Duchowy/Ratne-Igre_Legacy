#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"
#include "movement.h"


struct graph_data
{
std::array<short,100> node;
short low_pos;
short mid_pos;
short high_pos;
};

void update_graph(graph_data * graph,state_change_limit * alter, float lower, float upper)
{
    double iterator = (double) (upper - lower)/100.0;

    graph->low_pos = 0;
    graph->high_pos = 99;
    graph->mid_pos = 49;


    for(int i = 0; i<100; i++)
    {
        if(fabs(lower+i*iterator - alter->speed_limit[0])/2. <= iterator) graph->low_pos = i;
        if(fabs(lower+i*iterator - alter->speed_limit[1])/2. <= iterator) graph->high_pos = i;
        if(fabs(lower+i*iterator - alter->default_speed)/2. <= iterator) graph->mid_pos = i;

        graph->node[i] = (double) 100 * movement_coef_calculate(alter,lower+i*iterator);
    }
}


int process_click(std::array<box_string, 2> & button,sf::RenderWindow & display)
{
sf::Vector2i mouse = sf::Mouse::getPosition(display);

for(int i = 0; i < 2; i++)
{
if(button[i].x - button[i].width/2 <= mouse.x && mouse.x <= button[i].x + button[i].width/2     &&      button[i].y - button[i].height/2 <= mouse.y && mouse.y <= button[i].y + button[i].height/2) return i;
}



return -1;
}






void map_button(std::array<box_string, 5> & button,struct selection * choice)
{

switch(choice->player_jet)
        {
            case MIG21: button[0].name = "MIG21"; button[0].desc = "+Excellent maneuverability\n-Low ammo"; break;
            case F4: button[0].name = "F4 PHANTOM"; button[0].desc = "+Decent maneuverability\n+High ammo\n+Decent survivability"; break;
            case F104: button[0].name = "F104"; button[0].desc = "+High top speed\n-Low survivability\n-Slow turn rate"; break;
            case HARRIER: button[0].name = "HARRIER"; button[0].desc = "+High ammo\n-Decent survivability\n-Terrible maneuverability"; break;
        }
for(int i = 0+1 ; i<3 + 1; i++)
{
switch(choice->weapon[i-1])
        {
            case SHVAK: button[i].name = "SHVAK"; button[i].desc = "+High damage\n-Slow firerate"; break;
            case ADEN: button[i].name = "ADEN"; button[i].desc = "+Decent damage\n+Fast firerate\n-High spread\n-Low ammo"; break;
            case GATLING: button[i].name = "GATLING"; button[i].desc = "+Fast firerate\n+High ammo\n-Poor damage"; break;
            case INFRARED: button[i].name = "INFRARED"; button[i].desc = "+Fast\n-Heat guided"; break;
            case RADAR: button[i].name = "RADAR"; button[i].desc = "+Lock from all angles\n-Poor range"; break;
            case FLAK:button[i].name = "FLAK"; button[i].desc = "+High damage\n+AOE damage\n-Slow firerate\n-Low ammo"; break;
            case ZUNI:button[i].name = "ZUNI"; button[i].desc = "+High damage\n+AOE damage\n+Fast firerate\n-Hard to aim"; break;
            case RAILGUN: button[i].name = "RAILGUN"; button[i].desc = "+Fast projectiles\n+Low ammo\n-Mediocre damage\n-Hard to aim"; break;
            case GUNPOD: button[i].name = "GUNPOD"; button[i].desc = "+Fast firerate\n+High ammo\n-Mediocre damage"; break;
        }



}


}

void update_button_pos(std::array<box_string, 5> & button,std::array<box_string, 2> & button_click,  sf::RenderWindow & display)
{
for(int i = 0; i<5; i++)
{
//button[i].x = al_get_display_width(alleg5->display)*(1+2*i)/8;
button[i].x = (float) display.getSize().x/2 + 250*i - 375;

button[i].y = display.getSize().y * 7/10;

}

button_click[0].x = button_click[0].width/2 + 30;
button_click[0].y = display.getSize().y - button_click[0].height/2 - 30;

button_click[1].x = display.getSize().x - button_click[1].width/2 - 30;
button_click[1].y = display.getSize().y - button_click[1].height/2 - 30;

}


void update(std::array<box_string, 5> & button,struct selection * choice, sf::Vector2i & mouse, int decision)
{
short number= -1;


for(int i = 0; i<4 && number ==-1; i++)
    {
    if(   (button[i].x - button[i].width/2 < mouse.x && mouse.x < button[i].x + button[i].width/2) && (button[i].y - button[i].height/2 < mouse.y && mouse.y < button[i].y + button[i].height/2)) number = i;
    }
switch(number)
{
    case 0: //jet
    {
        if(decision > 0 && choice->player_jet < ENUM_JET_TYPE_FIN-1) choice->player_jet++;
        if(decision < 0 && choice->player_jet > 0) choice->player_jet--;
    }
    break;
    case 1: //gun
    {
        if(decision > 0) choice->weapon[number-1] = (choice->weapon[number-1] < ENUM_GUN_TYPE_FIN-1 ? choice->weapon[number-1] + 1 : ENUM_GUN_TYPE_FIN-1);
        if(decision < 0) choice->weapon[number-1] = (choice->weapon[number-1] > 0 ? choice->weapon[number-1]-1 : 0);
    }
    break;
    case 2: //missile
    {
        if(decision > 0) choice->weapon[number-1] = (choice->weapon[number-1] < ENUM_MSL_TYPE_FIN-1 ? choice->weapon[number-1] + 1 : ENUM_MSL_TYPE_FIN-1);
        if(decision < 0) choice->weapon[number-1] = (choice->weapon[number-1] > ENUM_GUN_TYPE_FIN ? choice->weapon[number-1]-1 : ENUM_GUN_TYPE_FIN);
    }
    break;
    case 3: //special
    {
        if(decision > 0 ) choice->weapon[number-1] = (choice->weapon[number-1] < ENUM_LAUNCHER_TYPE_FIN-1 ? choice->weapon[number-1] + 1 : ENUM_LAUNCHER_TYPE_FIN-1);
        if(decision < 0 ) choice->weapon[number-1] = (choice->weapon[number-1] > ENUM_MSL_TYPE_FIN ? choice->weapon[number-1]-1 : ENUM_MSL_TYPE_FIN);
    }
    break;
    
    default:
    break;
    
}
}




void draw(std::array<box_string, 5> & button,std::array<box_string, 2> & button_click,struct asset_data * assets,sf::RenderWindow & display, struct Player_Data * player, graph_data * primary, graph_data * secondary )
{

/*########
# SCROLL #
########*/


for(int i = 0; i<5; i++)
{
sf::RectangleShape button_inst(sf::Vector2f(button[i].width,button[i].height));
button_inst.setFillColor(sf::Color(0,20,20,255));
button_inst.setOrigin(sf::Vector2f(button[i].width/2,button[i].height/2));
button_inst.setPosition(sf::Vector2f(button[i].x,button[i].y));
display.draw(button_inst);

sf::Text sf_name(button[i].name,assets->font,12);
sf_name.setFillColor(sf::Color(240,240,240,255));
sf_name.setOrigin(sf_name.getGlobalBounds().width/2,0);
sf_name.setPosition(button[i].x,button[i].y);
display.draw(sf_name);

sf::Text sf_desc(button[i].desc,assets->font,12);
sf_desc.setFillColor(sf::Color(240,240,240,255));
sf_desc.setOrigin(sf_desc.getGlobalBounds().width/2,0);
sf_desc.setPosition(button[i].x ,button[i].y - button[i].height/2 - sf_desc.getLocalBounds().height - 12);
display.draw(sf_desc);
}

{
sf::Sprite sf_player;
sf_player.setTexture(assets->jet_texture[player->choice.player_jet]);
sf_player.setOrigin(24,24);
sf_player.setPosition(button[0].x,button[0].y - button[0].height/2 - 24 - 48);
display.draw(sf_player);
}

/*########
# BUTTON #
########*/


for(int i = 0; i< 2; i++)
{
sf::RectangleShape sf_button(sf::Vector2f(   button_click[i].width,button_click[i].height));
sf_button.setFillColor(sf::Color(20,20,0,255));
sf_button.setOrigin(button_click[i].width/2,button_click[i].height/2);
sf_button.setPosition(button_click[i].x,button_click[i].y);
display.draw(sf_button);

sf::Text sf_text(button_click[i].name,assets->font,10);
sf_text.setPosition(button_click[i].x,button_click[i].y);
sf_text.setColor(sf::Color(240,240,240,255));
display.draw(sf_text);

}






/*########
## GRAPH #
########*/
/*
int graph_pos_y = 220; //offset from button 0
al_draw_filled_rectangle(button[0].x - 50, button[0].y-graph_pos_y, button[0].x + 50,button[0].y-graph_pos_y-100,al_map_rgb(0,0,0));




for(int i = 0 ; i< 3; i++)
{
    al_draw_line(button[0].x - 50,button[0].y-graph_pos_y - 25 -i*25,button[0].x + 50,button[0].y-graph_pos_y - 25 -i*25,al_map_rgb(240,240,240),0.6);
    al_draw_textf(alleg5->font,al_map_rgb(240,240,240),button[0].x - 90,button[0].y-graph_pos_y-30-i*25,0,"%.2f",(float)(i+1)*0.25 );
}
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[0].x - 90,button[0].y-graph_pos_y - 100 - 10,0,"%%deg/s");
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[0].x + 80,button[0].y-graph_pos_y,0,"IAS");





for(int i = 0; i<100; i++) al_draw_pixel(button[0].x - 50+i,button[0].y-graph_pos_y - primary->node[i],al_map_rgb(120,120,0));
al_draw_line(button[0].x - 50 + primary->low_pos,  button[0].y-graph_pos_y,
button[0].x - 50 + primary->low_pos,button[0].y-graph_pos_y - primary->node[primary->low_pos],al_map_rgb(120,120,0),0.5);
al_draw_line(button[0].x - 50 + primary->mid_pos,  button[0].y-graph_pos_y,
button[0].x - 50 + primary->mid_pos,button[0].y-graph_pos_y - primary->node[primary->mid_pos],al_map_rgb(120,120,0),0.5);
al_draw_line(button[0].x - 50 + primary->high_pos,  button[0].y-graph_pos_y,
button[0].x - 50 + primary->high_pos,button[0].y-graph_pos_y - primary->node[primary->high_pos],al_map_rgb(120,120,0),0.5);

al_draw_textf(alleg5->font,al_map_rgb(240,240,240),button[0].x - 50 + primary->low_pos ,button[0].y-graph_pos_y,ALLEGRO_ALIGN_CENTRE,"%.2f",assets->jet_data[player->choice.player_jet].alter_limit.speed_limit[0]);
al_draw_textf(alleg5->font,al_map_rgb(240,240,240),button[0].x - 50 + primary->high_pos,button[0].y-graph_pos_y,ALLEGRO_ALIGN_CENTRE,"%.2f",assets->jet_data[player->choice.player_jet].alter_limit.speed_limit[1]);

al_draw_textf(alleg5->font,al_map_rgb(120,120,0),button[0].x - 120,button[0].y-graph_pos_y-250,0,"%.2f max deg/s",assets->jet_data[player->choice.player_jet].alter_limit.alter.turn_speed * 180. / PI * assets->config.FPS);



if(player->mod[player->choice.player_jet].engaged)
{
ALLEGRO_COLOR color_override = al_map_rgb(240,180,100);
for(int i = 0; i<100; i++) al_draw_pixel(button[0].x - 50+i,button[0].y-graph_pos_y - secondary->node[i],color_override);
al_draw_line(button[0].x - 50 + secondary->low_pos,  button[0].y-graph_pos_y,
button[0].x - 50 + secondary->low_pos,button[0].y-graph_pos_y - secondary->node[secondary->low_pos],color_override,0.5);
al_draw_line(button[0].x - 50 + secondary->mid_pos,  button[0].y-graph_pos_y,
button[0].x - 50 + secondary->mid_pos,button[0].y-graph_pos_y - secondary->node[secondary->mid_pos],color_override,0.5);
al_draw_line(button[0].x - 50 + secondary->high_pos,  button[0].y-graph_pos_y,
button[0].x - 50 + secondary->high_pos,button[0].y-graph_pos_y - secondary->node[secondary->high_pos],color_override,0.5);

al_draw_textf(alleg5->font,color_override,button[0].x - 50 + secondary->low_pos,button[0].y-graph_pos_y+10,ALLEGRO_ALIGN_CENTRE,"%.2f",player->custom_stat[player->choice.player_jet]->speed_limit[0]);
al_draw_textf(alleg5->font,color_override,button[0].x - 50 + secondary->high_pos,button[0].y-graph_pos_y+10,ALLEGRO_ALIGN_CENTRE,"%.2f",player->custom_stat[player->choice.player_jet]->speed_limit[1]);

al_draw_textf(alleg5->font,color_override,button[0].x ,button[0].y-graph_pos_y-250,0,"-> %.2f max deg/s",assets->jet_data[player->choice.player_jet].alter_limit.alter.turn_speed * 180. / PI * assets->config.FPS);
}



 


*/
}


int eq_select(struct LevelInst * lvl,struct asset_data * assets, sf::RenderWindow & display)
{
std::array<box_string, 5> button = {{{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 20, .height = 90}}};
std::array<box_string, 2> button_click = {{{.width = 190, .height = 90, .name = "<- MAP"},{.width = 190, .height = 90, .name = "MISSION ->"}}};


struct graph_data primary;
struct graph_data secondary;


display.setView(  display.getDefaultView());

update_button_pos(button,button_click,display);


lvl->radar.range_dist = 1800;
lvl->radar.range_rad = PI/6;
lvl->radar.turn_angle = PI/6;
lvl->radar.turn_speed = -(PI/3)/30;
lvl->radar.mode = 0;


update_graph(&primary,&assets->jet_data[lvl->player.choice.player_jet].alter_limit,   0.9 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[0], 1.1 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[1]    );
if(lvl->player.mod[lvl->player.choice.player_jet].engaged) update_graph(&secondary,lvl->player.custom_stat[lvl->player.choice.player_jet],   0.9 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[0], 1.1 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[1]    );



bool kill = 0;
bool redraw = 1;
bool quit = 0;
//al_get_mouse_state_axis(&mouse, 2) //for reading scroll position
sf::Event event;
while(!kill && !quit)
{
    while(display.pollEvent(event))
        {

            switch (event.type)
            {
            case sf::Event::Closed: quit = 1; break;
            case sf::Event::KeyReleased:
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Escape:
                    return LVL_SELECTION;
                    break;
                    case sf::Keyboard::F:
                    kill = 1;
                    break;
                }
            }
            break;


            }

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    sf::Vector2i mouse = sf::Mouse::getPosition(display);
                    update(button,&lvl->player.choice,mouse,event.mouseWheelScroll.delta);
                    update_graph(&primary,&assets->jet_data[lvl->player.choice.player_jet].alter_limit,   0.9 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[0], 1.1 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[1]    );
                    if(lvl->player.mod[lvl->player.choice.player_jet].engaged) update_graph(&secondary,lvl->player.custom_stat[lvl->player.choice.player_jet],   0.9 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[0], 1.1 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[1]    );
                }
            }
            
            
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                int decision = process_click(button_click,display);

                switch(decision)
                {
                    case 0:
                    return LVL_SELECTION;
                    break;
                    case 1:
                    kill = 1;
                    break;
                }

            }
        }


    {
        //draw sequence
        map_button(button,&lvl->player.choice);
        draw(button,button_click,assets,display,&lvl->player,&primary,&secondary);
        display.display();
        display.clear(sf::Color(27,27,27,255));
    }



}



if(quit) return QUIT;
return MISSION_INIT;
}

void destroy_level(asset_data * asset, LevelInst * level)
{
    if(level->gift)
    {
        delete level->gift;
        level->gift = nullptr;
    }
    for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++) 
    {
        if(object->ability) delete object->ability;
        if(object->overwrite_limit) delete object->overwrite_limit;
    }
    level->jet_q.clear();
    for(int i = 0; i < ENUM_JET_TYPE_FIN; i++) 
    {
        level->player.custom_stat[i] = nullptr;
    }


    for(std::vector<ProjInst>::iterator object = level->proj_q.begin(); object != level->proj_q.end(); object++)
    {
        if(object->alter) delete object->alter;
    }

    for(std::vector<ParticleInst>::iterator object = level->prt_q.begin(); object != level->prt_q.end(); object++)
    {
        if(object->color) delete object->color;
    }





    level->proj_q.clear();
    level->prt_q.clear();
    level->radar.node_q.clear();
    level->prompt_q.clear();
    level->tick = 0;
    level->nextID = 0;
    level->pauseEngaged = false;
    level->finished = false;
    level->finalPromptEngaged = false;
}



int spawn_level(asset_data * asset, LevelInst * level)
{
    
    destroy_level(asset,level);
    level->gift = (asset->lvl_data[level->level_name].isBoss && asset->lvl_data[level->level_name].next_level == ENUM_BKGR_TYPE_FIN ? spawn_riven() : nullptr);

    refresh_riven(level,asset);
    std::copy(asset->lvl_data[level->level_name].enemy_quality, asset->lvl_data[level->level_name].enemy_quality+ENUM_BOSS_TYPE_FIN,level->enemy_quality);
    JetInst player = jet_spawn(asset,&level->player.choice,(level->player.mod[level->player.choice.player_jet].engaged ? level->player.custom_stat[level->player.choice.player_jet] : nullptr),0,level->nextID);
    level->jet_q.push_back(player);
    level->nextID++;
    level->jet_q.front().curr.x = 300;
    level->jet_q.front().curr.y = asset->bkgr_texture[level->level_name].getSize().y/2;
    enemy_spawn(level,asset);
    
    asset->scale_factor = asset->config.zoomUpperLimit;


    return MISSION;
}





void level_init(asset_data * asset)
{
for(int i = 0; i< ENUM_BKGR_TYPE_FIN;i++)
{
        
    switch(i)
    {
        case BERLIN:
        {
        int amnt[] = {4,6,1,0,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].next_level = ATLANTIC;
        asset->lvl_data[i].isBoss = false;
        asset->lvl_data[i].isNight = true;
        break;
        }
        case PFERD:
        {
        int amnt[] = {2,0,0,2,0,0};
        //int amnt[] = {0,0,0,0,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].next_level = ENUM_BKGR_TYPE_FIN;
        asset->lvl_data[i].isBoss = false;
        asset->lvl_data[i].isNight = false;
        break;
        }
        case INDIA:
        {
        int amnt[] = {8,6,0,1,0,0};
        //int amnt[] = {0,0,0,12,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].next_level = DNEPR;
        asset->lvl_data[i].isBoss = false;
        asset->lvl_data[i].isNight = false;
        break;
        }
        case DNEPR:
        {
        int amnt[] = {3,0,0,0,2,0};
        //int amnt[] = {0,0,0,0,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].next_level = ENUM_BKGR_TYPE_FIN;
        asset->lvl_data[i].isBoss = true;
        asset->lvl_data[i].isNight = false;
        break;
        }
        case ATLANTIC:
        {
        //int amnt[] = {0,0,0,0,0,0};
        int amnt[] = {0,3,3,0,0,1};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].next_level = ENUM_BKGR_TYPE_FIN;
        asset->lvl_data[i].isBoss = true;
        asset->lvl_data[i].isNight = false;
        break;
        }


    }
asset->lvl_data[i].map_height = asset->bkgr_texture[i].getSize().y;
asset->lvl_data[i].map_width = asset->bkgr_texture[i].getSize().x;


}





}
