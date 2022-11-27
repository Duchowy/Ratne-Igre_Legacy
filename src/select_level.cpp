#include "jet.h"
#include "level.h"
#include "main.h"
#include "load_level.h"




struct node
{
    int x_pos;
    int y_pos;
    int size;
    sf::Color color;
    int type;
};

void debug_data(sf::RenderWindow & display)
{
    //al_draw_multiline_textf(alleg5->font,al_map_rgb(240,0,240),5,5,200,10,0,"%d x_pos\n %d y_pos\nMSAA flag: %d",ref->x_pos,ref->y_pos,al_get_display_option(alleg5->display,ALLEGRO_SAMPLES));
}







void draw_node(std::array<node,ENUM_LVL_TYPE_FIN>::iterator nod,sf::RenderWindow & display)
{
int window_width = display.getSize().x;
int window_height = display.getSize().y;


sf::RectangleShape node(sf::Vector2f(nod->size,nod->size));
node.setOrigin(nod->size/2,nod->size/2);
node.setPosition(nod->x_pos, nod->y_pos);
node.setFillColor(nod->color);
node.setOutlineThickness(nod->size/6);
node.setOutlineColor(sf::Color(27,27,0,255));
display.draw(node);



}






void render(std::array<node,ENUM_LVL_TYPE_FIN> & node_array, box_string * prompt ,struct asset_data * asset, sf::RenderWindow & display, int * tick)
{
int window_width = display.getSize().x;
int window_height = display.getSize().y;


sf::Sprite sf_bkgr;
sf_bkgr.setTexture(asset->ui_texture[0]);
sf_bkgr.setPosition(0,0);
display.draw(sf_bkgr);




for(int i = 0; i< ENUM_LVL_TYPE_FIN; i++)
{
    int sqr_dist = ((*tick)/5) * 3 +node_array[i].size/2;
    int sqr2_dist = ((*tick)/6) * 2 +node_array[i].size/2 - 4;
    //al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    sf::RectangleShape rectangle(sf::Vector2f(2*sqr_dist,2*sqr_dist));
    rectangle.setOrigin(sqr_dist,sqr_dist);
    rectangle.setFillColor(sf::Color(0,0,0,0));
    rectangle.setOutlineColor(sf::Color(200,27,27,255));
    rectangle.setOutlineThickness(2);
    rectangle.setPosition( node_array[i].x_pos,  node_array[i].y_pos);
    display.draw(rectangle);



    sf::RectangleShape rectangle2(sf::Vector2f(2*sqr2_dist,2*sqr2_dist));
    rectangle2.setOrigin(sqr2_dist,sqr2_dist);
    rectangle2.setFillColor(sf::Color(0,0,0,0));
    rectangle2.setOutlineColor(sf::Color(200,27,27,127));
    rectangle2.setOutlineThickness(2);
    rectangle2.setPosition( node_array[i].x_pos,  node_array[i].y_pos);
    display.draw(rectangle2);

}

for(int i = 0; i< ENUM_LVL_TYPE_FIN; i++) draw_node(node_array.begin()+i,display);




if(prompt != nullptr)
{
    sf::RectangleShape sf_prompt(sf::Vector2f(prompt->width,prompt->height));
    sf_prompt.setPosition(prompt->x,   prompt->y );
    sf_prompt.setOrigin(prompt->width/2,prompt->height/2);
    sf_prompt.setFillColor(sf::Color(0,27,27,255));
    display.draw(sf_prompt);

    sf::RectangleShape sf_foot(sf::Vector2f(prompt->width,10 + 12));
    sf_foot.setPosition( prompt->x-prompt->width/2, prompt->y-prompt->height/2);
    sf_foot.setFillColor(sf::Color(27,27,0,255));
    display.draw(sf_foot);


    sf::Text sf_title(prompt->name,asset->font,10);
    sf_title.setFillColor(sf::Color(255,180,60,255));
    sf_title.setPosition( prompt->x - prompt->width/2 +5, prompt->y - prompt->height/2 +5);
    display.draw(sf_title);

    sf::Text sf_desc(prompt->desc,asset->font,10);
    sf_desc.setFillColor(sf::Color(200,200,127,255));
    sf_desc.setOrigin(sf_desc.getLocalBounds().width/2,0);
    sf_desc.setPosition( prompt->x,  prompt->y - prompt->height/3);
    display.draw(sf_desc);




}



#ifdef NDEBUG
    debug_data(display);
#endif

display.display();
display.clear(sf::Color(27,27,27,255));

}


