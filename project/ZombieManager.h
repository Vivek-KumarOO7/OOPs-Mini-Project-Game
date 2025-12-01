#pragma once
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include"Zombie.h"
#include<vector>

class Player;
class ParticleSystem;

class ZombieManager{
private:
    // ========= Zombie data here =========
    std::vector<Zombie> zombies;
    sf::Vector2i windowSize;

    int waveNumber = 1;
    int timeBtwWaves = 10;
    int nextWaveIn;
    int timeTillNextWave;
    int maxTimeBtwWaves = 45;   // seconds
    int maxZombiesSpawn = 100;
    float timeSinceLastWave;

    sf::SoundBuffer zombieBiteSoundBuffer;
    sf::Sound zombieBiteSound;
public:
    ZombieManager(sf::Vector2i windowSize);

    void update(float deltaTime, Player& player, float brightness, ParticleSystem& particleSystem);
    void draw(sf::RenderWindow& window);
    // void spawnZombies();
    void spawnZombies(const sf::Vector2f& playerPos);
    int getTimeTillNextWave() const { return timeTillNextWave; }
    int getWaveNumber() const { return waveNumber; }

    zombieType getRandomZombieType();
    sf::Vector2f getaRandomSpawnLocation(const sf::Vector2f& playerPos);
};