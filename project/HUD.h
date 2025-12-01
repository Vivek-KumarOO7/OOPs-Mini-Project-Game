#ifndef HUD_H
#define HUD_H

#include"World.h"
#include"Player.h"
#include<string>
#include<SFML/Graphics.hpp>

class HUD{
private:
    sf::Font font;
    sf::Text text;

public:
    HUD(std::string pathToFont);
    void draw(sf::RenderWindow& window, Player& player, World& world, ZombieManager& ZombieManager);
    void writeText(sf::RenderWindow& window, int x, int y, std::string str, sf::Color& color);
};


#endif