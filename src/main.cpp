#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"
#include "select_level.h"

#include <libconfig.h++>
using namespace libconfig;
//#define TPS 30.0

bool allegro_init();
void texture_init(struct asset_data *, bool);
void load_settings(struct asset_data *);
void particle_init(struct asset_data *);

void load_save(struct LevelInst * level)
{

FILE * input = fopen("gamedata.sav", "rb");
if(!input) return;

fread(level->player.mod,sizeof(struct riven),ENUM_JET_TYPE_FIN,input);

fclose(input);
return;
}

void save_save(struct LevelInst * level)
{
    FILE * output = fopen("gamedata.sav", "wb");
    if(!output) return;
    fwrite(level->player.mod,sizeof(struct riven),4,output);
    fclose(output);
}

void load_config(config_data * config)
{
    Config cfg;
    try{
    cfg.readFile("config.cfg");
    }
    catch(const FileIOException &fioex)
    {
    std::cerr << "I/O error while reading file." << std::endl;
    return;
    }
    catch(const ParseException &pex)
    {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return;
    }

//render section


  try{
    config->default_display_width = cfg.lookup("default_display_width");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'default_display_width' setting in configuration file." << std::endl;
    }
    if(config->default_display_width < 1000) config->default_display_width = 1000;


    try{
    config->default_display_height = cfg.lookup("default_display_height");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'default_display_height' setting in configuration file." << std::endl;
    }
    if(config->default_display_height < 600) config->default_display_height = 600;


    try{
        config->particlesEnabled = cfg.lookup("particlesEnabled");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'particlesEnabled' setting in configuration file." << std::endl;
    }



    try{
        config->verticalSyncEnabled = cfg.lookup("verticalSyncEnabled");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'verticalSyncEnabled' setting in configuration file." << std::endl;
    }
   
    int MSAA;
    try{
        MSAA = cfg.lookup("MSAA");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'MSAA' setting in configuration file." << std::endl;
    }
    if(MSAA < 0) MSAA = 0;
    if(MSAA > 8) MSAA = 8;
    config->MSAA = MSAA;
    
    try{
        config->oglEnabled = cfg.lookup("oglEnabled");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'oglEnabled' setting in configuration file." << std::endl;
    }

//gameplay section

    try{
        config->FPS = cfg.lookup("FPS");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'FPS' setting in configuration file." << std::endl;
    }
    if(config->FPS <= 0.) config->FPS = 60.;

    try{
        config->fadeDistance = cfg.lookup("fadeDistance");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'fadeDistance' setting in configuration file." << std::endl;
    }
    if(config->fadeDistance < 0.) config->fadeDistance = 800.;

    try{
        config->fadingLength = cfg.lookup("fadingLength");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'fadingLength' setting in configuration file." << std::endl;
    }
    if(config->fadingLength < 0.) config->fadingLength = 200.;


}




int main()
{
srand(time(NULL));
if(!allegro_init()) return 1;

asset_data * assets = new asset_data;
*assets = {.config = 
    {.scaleUI = 1.,
    .default_display_width = 1000,
    .default_display_height = 600,
    .particlesEnabled = 1,
    .verticalSyncEnabled = 0,
    .MSAA = 8,
    .FPS = 60,
    .fadeDistance = 800,
    .fadingLength = 200
    }
    };
load_config(&assets->config);

if(assets->config.oglEnabled) al_set_new_display_flags(ALLEGRO_OPENGL);
al_set_new_display_flags(ALLEGRO_RESIZABLE);
if(assets->config.MSAA)
{
al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
al_set_new_display_option(ALLEGRO_SAMPLES, assets->config.MSAA, ALLEGRO_SUGGEST);
}
al_set_new_display_option(ALLEGRO_RENDER_METHOD, 1, ALLEGRO_SUGGEST);

allegro5_data alleg5;
alleg5.display = al_create_display(assets->config.default_display_width,assets->config.default_display_height);

alleg5.queue = al_create_event_queue();
alleg5.font = al_load_ttf_font("font.ttf",12,0);
alleg5.timer = al_create_timer(1.0/(double)assets->config.FPS);

al_register_event_source(alleg5.queue,al_get_keyboard_event_source());
al_register_event_source(alleg5.queue,al_get_mouse_event_source());
al_register_event_source(alleg5.queue,al_get_display_event_source(alleg5.display));
al_register_event_source(alleg5.queue,al_get_timer_event_source(alleg5.timer));


if(!alleg5.display) return 0;
bool kill = 0;




texture_init(assets,1);
struct LevelInst lvl = {.level_name = ENUM_LVL_TYPE_FIN, .player = {.choice = {.player_jet = 0, .weapon = {0, ENUM_GUN_TYPE_FIN, ENUM_MSL_TYPE_FIN}}, .custom_stat = {nullptr,nullptr,nullptr,nullptr}}};
for(int i = 0; i< ENUM_JET_TYPE_FIN; i++) lvl.player.mod[i].engaged = false;

jet_init(assets);
abl_init(assets);
boss_init(assets);
projectile_init(assets);
launcher_init(assets);

level_init(assets);

particle_init(assets);
load_save(&lvl);
short state = LVL_SELECTION;
al_start_timer(alleg5.timer);
while(!kill)
{
    switch(state)
    {
    case LVL_SELECTION: state = lvl_select(&lvl,assets,&alleg5); break;
    case EQ_SELECTION: state = eq_select(&lvl,assets,&alleg5); break;
    case MISSION_INIT: state = spawn_level(assets,&lvl); break;
    case MISSION: state = level(&alleg5,assets,&lvl); break;
    case QUIT: kill = 1; break;
    }



}
texture_init(assets,0);
al_destroy_display(alleg5.display);
al_destroy_event_queue(alleg5.queue);
al_destroy_timer(alleg5.timer);
al_destroy_font(alleg5.font);
save_save(&lvl);
destroy_level(assets,&lvl);
delete assets;
}

