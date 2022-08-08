#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"

struct camera
{
int x_pos;
int y_pos;
};


struct node
{
    int x_pos;
    int y_pos;
    int size;
    ALLEGRO_COLOR color;
    int type;
};

void debug_data(struct camera * ref, ALLEGRO_FONT * font)
{
    al_draw_multiline_textf(font,al_map_rgb(240,0,240),5,5,200,10,0,"%d x_pos\n %d y_pos",ref->x_pos,ref->y_pos);
}







void draw_node(struct camera * ref,std::array<node,ENUM_LVL_TYPE_FIN>::iterator nod,int window_width, int window_height)
{


al_draw_filled_rectangle(  (nod->x_pos-nod->size/2 - ref->x_pos) +window_width/2,    (nod->y_pos-nod->size/2 - ref->y_pos)+window_height/2,
 (nod->x_pos+nod->size/2  - ref->x_pos) +window_width/2   ,    (nod->y_pos+nod->size/2 - ref->y_pos)+window_height/2,
nod->color);

al_draw_rectangle((nod->x_pos-nod->size/3 - ref->x_pos) +window_width/2,    (nod->y_pos-nod->size/3 - ref->y_pos)+window_height/2,
 (nod->x_pos+nod->size/3  - ref->x_pos) +window_width/2   ,    (nod->y_pos+nod->size/3 - ref->y_pos)+window_height/2,al_map_rgb(27,27,0),nod->size/6);


}






void render(struct camera * ref, std::array<node,ENUM_LVL_TYPE_FIN> & node_array, box_string * prompt ,struct asset_data * asset, allegro5_data* alleg5, int * tick)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);

int cam_x = window_width/2 -ref->x_pos;
int cam_y = window_height/2 - ref->y_pos;

al_draw_bitmap(asset->ui_texture[0],-ref->x_pos+window_width/2,-ref->y_pos+window_height/2,0);





for(int i = 0; i< ENUM_LVL_TYPE_FIN; i++)
{
    int sqr_dist = ((*tick)/5) * 3 +node_array[i].size/2;
    int sqr2_dist = ((*tick)/6) * 2 +node_array[i].size/2 - 4;
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_draw_rectangle(cam_x + node_array[i].x_pos - sqr_dist, cam_y + node_array[i].y_pos - sqr_dist,
    cam_x + node_array[i].x_pos + sqr_dist, cam_y + node_array[i].y_pos + sqr_dist,al_map_rgba(200,27,27,255),2);
    al_draw_rectangle(cam_x + node_array[i].x_pos - sqr2_dist, cam_y + node_array[i].y_pos - sqr2_dist,
    cam_x + node_array[i].x_pos + sqr2_dist, cam_y + node_array[i].y_pos + sqr2_dist,al_map_rgba(200,27,27,127),2);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); //default blending
}

for(int i = 0; i< ENUM_LVL_TYPE_FIN; i++) draw_node(ref,node_array.begin()+i,window_width,window_height);




if(prompt != nullptr)
{
    al_draw_filled_rectangle(  (prompt->x - prompt->width/2 - ref->x_pos) +window_width/2,    (prompt->y - prompt->height/2 - ref->y_pos)+window_height/2,
    (prompt->x + prompt->width/2  - ref->x_pos) +window_width/2   ,    (prompt->y + prompt->height/2 - ref->y_pos)+window_height/2,
    al_map_rgb(0,27,27));


    al_draw_filled_rectangle( cam_x + prompt->x - prompt->width/2, cam_y + prompt->y - prompt->height/2,
    cam_x + prompt->x + prompt->width/2, cam_y + prompt->y - prompt->height/2 + 10 + al_get_font_ascent(alleg5->font),
    al_map_rgb(27,27,0));


    al_draw_text(alleg5->font,al_map_rgb(255,180,60),cam_x + prompt->x - prompt->width/2 +5, cam_y + prompt->y - prompt->height/2 +5, 0, prompt->name.c_str());

    al_draw_multiline_text(alleg5->font,al_map_rgb(200,200,127),cam_x + prompt->x, cam_y + prompt->y - prompt->height/3,prompt->width-30,12, ALLEGRO_ALIGN_CENTER, prompt->desc.c_str());

}



#ifdef DEBUG
    debug_data(ref,alleg5->font);
#endif

al_flip_display();
al_clear_to_color(al_map_rgb(27,27,27));

}


int update_node(struct camera * ref, ALLEGRO_MOUSE_STATE * mouse, std::array<node,ENUM_LVL_TYPE_FIN> & node_array, box_string * prompt, allegro5_data* alleg5)
{
int window_width = al_get_display_width(alleg5->display);
int window_height = al_get_display_height(alleg5->display);


if(prompt)
{

if(prompt->x - prompt->width/2 - ref->x_pos + window_width/2 <= mouse->x && mouse->x <= prompt->x + prompt->width/2 - ref->x_pos + window_width/2 &&
prompt->y - prompt->height/2 - ref->y_pos + window_height/2 <= mouse->y && mouse->y <= prompt->y + prompt->height/2 - ref->y_pos + window_height/2)
return -1;
    
}




for(int i = 0; i< ENUM_LVL_TYPE_FIN; i++)
{
    
    
if((node_array[i].x_pos - node_array[i].size/2 - ref->x_pos) +window_width/2 <= mouse->x && mouse->x <=   (node_array[i].x_pos + node_array[i].size/2  - ref->x_pos) +window_width/2  &&
(node_array[i].y_pos - node_array[i].size/2 - ref->y_pos)+window_height/2 <= mouse->y && mouse->y <= (node_array[i].y_pos + node_array[i].size/2 - ref->y_pos)+window_height/2)
{
return i;
}
}


return ENUM_LVL_TYPE_FIN;
}

