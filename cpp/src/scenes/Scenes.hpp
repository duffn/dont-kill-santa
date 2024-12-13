#pragma once

#pragma once

enum Scenes {
    None,
    Unset,
    Title,
    Instructions,
    Game,
};

enum class SceneChangeType {
    None,      // No scene change requested
    Change,    // Directly change the scene, no transition
    Transition // Use a fade transition
};

struct SceneChangeRequest {
    Scenes nextScene;
    SceneChangeType changeType;
};