bool allegro_init()
{
if(!al_init()) return 0;
if(!al_init_font_addon()) return 0;
if(!al_init_image_addon()) return 0;
if(!al_init_primitives_addon()) return 0;
if(!al_install_keyboard()) return 0;
if(!al_install_mouse()) return 0;
if(!al_init_ttf_addon()) return 0;
return 1;
}


void texture_init(struct asset_data * lvl, bool load)
{
    if(load)
    {
    lvl->bkgr_texture[BERLIN] = al_load_bitmap("texture/bkgr/berlin.jpg");
    lvl->bkgr_texture[INDIA] = al_load_bitmap("texture/bkgr/india.jpg");
    lvl->bkgr_texture[PFERD] = al_load_bitmap("texture/bkgr/pferd.png");
    lvl->bkgr_texture[DNEPR] = al_load_bitmap("texture/bkgr/dnepr.jpg");
    lvl->bkgr_texture[ATLANTIC] = al_load_bitmap("texture/bkgr/atlantic.jpg");
        //projectile
    lvl->proj_texture[SLUG] = al_load_bitmap("texture/bullet/slug.png");
    lvl->proj_texture[AIRBURST] = al_load_bitmap("texture/bullet/airburst.png");
    lvl->proj_texture[IR_M] = al_load_bitmap("texture/missile/infrared.png");
    lvl->proj_texture[RAD_M] = al_load_bitmap("texture/missile/radar.png");

        //jet
    lvl->jet_texture[MIG21] = al_load_bitmap("texture/jet/mig21.png");
    lvl->jet_texture[F4] = al_load_bitmap("texture/jet/f4.png");
    lvl->jet_texture[F104] = al_load_bitmap("texture/jet/f104.png");
    lvl->jet_texture[HARRIER] = al_load_bitmap("texture/jet/harrier.png");
    lvl->jet_texture[MIG29] = al_load_bitmap("texture/jet/mig29.png");
    lvl->jet_texture[SR91] = al_load_bitmap("texture/jet/sr91.png");

        //particle
    lvl->prt_data[FLARE].texture = al_load_bitmap("texture/particle/flare.png");

        //ui
    lvl->ui_texture[0] = al_load_bitmap("texture/ui/worldmap.jpg");
    }
    else
    {
        al_destroy_bitmap(lvl->bkgr_texture[BERLIN]);
        al_destroy_bitmap(lvl->bkgr_texture[INDIA]);
        al_destroy_bitmap(lvl->bkgr_texture[PFERD]);
        al_destroy_bitmap(lvl->bkgr_texture[DNEPR]);
        al_destroy_bitmap(lvl->bkgr_texture[ATLANTIC]);
        al_destroy_bitmap(lvl->proj_texture[SLUG]);
        al_destroy_bitmap(lvl->proj_texture[AIRBURST]);
        al_destroy_bitmap(lvl->proj_texture[IR_M]);
        al_destroy_bitmap(lvl->proj_texture[RAD_M]);
        al_destroy_bitmap(lvl->jet_texture[MIG21]);
        al_destroy_bitmap(lvl->jet_texture[F4]);
        al_destroy_bitmap(lvl->jet_texture[F104]);
        al_destroy_bitmap(lvl->jet_texture[HARRIER]);
        al_destroy_bitmap(lvl->jet_texture[MIG29]);
        al_destroy_bitmap(lvl->jet_texture[SR91]);
        al_destroy_bitmap(lvl->prt_data[FLARE].texture);
        al_destroy_bitmap(lvl->ui_texture[0]);

    }


}

void particle_init(struct asset_data * asset)
{
    for(int i = 0; i< ENUM_PRT_TYPE_FIN;i++)
    {
        switch(i)
        {
            case FLARE:
            {
                asset->prt_data[i].decay = 90;
            break;
            }
        }


    }



}




void load_settings(struct asset_data * master)
{





}

