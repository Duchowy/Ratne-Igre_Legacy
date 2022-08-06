#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"
#include "movement.h"





int process_click(std::array<box_string, 2> & button, ALLEGRO_MOUSE_STATE * mouse)
{

al_get_mouse_state(mouse);

for(int i = 0; i < 2; i++)
{
if(button[i].x - button[i].width/2 <= mouse->x && mouse->x <= button[i].x + button[i].width/2     &&      button[i].y - button[i].height/2 <= mouse->y && mouse->y <= button[i].y + button[i].height/2) return i;
}



return -1;
}










void map_button(std::array<box_string, 3> & button,struct curr_selection * choice)
{

switch(choice->item.player_jet)
        {
            case MIG21: button[0].name = "MIG21"; button[0].desc = "+Excellent maneuverability\n-Low ammo"; break;
            case F4: button[0].name = "F4 PHANTOM"; button[0].desc = "+Decent maneuverability\n+High ammo\n+Decent survivability"; break;
            case F104: button[0].name = "F104"; button[0].desc = "+High top speed\n-Low survivability\n-Slow turn rate"; break;
            case HARRIER: button[0].name = "HARRIER"; button[0].desc = "+High ammo\n-Decent survivability\n-Terrible maneuverability"; break;
        }
switch(choice->item.player_gun)
        {
            case SHVAK: button[1].name = "SHVAK"; button[1].desc = "+High damage\n-Slow firerate"; break;
            case ADEN: button[1].name = "ADEN"; button[1].desc = "+Decent damage\n+Fast firerate\n-High spread\n-Low ammo"; break;
            case GATLING: button[1].name = "GATLING"; button[1].desc = "+Fast firerate\n+High ammo\n-Poor damage"; break;
        }
switch(choice->item.player_msl)
        {
            case IR: button[2].name = "INFRARED"; button[2].desc = "+Fast\n-Heat guided"; break;
            case RAD: button[2].name = "RADAR"; button[2].desc = "+Lock from all angles\n-Poor range"; break;
        }
/*
switch(choice->selection)
        {
            case BERLIN: button[3].name = "BERLIN"; button[3].desc = "Expect heavy resistance"; break;
            case INDIA: button[3].name = "INDIA"; button[3].desc = "Search & destroy"; break;
            case PFERD: button[3].name = "PFERD"; button[3].desc = "What?";break;
        }
*/

}

void update_button_pos(std::array<box_string, 3> & button,std::array<box_string, 2> & button_click,  allegro5_data* alleg5)
{
for(int i = 0; i<3; i++)
{
button[i].x = al_get_display_width(alleg5->display)*(1+2*i)/8;
button[i].y = al_get_display_height(alleg5->display) * 7/10;

}

button_click[0].x = button_click[0].width/2 + 30;
button_click[0].y = al_get_display_height(alleg5->display) - button_click[0].height/2 - 30;

button_click[1].x = al_get_display_width(alleg5->display) - button_click[1].width/2 - 30;
button_click[1].y = al_get_display_height(alleg5->display) - button_click[1].height/2 - 30;

}

void update_graph(std::array<int,100> & graph_nodes,state_change_limit * alter)
{
    double iterator = (double) (alter->speed_limit[1] - alter->speed_limit[0])/100.0;
    double low = alter->speed_limit[0];

    for(int i = 0; i<100; i++)
    {
        graph_nodes[i] = (double) 100 * movement_coef_calculate(alter,low+i*iterator);
    }
}




