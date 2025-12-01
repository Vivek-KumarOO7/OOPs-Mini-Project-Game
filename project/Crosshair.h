#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include<SFML/Graphics.hpp>

class Crosshair{
private:
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Crosshair();

    void update(sf::Vector2i mousePos);
    void draw(sf::RenderWindow& window);
};

#endif