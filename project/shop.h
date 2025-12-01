#ifndef SHOP_H
#define SHOP_H

#include<memory>        // for unique pointer
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "Player.h"

class ShopItem {
public:
    std::string name;
    std::string description;
    int basePrice;
    int maxLevel;
    int currentLevel;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::FloatRect bounds;

    ShopItem(std::string name, std::string desc, int basePrice, int maxLevel, const std::string& texturePath)
        : name(name), description(desc), basePrice(basePrice), maxLevel(maxLevel), currentLevel(0)
    {
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setScale(0.1f, 0.1f);
    }

    int getPrice() const {
        if (maxLevel == 999) return basePrice;
        else return basePrice + (basePrice * currentLevel) / 2;
    }

    bool canUpgrade() const {
        return currentLevel < maxLevel;
    }
};

class Shop {
private:
    std::vector<std::unique_ptr<ShopItem>> items;
    sf::Font font;
    sf::Text titleText;
    sf::RectangleShape panel;
    bool isOpen;

    // SFX
    sf::SoundBuffer buySoundBuffer;
    sf::Sound buySound;

    sf::SoundBuffer cantBuySoundBuffer;
    sf::Sound cantBuySound;

    sf::SoundBuffer armorRechargeSoundBuffer;
    sf::Sound armorRechargeSound;

    sf::SoundBuffer healSoundBuffer;
    sf::Sound healSound;

public:
    Shop();
    void addItem(std::string name, std::string desc, int basePrice, int maxLevel, const std::string& texturePath);
    void toggle();
    bool getIsOpen() const { return isOpen; }
    void draw(sf::RenderWindow& window, const Player& player);
    void handleClick(sf::Vector2f mousePos, Player& player);
};

#endif
