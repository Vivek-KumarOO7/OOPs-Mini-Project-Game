#include "ParticleSystem.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()
#include<iostream>

ParticleSystem::ParticleSystem()
    : gravity(2000.f), defaultLifetime(0.5f) {
    // Seed the random number generator
    srand(static_cast<unsigned>(time(nullptr)));
    // Initialize VertexArray as Quads (4 points per blood stain)
    bloodVertexArray.setPrimitiveType(sf::Quads);
    bloodVertexArray.resize(MAX_BLOOD_STAINS * 4); 

    // Initialize all vertices to transparent so they don't show up initially
    for (int i = 0; i < MAX_BLOOD_STAINS * 4; ++i) {
        bloodVertexArray[i].color = sf::Color::Transparent;
    }
}

// Adds particles to the Particle vector
void ParticleSystem::addEffect(sf::Vector2f position, int count, sf::Vector2f direction, float scale, bool deadStain) {
    // Get the base angle from the bullet's direction
    float baseAngle = std::atan2(direction.y, direction.x);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.lifetime = defaultLifetime * (0.5f + (rand() % 100) / 100.f); // Random lifetime

        // --- Velocity ---
        // Create a 90-degree splash cone (+/- 45 degrees)
        float angleOffset = (rand() % 90 - 45) * 3.14159f / 180.f;
        float angle = baseAngle + angleOffset;
        
        // Random speed
        float speed = 200.f + (rand() % 300); 
        
        p.velocity.x = std::cos(angle) * speed;
        p.velocity.y = std::sin(angle) * speed;

        // --- Shape ---
        float scaleRand = 1 + (rand() % 50) / 100;      // 1 to 1.5 times
        scale *= scaleRand;
        p.shape.setSize(sf::Vector2f(scale, scale));
        p.shape.setFillColor(sf::Color(180, 0, 0, 255)); // Dark red
        p.shape.setPosition(position);

        p.isDeadStain = deadStain;
        particles.push_back(p);
    }
    // if (deadStain){
    //     std::cout << "Blood Stain Count: " << bloodStainIndex << std::endl;
    // }
}

void ParticleSystem::update(float deltaTime) {
    // We must iterate backwards when removing items
    for (int i = particles.size() - 1; i >= 0; --i) {
        Particle& p = particles[i];
        p.lifetime -= deltaTime;

        // If particle is dead, remove it
        if (p.lifetime <= 0) {
            if (p.isDeadStain) {
                // Calculate the 4 corners of the quad based on position and size
                sf::Vector2f pos = p.shape.getPosition();
                sf::Vector2f size = p.shape.getSize();
                sf::Color color(150, 0, 0, 100); // Blood color

                // Index in the vertex array (4 vertices per stain)
                int vIdx = currentBloodIndex * 4;

                // Top-Left
                bloodVertexArray[vIdx + 0].position = pos;
                bloodVertexArray[vIdx + 0].color = color;

                // Top-Right
                bloodVertexArray[vIdx + 1].position = sf::Vector2f(pos.x + size.x, pos.y);
                bloodVertexArray[vIdx + 1].color = color;

                // Bottom-Right
                bloodVertexArray[vIdx + 2].position = sf::Vector2f(pos.x + size.x, pos.y + size.y);
                bloodVertexArray[vIdx + 2].color = color;

                // Bottom-Left
                bloodVertexArray[vIdx + 3].position = sf::Vector2f(pos.x, pos.y + size.y);
                bloodVertexArray[vIdx + 3].color = color;

                // Advance circular buffer index
                currentBloodIndex = (currentBloodIndex + 1) % MAX_BLOOD_STAINS;
            }
            particles.erase(particles.begin() + i);
        } else {
            // --- Physics ---
            // p.velocity.y += gravity * deltaTime; // Apply gravity
            p.shape.move(p.velocity * deltaTime); // Apply movement

            // --- Graphics ---
            // Fade out
            float lifePercent = p.lifetime / defaultLifetime;
            int alpha;
            if (p.isDeadStain){
                alpha = std::max(75, (int)(255 * lifePercent));
            } else{
                alpha = (int)(255 * lifePercent);
            }
            sf::Color color = p.shape.getFillColor();
            color.a = alpha;
            p.shape.setFillColor(color);
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    for (const auto& p : particles) {
        window.draw(p.shape);
    }
    window.draw(bloodVertexArray);
}