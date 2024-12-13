#pragma once

#include <raylib.h>
#include <string>

class AudioManager {
  private:
    AudioManager() = delete;

    static bool initialized_;
    static Music music_;

  public:
    static void initialize(const std::string &musicFilePath,
                           float volume = 0.5f);
    static void update();
    static void cleanup();
    static void set_volume(float volume);
    static void pause();
    static void resume();
};