void refresh_riven(struct LevelInst * level,struct asset_data * asset)
{
    for(int i = 0; i< ENUM_JET_TYPE_FIN; i++)
    {
        if(level->player.mod[i].engaged)
        {
            level->player.custom_stat[i] = process_riven(&level->player.mod[i],&asset->jet_data[i].alter_limit);
        }
        else
        {
            delete level->player.custom_stat[i];
            level->player.custom_stat[i] = nullptr;
        }



    }




}




int lvl_select(struct LevelInst * level,struct asset_data * asset, allegro5_data* alleg5)
{
int tick = 0;
refresh_riven(level,asset);

int lvl_selected = level->level_name;
std::array<node,ENUM_LVL_TYPE_FIN> node_array {{{1586,358,20,al_map_rgb(120,120,120),0} , {2148,588,20,al_map_rgb(120,120,120),1} , {1020,864,20,al_map_rgb(120,120,0),2}}};

box_string lvl_select_prompt_data[ENUM_LVL_TYPE_FIN] = 
{
    {.x = node_array[0].x_pos, .y = node_array[0].y_pos,300,200, .name = "BERLIN", .desc = "Enemy forces were spotted operating in the area of former Berlin. Investigate, engage & shoot down anyone you encounter. Leave no one flying.\n\nIf they retreat, follow them up their den. Exterminate everyone."  },
    {.x = node_array[1].x_pos, .y = node_array[1].y_pos,300,200, .name = "INDIA", .desc = "Hostile air force seeks to establish air superiority in the area of the Indian penninsula. Don't let that happen.\n\nYou should not fly too far away though, southern europe is not anymore under our control."  },
    {.x = node_array[2].x_pos, .y = node_array[2].y_pos,300,200,.name = "PFERD", .desc = "Training area. A simulation of sort." }
    };

box_string * prompt = (lvl_selected == ENUM_LVL_TYPE_FIN ? nullptr : lvl_select_prompt_data + lvl_selected);

struct camera ref = {al_get_bitmap_width(asset->ui_texture[0])/2,al_get_bitmap_height(asset->ui_texture[0])/2};


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
        }
        break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE: quit = 1; break;
        case ALLEGRO_EVENT_TIMER: redraw = 1; break;
        case ALLEGRO_EVENT_MOUSE_AXES:
        {
            break;
        }
        case ALLEGRO_EVENT_KEY_DOWN:
        {
            switch(alleg5->event.keyboard.keycode)
            {
                case ALLEGRO_KEY_ESCAPE:
                return QUIT;
                break;
                case ALLEGRO_KEY_F: 
                if(prompt != nullptr) kill = 1; 
                break;
            }
        break;
        }
        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            {
                int update_selected = update_node(&ref,&mouse,node_array,prompt,alleg5);
                
                if(update_selected != lvl_selected && update_selected != -1)
                {
                lvl_selected = update_selected;
                if(lvl_selected == ENUM_LVL_TYPE_FIN) prompt = nullptr;
                else prompt = lvl_select_prompt_data + lvl_selected;
                }

                if(update_selected == -1) kill = 1;


                
            }
        break;
    }

if(redraw && al_is_event_queue_empty(alleg5->queue))
{   
    {
    al_get_mouse_state(&mouse);
            if(mouse.x < 80) ref.x_pos = (ref.x_pos - 12 >= 0 ?  ref.x_pos - 12 : 0);
            if(mouse.x > al_get_display_width(alleg5->display) - 80) ref.x_pos = (ref.x_pos + 12 <= al_get_bitmap_width(asset->ui_texture[0]) ?  ref.x_pos + 12 : al_get_bitmap_width(asset->ui_texture[0]));
            if(mouse.y < 80) ref.y_pos = (ref.y_pos - 12 >= 0 ?  ref.y_pos - 12 : 0);
            if(mouse.y > al_get_display_height(alleg5->display) - 80) ref.y_pos = (ref.y_pos + 12 <= al_get_bitmap_height(asset->ui_texture[0]) ?  ref.y_pos + 12 : al_get_bitmap_height(asset->ui_texture[0]));
    
    }

    render(&ref,node_array,prompt,asset,alleg5,&tick);
    
    redraw = 0;
    tick = (tick + 1 >= 40 ? 0 : tick + 1 );
    
}



}




if(quit) return QUIT;
else 
level->level_name = lvl_selected;

return EQ_SELECTION;
}