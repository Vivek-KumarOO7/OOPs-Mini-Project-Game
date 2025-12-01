// --- Includes ---
#include "shop.h"
#include "Player.h"
#include "World.h"
#include "LightOverlay.h"
#include "HUD.h"
#include "Zombie.h"
#include "Paths.h"
#include "Crosshair.h"
#include "Landmine.h"
#include "AudioManager.h" 
#include "ParticleSystem.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

// --- Game State Enum ---
enum class GameState {
    MainMenu,
    InGame,
    GameOver
};

// --- High Score Functions ---
int loadHighScore() {
    int score = 0;
    std::ifstream file(high_score_file_path);
    if (file.is_open()) {
        file >> score;
        file.close();
    }
    return score;
}

void saveHighScore(int newScore, int& currentHighScore) {
    if (newScore > currentHighScore) {
        currentHighScore = newScore;
        std::ofstream file(high_score_file_path);
        if (file.is_open()) {
            file << currentHighScore;
            file.close();
        }
    }
}

// --- Helper: Text Drawing ---
void drawText(sf::RenderWindow& window, const std::string& str, const sf::Font& font, int size, sf::Vector2f pos, sf::Color color, bool centered = false) {
    sf::Text text(str, font, size);
    text.setFillColor(color);
    if (centered) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    }
    text.setPosition(pos);
    window.draw(text);
}

