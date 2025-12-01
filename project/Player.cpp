#include"Paths.h"
#include "Player.h"
#include"World.h"
// #include<iostream>

Player::Player(const sf::FloatRect& worldBounds, float minVisZone = 400.f) : minVisibleZone(minVisZone) {
    // Player Texture
    if(!texture.loadFromFile(player_texture_path)){
        throw std::runtime_error("Failed to load player texture.");
    }
    
    // sfx
    // Shot
    if (!gunshotBuffer.loadFromFile(akm_shot_sound_path)) {
        throw std::runtime_error("Failed to load gunshot sound.");
    }
    gunshotSound.setBuffer(gunshotBuffer);
    gunshotSound.setVolume(40.f); // adjust between 0–100
    // Gun click
    if (!cantFireSoundBuffer.loadFromFile(cant_fire_sound_path)) {
        throw std::runtime_error("Failed to load gunshot sound.");
    }
    cantFireSound.setBuffer(cantFireSoundBuffer);
    cantFireSound.setVolume(40.f); // adjust between 0–100
    
    // Explosion sound;
    if (!explosionBuffer.loadFromFile(landmine_explosion_sound_path)) {
        std::cerr << "Failed to load explosion sound" << std::endl;
    }
    explosionSound.setBuffer(explosionBuffer);
    explosionSound.setVolume(explosionVolume);

    // Heartbeat sound;
    if (!heartbeatSoundBuffer.loadFromFile(heartbeat_sound_path)) {
        std::cerr << "Failed to load Heartbeat sound" << std::endl;
    }
    heartbeatSound.setBuffer(heartbeatSoundBuffer);
    heartbeatSound.setVolume(100.f);
    
    // Player stats
    maxArmor = 50.f;
    armor = 0.f;
    maxHealth = 100.f;
    health = maxHealth;
    walkSpeed = 3.5f;
    sprintSpeed = 7.f;
    speed = walkSpeed;      // pixels per second
    isRunning = false;
    landmineInventory = 0;

    // reloading
    if (!gunreloadBuffer.loadFromFile(akm_reload_sound_path)) {
        throw std::runtime_error("Failed to load gunshot sound.");
    }
    gunreloadSound.setBuffer(gunreloadBuffer);
    gunreloadSound.setVolume(40.f); // adjust between 0–100

    sprite.setTexture(texture);

    // setting origin to center
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width/2 - 20, bounds.height/2);

    // setting initial postion: Spawning
    sprite.setPosition(worldBounds.width / 2, worldBounds.height / 2);

    // set scale of player
    sprite.setScale(0.65f, 0.65f);

    // Muzzle flash setup
    muzzleFlash.setOrigin(0.f, 2.f); // aligns flash from gun muzzle
    showMuzzleFlash = false;

    // Gun setup
    bulletDamage = 20;
    magazineSize = 30;
    ammoInMag = 30;
    totalAmmo = 60;
    isReloading = false;
    reloadTime = 2.75f;
    fireRate = 0.15f;
}

sf::Vector2f Player::getGunMuzzlePosition(float forwardOffset = 79.f, float sideOffset = 19.f) const {
    float angleRad = sprite.getRotation() * 3.14159265f / 180.f;
    sf::Vector2f pos = sprite.getPosition();
    return {
        pos.x + std::cos(angleRad) * forwardOffset - std::sin(angleRad) * sideOffset,
        pos.y + std::sin(angleRad) * forwardOffset + std::cos(angleRad) * sideOffset
    };
}

void Player::handleInput(float deltaTime)
{
    sf::Vector2f movement(0.f, 0.f);

    // ======================== Movement ===========================
    // check if the player is running or not
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
        isRunning = true;
        speed = sprintSpeed;
    } else{
        isRunning = false;
        speed = walkSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= speed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += speed * deltaTime;

    // Normalise diagonal movement to prevent faster motion
    if (movement.x != 0.f || movement.y != 0.f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement /= length;
    }
    movement *= speed;

    sprite.move(movement);

// ======================== Shooting ===========================
    static bool wasMousePressedLastFrame = false;
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if (isMousePressed) {
        if (!isReloading && !isRunning) {
            if (ammoInMag > 0 && shootClock.getElapsedTime().asSeconds() > fireRate) {
                sf::Vector2f pos = getGunMuzzlePosition();
                bullets.emplace_back(pos, sprite.getRotation());
                shootClock.restart();
                ammoInMag--;
                gunshotSound.play();

                // Muzzle flash
                muzzleFlash.setSize(sf::Vector2f(15.f + rand() % 5, 6.f));
                muzzleFlash.setFillColor(sf::Color(255, 205 + rand() % 50, 100));
                muzzleFlash.setPosition(pos);
                muzzleFlash.setRotation(sprite.getRotation());
                showMuzzleFlash = true;
                muzzleClock.restart();
            }
            // Empty mag — play click once per hold
            else if (ammoInMag == 0 && !wasMousePressedLastFrame) {
                cantFireSound.play();
            }
        }
    } else {
        // Reset when released
        wasMousePressedLastFrame = false;
    }
    // Update press state for next frame
    wasMousePressedLastFrame = isMousePressed;

// ========================== Reloading ===========================
// When pressing R
    if (((sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !isReloading && ammoInMag < magazineSize) || (!isReloading && ammoInMag == 0)) && totalAmmo) {
        isReloading = true;
        reloadClock.restart();
        // play reloading sound
        gunreloadSound.play();
    }
    
    // While reloading
    if (isReloading && reloadClock.getElapsedTime().asSeconds() >= reloadTime) {
        int spaceLeft = magazineSize - ammoInMag;
        if (totalAmmo >= spaceLeft){
            ammoInMag += spaceLeft;
            totalAmmo -= spaceLeft;
        } else {
            ammoInMag += totalAmmo;
            totalAmmo = 0;
        }
        isReloading = false;
    }

    // =================== Place Landmine ===================
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        // MODIFIED: Check inventory and cooldown
        if (landmineInventory > 0 && minePlaceClock.getElapsedTime().asSeconds() > mineCooldown) {
            Landmine mine;
            mine.placeMine(getPosition());
            landmines.push_back(mine);
            minePlaceClock.restart();
            
            landmineInventory--;
        }
    }
}

