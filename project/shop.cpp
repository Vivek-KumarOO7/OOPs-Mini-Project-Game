#include "Shop.h"
#include"Paths.h"
#include<memory>        // for unique pointer

Shop::Shop() : isOpen(false) {
    font.loadFromFile(font_path);

    if (!buySoundBuffer.loadFromFile(buy_sound_path)){
        throw std::runtime_error("Failed to load Buy Sound File.");
    }
    buySound.setBuffer(buySoundBuffer);
    buySound.setVolume(75.f);

    if (!cantBuySoundBuffer.loadFromFile(cant_buy_sound_path)){
        throw std::runtime_error("Failed to load Cant Buy Sound File.");
    }
    cantBuySound.setBuffer(cantBuySoundBuffer);
    cantBuySound.setVolume(50.f);

    if (!armorRechargeSoundBuffer.loadFromFile(armor_recharge_sound_path)){
        throw std::runtime_error("Failed to load Health Recharge Sound File.");
    }
    armorRechargeSound.setBuffer(armorRechargeSoundBuffer);
    armorRechargeSound.setVolume(50.f);
    
    if (!healSoundBuffer.loadFromFile(heal_sound_path)){
        throw std::runtime_error("Failed to load Health Recharge Sound File.");
    }
    healSound.setBuffer(healSoundBuffer);
    healSound.setVolume(50.f);

    titleText.setFont(font);
    titleText.setString("SHOP");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color::Yellow);
    
    panel.setSize(sf::Vector2f(420, 500));
    panel.setFillColor(sf::Color(0, 0, 0, 180));
    panel.setOutlineThickness(3);
    panel.setOutlineColor(sf::Color(255, 255, 0, 130));
}

void Shop::addItem(std::string name, std::string desc, int basePrice, int maxLevel, const std::string &texturePath)
{
    items.emplace_back(std::make_unique<ShopItem>(name, desc, basePrice, maxLevel, texturePath));
}

void Shop::toggle()
{
    isOpen = !isOpen;
}

void Shop::draw(sf::RenderWindow& window, const Player& player) {
    if (!isOpen) return;
    
    float y = 340;
    titleText.setPosition(70, y - 60);
    panel.setPosition(50, y - 70);
    panel.setSize(sf::Vector2f(420, 100 * items.size() - 20));
    window.draw(panel);
    window.draw(titleText);

    for (auto& item : items) {
        item->sprite.setPosition(70, y);
        window.draw(item->sprite);

        sf::Text nameText(item->name + " (" + std::to_string(item->getPrice()) + ")", font, 20);
        nameText.setPosition(150, y + 10);
        nameText.setFillColor(sf::Color::White);
        window.draw(nameText);

        if (item->maxLevel != 999){
            sf::Text lvlText("Lvl: " + std::to_string(item->currentLevel) + "/" + std::to_string(item->maxLevel), font, 18);
            lvlText.setPosition(150, y + 40);
            lvlText.setFillColor(sf::Color::Cyan);
            window.draw(lvlText);
        }

        item->bounds = sf::FloatRect(70, y, 300, 60);
        y += 80;
    }

    sf::Text coinsText("Coins: " + std::to_string(player.getCoins()), font, 22);
    coinsText.setFillColor(sf::Color::Green);
    coinsText.setPosition(70, y - 15);
    window.draw(coinsText);
}

void Shop::handleClick(sf::Vector2f mousePos, Player& player) {
    if (!isOpen) return;

    for (auto& item : items) {
        if (item->bounds.contains(mousePos)) {
            if (player.getCoins() >= item->getPrice() && item->canUpgrade()) {
                player.addCoins(-item->getPrice());
                item->currentLevel++;

                if (item->name == "Buy Magazine") {
                    player.addAmmo(30);
                    buySound.play();
                } else if (item->name == "Upgrade Ammunition") {
                    player.increaseDamage(5);
                    buySound.play();
                } else if (item->name == "Upgrade Fire Rate") {
                    player.increaseFireRate();
                    buySound.play();
                } else if (item->name == "Buy Armor") {
                    player.repairOrAddArmour(50.f);
                    buySound.play();
                    armorRechargeSound.play();
                } else if (item->name == "Health Pack") {
                    player.heal(50);
                    buySound.play();
                    healSound.play();
                } else if (item->name == "Land Mine"){
                    player.addLandmine();
                    buySound.play();
                }
                // std::cout << "Purchased: " << item->name << std::endl;
            } else{
                cantBuySound.play();
            }
        }
    }
}