// =================================================================
// --- STATE 1: IN-GAME (Your original main function) ---
// =================================================================
// Returns the player's score when they die
GameState runGame(sf::RenderWindow& window, sf::Font& font, AudioManager& audio, int& outScore) {
    // ===== World Setup =====
    float worldHeight = 4000.f;
    float worldWidth = 4000.f;
    float dayLength = 120.f;
    float dayStartTime = 7.f;
    World world(worldWidth, worldHeight, dayStartTime, dayLength);
    sf::FloatRect worldBounds(0.f, 0.f, worldWidth, worldHeight);
    
    // ===== Player Setup =====
    float minVisZone = 625.f;
    Player player(worldBounds, minVisZone);
    
    Crosshair crosshair;
    LightOverlay lightOverlay(window.getSize().x, window.getSize().y, player);

    ZombieManager zombieManager({1600, 900});
    ParticleSystem particleSystem;

    // ===== Camera Setup =====
    sf::View view(sf::FloatRect(0.f, 0.f, 1600.f, 900.f));
    view.setCenter(player.getPosition());

    // Hud setup;
    HUD hud(font_path);

    // Shop setup
    Shop shop;
    shop.addItem("Buy Magazine", "Adds 30 bullets", 40, 999, icon_magazine);
    shop.addItem("Upgrade Ammunition", "Increase damage", 75, 5, icon_ammunition);
    shop.addItem("Upgrade Fire Rate", "Shoot faster", 60, 3, icon_fire_rate);
    shop.addItem("Buy Armor", "Adds or repairs armour", 100, 999, icon_vest_blue);
    shop.addItem("Health Pack", "Restores 50 health", 75, 999, icon_health_pack);
    shop.addItem("Land Mine", "Buy a land mine", 200, 999, icon_landmine);

    sf::Clock clock;

    // --- Start Game Music ---
    audio.playGameMusic();

    // --- PAUSE MENU SETUP ---
    bool isPaused = false;

    // Semi-transparent overlay
    sf::RectangleShape pauseOverlay;
    pauseOverlay.setSize((sf::Vector2f)window.getSize());
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150)); // Black with 150/255 alpha

    // Pause menu buttons
    sf::Text resumeButton("RESUME", font, 50);
    resumeButton.setFillColor(sf::Color::White);
    resumeButton.setOrigin(resumeButton.getLocalBounds().width / 2, resumeButton.getLocalBounds().height / 2);
    resumeButton.setPosition(window.getSize().x / 2.f, 350.f);

    sf::Text restartButton("RESTART", font, 50);
    restartButton.setFillColor(sf::Color::White);
    restartButton.setOrigin(restartButton.getLocalBounds().width / 2, restartButton.getLocalBounds().height / 2);
    restartButton.setPosition(window.getSize().x / 2.f, 450.f);

    sf::Text exitButton("MAIN MENU", font, 50);
    exitButton.setFillColor(sf::Color::White);
    exitButton.setOrigin(exitButton.getLocalBounds().width / 2, exitButton.getLocalBounds().height / 2);
    exitButton.setPosition(window.getSize().x / 2.f, 550.f);
    // --- END PAUSE MENU SETUP ---

    // --- MAIN GAME LOOP ---
    while (window.isOpen()) {
        // --- Get DeltaTime ---
        float deltaTime = clock.restart().asSeconds();
        
        // --- Update Audio ---
        audio.update(deltaTime);
        
        // --- Update Crosshair (uses screen space, safe to call anytime) ---
        crosshair.update(sf::Mouse::getPosition(window));

        // --- Update Camera Logic ---
        // 1. Update camera position based on player
        sf::Vector2f playerPos = player.getPosition();
        sf::Vector2f currentCenter = view.getCenter();
        float smoothSpeed = 2.0f;
        sf::Vector2f newCenter = currentCenter + (playerPos - currentCenter) * smoothSpeed * deltaTime;
        sf::Vector2f halfSize = view.getSize() / 2.f;
        newCenter.x = std::max(halfSize.x, std::min(newCenter.x, world.getSize().x - halfSize.x));
        newCenter.y = std::max(halfSize.y, std::min(newCenter.y, world.getSize().y - halfSize.y));
        view.setCenter(newCenter);

        // 2. SET THE WINDOW'S VIEW
        window.setView(view);

        // Get mouse pos in screen space for pause menu
        sf::Vector2f mouseScreenPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());

        // --- Handle Events (Poll after setting view) ---
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // --- PAUSE/UNPAUSE ---
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                isPaused = !isPaused; // Toggle pause
                window.setMouseCursorVisible(isPaused); // Show/hide mouse
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
                shop.toggle();
            }

            // --- MOUSE CLICKS ---
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (isPaused) {
                    // --- Handle Pause Menu Clicks ---
                    if (resumeButton.getGlobalBounds().contains(mouseScreenPos)) {
                        isPaused = false;
                        window.setMouseCursorVisible(false);
                    }
                    if (restartButton.getGlobalBounds().contains(mouseScreenPos)) {
                        return GameState::InGame; // Tell main() to restart
                    }
                    if (exitButton.getGlobalBounds().contains(mouseScreenPos)) {
                        return GameState::MainMenu; // Tell main() to go to menu
                    }
                } else {
                    // --- Handle Shop Clicks (if not paused) ---
                    if (shop.getIsOpen()) {
                        sf::Vector2f mouseShopPos = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
                        shop.handleClick(mouseShopPos, player);
                    }
                }
            }
        }

        // --- Update Game Logic ---
        // --- Update Game Logic (ONLY if not paused) ---
        if (!isPaused) {
            player.handleInput(deltaTime);
            world.update(deltaTime, player);
            zombieManager.update(deltaTime, player, world.getCurrentBrightness(), particleSystem);
            player.update(window, deltaTime, worldBounds, world);
            particleSystem.update(deltaTime);
        } else {
            // --- Update Pause Menu (Hover effects) ---
            if (resumeButton.getGlobalBounds().contains(mouseScreenPos))
                resumeButton.setFillColor(sf::Color::Yellow);
            else
                resumeButton.setFillColor(sf::Color::White);

            if (restartButton.getGlobalBounds().contains(mouseScreenPos))
                restartButton.setFillColor(sf::Color::Yellow);
            else
                restartButton.setFillColor(sf::Color::White);

            if (exitButton.getGlobalBounds().contains(mouseScreenPos))
                exitButton.setFillColor(sf::Color::Yellow);
            else
                exitButton.setFillColor(sf::Color::White);
        }

        // --- Check for Game Over (ONLY if not paused) ---
        if (!isPaused && !player.getAliveStatus()) {
            outScore = player.getScore();
            return GameState::GameOver; 
        }
        
        // ======== Draw Game World ========
        window.clear();
        world.draw(window);
        particleSystem.draw(window);
        zombieManager.draw(window);
        player.draw(window);
        
        float brightness = world.getCurrentBrightness();
        lightOverlay.update(view, brightness, player);
        lightOverlay.draw(window);
        
        // === Draw HUD (Screen Space) ===
        // Switch back to the default view for the HUD
        window.setView(window.getDefaultView()); 
        hud.draw(window, player, world, zombieManager);
        shop.draw(window, player);

        // --- Draw Pause Menu (ONLY if paused) ---
        if (isPaused) {
            window.draw(pauseOverlay);
            drawText(window, "PAUSED", font, 80, {window.getSize().x / 2.f, 200.f}, sf::Color::White, true);
            window.draw(resumeButton);
            window.draw(restartButton);
            window.draw(exitButton);
        }
        
        // Crosshair is drawn last (unless paused)
        if (!isPaused) {
            crosshair.draw(window);
        }
        
        // --- Display ---
        window.display();
    }
    return GameState::MainMenu; // Should not be reached
}

