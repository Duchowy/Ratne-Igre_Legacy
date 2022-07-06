#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"

//#define TPS 30.0

bool allegro_init();
void texture_init(struct asset_data *, bool);
void load_settings(struct asset_data *);

int main()
{
srand(time(NULL));
if(!allegro_init()) return 1;
al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
allegro5_data alleg5;
alleg5.display = al_create_display(window_width,window_height);

alleg5.queue = al_create_event_queue();
alleg5.font = al_create_builtin_font();
alleg5.timer = al_create_timer(1.0/FPS);

al_register_event_source(alleg5.queue,al_get_keyboard_event_source());
al_register_event_source(alleg5.queue,al_get_mouse_event_source());
al_register_event_source(alleg5.queue,al_get_display_event_source(alleg5.display));
al_register_event_source(alleg5.queue,al_get_timer_event_source(alleg5.timer));


if(!alleg5.display) return 0;
bool kill = 0;
asset_data assets;
texture_init(&assets,1);
struct LevelInst lvl = {.level_name = ENUM_BKGR_TYPE_FIN};
jet_init(&assets);
gun_init(&assets);
msl_init(&assets);
level_init(&assets);
bullet_init(&assets);

short state = SELECTION;
al_start_timer(alleg5.timer);
while(!kill)
{
    switch(state)
    {
    case SELECTION: state = level_select(&lvl,&assets,&alleg5); break;
    case MISSION_INIT: state = spawn_level(&assets,&lvl); break;
    case MISSION: state = level(&alleg5,&assets,&lvl); break;
    case QUIT: kill = 1; break;
    }



}
texture_init(&assets,0);
al_destroy_display(alleg5.display);
al_destroy_event_queue(alleg5.queue);
al_destroy_timer(alleg5.timer);
al_destroy_font(alleg5.font);
}

bool allegro_init()
{
if(!al_init()) return 0;
if(!al_init_font_addon()) return 0;
if(!al_init_image_addon()) return 0;
if(!al_init_primitives_addon()) return 0;
if(!al_install_keyboard()) return 0;
if(!al_install_mouse()) return 0;
return 1;
}


void texture_init(struct asset_data * lvl, bool load)
{
    if(load)
    {
    lvl->bkgr_texture[BERLIN] = al_load_bitmap("texture/bkgr/berlin.jpg");
    lvl->bkgr_texture[INDIA] = al_load_bitmap("texture/bkgr/india.jpg");
    lvl->bkgr_texture[PFERD] = al_load_bitmap("texture/bkgr/pferd.png");
        //bullet
    lvl->bullet_texture[SLUG][0] = al_load_bitmap("texture/bullet/slug.png");
    lvl->bullet_texture[SLUG][1] = al_load_bitmap("texture/bullet/slug_flare.png");
        //jet
    lvl->jet_texture[MIG21] = al_load_bitmap("texture/jet/mig21.png");
    lvl->jet_texture[F4] = al_load_bitmap("texture/jet/f4.png");
    lvl->jet_texture[F104] = al_load_bitmap("texture/jet/f104.png");
    lvl->jet_texture[HARRIER] = al_load_bitmap("texture/jet/harrier.png");
        //msl
    lvl->msl_texture[IR] = al_load_bitmap("texture/missile/infrared.png");
    lvl->msl_texture[RAD] = al_load_bitmap("texture/missile/radar.png");
    }
    else
    {
        al_destroy_bitmap(lvl->bkgr_texture[BERLIN]);
        al_destroy_bitmap(lvl->bkgr_texture[INDIA]);
        al_destroy_bitmap(lvl->bkgr_texture[PFERD]);
        al_destroy_bitmap(lvl->bullet_texture[SLUG][0]);
        al_destroy_bitmap(lvl->bullet_texture[SLUG][1]);
        al_destroy_bitmap(lvl->jet_texture[MIG21]);
        al_destroy_bitmap(lvl->jet_texture[F4]);
        al_destroy_bitmap(lvl->jet_texture[F104]);
        al_destroy_bitmap(lvl->jet_texture[HARRIER]);
        al_destroy_bitmap(lvl->msl_texture[IR]);
        al_destroy_bitmap(lvl->msl_texture[RAD]);

    }


}

void load_settings(struct asset_data * master)
{





}

