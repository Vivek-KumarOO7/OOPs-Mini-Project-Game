#include "Crosshair.h"
#include"Paths.h"

Crosshair::Crosshair()
{
    if(!texture.loadFromFile(icon_crosshair_path)){
        throw std::runtime_error("Failed to load the Crosshair Icon.");
    }

    float scale = 0.06f;
    sprite.setTexture(texture);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2, bounds.height / 2);
    sprite.setScale(scale, scale);
}

void Crosshair::update(sf::Vector2i mousePos)
{
    sprite.setPosition(mousePos.x, mousePos.y);
}

void Crosshair::draw(sf::RenderWindow &window)
{
    window.draw(sprite);
}