void Player::update(sf::RenderWindow &window, float deltaTime, const sf::FloatRect& worldBounds, const World& world)
{
    // Only play heartbeat once when the health drop bellow 40, reset only when health above 40
    if (health >= 40.f) isHeartbeatPlaying = false;
    if (!isHeartbeatPlaying && health < 40.f){
        isHeartbeatPlaying = true;
        heartbeatClock.restart();
        heartbeatSound.play();
    }
    if (isHeartbeatPlaying){
        float time = heartbeatClock.getElapsedTime().asSeconds();
        float percentage;
        if (time < heartBeatFadeTime){      // fade in
            percentage = std::min(time, heartBeatFadeTime) / heartBeatFadeTime;
        } else if (time >= heartbeatSoundDuration - heartBeatFadeTime){     // fade out
            percentage = std::max(0.f, heartbeatSoundDuration - time) / heartBeatFadeTime;
        } else{     // stable
            percentage = 1;
        }
        heartbeatSound.setVolume(maxHeartbeatVolume * percentage);
    }
    
// ====================== Movement and Rotation ========================
    // getting mouse position relative to the game window
    sf::Vector2f mousePos = (sf::Vector2f)window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f playerPos = sprite.getPosition();
    
    // calculate the angle in radians and converting to degrees
    float dx = mousePos.x - playerPos.x;
    float dy = mousePos.y - playerPos.y;

    float angle = atan2(dy, dx) * 180 / 3.14159f;   // final angle in Degrees
    
    // Rotating player;
    float brightness = world.getCurrentBrightness();
    float da = 0.0f;
    if (brightness < 0.5f) da = 3.15f;
    else da = 2.5f;
    sprite.setRotation(angle - da);
    // std::cout << da << '\n';

    // ====================== Clamp to World Bounds =========================
    sf::Vector2f pos = sprite.getPosition();
    sf::FloatRect bounds = sprite.getGlobalBounds();

    // Ensure the player's sprite stays fully inside the world
    if (pos.x - bounds.width / 2.f < worldBounds.left)
        pos.x = worldBounds.left + bounds.width / 2.f;
    if (pos.y - bounds.height / 2.f < worldBounds.top)
        pos.y = worldBounds.top + bounds.height / 2.f;
    if (pos.x + bounds.width / 2.f > worldBounds.left + worldBounds.width)
        pos.x = worldBounds.left + worldBounds.width - bounds.width / 2.f;
    if (pos.y + bounds.height / 2.f > worldBounds.top + worldBounds.height)
        pos.y = worldBounds.top + worldBounds.height - bounds.height / 2.f;

    sprite.setPosition(pos);

    // =============== Update all bullets ====================
    for (auto& b : bullets){
        b.update(deltaTime);
    }
    
    // Remove bullet
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [&](Bullet& b) {return !b.isAlive(window);}), bullets.end());


    // ====================== Muzzle Flash =========================
    if (showMuzzleFlash && muzzleClock.getElapsedTime().asSeconds() > muzzleFlashDuration) {
        showMuzzleFlash = false;
    }

    // Updating mines
    for (auto& mine : landmines){
        mine.update(deltaTime);
    }

    // Remove exploded mines that finished animation
    landmines.erase(std::remove_if(landmines.begin(), landmines.end(),
        [](const Landmine& m){ return m.hasExploded(); }), landmines.end());
   
}

void Player::draw(sf::RenderWindow& window) {
    // Bullets
    for (auto& b : bullets){
        b.draw(window);
    }

    // Landmines
    if (!landmines.empty()){
        for (auto& mine : landmines){
            if (mine.isActiveNow()){        // Player under explosion
                mine.draw(window);
                // Player
                window.draw(sprite);    
            } else{                         // Player above mine
                window.draw(sprite);
                mine.draw(window);
            }
        }
    } else{
        window.draw(sprite);
    }
    
    // Display muzzle flash
    if (showMuzzleFlash){
        window.draw(muzzleFlash);
    }
}

void Player::takeDamage(float amount)
{
    if (isAlive) {
        if (armor > 0.f){
            armor -= amount * 0.67f;
            health -= amount * 0.33f;
            if (armor <= 0.f) armor = 0.f;
        } else{
            health -= amount;
        }
        if (health <= 0.f) {
            health = 0.f;
            isAlive = false;
        }
    }
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}