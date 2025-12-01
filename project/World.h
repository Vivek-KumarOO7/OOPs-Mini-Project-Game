#ifndef WORLD_H
#define WORLD_H

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include"ZombieManager.h"
#include"Zombie.h"

class Player;
class ParticleSystem;

class World {
private:
    sf::VertexArray backgroundGradient;
    std::vector<sf::Sprite> grassPatches;
    std::vector<sf::Texture> grassTextures;
    int worldWidth, worldHeight;
    int nightSurvived = 0;
    bool isNight = false;

    float timeOfDay;   // in hours (0–24)
    float dayLength;   // duration of one full day–night cycle (in seconds)

    float currentBrightness;

    

public:
    World(int width, int height, float dayStartTime, float dayLen);
    void update(float deltaTime, Player& player);
    void draw(sf::RenderWindow& window);

    sf::Vector2f getSize() const;
    float getCurrentBrightness() const;
    float getTimeOfDay() const { return timeOfDay; }
    sf::Vector2i getWorldSize() const { return {worldWidth, worldHeight}; }
    int getNightSurvived() const { return nightSurvived; }
};

#endif
