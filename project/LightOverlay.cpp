// LightOverlay.cpp
#include "LightOverlay.h"
#include <iostream>

LightOverlay::LightOverlay(unsigned int width, unsigned int height, Player& player)
    : texWidth(width), texHeight(height), minVisibleZone(player.getMinVisZone())
{
    darknessTexture.create(width, height);

    // Radial Gradient act as light mask
    lightTexture = createRadialGradientTexture(512); // 512×512 radial gradient
    lightSprite.setTexture(lightTexture);
    lightSprite.setOrigin(256.f, 256.f);

    nightColor = sf::Color(255, 255, 255, 255); // base darkness
}

sf::Texture LightOverlay::createRadialGradientTexture(unsigned int size)
{
    sf::Image img;
    img.create(size, size, sf::Color::Transparent);

    sf::Vector2f center(size / 2.f, size / 2.f);
    float maxDist = size / 2.f;

    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            float dx = x - center.x;
            float dy = y - center.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            float improveVisibility = 0.2f;     // by 20%
            float alpha = 1.0f - std::min(dist / (maxDist * (1.0f + improveVisibility)), 1.0f);
            // Smooth fall-off
            alpha = std::pow(alpha, 2.0f);  
            sf::Uint8 a = static_cast<sf::Uint8>(alpha * 255);
            img.setPixel(x, y, sf::Color(255, 255, 255, a));
        }
    }

    sf::Texture tex;
    tex.loadFromImage(img);
    tex.setSmooth(true);
    return tex;
}

void LightOverlay::update(const sf::View& view, float brightness, Player& player) {
    // Clamp brightness
    brightness = std::clamp(brightness, 0.f, 1.f);

    // Map brightness to colour intensity
    float threshold = 0.3f;
    float temp = (brightness < threshold) ? 0.f : (brightness - threshold);
    float colorBrightness = (temp - 0.0f) / ((1.0f - threshold) - 0.0f);
    colorBrightness = std::clamp(colorBrightness, 0.f, 1.f);

    nightColor.r = static_cast<sf::Uint8>(colorBrightness * 255.f);
    nightColor.g = static_cast<sf::Uint8>(colorBrightness * 255.f);
    nightColor.b = static_cast<sf::Uint8>(colorBrightness * 255.f);

    darknessTexture.clear(nightColor);

    // Compute view info
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f viewTopLeft(viewCenter.x - viewSize.x / 2.f, viewCenter.y - viewSize.y / 2.f);

    // Use player's position RELATIVE TO CAMERA (so it stays stable)
    sf::Vector2f playerWorldPos = player.getPosition();
    sf::Vector2f localPos = playerWorldPos - viewTopLeft;

    // Adjust visible zone
    minVisibleZone = player.getMinVisZone();
    float lightRadius = minVisibleZone + 1300.f * temp;

    lightSprite.setPosition(localPos);
    lightSprite.setScale(
        (lightRadius * 2.f) / lightTexture.getSize().x,
        (lightRadius * 2.f) / lightTexture.getSize().y
    );

    // clear before drawing new light
    darknessTexture.clear(nightColor);

    // Draw the light into the darkness texture
    sf::RenderStates states(sf::BlendAdd);
    darknessTexture.draw(lightSprite, states);
    darknessTexture.display();
}

void LightOverlay::draw(sf::RenderWindow& window) {
    sf::Sprite overlay(darknessTexture.getTexture());

    sf::View currentView = window.getView();
    sf::Vector2f viewSize = currentView.getSize();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewTopLeft(viewCenter.x - viewSize.x / 2.f, viewCenter.y - viewSize.y / 2.f);

    overlay.setPosition(viewTopLeft);

    window.draw(overlay, sf::BlendMultiply);
}