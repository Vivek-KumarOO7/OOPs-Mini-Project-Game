#include "ZombieManager.h"
#include"Paths.h"
#include"Player.h"
#include "World.h"
#include "ParticleSystem.h"
#include<iostream>
#include<cmath>

ZombieManager::ZombieManager(sf::Vector2i winSize) : windowSize(winSize)
{
    if (!zombieBiteSoundBuffer.loadFromFile(zombie_bite_sound_path)){
        throw std::runtime_error("Failed to Load Zomibe bite sound.");
    }
    zombieBiteSound.setBuffer(zombieBiteSoundBuffer);
    zombieBiteSound.setVolume(20.f);
    timeSinceLastWave = 0.f;
}

void ZombieManager::update(float deltaTime, Player &player, float brightness, ParticleSystem& particleSystem)
{
    // =========== Update Zombies ==============
    // Spawning Zombies;
    if (waveNumber > 15) maxTimeBtwWaves = 60;
    else if (waveNumber > 11) maxTimeBtwWaves = 55;
    else if (waveNumber > 8) maxTimeBtwWaves = 50;

    nextWaveIn = std::min(maxTimeBtwWaves, (timeBtwWaves + (int)(4.25 * waveNumber)));    // seconds
    timeSinceLastWave += deltaTime;

    if (timeSinceLastWave >= nextWaveIn){
        spawnZombies(player.getPosition());
        timeSinceLastWave = 0.f; // Reset the timer
        waveNumber++;
    }
    timeTillNextWave = (int)(nextWaveIn - timeSinceLastWave);

    // Bullet–Zombie collision
    for (auto& bullet : player.getBullets()) {
        for (auto& z : zombies) {
            if (bullet.getBounds().intersects(z.getBounds())) {
                z.takeDamage(player.getBulletDamage());
                // --- PARTICLE CODE ---
                zombieType zType = z.getZombieType();
                float scale = 4.f;
                if (zType == sprinter){
                    scale = 3.f;
                } else if (zType == tank){
                    scale = 5.f;
                } else{
                    scale = 4.f;
                }
                float angleRad = bullet.getRotation() * 3.14159f / 180.f;       // Degrees --> Radians
                sf::Vector2f direction(std::cos(angleRad), std::sin(angleRad));
                // Create a 20-particle splash at the bullet's location
                particleSystem.addEffect(bullet.getPosition(), 20, direction, scale, false);

                bullet.setToRemove(true);
                if (z.isDead()){
                    int bloodCount = 22;
                    if (zType == tank){
                        bloodCount = 35;
                    } else if(zType == sprinter){
                        bloodCount = 15;
                    } else{
                        bloodCount = 22;
                    }
                    particleSystem.addEffect(z.getPosition(), bloodCount, direction, 10.f, true);
                    int scoreMultiplier = 1;
                    int coinMultiplier = 1;
                    int gain = z.getHealth() / 10;
                    if (z.getRageStatus()){
                        scoreMultiplier = 2;
                        coinMultiplier = 1.35;
                    } else{
                        scoreMultiplier = 1;
                        coinMultiplier = 1;
                    }
                    int scoreGained = z.getScore() * scoreMultiplier;
                    gain = gain * coinMultiplier;
                    gain = std::min(gain, 25);
                    player.addCoins(gain);
                    player.addScore(scoreGained);
                    player.addZombiesKilled(1);
                } 
            }
        }
    }
    
    // --- Landmine–Zombie Logic ---
    for (auto& mine : player.getMines()) 
    {
        // 1. Check if an active mine should be triggered by a zombie or player
        if (mine.isTriggerable()) {
            bool triggeredThisFrame = false; // Flag to prevent double-trigger

            // Check zombies first
            for (auto& z : zombies) {
                sf::Vector2f diff = z.getPosition() - mine.getPosition();
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                //  trigger Range
                if (dist < mine.getTriggerRange()) { 
                    mine.trigger();
                    player.playExposionSound();
                    triggeredThisFrame = true;
                    break;
                }
            }
            
            // Check player (only if not already triggered by a zombie)
            if (!triggeredThisFrame) { // to prevent double trigger
                sf::Vector2f diff = player.getPosition() - mine.getPosition();
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                //  trigger Range
                if (dist < mine.getTriggerRange()) { 
                    mine.trigger();
                    player.playExposionSound();
                }
            }
        }
        
        // 2. Check if this mine is in its "deal damage" phase (just exploded)
        if (mine.shouldDealDamage()) {
            // Apply damage to ALL zombies in range
            for (auto& z : zombies) {
                sf::Vector2f zPos = z.getPosition();
                sf::Vector2f minePos = mine.getPosition();
                sf::Vector2f diff = zPos- minePos;
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                
                // Get damage based on falloff
                float damage = mine.getDamageAtDistance(dist);
                
                if (damage > 0) {
                    z.takeDamage(damage);
                    // Check if zombie was killed by the explosion
                    if (z.isDead()) {
                        zombieType zType = z.getZombieType();
                        int bloodCount = 22;
                        if (zType == tank){
                            bloodCount = 30;
                        } else if(zType == sprinter){
                            bloodCount = 15;
                        } else{
                            bloodCount = 22;
                        }
                        // getting angle for blood spray
                        float dx = zPos.x - minePos.x;
                        float dy = zPos.y - minePos.y;
                        float bloodSprayAngle = atan2(dy, dx);
                        sf::Vector2f direction(std::cos(bloodSprayAngle), std::sin(bloodSprayAngle));
                        particleSystem.addEffect(z.getPosition(), bloodCount, direction, 10.f, true);

                        int scoreMultiplier = z.getRageStatus() ? 2 : 1;
                        float coinMultiplier = z.getRageStatus() ? 1.35f : 1.0f;
                        int gain = (z.getHealth() / 10) * coinMultiplier;
                        
                        int scoreGained = z.getScore() * scoreMultiplier;
                        gain = std::min(gain, 25);
                        
                        player.addCoins(gain);
                        player.addScore(scoreGained);
                        player.addZombiesKilled(1);
                    }
                }
            }
            // Dealing damage to player
            sf::Vector2f diff = player.getPosition() - mine.getPosition();
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            // Get damage based on falloff
            float damage = mine.getDamageAtDistance(dist);
            if (damage > 0) player.takeDamage(damage);
            // Mark damage as dealt so it only happens once
            mine.markDamageAsDealt();
        }
    }
    // --- End of Landmine Logic ---

    // Remove dead zombies
    zombies.erase(std::remove_if(zombies.begin(), zombies.end(), [](const Zombie& z){ return z.isDead(); }), zombies.end());

    // ================== To Avoid Overlap Variables ===================
    const float minZombieDist = 60.f;      // distance to maintain between zombies
    const float separationStrength = 20.f; // strength of zombie–zombie separation
    const float playerAvoidDist = 60.f;    // min distance from player
    const float playerRepelStrength = 100.f; // how strongly zombies avoid overlapping player
    
    for (auto& z : zombies) {
        // ===== Zombie Tint Based on Night Darkness =====
        sf::Color finalColour = sf::Color::White; // default (daytime)
        float threshold = 0.4f;
        if (brightness <= threshold) {
            z.setRageTo(true);
            // Map brightness from [0.3 → 0.0] → [0.0 → 1.0]
            float intensity = (threshold - brightness) /threshold;
            if (intensity < 0.f) intensity = 0.f;
            if (intensity > 1.f) intensity = 1.f;

            // Red tint increases with darkness
            // std::cout << brightness << ", " << intensity << "\n";
            sf::Uint8 r = 255;
            sf::Uint8 g = static_cast<sf::Uint8>(255 * (1.f - 0.4f * intensity)); // reduce green
            sf::Uint8 b = static_cast<sf::Uint8>(255 * (1.f - 0.4f * intensity)); // reduce blue

            finalColour = sf::Color(r, g, b);
        } else{
            z.setRageTo(false);
        }

        // Apply final colour
        z.getZomSprite().setColor(finalColour);
        // Updating stats
        z.update();

        // ================== Zombies attacking logic ===================
        sf::Vector2f playerPos = player.getPosition();
        sf::Vector2f zombiePos = z.getPosition();

        float damageRange = z.getDamageRange();

        float dx = playerPos.x - zombiePos.x;
        float dy = playerPos.y - zombiePos.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // If close enough to bite
        if (dist < damageRange && z.getDamageClock().getElapsedTime().asSeconds() >= z.getDamageCooldown()) {
            player.takeDamage(z.getDamageAmount());
            z.getDamageClock().restart();
            zombieBiteSound.play();

            float bloodSprayAngle = atan2(dy, dx);
            sf::Vector2f direction(std::cos(bloodSprayAngle), std::sin(bloodSprayAngle));
            particleSystem.addEffect(player.getPosition(), 20, direction, 4.f, false);
        }
        
        // ================== To Avoid Overlap Logic here ===================
        // ---- 1. Direction toward player ----
        sf::Vector2f dirToPlayer = playerPos - zombiePos;
        float len = std::sqrt(dirToPlayer.x * dirToPlayer.x + dirToPlayer.y * dirToPlayer.y);
        if (len > 0.001f) dirToPlayer /= len;

        // ---- 2. Separation from other zombies ----
        sf::Vector2f separation(0.f, 0.f);
        for (auto& other : zombies) {
            if (&z == &other) continue;
            sf::Vector2f diff = zombiePos - other.getPosition();
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist < minZombieDist && dist > 0.001f) {
                diff /= dist;
                separation += diff * (minZombieDist - dist);
            }
        }

        // ---- 3. Repulsion from player (so they don’t overlap) ----
        sf::Vector2f repel(0.f, 0.f);
        float distToPlayer = std::sqrt(
            (zombiePos.x - playerPos.x) * (zombiePos.x - playerPos.x) +
            (zombiePos.y - playerPos.y) * (zombiePos.y - playerPos.y)
        );

        if (distToPlayer < playerAvoidDist && distToPlayer > 0.001f) {
            repel = (zombiePos - playerPos) / distToPlayer;
            repel *= (playerAvoidDist - distToPlayer);
        }

        // ---- 4. Combine movement ----
        sf::Vector2f totalVel =
            (dirToPlayer * z.getSpeed()) +         // chase player
            (separation * separationStrength) +    // separate from others
            (repel * playerRepelStrength);         // avoid player

        z.move(totalVel * deltaTime);

        // ---- 5. Rotate to face player ----
        float angle = std::atan2(dirToPlayer.y, dirToPlayer.x) * 180.f / 3.14159f;
        z.setRotation(angle);
    }
}

