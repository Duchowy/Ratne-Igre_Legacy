#include "main.h"
#include "jet.h"
#include "level.h"
#include "load_level.h"

void map_button(std::array<struct button_scroll, 4> & button,struct curr_selection * choice)
{

switch(choice->player_jet)
        {
            case MIG21: button[0].name = "MIG21"; button[0].desc = "+Excellent maneuverability\n-Low ammo"; break;
            case F4: button[0].name = "F4 PHANTOM"; button[0].desc = "+Decent maneuverability\n+High ammo\n+Decent survivability"; break;
            case F104: button[0].name = "F104"; button[0].desc = "+High top speed\n-Low survivability\n-Slow turn rate"; break;
            case HARRIER: button[0].name = "HARRIER"; button[0].desc = "+High ammo\n-Decent survivability\n-Terrible maneuverability"; break;
        }
switch(choice->player_gun)
        {
            case SHVAK: button[1].name = "SHVAK"; button[1].desc = "+High damage\n-Slow firerate"; break;
            case ADEN: button[1].name = "ADEN"; button[1].desc = "+Decent damage\n+Fast firerate\n-High spread\n-Low ammo"; break;
            case GATLING: button[1].name = "GATLING"; button[1].desc = "+Fast firerate\n+High ammo\n-Poor damage"; break;
        }
switch(choice->player_rkt)
        {
            case IR: button[2].name = "INFRARED"; button[2].desc = "+Fast\n-Heat guided"; break;
            case RAD: button[2].name = "RADAR"; button[2].desc = "+Lock from all angles\n-Poor range"; break;
        }
switch(choice->selection)
        {
            case BERLIN: button[3].name = "BERLIN"; button[3].desc = "Expect heavy resistance"; break;
            case INDIA: button[3].name = "INDIA"; button[3].desc = "Search & destroy"; break;
            case PFERD: button[3].name = "PFERD"; button[3].desc = "What?";break;
        }


}




void init_button(std::array<struct button_scroll, 4> & button, allegro5_data* alleg5)
{
/*###############
#################
#### l90 h90 ####
#################
###############*/
for(int i = 0; i<4; i++)
{
    button[i].x = al_get_display_width(alleg5->display)*(1+2*i)/8;
    button[i].y = al_get_display_height(alleg5->display) * 7/10;
    button[i].width = 190;
    button[i].height = 90;
}
}


void update(std::array<struct button_scroll, 4> & button,struct curr_selection * choice,ALLEGRO_MOUSE_STATE *mouse)
{
short number= -1;


int decision = al_get_mouse_state_axis(mouse, 2);
for(int i = 0; i<4 && number ==-1; i++)
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
        if(decision > 0 && choice->player_gun < ENUM_GUN_TYPE_FIN-1) choice->player_gun++;
        if(decision < 0 && choice->player_gun > 0) choice->player_gun--;
    }
    break;
    case 2: //missile
    {
        if(decision > 0 && choice->player_rkt < ENUM_MSL_TYPE_FIN-1) choice->player_rkt++;
        if(decision < 0 && choice->player_rkt > 0) choice->player_rkt--;
    }
    break;
    case 3: //level
    {
        if(decision > 0 && choice->selection < ENUM_BKGR_TYPE_FIN-1) choice->selection++;
        if(decision < 0 && choice->selection > 0) choice->selection--;
    }
    break;
    default:
    break;
}
al_set_mouse_z(0); //zero the scroll
}

void draw(std::array<struct button_scroll, 4> & button,struct asset_data * assets,allegro5_data* alleg5, struct curr_selection * choice)
{
for(int i = 0; i<4; i++)
{
al_draw_filled_rectangle(button[i].x - button[i].width/2,button[i].y - button[i].height/2,button[i].x + button[i].width/2,button[i].y + button[i].height/2,
al_map_rgb(0,20,20));
al_draw_text(alleg5->font,al_map_rgb(240,240,240),button[i].x,button[i].y,ALLEGRO_ALIGN_CENTRE,button[i].name.c_str());
al_draw_multiline_text(alleg5->font,al_map_rgb(240,240,240),button[i].x,button[i].y-button[i].height/2-40,200,al_get_font_line_height(alleg5->font),ALLEGRO_ALIGN_CENTRE,button[i].desc.c_str());
}
al_draw_scaled_rotated_bitmap(assets->jet_texture[choice->player_jet],al_get_bitmap_width(assets->jet_texture[choice->player_jet])/2,al_get_bitmap_height(assets->jet_texture[choice->player_jet])/2,
button[0].x,button[0].y-button[0].height/2-al_get_bitmap_height(assets->jet_texture[choice->player_jet])-40,2,2,0,0);

}


