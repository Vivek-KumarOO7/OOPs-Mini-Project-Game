#include "Landmine.h"
#include "Paths.h" 
#include <iostream>

// static variables
sf::Texture Landmine::landmineTexture;
sf::Texture Landmine::explosionTextures[6];

// This funtion is called in main.cpp to load the assets
bool Landmine::loadAssets() {
    if (!landmineTexture.loadFromFile(landmine_texture_path)) {
        std::cerr << "Failed to load landmine texture" << std::endl;
        return false;
    }

    if (!explosionTextures[0].loadFromFile(landmine_explosion_frame_0_path)) {
        std::cerr << "Failed to load explosion frame 1" << std::endl;
        return false;
    }
    if (!explosionTextures[1].loadFromFile(landmine_explosion_frame_1_path)) {
        std::cerr << "Failed to load explosion frame 2" << std::endl;
        return false;
    }
    if (!explosionTextures[2].loadFromFile(landmine_explosion_frame_2_path)) {
        std::cerr << "Failed to load explosion frame 3" << std::endl;
        return false;
    }
    if (!explosionTextures[3].loadFromFile(landmine_explosion_frame_3_path)) {
        std::cerr << "Failed to load explosion frame 4" << std::endl;
        return false;
    }
    if (!explosionTextures[4].loadFromFile(landmine_explosion_frame_4_path)) {
        std::cerr << "Failed to load explosion frame 5" << std::endl;
        return false;
    }
    if (!explosionTextures[5].loadFromFile(landmine_explosion_frame_5_path)) {
        std::cerr << "Failed to load explosion frame 6" << std::endl;
        return false;
    }
    
    return true;
}


Landmine::Landmine() {
    // Set textures and sounds
    sprite.setTexture(landmineTexture);
    sprite.setColor(sf::Color(100, 100, 100, 255));
    sprite.setScale(0.05f, 0.05f);
    

    // Set origins to center
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    explosionSprite.setTexture(explosionTextures[0]); 
    
    // Set origin for explosion (assumes all 6 frames are the same size)
    sf::FloatRect explosionBounds = explosionSprite.getLocalBounds();
    explosionSprite.setOrigin(explosionBounds.width / 2.f, explosionBounds.height / 2.f);

    isActive = true;
    isExploding = false;
    explosionFinished = false;
    damageDealt = false;
    currentFrame = 0;
    isArmed = false;
}

void Landmine::placeMine(sf::Vector2f pos) {
    position = pos;
    sprite.setPosition(pos);
    armingClock.restart();
}

void Landmine::trigger() {
    if (isActive) { // Only trigger once
        isActive = false;
        isExploding = true;
        frameClock.restart();

        currentFrame = 0;
        explosionSprite.setTexture(explosionTextures[0]);
        explosionSprite.setPosition(position);
    }
}

void Landmine::update(float deltaTime) {
    if (isActive && !isArmed) {
        if (armingClock.getElapsedTime().asSeconds() >= armingTime) {
            isArmed = true;
            sprite.setColor(sf::Color(255, 255, 255, 255));     // normal color when armed
        }
    }
    if (isExploding && !explosionFinished) {
        if (frameClock.getElapsedTime().asSeconds() >= frameTime) {
            currentFrame++;

            if (currentFrame >= totalFrames) {
                explosionFinished = true;
            } else {
                // Set the sprite to the next texture in the animation
                explosionSprite.setTexture(explosionTextures[currentFrame]);
                explosionSprite.setScale(5.f, 5.f);
            }
            frameClock.restart();
        }
    }
}

void Landmine::draw(sf::RenderWindow& window) {
    if (isActive) {
        window.draw(sprite);
    } 
    else if (isExploding && !explosionFinished) {
        window.draw(explosionSprite);
    }
}

float Landmine::getDamageAtDistance(float distance) const {
    if (distance > range) {
        return 0.f;
    }
    float damagePercent = 1.0f - (distance / range);
    return maxDamage * damagePercent;
}