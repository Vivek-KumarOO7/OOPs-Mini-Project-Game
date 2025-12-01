#pragma once
#include <SFML/Graphics.hpp>


enum zombieType {normal = 0, sprinter = 1, tank = 2};

class Zombie {
private:
    sf::Sprite sprite;
    float baseSpeed;
    float speed;
    float maxSpeed;
    float baseHealth;
    float health;
    bool isRaged;
    int score;
    zombieType type;


    sf::Clock damageClock; // keeps track of when the zombie last attacked
    float damageCooldown; // seconds between attacks
    float baseDamageAmount;
    float damageAmount;   // damage per attack
    float damageRange;    // Melee range of Zombie

public:
    static sf::Texture texture;

    Zombie(const sf::Vector2f& startPos, float health, float speed, float dmg, int score, zombieType type);

    void update();
    void draw(sf::RenderWindow& window);
    
    void move(const sf::Vector2f& offset) { sprite.move(offset); }
    void setRotation(float angle) { sprite.setRotation(angle); }

    void takeDamage(float amount);
    bool isDead() const;

    sf::Vector2f getPosition() const { return sprite.getPosition(); }
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
    float getSpeed() const { return speed; }
    float getDamageRange() const { return damageRange; }
    float getDamageAmount() const { return damageAmount; }
    sf::Clock& getDamageClock() { return damageClock; }
    float getDamageCooldown() const { return damageCooldown; }
    sf::Sprite& getZomSprite() { return sprite; }
    bool getRageStatus() const { return isRaged; }
    int getScore() const { return score; }
    int getHealth() const { return baseHealth; }
    zombieType getZombieType() const { return type; }

    void setRageTo(bool rage) { isRaged = rage; }
    


};
