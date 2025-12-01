#include "AudioManager.h"
#include"Paths.h"
#include <iostream>

// Paths to the music files
const std::string MENU_MUSIC_PATH = mainmenu_music_path;
const std::vector<std::string> GAME_MUSIC_PATHS = {
    bgm_0_path,
    bgm_1_path,
    bgm_2_path,
    bgm_3_path,
    bgm_4_path
};

AudioManager::AudioManager()
    : fadeState(FadeState::None),
      fadeDuration(2.0f), // 2-second fade
      targetVolume(20.0f), // maxVolume
      isGamePlaylist(false),
      rng(std::random_device{}()) // Initialize random number generator
{
    music.setVolume(0.f); 
}

void AudioManager::loadMusic() {
    menuMusicPath = MENU_MUSIC_PATH;
    gameMusicPaths = GAME_MUSIC_PATHS;

    // Shuffle the playlist once at the start
    std::shuffle(gameMusicPaths.begin(), gameMusicPaths.end(), rng);
}

void AudioManager::playMenuMusic() {
    isGamePlaylist = false;
    if (music.getStatus() == sf::Music::Playing && nextTrackPath == menuMusicPath) {
        return; // Already playing or fading to this
    }
    nextTrackPath = menuMusicPath;
    startFadeOut();
}

void AudioManager::playGameMusic() {
    isGamePlaylist = true;
    playNextGameTrack();
}

void AudioManager::playGameOverMusic() {
    // Same as Main menu music
    playMenuMusic();
}

void AudioManager::playNextGameTrack() {
    // Get the next track, if we're at the end, reshuffle
    static int currentTrack = 0;
    if (currentTrack >= gameMusicPaths.size()) {
        currentTrack = 0;
        std::shuffle(gameMusicPaths.begin(), gameMusicPaths.end(), rng);
    }
    
    if (music.getStatus() == sf::Music::Playing && nextTrackPath == gameMusicPaths[currentTrack]) {
        return; // Already playing or fading to this
    }

    nextTrackPath = gameMusicPaths[currentTrack];
    currentTrack++;
    startFadeOut();
}

void AudioManager::startFadeOut() {
    if (music.getStatus() == sf::Music::Playing) {
        fadeState = FadeState::Out;
        fadeClock.restart();
    } else {
        // If nothing is playing, just fade in the new track
        startFadeIn();
    }
}

void AudioManager::startFadeIn() {
    if (!music.openFromFile(nextTrackPath)) {
        std::cerr << "Error loading music: " << nextTrackPath << std::endl;
        return;
    }
    music.setLoop(isGamePlaylist ? false : true); // Loop menu, not game tracks
    music.play();
    fadeState = FadeState::In;
    fadeClock.restart();
}

void AudioManager::update(float deltaTime) {
    // 1. Handle Fading
    float elapsed = fadeClock.getElapsedTime().asSeconds();

    if (fadeState == FadeState::Out) {
        if (elapsed >= fadeDuration) {
            music.stop();
            music.setVolume(0.f);
            startFadeIn(); // Start fading in the *next* track
        } else {
            float volume = targetVolume * (1.f - (elapsed / fadeDuration));
            music.setVolume(volume);
        }
    } else if (fadeState == FadeState::In) {
        if (elapsed >= fadeDuration) {
            music.setVolume(targetVolume);
            fadeState = FadeState::None;
        } else {
            float volume = targetVolume * (elapsed / fadeDuration);
            music.setVolume(volume);
        }
    }

    // 2. Handle Game Playlist
    if (isGamePlaylist && 
        fadeState == FadeState::None && 
        music.getStatus() == sf::Music::Stopped)
    {
        // If a game track finished, play the next one
        playNextGameTrack();
    }
}