void ZombieManager::draw(sf::RenderWindow& window)
{
    // Draw Zombies
    for (auto& z : zombies){
        z.draw(window);
    }
}

void ZombieManager::spawnZombies(const sf::Vector2f& playerPos)
{
    int baseCount = 5;                      // zombies in wave 1
    float scalingFactor = 1.25f;            // how fast waves scale
    int numZombies = baseCount + static_cast<int>(scalingFactor * std::pow(waveNumber, 1.5f));
    
    // Cap zombies so it doesn’t become overwhelming 
    numZombies = std::min(numZombies, maxZombiesSpawn);
    
    // std::cout << "Wave: " << waveNumber << ", Spawned: " << numZombies << " Zombies.\n";
    for (int i = 0; i < numZombies; ++i) {
        sf::Vector2f spawnLocation = getaRandomSpawnLocation(playerPos);
        zombieType type = getRandomZombieType();

        int minSpeed;
        int minHealth;
        int minDamage = 10;

        if (type == sprinter) {
            minSpeed = 120;
            minHealth = 20;
        } else if (type == tank) {
            minSpeed = 60;
            minHealth = 150;
            minDamage = 20;
        } else { // normal
            minSpeed = 90;
            minHealth = 50;
        }

        // Wave-based stat scaling
        float waveDifficulty = 1.0f + (waveNumber * 0.05f);  // +5% per wave
        float health = (minHealth + rand() % 100) * waveDifficulty;
        float speed  = (minSpeed + rand() % 50) * waveDifficulty;
        float damage = (minDamage + rand() % 10) * waveDifficulty;

        int score = static_cast<int>(health);

        // if (type == sprinter)
        //     std::cout << "Sprinter spawned --> Speed: " << speed << ", Health: " << health << "\n";
        // else if (type == tank)
        //     std::cout << "Tank spawned --> Speed: " << speed << ", Health: " << health << "\n";

        zombies.emplace_back(spawnLocation, health, speed, damage, score, type);
    }
    // std::cout << "____________________________________________\n";
}

