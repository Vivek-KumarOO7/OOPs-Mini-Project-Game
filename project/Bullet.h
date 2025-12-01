#ifndef BULLET_H
#define BULLET_H

#include<SFML/Graphics.hpp>

class Bullet{
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speed = 2500.f;
    bool isAliveFlag;
    float angle;

    sf::Clock lifeTime;

public:
    Bullet(sf::Vector2f startPos, float angleDeg);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    bool isAlive(sf::RenderWindow& window) const;

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    void setToRemove(bool flag);

    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float getRotation() const { return angle; }
    
};  

#endif