int update_node( std::array<node,ENUM_LVL_TYPE_FIN> & node_array, box_string * prompt, sf::RenderWindow & display)
{
sf::Vector2i mouse =static_cast<sf::Vector2i> (display.mapPixelToCoords( sf::Mouse::getPosition(display)))   ;


int window_width = display.getSize().x;
int window_height = display.getSize().y;


if(prompt)
{

if(prompt->x - prompt->width/2  <= mouse.x && mouse.x <= prompt->x + prompt->width/2  &&
prompt->y - prompt->height/2  <= mouse.y && mouse.y <= prompt->y + prompt->height/2  )
return -1;
    
}




for(int i = 0; i< ENUM_LVL_TYPE_FIN; i++)
{
    
    
if((node_array[i].x_pos - node_array[i].size/2 )  <= mouse.x && mouse.x <=   (node_array[i].x_pos + node_array[i].size/2 )   &&
(node_array[i].y_pos - node_array[i].size/2) <= mouse.y && mouse.y <= (node_array[i].y_pos + node_array[i].size/2 ))
{
return i;
}
}


return ENUM_LVL_TYPE_FIN;
}






int lvl_select(struct LevelInst * level,struct asset_data * asset, sf::RenderWindow & display)
{
int tick = 0;
refresh_riven(level,asset);

display.setView(  sf::View(  static_cast<sf::Vector2f>(asset->ui_texture[0].getSize()/2), static_cast<sf::Vector2f>( display.getSize())));

int lvl_selected = level->level_name;
std::array<node,ENUM_LVL_TYPE_FIN> node_array {{{1586,358,20,sf::Color(120,120,120,255),0} , {2148,588,20,sf::Color(120,120,120,255),1} , {1020,864,20,sf::Color(120,120,0,255),2}}};

box_string lvl_select_prompt_data[ENUM_LVL_TYPE_FIN] = 
{
    {.x = node_array[0].x_pos, .y = node_array[0].y_pos,300,200, .name = "BERLIN", .desc = "Enemy forces were spotted operating in the area of former Berlin. Investigate, engage & shoot down anyone you encounter. Leave no one flying.\n\nIf they retreat, follow them up their den. Exterminate everyone."  },
    {.x = node_array[1].x_pos, .y = node_array[1].y_pos,300,200, .name = "INDIA", .desc = "Hostile air force seeks to establish air superiority in the area of the Indian penninsula. Don't let that happen.\n\nYou should not fly too far away though, southern europe is not anymore under our control."  },
    {.x = node_array[2].x_pos, .y = node_array[2].y_pos,300,200,.name = "PFERD", .desc = "Training area. A simulation of sort." }
    };

box_string * prompt = (lvl_selected == ENUM_LVL_TYPE_FIN ? nullptr : lvl_select_prompt_data + lvl_selected);

//struct camera ref = {asset->ui_texture[0].getSize().x/2,asset->ui_texture[0].getSize().y/2};


bool kill = 0;
bool quit = 0;

sf::Event event_q;

while(!kill && !quit)
{
    while (display.pollEvent(event_q))
    {
        /*case ALLEGRO_EVENT_DISPLAY_RESIZE:
        {
            al_acknowledge_resize(alleg5->display); 
            if(asset->config.autoUIscale) asset->config.UIscale = calculateUIscale(al_get_display_width(alleg5->display), al_get_display_height(alleg5->display));
        }
        break;*/
        switch (event_q.type)
        {
        case sf::Event::Closed: quit = 1; break;

        case sf::Event::KeyReleased:
        {
            switch(event_q.key.code)
            {
                case sf::Keyboard::Escape:
                return QUIT;
                break;
                case sf::Keyboard::F:
                if(prompt != nullptr) kill = 1;
                break;
                default: break;
            }
        }
        break;

        default: break;
        }
        //if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return QUIT;
        //if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)) if(prompt != nullptr) kill = 1; 

        if (event_q.type == sf::Event::MouseButtonPressed && event_q.mouseButton.button == sf::Mouse::Left)
            {
                int update_selected = update_node(node_array,prompt,display);
                
                if(update_selected != lvl_selected && update_selected != -1)
                {
                lvl_selected = update_selected;
                if(lvl_selected == ENUM_LVL_TYPE_FIN) prompt = nullptr;
                else prompt = lvl_select_prompt_data + lvl_selected;
                }

                if(update_selected == -1) kill = 1;


                
            }
    }


{   
    {
        sf::Vector2i mouse =  sf::Mouse::getPosition(display);
        
        sf::View viewport = display.getView();

        
        sf::Vector2f ref = viewport.getCenter();
            if(mouse.x < 80) ref.x = (ref.x - 12 >= 0 ?  ref.x - 12 : 0);
            if(mouse.x > display.getSize().x - 80) ref.x = (ref.x + 12 <= asset->ui_texture[0].getSize().x ?  ref.x + 12 : asset->ui_texture[0].getSize().x);
            if(mouse.y < 80) ref.y = (ref.y - 12 >= 0 ?  ref.y - 12 : 0);
            if(mouse.y > display.getSize().y - 80) ref.y = (ref.y + 12 <= asset->ui_texture[0].getSize().y ?  ref.y + 12 : asset->ui_texture[0].getSize().y);
        viewport.setCenter(ref);
        display.setView(viewport);
    }

    render(node_array,prompt,asset,display,&tick);
    
    tick = (tick + 1 >= 40 ? 0 : tick + 1 );
    
}



}




if(quit) return QUIT;
else 
level->level_name = lvl_selected;

return EQ_SELECTION;
}