zombieType ZombieManager::getRandomZombieType()
{
    int sprinterChance = std::min(10 + waveNumber * 2, 25);  // up to 25%
    int tankChance = std::min(10 + waveNumber, 25);          // up to 25%
    int normalChance = 100 - (sprinterChance + tankChance);

    int randTypeChance = rand() % 100;

    if (randTypeChance < normalChance)
        return normal;
    else if (randTypeChance < normalChance + sprinterChance)
        return sprinter;
    else
        return tank;
    
}

sf::Vector2f ZombieManager::getaRandomSpawnLocation(const sf::Vector2f& playerPos)
{
    const int windowWidth = windowSize.x;
    const int windowHeight = windowSize.y;

    // Margin outside the visible area
    const float spawnMarginMin = 400.f;
    const float spawnMarginMax = 1000.f + (waveNumber * 100.f);

    // Random side: 0=top, 1=bottom, 2=left, 3=right
    int side = rand() % 4;

    float x, y;
    float offset = spawnMarginMin + static_cast<float>(rand()) / RAND_MAX * (spawnMarginMax - spawnMarginMin);

    switch (side) {
        case 0: // Top
            x = playerPos.x - windowWidth / 2 + static_cast<float>(rand() % windowWidth);
            y = playerPos.y - windowHeight / 2 - offset;
            break;
        case 1: // Bottom
            x = playerPos.x - windowWidth / 2 + static_cast<float>(rand() % windowWidth);
            y = playerPos.y + windowHeight / 2 + offset;
            break;
        case 2: // Left
            x = playerPos.x - windowWidth / 2 - offset;
            y = playerPos.y - windowHeight / 2 + static_cast<float>(rand() % windowHeight);
            break;
        case 3: // Right
            x = playerPos.x + windowWidth / 2 + offset;
            y = playerPos.y - windowHeight / 2 + static_cast<float>(rand() % windowHeight);
            break;
    }
    return sf::Vector2f(x, y);
}
