#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class ParticleSystem {
private:
    // Defines a single blood droplet
    struct Particle {
        sf::Vector2f velocity;
        float lifetime;
        sf::RectangleShape shape;
        bool isDeadStain;
    };

    std::vector<Particle> particles;
    int MAX_BLOOD_STAINS = 4000;
    int currentBloodIndex = 0;
    sf::VertexArray bloodVertexArray;
    float gravity;
    float defaultLifetime;

public:
    ParticleSystem();

    // The main function to create a blood splash
    void addEffect(sf::Vector2f position, int count, sf::Vector2f direction, float scale, bool deadStain);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
};

#endif