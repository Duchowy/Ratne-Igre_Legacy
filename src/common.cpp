#include "common.h"


sf::Vector2f operator/(const sf::Vector2f & vec, double num)
{
    return sf::Vector2f(vec.x/num,vec.y/num);
}

sf::Vector2u operator/(const sf::Vector2u & vec, int num)
{
    return sf::Vector2u(vec.x/num,vec.y/num);
}



sf::Vector2f Button::getSize()
{
    return this->box.front().getSize();
}

sf::Vector2f Button::getPosition()
{
    return this->box.front().getPosition();
}

void Button::setBoxPosition(sf::Vector2f vector)
{
    this->box.front().setPosition(vector);
}

void Button::setBoxPosition(size_t box_id, sf::Vector2f vector)
{

if(box_id < this->box.size())
{
    this->box[box_id].setPosition(this->box.front().getPosition() - sf::Vector2f(this->box.front().getSize().x/2,this->box.front().getSize().y/2)); //set to height
    this->box[box_id].move(vector);
}

}


void Button::setBoxPosition(size_t box_id,float x_pos, float y_pos)
{

if(box_id < this->box.size() && box_id > 0)
{
    this->box[box_id].setPosition(this->box.front().getPosition() - sf::Vector2f(this->box.front().getSize().x/2,this->box.front().getSize().y/2)); //set to height

    if((x_pos >= 0.f && x_pos <= 1.f) && (y_pos >= 0.f && y_pos <= 1.f))
    {
        this->box[box_id].move(sf::Vector2f(this->box.front().getSize().x * x_pos,this->box.front().getSize().y * y_pos));
    }

}

}




void Button::setPosition(sf::Vector2f vector)
{
sf::Vector2f delta = vector - this->getPosition();
this->move(delta);
}


void Button::setTextPosition(size_t text_id,float x_pos, float y_pos)
{

if(text_id < this->text.size())
{
    this->text[text_id].setPosition(this->box.front().getPosition() - sf::Vector2f(this->box.front().getSize().x/2,this->box.front().getSize().y/2)); //set to height

    if((x_pos >= 0.f && x_pos <= 1.f) && (y_pos >= 0.f && y_pos <= 1.f))
    {
        this->text[text_id].move(sf::Vector2f(this->box.front().getSize().x * x_pos,this->box.front().getSize().y * y_pos));
    }

}

}


void Button::setTextPosition(size_t text_id, sf::Vector2f vector)
{

if(text_id < this->text.size())
{
    this->text[text_id].setPosition(this->box.front().getPosition() - sf::Vector2f(this->box.front().getSize().x/2,this->box.front().getSize().y/2)); //set to height
    this->text[text_id].move(vector);
}

}


void Button::move(sf::Vector2f vector)
{
    for(auto & obj : this->box) obj.move(vector);
    for(auto & obj : this->text) obj.move(vector);
}


void Button::draw(sf::RenderTarget& target)
{
    for(auto & obj : this->box) target.draw(obj);
    for(auto & obj : this->text) target.draw(obj);
}

void Button::centerOrigin()
{
this->box.front().setOrigin(this->box.front().getSize()/2);
for(auto & obj : this->text) obj.setOrigin(obj.getLocalBounds().width/2,0);
}