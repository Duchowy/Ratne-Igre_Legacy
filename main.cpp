#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"
#include "select_level.h"
#include "render_level.h"

#include <libconfig.h++>
using namespace libconfig;
//#define TPS 30.0

bool allegro_init();
void texture_init(struct asset_data *);
void particle_init(struct asset_data *);
void load_config(config_data * config);

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

float calculateUIscale(float width, float height)
{
float widthRatio = width / 1000.;
float heightRatio = height / 600.;
if(widthRatio > heightRatio) return heightRatio;
else return widthRatio;
}







int main()
{
srand(time(NULL));
if(!allegro_init()) return 1;

asset_data * assets = new asset_data;
*assets = {.config = 
    {
    .default_display_width = 1000,
    .default_display_height = 600,
    .particlesEnabled = true,
    .verticalSyncEnabled = 0,
    .MSAA = 2,
    .autoUIscale = 0,
    .UIscale = 1.,
    .zoomUpperLimit = 1.0,
    .zoomLowerLimit = 0.8,
    .radarType = 1,
    .additionalRadar = false,
    .FPS = 60,
    .fadeDistance = 800,
    .fadingLength = 200,
    .collisionAccCoef = 1.0
    }
    };
load_config(&assets->config);
/*
if(assets->config.oglEnabled) al_set_new_display_flags(ALLEGRO_OPENGL);
al_set_new_display_flags(ALLEGRO_RESIZABLE);
if(assets->config.MSAA)
{
al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_REQUIRE);
al_set_new_display_option(ALLEGRO_SAMPLES, assets->config.MSAA, ALLEGRO_SUGGEST);
}
al_set_new_display_option(ALLEGRO_RENDER_METHOD, 1, ALLEGRO_SUGGEST);


allegro5_data alleg5;
alleg5.display = al_create_display(assets->config.default_display_width,assets->config.default_display_height);
al_set_window_constraints(alleg5.display,1000,600,0,0);
al_apply_window_constraints(alleg5.display,1);
alleg5.queue = al_create_event_queue();
alleg5.font = al_load_ttf_font("font.ttf",12,0);
alleg5.timer = al_create_timer(1.0/(double)assets->config.FPS);
*/

sf::RenderWindow display(sf::VideoMode(assets->config.default_display_width,assets->config.default_display_height),"Ratne Igre");
display.setFramerateLimit(60);

bool kill = 0;




texture_init(assets);
struct LevelInst lvl = {.level_name = ENUM_LVL_TYPE_FIN, .player = {.choice = {.player_jet = 0, .weapon = {0, ENUM_GUN_TYPE_FIN, ENUM_MSL_TYPE_FIN}, .multiplier = {1.0f,1.0f,0.0f}}, .custom_stat = {nullptr,nullptr,nullptr,nullptr}}};
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
while(!kill)
{
    switch(state)
    {
    case LVL_SELECTION: state = lvl_select(&lvl,assets,display); break;
    case EQ_SELECTION: state = eq_select(&lvl,assets,display); break;
    case MISSION_INIT: state = spawn_level(assets,&lvl); break;
    case MISSION: state = level(display,assets,&lvl); break;
    case QUIT: kill = 1; break;
    }



}


save_save(&lvl);
destroy_level(assets,&lvl);
delete assets;
}

bool allegro_init()
{
    /*
if(!al_init()) return 0;
if(!al_init_font_addon()) return 0;
if(!al_init_image_addon()) return 0;
if(!al_init_primitives_addon()) return 0;
if(!al_install_keyboard()) return 0;
if(!al_install_mouse()) return 0;
if(!al_init_ttf_addon()) return 0;
if(!al_install_audio()) return 0;
if (!al_init_acodec_addon()) return 0;
if(!al_reserve_samples(80)) if(!al_reserve_samples(50)) if(!al_reserve_samples(30)) return 0;
return 1;
*/
return 1;
}


