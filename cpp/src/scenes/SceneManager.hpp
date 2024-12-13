#pragma once

#include "BaseScene.hpp"
#include "Scenes.hpp"

#include <memory>

class SceneManager {
  private:
    static std::unique_ptr<BaseScene> current_scene_instance_;
    static Scenes current_scene_;

    // Transition control
    static bool on_transition_;
    static bool trans_fade_out_;
    static float trans_alpha_;
    static Scenes trans_from_scene_;
    static Scenes trans_to_scene_;

    static void transition_to_scene(Scenes scene);
    static void change_to_scene(Scenes scene);
    static void update_transition(float dt);

    static Scenes get_current_scene();

  public:
    static void initialize();
    static void set_current_scene(Scenes scene);
    static void update(float dt);
    static void draw();
    static void cleanup();

    static bool is_on_transition();
    static void draw_transition();
};