int level_select(struct lvl_dat * lvl,struct asset_data * assets, allegro5_data* alleg5)
{
std::array<struct button_scroll, 4> button;
struct curr_selection choice;

init_button(button,alleg5);

if(lvl->level_name != ENUM_BKGR_TYPE_FIN)
{
choice.player_jet = lvl->player.type;
choice.player_gun = lvl->player.weap[0];
choice.player_rkt = lvl->player.weap[1];
choice.selection = lvl->level_name;
}
else
{
choice.player_jet = 0;
choice.player_gun = 0;
choice.player_rkt = 0;
choice.selection = 0;
}






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
        case ALLEGRO_EVENT_DISPLAY_CLOSE: quit = 1; break;
        case ALLEGRO_EVENT_TIMER: redraw = 1; break;
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            al_get_mouse_state(&mouse);
            if(al_get_mouse_state_axis(&mouse, 2) != 0) update(button,&choice,&mouse);
            break;
        }
        case ALLEGRO_EVENT_KEY_DOWN:
        {
            switch(alleg5->event.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                return QUIT;
                break;
                case ALLEGRO_KEY_ENTER: kill = 1; break;
            }
        break;
        }
    }

if(redraw && al_is_event_queue_empty(alleg5->queue))
{
    //draw sequence
    map_button(button,&choice);
    draw(button,assets,alleg5,&choice);
    al_flip_display();
    al_clear_to_color(al_map_rgb(27,27,27));
    redraw = 0;
}



}













switch(choice.selection)
{
    case BERLIN:
    {
    int amnt[] = {4,6,1,0};
    lvl->level_name = BERLIN;
    std::copy(amnt,amnt+ENUM_JET_TYPE_FIN,lvl->enemy_quality[0]);
    lvl->enemy_amount = 11;
    lvl->map_height = al_get_bitmap_height(assets->bkgr_texture[lvl->level_name]);
    lvl->map_width = al_get_bitmap_width(assets->bkgr_texture[lvl->level_name]);
    break;
    }
    case PFERD:
    {
    int amnt[] = {2,0,0,2};
    lvl->level_name = PFERD;
    std::copy(amnt,amnt+ENUM_JET_TYPE_FIN,lvl->enemy_quality[0]);
    lvl->enemy_amount = 4;
    lvl->map_height = al_get_bitmap_height(assets->bkgr_texture[lvl->level_name]);
    lvl->map_width = al_get_bitmap_width(assets->bkgr_texture[lvl->level_name]);
    break;
    }
    case INDIA:
    {
    int amnt[] = {8,6,0,1};
    lvl->level_name = INDIA;
    std::copy(amnt,amnt+ENUM_JET_TYPE_FIN,lvl->enemy_quality[0]);
    lvl->enemy_amount = 15;
    lvl->map_height = al_get_bitmap_height(assets->bkgr_texture[lvl->level_name]);
    lvl->map_width = al_get_bitmap_width(assets->bkgr_texture[lvl->level_name]);
    break;
    }
    std::copy(lvl->enemy_quality[0],lvl->enemy_quality[0]+ENUM_JET_TYPE_FIN,lvl->enemy_quality[1]);
}

lvl->player.weap[0] = choice.player_gun;
lvl->player.weap[1] = choice.player_rkt;
jet_init(&(lvl->player),lvl,choice.player_jet,0);
lvl->player.x=window_width/2;
lvl->player.y=al_get_bitmap_height(assets->bkgr_texture[lvl->level_name])/2;

if(quit) return QUIT;
return MISSION;
}