void update(std::array<box_string, 3> & button,struct curr_selection * choice,ALLEGRO_MOUSE_STATE *mouse)
{
short number= -1;


int decision = al_get_mouse_state_axis(mouse, 2);
for(int i = 0; i<3 && number ==-1; i++)
    {
    if(   (button[i].x - button[i].width/2 < mouse->x && mouse->x < button[i].x + button[i].width/2) && (button[i].y - button[i].height/2 < mouse->y && mouse->y < button[i].y + button[i].height/2)) number = i;
    }
switch(number)
{
    case 0: //jet
    {
        if(decision > 0 && choice->item.player_jet < ENUM_JET_TYPE_FIN-1) choice->item.player_jet++;
        if(decision < 0 && choice->item.player_jet > 0) choice->item.player_jet--;
    }
    break;
    case 1: //gun
    {
        if(decision > 0 && choice->item.player_gun < ENUM_GUN_TYPE_FIN-1) choice->item.player_gun++;
        if(decision < 0 && choice->item.player_gun > 0) choice->item.player_gun--;
    }
    break;
    case 2: //missile
    {
        if(decision > 0 && choice->item.player_msl < ENUM_MSL_TYPE_FIN-1) choice->item.player_msl++;
        if(decision < 0 && choice->item.player_msl > 0) choice->item.player_msl--;
    }
    break;
    /*
    case 3: //level
    {
        if(decision > 0 && choice->selection < ENUM_LVL_TYPE_FIN-1) choice->selection++;
        if(decision < 0 && choice->selection > 0) choice->selection--;
    }
    break;
    */
    default:
    break;
}
al_set_mouse_z(0); //zero the scroll
}

void draw(std::array<box_string, 3> & button,std::array<box_string, 2> & button_click,struct asset_data * assets,allegro5_data* alleg5, struct curr_selection * choice, std::array<int,100> & graph_nodes)
{

/*########
# SCROLL #
########*/


for(int i = 0; i<3; i++)
{
al_draw_filled_rectangle(button[i].x - button[i].width/2,button[i].y - button[i].height/2,button[i].x + button[i].width/2,button[i].y + button[i].height/2,
al_map_rgb(0,20,20));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[i].x,button[i].y,ALLEGRO_ALIGN_CENTRE,button[i].name.c_str());
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),button[i].x,button[i].y-button[i].height/2-40,200,al_get_font_line_height(alleg5->font),ALLEGRO_ALIGN_CENTRE,button[i].desc.c_str());
}
al_draw_scaled_rotated_bitmap(assets->jet_texture[choice->item.player_jet],al_get_bitmap_width(assets->jet_texture[choice->item.player_jet])/2,al_get_bitmap_height(assets->jet_texture[choice->item.player_jet])/2,
button[0].x,button[0].y-button[0].height/2-al_get_bitmap_height(assets->jet_texture[choice->item.player_jet])-40,2,2,0,0);

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

al_draw_filled_rectangle(button[0].x - 50, button[0].y-200, button[0].x + 50,button[0].y-300,al_map_rgb(0,0,0));




for(int i = 0 ; i< 3; i++)
{
    al_draw_line(button[0].x - 50,button[0].y-225 -i*25,button[0].x + 50,button[0].y-225 -i*25,al_map_rgb(240,240,240),0.6);
    al_draw_textf(alleg5->font,al_map_rgb(240,240,240),button[0].x - 90,button[0].y-230-i*25,0,"%.2f",(float)(i+1)*0.25 );
}
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[0].x - 90,button[0].y-310,0,"%°/s");
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[0].x + 80,button[0].y-200,0,"IAS");

al_draw_textf(alleg5->font,al_map_rgb(240,240,240),button[0].x - 50,button[0].y-200,ALLEGRO_ALIGN_CENTRE,"%.2f",assets->jet_data[choice->item.player_jet].alter_limit.speed_limit[0]);
al_draw_textf(alleg5->font,al_map_rgb(240,240,240),button[0].x + 50,button[0].y-200,ALLEGRO_ALIGN_CENTRE,"%.2f",assets->jet_data[choice->item.player_jet].alter_limit.speed_limit[1]);



for(int i = 0; i<100; i++) al_draw_pixel(button[0].x - 50+i,button[0].y-200 - graph_nodes[i],al_map_rgb(120,120,0));


 



}


