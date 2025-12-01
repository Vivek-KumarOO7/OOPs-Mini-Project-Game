#ifndef PLAYER_H
#define PLAYER_H

#include"Bullet.h"
#include"World.h"
#include"Landmine.h"
#include<SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<cmath>
#include<vector>
#include<iostream>

class Player{
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    float walkSpeed;
    float sprintSpeed;
    bool isRunning;
    
    // Player stats
    int maxArmor;
    int armor;
    int maxHealth;
    int health;
    float minVisibleZone;
    bool isAlive = true;
    int score = 0;
    int zombiesKilled = 0;
    int coins = 500; // starting coins
    
    // Heartbeat sound
    sf::Clock heartbeatClock;
    float heartBeatFadeTime = 3.f;
    float maxHeartbeatVolume = 40.f;
    float heartbeatSoundDuration = 7.f;
    bool isHeartbeatPlaying = false;
    
    // Shooting variables
    int bulletDamage;
    int magazineSize;
    int ammoInMag;
    int totalAmmo;
    bool isReloading;
    float reloadTime;
    float fireRate;
    sf::Clock reloadClock;

    // Landmine
    std::vector<Landmine> landmines;  // This is for mines placed on the ground
    int landmineInventory;            // Tracks mines player can place
    sf::Clock minePlaceClock;
    float mineCooldown = 2.f;
    float explosionVolume = 40.f;
    
    std::vector<Bullet> bullets;    // store all active bullets
    sf::Clock shootClock;           // controls the firing rate

    // muzzle flash
    sf::RectangleShape muzzleFlash;
    sf::Clock muzzleClock;
    float muzzleFlashDuration = 0.03f;   // 30ms
    bool showMuzzleFlash;

    // sfx for gun
    sf::SoundBuffer gunshotBuffer;
    sf::Sound gunshotSound;

    sf::SoundBuffer gunreloadBuffer;
    sf::Sound gunreloadSound;

    sf::SoundBuffer cantFireSoundBuffer;
    sf::Sound cantFireSound;

    sf::SoundBuffer explosionBuffer;
    sf::Sound explosionSound;

    sf::SoundBuffer heartbeatSoundBuffer;
    sf::Sound heartbeatSound;

public:
    Player(const sf::FloatRect& worldBounds, float minVisZone);
    void handleInput(float deltaTime);
    void update(sf::RenderWindow& window, float deltaTime, const sf::FloatRect& worldBounds, const World& world);
    void draw(sf::RenderWindow& window);

    void takeDamage(float amount);

    sf::Vector2i getHealth() const { return {health, maxHealth}; }
    sf::Vector2i getArmor() const { return {armor, maxArmor}; }
    bool getReloadStatus() const { return isReloading; }
    sf::Vector2i getAmmoStatus() const { return {ammoInMag, magazineSize}; }
    int getTotalAmmo() const { return totalAmmo; }
    float getMinVisZone() const { return minVisibleZone; }
    std::vector<Landmine>& getMines() { return landmines; }

    sf::Vector2f getPosition() const;
    sf::Vector2f getGunMuzzlePosition(float forwardOffset, float sideOffset) const;
    std::vector<Bullet>& getBullets() { return bullets; }
    int getBulletDamage() const { return bulletDamage; }
    bool getAliveStatus() const { return isAlive; }
    int getLandmineCount() const { return landmineInventory; }

    // For Shop
    void addAmmo(int amount) { totalAmmo += amount; }
    void increaseDamage(int amount) { bulletDamage += amount; }
    void increaseFireRate() { fireRate -= 0.025f; }
    void repairOrAddArmour(int amount) { armor = std::min((armor + amount), maxArmor); }
    void heal(int amount) { health = std::min(health + amount, maxHealth); }
    void addCoins(int amount) { coins += amount; }
    void addLandmine() { landmineInventory++; }

    int getCoins() const { return coins; }
    int getScore() const { return score; }
    void addScore(int amt) { score += amt; }
    int getZombiesKilled() { return zombiesKilled; }
    void addZombiesKilled(int amt) { zombiesKilled += amt; }
    void playExposionSound() { explosionSound.play(); }

};

#endif