#include "World.h"
#include"Player.h"
#include"Zombie.h"
#include "Paths.h"
#include "ParticleSystem.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>

World::World(int width, int height, float dayStartTime, float dayLen)
    : worldWidth(width), worldHeight(height), timeOfDay(dayStartTime), dayLength(dayLen)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // ================= Gradient Background =================
    backgroundGradient.setPrimitiveType(sf::Quads);
    backgroundGradient.resize(4);

    backgroundGradient[0].position = sf::Vector2f(0.f, 0.f);
    backgroundGradient[1].position = sf::Vector2f(worldWidth, 0.f);
    backgroundGradient[2].position = sf::Vector2f(worldWidth, worldHeight);
    backgroundGradient[3].position = sf::Vector2f(0.f, worldHeight);

    // ================= Grass Generation =================
    std::vector<std::string> texturePaths = {
        grass1_texture_path,
        grass2_texture_path,
        grass3_texture_path
    };

    grassTextures.resize(texturePaths.size());
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        if (!grassTextures[i].loadFromFile(texturePaths[i])) {
            std::cerr << "Failed to load " << texturePaths[i] << std::endl;
        }
    }

    const int patchCount = 300;
    for (int i = 0; i < patchCount; ++i) {
        int textureIndex = std::rand() % grassTextures.size();
        sf::Sprite grass(grassTextures[textureIndex]);

        float x = static_cast<float>(std::rand() % worldWidth);
        float y = static_cast<float>(std::rand() % worldHeight);
        grass.setPosition(x, y);

        float scale = 0.6f + static_cast<float>(std::rand() % 40) / 100.f;
        grass.setScale(scale, scale);

        grassPatches.push_back(grass);
    }

}

// ================= DAY–NIGHT GRADIENT UPDATE =================
void World::update(float deltaTime, Player& player) {
    // Advance time
    timeOfDay += (24.f / dayLength) * deltaTime;
    if (timeOfDay >= 24.f) timeOfDay -= 24.f;
    
    // Store previous night state before update
    bool wasNight = isNight;

    // Determine whether it's currently night
    if ((timeOfDay >= 19.f && timeOfDay <= 24.f) || (timeOfDay >= 0.f && timeOfDay <= 6.f)) {
        isNight = true;
    } else {
        isNight = false;
    }

    // Detect transition from night → day
    if (wasNight && !isNight) {
        nightSurvived++;
    }

    // brightness oscillates smoothly from 0.15 (night) to 0.55 (day)
    float brightness = 0.35f + 0.20f * std::sin((timeOfDay / 24.f) * 2 * 3.14159f - 3.14159f / 2);
    currentBrightness = brightness;
    // std::cout << brightness << std::endl;

    // Top and bottom colours for day/night
    sf::Color topDay(100, 130, 70);
    sf::Color bottomDay(80, 110, 60);

    sf::Color topNight(8, 10, 6);
    sf::Color bottomNight(5, 8, 4);

    // Blend colours
    sf::Color topBlend, bottomBlend;
    topBlend.r = static_cast<sf::Uint8>(topNight.r + (topDay.r - topNight.r) * brightness);
    topBlend.g = static_cast<sf::Uint8>(topNight.g + (topDay.g - topNight.g) * brightness);
    topBlend.b = static_cast<sf::Uint8>(topNight.b + (topDay.b - topNight.b) * brightness);

    bottomBlend.r = static_cast<sf::Uint8>(bottomNight.r + (bottomDay.r - bottomNight.r) * brightness);
    bottomBlend.g = static_cast<sf::Uint8>(bottomNight.g + (bottomDay.g - bottomNight.g) * brightness);
    bottomBlend.b = static_cast<sf::Uint8>(bottomNight.b + (bottomDay.b - bottomNight.b) * brightness);

    // Apply colours to the gradient
    backgroundGradient[0].color = topBlend;
    backgroundGradient[1].color = topBlend;
    backgroundGradient[2].color = bottomBlend;
    backgroundGradient[3].color = bottomBlend;

}

void World::draw(sf::RenderWindow& window) {
    window.draw(backgroundGradient);

    // Draw Grass
    for (auto& g : grassPatches){
        window.draw(g);
    }

}

sf::Vector2f World::getSize() const {
    return sf::Vector2f(worldWidth, worldHeight);
}

float World::getCurrentBrightness() const {
    // assumeing raw brightness (day-night oscillation) goes from 0.15 to 0.55
    float normalised = (currentBrightness - 0.15f) / (0.55f - 0.15f);
    // std::cout << "Curr: " << currentBrightness << ", " << "Nor: " << normalised << "\n";
    if (normalised < 0.f) normalised = 0.f;
    if (normalised > 1.f) normalised = 1.f;
    return normalised;
}
