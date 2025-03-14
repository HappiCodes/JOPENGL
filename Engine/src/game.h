#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "resourceManager.h"
#include "model.h"
#include "prefab.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    bool                    OldKeys[1024];
    unsigned int            Width, Height;
    GLFWwindow*             Window;
    // Temp
    Model* ourModel = nullptr;
    Prefab* cubePrefab = nullptr;
    // Camera
    Camera camera;
    float mouseXPos, mouseLastXPos, mouseYPos, mouseLastYPos;
    bool firstMouse = true;

    // Renderer
    bool wireFrameDrawMode = false;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
};

#endif