int eq_select(struct LevelInst * lvl,struct asset_data * assets, allegro5_data* alleg5)
{
std::array<box_string, 3> button = {{{.width = 190, .height = 90},{.width = 190, .height = 90},{.width = 190, .height = 90}}};
std::array<box_string, 2> button_click = {{{.width = 190, .height = 90, .name = "<- MAP"},{.width = 190, .height = 90, .name = "MISSION ->"}}};

struct curr_selection choice;
std::array<int,100> graph_nodes;

update_button_pos(button,button_click,alleg5);






choice.item.player_jet = lvl->player.item.player_jet;
choice.item.player_gun = lvl->player.item.player_gun;
choice.item.player_msl = lvl->player.item.player_msl;
choice.item.player_spc = lvl->player.item.player_spc;
lvl->radar.range_dist = 1800;
lvl->radar.range_rad = PI/6;
lvl->radar.turn_angle = PI/6;
lvl->radar.turn_speed = -(PI/3)/30;
lvl->radar.mode = 0;


update_graph(graph_nodes,&assets->jet_data[choice.item.player_jet].alter_limit);




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
                update(button,&choice,&mouse);
                update_graph(graph_nodes,&assets->jet_data[choice.item.player_jet].alter_limit);
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
    map_button(button,&choice);
    draw(button,button_click,assets,alleg5,&choice,graph_nodes);
    al_flip_display();
    al_clear_to_color(al_map_rgb(27,27,27));
    redraw = 0;
}



}










lvl->player.item = choice.item;


if(quit) return QUIT;
return MISSION_INIT;
}

int spawn_level(asset_data * asset, LevelInst * level)
{
    
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    for(std::vector<JetInst>::iterator object = level->jet_q.begin(); object != level->jet_q.end(); object++) if(object->ability) delete object->ability;
    level->jet_q.clear();
    level->bullet_q.clear();
    level->msl_q.clear();
    level->prt_q.clear();
    level->radar.node_q.clear();
    level->prompt_q.clear();
    level->pauseEngaged = false;
    level->finished = false;
    level->finalPromptEngaged = false;




    std::copy(asset->lvl_data[level->level_name].enemy_quality, asset->lvl_data[level->level_name].enemy_quality+ENUM_BOSS_TYPE_FIN,level->enemy_quality);
    level->player = jet_spawn(asset,&level->player.item,0);
    level->jet_q.push_back(level->player);
    level->jet_q.front().curr.x = 300;
    level->jet_q.front().curr.y = al_get_bitmap_height(asset->bkgr_texture[level->level_name])/2;
    enemy_spawn(level,asset);
    asset->scale_factor = 1.0;


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
        break;
        }
        case PFERD:
        {
        //int amnt[] = {2,0,0,2,0,0};
        int amnt[] = {0,0,0,0,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
        asset->lvl_data[i].next_level = DNEPR;
        break;
        }
        case INDIA:
        {
        int amnt[] = {8,6,0,1,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
        asset->lvl_data[i].next_level = DNEPR;
        break;
        }
        case DNEPR:
        {
        //int amnt[] = {3,0,0,0,2,0};
        int amnt[] = {0,0,0,0,0,0};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
        asset->lvl_data[i].next_level = ENUM_BKGR_TYPE_FIN;
        break;
        }
        case ATLANTIC:
        {
        int amnt[] = {0,3,3,0,0,1};
        std::copy(amnt,amnt+ENUM_BOSS_TYPE_FIN,asset->lvl_data[i].enemy_quality);
        asset->lvl_data[i].map_height = al_get_bitmap_height(asset->bkgr_texture[i]);
        asset->lvl_data[i].map_width = al_get_bitmap_width(asset->bkgr_texture[i]);
        asset->lvl_data[i].next_level = ENUM_BKGR_TYPE_FIN;
        break;
        }


    }
}





}