void texture_init(struct asset_data * lvl)
{
    lvl->font.loadFromFile("font.ttf");

    lvl->bkgr_texture[BERLIN].loadFromFile("texture/bkgr/berlin.jpg");
    lvl->bkgr_texture[INDIA].loadFromFile("texture/bkgr/india.jpg");
    lvl->bkgr_texture[PFERD].loadFromFile("texture/bkgr/pferd.png");
    lvl->bkgr_texture[DNEPR].loadFromFile("texture/bkgr/dnepr.jpg");
    lvl->bkgr_texture[ATLANTIC].loadFromFile("texture/bkgr/atlantic.jpg");
        //projectile
    lvl->proj_texture[SLUG].loadFromFile("texture/bullet/slug.png");
    lvl->proj_texture[AIRBURST].loadFromFile("texture/bullet/airburst.png");
    lvl->proj_texture[IR_M].loadFromFile("texture/missile/infrared.png");
    lvl->proj_texture[RAD_M].loadFromFile("texture/missile/radar.png");
    lvl->proj_texture[UNGUIDED].loadFromFile("texture/missile/unguided.png");

        //jet
    lvl->jet_texture[MIG21].loadFromFile("texture/jet/mig21.png");
    lvl->jet_texture[F4].loadFromFile("texture/jet/f4.png");
    lvl->jet_texture[F104].loadFromFile("texture/jet/f104.png");
    lvl->jet_texture[HARRIER].loadFromFile("texture/jet/harrier.png");
    lvl->jet_texture[MIG29].loadFromFile("texture/jet/mig29.png");
    lvl->jet_texture[SR91].loadFromFile("texture/jet/sr91.png");

        //particle
    lvl->prt_texture[FLARE].loadFromFile("texture/particle/flare.png");
    lvl->prt_texture[EXPLOSION].loadFromFile("texture/particle/explosion_circular.png");
    lvl->prt_texture[EXPLOSION_AIRBURST].loadFromFile("texture/particle/explosion_airburst.png");
    lvl->prt_texture[SONIC_CONE].loadFromFile("texture/particle/sonic_cone.png");
        //ui
    lvl->ui_texture[0].loadFromFile("texture/ui/worldmap.jpg");
    lvl->ui_texture[1].loadFromFile("texture/ui/darkness.png");
/*
    lvl->sound[0] = al_load_sample("sound/gunshot_shvak.ogg");
    lvl->sound[1] = al_load_sample("sound/gunshot_aden.ogg");
    lvl->sound[2] = al_load_sample("sound/gunshot_gatling.ogg");
    lvl->sound[3] = al_load_sample("sound/launch_missile.ogg");
    */



}

void particle_init(struct asset_data * asset)
{
    asset->prt_data[FLARE] = {.decay = 90, .anim = {.isAnimated = false,0,0}};


    asset->prt_data[EXPLOSION] = {
        .decay = 32-1, //extract one because it would go off-chart
        .anim = {.isAnimated = true, .animationClock = 10, .textureCount = 4},
        };
    asset->prt_data[EXPLOSION_AIRBURST] = {
        .decay = 35-1, //extract one because it would go off-chart
        .anim = {.isAnimated = true, .animationClock = 7, .textureCount = 5},
        };
    asset->prt_data[JET] = {
        .decay = 180,
        .anim = {.isAnimated = false, 0,0}
    };
    asset->prt_data[PIXEL] = {
        .decay = 60,
        .anim = {.isAnimated = false, 0,0}
    };


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
    if(MSAA > 3) MSAA = 3;
    config->MSAA = MSAA;
    
    try{
        config->oglEnabled = cfg.lookup("oglEnabled");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'oglEnabled' setting in configuration file." << std::endl;
    }




//mission UI section
try{
        config->autoUIscale = cfg.lookup("autoUIscale");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'autoUIscale' setting in configuration file." << std::endl;
    }

if(!config->autoUIscale)
{
try{
        config->UIscale = cfg.lookup("UIscale");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'UIscale' setting in configuration file." << std::endl;
    }
if(config->UIscale <= 0) config->UIscale = 1.;
}
else config->UIscale = calculateUIscale(config->default_display_width, config->default_display_height);





try{
        config->zoomUpperLimit = cfg.lookup("zoomUpperLimit");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'zoomUpperLimit' setting in configuration file." << std::endl;
    }
if(config->zoomUpperLimit < 0) config->zoomUpperLimit = 1.0;

try{
        config->zoomLowerLimit = cfg.lookup("zoomLowerLimit");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'zoomLowerLimit' setting in configuration file." << std::endl;
    }

if(config->zoomLowerLimit < 0) config->zoomLowerLimit = 0.8;
if(config->zoomLowerLimit > config->zoomUpperLimit) config->zoomUpperLimit = config->zoomLowerLimit + 0.2;








try{
        config->radarType = (int)cfg.lookup("radarType");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'radarType' setting in configuration file." << std::endl;
    } 
if(config->radarType < 0) config->radarType = 1;

try{
        config->additionalRadar = cfg.lookup("additionalRadar");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'additionalRadar' setting in configuration file." << std::endl;
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

    try{
        config->collisionAccCoef = cfg.lookup("collisionAccCoef");
    }
    catch(const SettingNotFoundException &nfex)
    {
    std::cerr << "No 'collisionAccCoef' setting in configuration file." << std::endl;
    }
    if(config->collisionAccCoef < 0.) config->collisionAccCoef = 0.0;

    


}


