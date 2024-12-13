#include <iostream>
#include <memory>

#include "SceneManager.hpp"
#include "scenes/GameScene/GameScene.hpp"
#include "scenes/InstructionsScene/InstructionsScene.hpp"
#include "scenes/TitleScene/TitleScene.hpp"
#include <raylib.h>

std::unique_ptr<BaseScene> SceneManager::current_scene_instance_ = nullptr;
Scenes SceneManager::current_scene_ = Unset;

bool SceneManager::on_transition_ = false;
bool SceneManager::trans_fade_out_ = false;
float SceneManager::trans_alpha_ = 0.0f;
Scenes SceneManager::trans_from_scene_ = Unset;
Scenes SceneManager::trans_to_scene_ = Unset;

void SceneManager::initialize() { SceneManager::set_current_scene(Unset); }

void SceneManager::set_current_scene(Scenes scene) {
    current_scene_instance_.reset();
    current_scene_ = scene;

    switch (scene) {
    case Unset:
        // No scene loaded
        break;
    case Title:
        current_scene_instance_ = std::make_unique<TitleScene>();
        break;
    case Instructions:
        current_scene_instance_ = std::make_unique<InstructionsScene>();
        break;
    case Game:
        current_scene_instance_ = std::make_unique<GameScene>();
        break;
    case None:
        std::cerr << "Error: Attempted to set current scene to None."
                  << std::endl;
        break;
    }
}

void SceneManager::update(float dt) {
    if (on_transition_) {
        update_transition(dt);
    } else {
        if (current_scene_instance_ != nullptr) {
            SceneChangeRequest result = current_scene_instance_->update(dt);
            if (result.nextScene != None && result.nextScene != Unset) {
                if (result.changeType == SceneChangeType::Change) {
                    change_to_scene(result.nextScene);
                } else if (result.changeType == SceneChangeType::Transition) {
                    transition_to_scene(result.nextScene);
                }
            }
        }
    }
}

void SceneManager::draw() {
    if (current_scene_instance_ != nullptr) {
        current_scene_instance_->draw();
    }
}

void SceneManager::cleanup() {
    current_scene_instance_ = nullptr;
    current_scene_ = Unset;
}

void SceneManager::transition_to_scene(Scenes scene) {
    on_transition_ = true;
    trans_fade_out_ = false;
    trans_from_scene_ = current_scene_;
    trans_to_scene_ = scene;
    trans_alpha_ = 0.0f;
}

void SceneManager::change_to_scene(Scenes scene) {
    set_current_scene(scene);
    on_transition_ = false;
    trans_fade_out_ = false;
    trans_alpha_ = 0.0f;
    trans_from_scene_ = Unset;
    trans_to_scene_ = Unset;
}

void SceneManager::update_transition(float dt) {
    const float fade_speed = 0.02f;

    if (!trans_fade_out_) {
        trans_alpha_ += fade_speed;
        if (trans_alpha_ >= 1.0f) {
            trans_alpha_ = 1.0f;

            current_scene_instance_.reset();
            set_current_scene(trans_to_scene_);

            trans_fade_out_ = true;
        }
    } else {
        trans_alpha_ -= fade_speed;
        if (trans_alpha_ <= 0.0f) {
            trans_alpha_ = 0.0f;
            on_transition_ = false;
            trans_fade_out_ = false;
            trans_from_scene_ = Unset;
            trans_to_scene_ = Unset;
        }
    }
}

void SceneManager::draw_transition() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Fade(BLACK, trans_alpha_));
}

bool SceneManager::is_on_transition() { return on_transition_; }

Scenes SceneManager::get_current_scene() { return current_scene_; }
