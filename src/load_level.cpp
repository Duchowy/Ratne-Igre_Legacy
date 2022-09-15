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


int process_click(std::array<box_string, 2> & button, ALLEGRO_MOUSE_STATE * mouse)
{

al_get_mouse_state(mouse);

for(int i = 0; i < 2; i++)
{
if(button[i].x - button[i].width/2 <= mouse->x && mouse->x <= button[i].x + button[i].width/2     &&      button[i].y - button[i].height/2 <= mouse->y && mouse->y <= button[i].y + button[i].height/2) return i;
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

void update_button_pos(std::array<box_string, 5> & button,std::array<box_string, 2> & button_click,  allegro5_data* alleg5)
{
for(int i = 0; i<5; i++) //item selection
{
button[i].x = (float) al_get_display_width(alleg5->display)/2 + 250*i - 375;

button[i].y = al_get_display_height(alleg5->display) * 7/10;

}

//msl and spc ratio
button[4].x = (button[3].x + button[2].x)/2;




button_click[0].x = button_click[0].width/2 + 30;
button_click[0].y = al_get_display_height(alleg5->display) - button_click[0].height/2 - 30;

button_click[1].x = al_get_display_width(alleg5->display) - button_click[1].width/2 - 30;
button_click[1].y = al_get_display_height(alleg5->display) - button_click[1].height/2 - 30;

}






void update(std::array<box_string, 5> & button,struct selection * choice,ALLEGRO_MOUSE_STATE *mouse)
{
short number= -1;


int decision = al_get_mouse_state_axis(mouse, 2);
for(int i = 0; i<5 && number ==-1; i++)
    {
    if(   (button[i].x - button[i].width/2 < mouse->x && mouse->x < button[i].x + button[i].width/2) && (button[i].y - button[i].height/2 < mouse->y && mouse->y < button[i].y + button[i].height/2)) number = i;
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
    case 4:
    {
        if(decision > 0 ) choice->multiplier[1] = (choice->multiplier[1] < 1.0f ? choice->multiplier[1] + 0.1f : 1.0f);
        if(decision < 0 ) choice->multiplier[1] = (choice->multiplier[1]-0.1f > 0.f ? choice->multiplier[1]-0.1f : 0.0f);
        choice->multiplier[2] = 1.0f - choice->multiplier[1];
    }
    break;
    default:
    break;
    
}
al_set_mouse_z(0); //zero the scroll
}

void draw(std::array<box_string, 5> & button,std::array<box_string, 2> & button_click,struct asset_data * assets,allegro5_data* alleg5, struct Player_Data * player, graph_data * primary, graph_data * secondary )
{

/*########
# SCROLL #
########*/


for(int i = 0; i<4; i++)
{
al_draw_filled_rectangle(button[i].x - button[i].width/2,button[i].y - button[i].height/2,button[i].x + button[i].width/2,button[i].y + button[i].height/2,
al_map_rgb(0,20,20));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[i].x,button[i].y,ALLEGRO_ALIGN_CENTRE,button[i].name.c_str());
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),button[i].x,button[i].y-button[i].height/2-50,200,al_get_font_line_height(alleg5->font),ALLEGRO_ALIGN_CENTRE,button[i].desc.c_str());
}
al_draw_scaled_rotated_bitmap(assets->jet_texture[player->choice.player_jet],al_get_bitmap_width(assets->jet_texture[player->choice.player_jet])/2,al_get_bitmap_height(assets->jet_texture[player->choice.player_jet])/2,
button[0].x,button[0].y-button[0].height/2-al_get_bitmap_height(assets->jet_texture[player->choice.player_jet])-50,2,2,0,0);


//scrolling rectangle
al_draw_filled_rectangle(button[4].x - button[4].width/2,button[4].y - button[4].height/2,button[4].x + button[4].width/2,button[4].y + button[4].height/2,
al_map_rgb(0,20,20));

al_draw_filled_rectangle(button[4].x - button[4].width/2, button[4].y + button[4].height/2, button[4].x, button[4].y + button[4].height/2 - player->choice.multiplier[1] * button[4].height,al_map_rgb(200,180,100));
al_draw_filled_rectangle(button[4].x, button[4].y + button[4].height/2, button[4].x + button[4].width/2, button[4].y + button[4].height/2 - player->choice.multiplier[2] * button[4].height,al_map_rgb(250,250,250));


