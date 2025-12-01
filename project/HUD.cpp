#include "HUD.h"

HUD::HUD(std::string pathToFont)
{
    // font
    if (!font.loadFromFile(pathToFont)) {  // path to your font file
        throw std::runtime_error("Failed to load font.");
    }
    text.setFont(font);
}

void HUD::draw(sf::RenderWindow &window, Player &player, World &world, ZombieManager& zombieManager)
{
    int y = 10;
    // =============== Displaying Player stats =================
    // Health
    sf::Color white(200, 200, 200, 255);
    sf::Color yellow(232, 197, 21, 255);
    sf::Color red(232, 21, 21, 255);
    sf::Color orange(232, 122, 23, 255);

    text.Bold;
    text.setCharacterSize(24);
    text.setOutlineThickness(1.f);

    writeText(window, 10, y, "Health: ", white);
    
    // printing: "90"/100
    int playerHealth = player.getHealth().x;
    if (playerHealth == 100) text.setPosition(110, y);
    else text.setPosition(120, y);
    if (playerHealth <= 40) text.setFillColor(red);
    text.setString(std::to_string(playerHealth));
    window.draw(text);
    
    // printing: /100
    writeText(window, 150, y, ("/" + std::to_string(player.getHealth().y)), white);
    

    writeText(window, 240, y, "Armor: ", white);
    // printing: "30"/50
    float armor = player.getArmor().x;
    if (armor == 0) text.setPosition(340, y);
    else text.setPosition(330, y);
    if (armor <= 25) text.setFillColor(orange);
    text.setString(std::to_string(player.getArmor().x));
    window.draw(text);

    // printing: /50
    writeText(window, 355, y, ("/" + std::to_string(player.getArmor().y)), white);
    
    // Displaying ammos left in Mag
    y += 30;
    if(player.getReloadStatus()){        // update ammo text
        writeText(window, 10, y, "Mag: ", white);
        writeText(window, 80, y, "Reloading...", yellow);
    } else{
        writeText(window, 10, y, ("Mag: " + std::to_string(player.getAmmoStatus().x) + "/" + std::to_string(player.getAmmoStatus().y)), white);
    }
    
    // Displaying total Ammos left
    y += 30;
    writeText(window, 10, y, "Ammo: ", white);
    
    text.setPosition(90, y);
    int totalAmmo = player.getTotalAmmo();
    if (totalAmmo <= 30) {
        text.setFillColor(red);
    } else if (totalAmmo < 60){
        text.setFillColor(orange);
    }
    text.setString(std::to_string(totalAmmo));
    window.draw(text);

    // Num of Landmines
    y += 30;
    writeText(window, 10, y, ("Landmines: " + std::to_string(player.getLandmineCount())), white);
    
    // Coins
    y += 30;
    writeText(window, 10, y, ("Coins: " + std::to_string(player.getCoins())), white);
    
    // Zombies Slain
    y += 30;
    writeText(window, 10, y, ("Zombies Killed: " + std::to_string(player.getZombiesKilled())), white);
    
    
    // ============== World Stats ================
    writeText(window, 1320, 10, ("Time: " + std::to_string((int)(world.getTimeOfDay())) + " hrs"), white);
    writeText(window, 1320, 35, ("Nights Survived: " + std::to_string(world.getNightSurvived())), white);

    // Wave Status;
    writeText(window, window.getSize().x / 2 - 125, window.getSize().y - 50, ("Next Wave In: " + std::to_string(zombieManager.getTimeTillNextWave())), white);
    
    // Score Status;
    writeText(window, window.getSize().x / 2 - 80, 10, ("Score: " + std::to_string(player.getScore())), white);
    writeText(window, window.getSize().x / 2 - 70, 40, ("Wave: " + std::to_string(zombieManager.getWaveNumber() - 1)), white);
}

void HUD::writeText(sf::RenderWindow &window, int x, int y, std::string str, sf::Color &color)
{
    text.setFillColor(color);
    text.setPosition(x, y);
    text.setString(str);
    window.draw(text);
}