// =================================================================
// --- STATE 2: MAIN MENU ---
// =================================================================
void runMainMenu(sf::RenderWindow& window, sf::Font& font, int highScore, GameState& currentState, AudioManager& audio) {
    sf::Text playButton("PLAY", font, 60);
    playButton.setFillColor(sf::Color::White);
    playButton.setOrigin(playButton.getLocalBounds().width / 2, playButton.getLocalBounds().height / 2);
    playButton.setPosition(window.getSize().x / 2.f, 400.f);

    audio.playMenuMusic(); // Tell audio manager to play menu music

    sf::Clock clock;
    while (window.isOpen() && currentState == GameState::MainMenu) {
        float deltaTime = clock.restart().asSeconds();
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Check for Play button click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (playButton.getGlobalBounds().contains(mousePos)) {
                    currentState = GameState::InGame; // Change state!
                }
            }
        }

        // --- Update ---
        audio.update(deltaTime); // Keep audio fading
        
        // Button hover effect
        if (playButton.getGlobalBounds().contains(mousePos)) {
            playButton.setFillColor(sf::Color::Yellow);
        } else {
            playButton.setFillColor(sf::Color::White);
        }

        // --- Draw ---
        window.clear(sf::Color(10, 10, 20)); // Dark blue background
        drawText(window, "ASHFALL: NIGHT OF THE DEAD", font, 70, {window.getSize().x / 2.f, 150.f}, sf::Color::Red, true);
        drawText(window, "HIGH SCORE: " + std::to_string(highScore), font, 30, {window.getSize().x / 2.f, 250.f}, sf::Color::Cyan, true);
        
        window.draw(playButton);

        // Instructions
        drawText(window, "INSTRUCTIONS", font, 24, {window.getSize().x / 2.f, 550.f}, sf::Color::White, true);
        drawText(window, "WASD: Move\nLeft Shift: Sprint(Can not shoot while Sprinting)\nMouse: Aim\nLeft Click: Shoot\nRight Click: Place Mine\nR: Reload\nB: Open Shop", font, 20, {window.getSize().x / 2.f - 150.f, 600.f}, sf::Color(200, 200, 200), false);

        window.display();
    }
}

// =================================================================
// --- STATE 3: GAME OVER ---
// =================================================================
void runGameOver(sf::RenderWindow& window, sf::Font& font, int highScore, int lastScore, GameState& currentState, AudioManager& audio) {
    window.setView(window.getDefaultView());
    
    sf::Text playAgainButton("PLAY AGAIN", font, 60);
    playAgainButton.setFillColor(sf::Color::White);
    playAgainButton.setOrigin(playAgainButton.getLocalBounds().width / 2, playAgainButton.getLocalBounds().height / 2);
    playAgainButton.setPosition(window.getSize().x / 2.f, 600.f);
    
    audio.playGameOverMusic(); // Tell audio manager to fade to menu music

    sf::Clock clock;
    while (window.isOpen() && currentState == GameState::GameOver) {
        float deltaTime = clock.restart().asSeconds();
        sf::Vector2f mousePos = (sf::Vector2f)sf::Mouse::getPosition(window);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Check for Play Again button click
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (playAgainButton.getGlobalBounds().contains(mousePos)) {
                    currentState = GameState::InGame; // Change state!
                }
            }
        }

        // --- Update ---
        audio.update(deltaTime); // Keep audio fading
        
        // Button hover effect
        if (playAgainButton.getGlobalBounds().contains(mousePos)) {
            playAgainButton.setFillColor(sf::Color::Yellow);
        } else {
            playAgainButton.setFillColor(sf::Color::White);
        }

        // --- Draw ---
        window.clear(sf::Color(20, 10, 10, 150)); // Dark red background
        drawText(window, "YOU DIED", font, 120, {window.getSize().x / 2.f, 200.f}, sf::Color::Red, true);
        drawText(window, "Your Score: " + std::to_string(lastScore), font, 40, {window.getSize().x / 2.f, 350.f}, sf::Color::White, true);
        drawText(window, "High Score: " + std::to_string(highScore), font, 40, {window.getSize().x / 2.f, 420.f}, sf::Color::Cyan, true);
        
        window.draw(playAgainButton);
        window.display();
    }
}

// =================================================================
// --- MAIN FUNCTION ---
// =================================================================
int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Ashfall: Night of the dead");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true); // Show mouse on menus

    // --- Load Global Assets ---
    sf::Font font;
    if (!font.loadFromFile(font_path)) {
        throw std::runtime_error("Failed to load font!");
    }
    
    // Load Landmine assets ONCE
    if (!Landmine::loadAssets()) {
        throw std::runtime_error("Failed to Load Landmine assets.");
    }

    AudioManager audio;
    audio.loadMusic();

    // --- Game State Variables ---
    int highScore = loadHighScore();
    int lastScore = 0;
    GameState currentState = GameState::MainMenu;

    // --- MASTER GAME LOOP ---
    while (window.isOpen()) {
        switch (currentState) {
            case GameState::MainMenu:
                window.setMouseCursorVisible(true);
                runMainMenu(window, font, highScore, currentState, audio);
                break;
            
            case GameState::InGame: {
                window.setMouseCursorVisible(false); // Hide mouse for gameplay
                    
                    // runGame now returns the *next* state
                    GameState nextState = runGame(window, font, audio, lastScore);
                    
                    if (nextState == GameState::GameOver) {
                        saveHighScore(lastScore, highScore);
                    }
                    
                    currentState = nextState; // Go to the new state
                    break;        
            }
                
            case GameState::GameOver:
                window.setMouseCursorVisible(true);
                runGameOver(window, font, highScore, lastScore, currentState, audio);
                break;
        }
    }
    return 0;
}