#include "Bullet.h"
#include<cmath>

Bullet::Bullet(sf::Vector2f startPos, float angleDeg)
{
    isAliveFlag = true;
    angle = angleDeg;

    float radius = 2.f;

    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(radius, radius);

    shape.setPosition(startPos);

    float angleRad = angleDeg * 3.14159265f / 180.f;
    velocity = sf::Vector2f(std::cos(angleRad) * speed, std::sin(angleRad) * speed);

    lifeTime.restart();
}

void Bullet::update(float deltaTime){
    float maxAliveTime = 2.f;
    if (lifeTime.getElapsedTime().asSeconds() > maxAliveTime) isAliveFlag = false;
    shape.move(velocity * deltaTime);
}

void Bullet::draw(sf::RenderWindow &window)
{
    window.draw(shape);
}

bool Bullet::isAlive(sf::RenderWindow& window) const {
    if (isAliveFlag) return true;
    else return false;
}

void Bullet::setToRemove(bool flag)
{
    isAliveFlag = !flag;
}
