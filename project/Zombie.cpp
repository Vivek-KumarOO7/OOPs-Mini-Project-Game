#include "Zombie.h"
#include"Paths.h"
#include <cmath>

sf::Texture Zombie::texture; // define static texture

Zombie::Zombie(const sf::Vector2f& startPos, float hth, float spd, float dmg, int s, zombieType ty)
    : baseSpeed(spd), baseHealth(hth), baseDamageAmount(dmg), score(s), type(ty)
{
    // Load shared texture only once
    if (texture.getSize().x == 0) {
        if (!texture.loadFromFile(zombie_texture_path)) {
            throw std::runtime_error("Failed to load zombie texture.");
        }
    }

    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    sprite.setPosition(startPos);

    if (type == sprinter){
        sprite.setScale(0.480f, 0.480f);
    } else if (type == tank){
        sprite.setScale(0.625f, 0.625f);
    } else{
        sprite.setScale(0.525f, 0.525f);
    }

    // Zombie stats setup
    health = baseHealth;
    damageCooldown = 2.0f;
    damageRange = 65.f;
    damageAmount = baseDamageAmount;
    speed = baseSpeed;
    maxSpeed = 280.f;
}

void Zombie::update()
{
    // Movement is handled in: Overlapping prevention
    // Rage mode
    if (isRaged) {
        speed = std::min((int)maxSpeed, (int)(1.5 * baseSpeed));
        damageAmount = 1.5 * baseDamageAmount;
    } else{
        speed = baseSpeed;
        damageAmount = baseDamageAmount;
    }
}

void Zombie::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

void Zombie::takeDamage(float amount)
{
    health -= amount;
}

bool Zombie::isDead() const
{
    return health <= 0.f;
}
