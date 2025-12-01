#ifndef LIGHTOVERLAY_H
#define LIGHTOVERLAY_H

#include"Player.h"
#include <SFML/Graphics.hpp>

class LightOverlay {
private:
    sf::RenderTexture darknessTexture;
    // sf::CircleShape lightCircle;
    sf::Texture lightTexture;
    sf::Sprite lightSprite;
    sf::Color nightColor;
    unsigned int texWidth, texHeight;
    float minVisibleZone;

public:
    LightOverlay(unsigned int width, unsigned int height, Player& player);

    sf::Texture createRadialGradientTexture(unsigned int size);
    
    void update(const sf::View& view, float brightness, Player& player);
    void draw(sf::RenderWindow& window);
};

#endif