/*########
# BUTTON #
########*/


for(int i = 0; i< 2; i++)
{
al_draw_filled_rectangle(button_click[i].x - button_click[i].width/2,button_click[i].y - button_click[i].height/2,button_click[i].x + button_click[i].width/2,button_click[i].y + button_click[i].height/2,
al_map_rgb(20,20,0));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button_click[i].x,button_click[i].y,ALLEGRO_ALIGN_CENTRE,button_click[i].name.c_str());



}






/*########
## GRAPH #
########*/
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



 



}


int eq_select(struct LevelInst * lvl,struct asset_data * assets, allegro5_data* alleg5)
{
std::array<box_string, 5> button = {{{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 20, .height = 90}}};
std::array<box_string, 2> button_click = {{{.width = 190, .height = 90, .name = "<- MAP"},{.width = 190, .height = 90, .name = "MISSION ->"}}};


struct graph_data primary;
struct graph_data secondary;


update_button_pos(button,button_click,alleg5);


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
ALLEGRO_MOUSE_STATE mouse;
al_set_mouse_z(0); //zero the scroll
//al_get_mouse_state_axis(&mouse, 2) //for reading scroll position
while(!kill && !quit)
{
    al_wait_for_event(alleg5->queue,&alleg5->event);
    switch (alleg5->event.type)
    {
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
        {
            al_acknowledge_resize(alleg5->display); 
            if(assets->config.autoUIscale) assets->config.UIscale = calculateUIscale(al_get_display_width(alleg5->display), al_get_display_height(alleg5->display));
            update_button_pos(button,button_click,alleg5);
        }
        break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE: quit = 1; break;
        case ALLEGRO_EVENT_TIMER: redraw = 1; break;
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            al_get_mouse_state(&mouse);
            if(al_get_mouse_state_axis(&mouse, 2) != 0)
            {
                update(button,&lvl->player.choice,&mouse);
                update_graph(&primary,&assets->jet_data[lvl->player.choice.player_jet].alter_limit,   0.9 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[0], 1.1 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[1]    );
                if(lvl->player.mod[lvl->player.choice.player_jet].engaged) update_graph(&secondary,lvl->player.custom_stat[lvl->player.choice.player_jet],   0.9 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[0], 1.1 * assets->jet_data[lvl->player.choice.player_jet].alter_limit.speed_limit[1]    );
            }
            break;
        }
        case ALLEGRO_EVENT_KEY_DOWN:
        {
            switch(alleg5->event.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                return LVL_SELECTION;
                break;
                case ALLEGRO_KEY_F: kill = 1; break;
            }
        break;
        }
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        {
            int decision = process_click(button_click,&mouse);

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
        break;
    }

if(redraw && al_is_event_queue_empty(alleg5->queue))
{
    //draw sequence
    map_button(button,&lvl->player.choice);
    draw(button,button_click,assets,alleg5,&lvl->player,&primary,&secondary);
    al_flip_display();
    al_clear_to_color(al_map_rgb(27,27,27));
    redraw = 0;
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
    
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    destroy_level(asset,level);
    level->gift = (asset->lvl_data[level->level_name].isBoss && asset->lvl_data[level->level_name].next_level == ENUM_BKGR_TYPE_FIN ? spawn_riven() : nullptr);

    refresh_riven(level,asset);
    std::copy(asset->lvl_data[level->level_name].enemy_quality, asset->lvl_data[level->level_name].enemy_quality+ENUM_BOSS_TYPE_FIN,level->enemy_quality);
    JetInst player = jet_spawn(asset,&level->player.choice,(level->player.mod[level->player.choice.player_jet].engaged ? level->player.custom_stat[level->player.choice.player_jet] : nullptr),0,level->nextID);
    level->jet_q.push_back(player);
    level->nextID++;
    level->jet_q.front().curr.x = 300;
    level->jet_q.front().curr.y = al_get_bitmap_height(asset->bkgr_texture[level->level_name])/2;
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
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
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
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
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
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
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
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
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
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
        asset->lvl_data[i].next_level = ENUM_BKGR_TYPE_FIN;
        asset->lvl_data[i].isBoss = true;
        asset->lvl_data[i].isNight = false;
        break;
        }


    }
}





}
