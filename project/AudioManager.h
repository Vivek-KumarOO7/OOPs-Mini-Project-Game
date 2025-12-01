#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <random>

class AudioManager {
public:
    AudioManager();
    void loadMusic();
    void playMenuMusic();
    void playGameMusic();
    void playGameOverMusic();
    void update(float deltaTime);

private:
    void startFadeOut();
    void startFadeIn();
    void playNextGameTrack();

    sf::Music music;
    std::string menuMusicPath;
    std::vector<std::string> gameMusicPaths;

    enum class FadeState { None, In, Out };
    FadeState fadeState;
    sf::Clock fadeClock;
    float fadeDuration;
    float targetVolume;

    bool isGamePlaylist;
    std::string nextTrackPath;

    // For random shuffling
    std::mt19937 rng;
};

#endif