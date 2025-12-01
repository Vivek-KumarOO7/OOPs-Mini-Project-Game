#ifndef LANDMINE_H
#define LANDMINE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>

class Landmine {
private:
    sf::Sprite sprite;
    sf::Vector2f position;

    // Explosion animation
    sf::Sprite explosionSprite;
    int totalFrames = 6; // MODIFIED: Now 6 frames total
    int currentFrame = 0;
    float frameTime = 0.06f; // Time per frame
    sf::Clock frameClock;
    bool isExploding = false;
    bool explosionFinished = false;

    // Mine stats
    float triggerRange = 60.f;
    float range = 280.f;
    int maxDamage = 500;
    bool isActive = true;
    bool damageDealt = false; 

    // for delay
    sf::Clock armingClock;      // Timer for arming
    float armingTime = 2.0f;    // 2 seconds
    bool isArmed = false;       // Becomes true after 2 seconds

    // SFX
    static sf::Texture landmineTexture;
    
    // MODIFIED: Replaced single sheet with an array of 6 textures
    static sf::Texture explosionTextures[6]; 
    

public:
    static bool loadAssets();

    Landmine();
    void placeMine(sf::Vector2f pos);

    void trigger();    // Start explosion animation
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    bool isTriggerable() const { return isActive && isArmed; }
    bool isActiveNow() const { return isActive; }
    bool isExplodingNow() const { return isExploding; }
    bool hasExploded() const { return explosionFinished; }
    sf::Vector2f getPosition() const { return position; }
    float getRange() const { return range; }

    bool shouldDealDamage() const { return isExploding && !damageDealt; }
    void markDamageAsDealt() { damageDealt = true; }

    float getDamageAtDistance(float distance) const;
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
    float getTriggerRange() const { return triggerRange; }
};

#endif