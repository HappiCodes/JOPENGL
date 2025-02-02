#include "game.h"

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{
    // Camera
    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    mouseLastXPos = Width / 2.0f;
    mouseLastYPos = Height / 2.0f;
}

void Game::Update(float dt)
{

}

void Game::ProcessInput(float dt)
{
    if(Keys[256]) glfwSetWindowShouldClose(Window, true);

    if (Keys[87])   camera.ProcessKeyboard(FORWARD, dt);
    if (Keys[83])   camera.ProcessKeyboard(BACKWARD, dt);
    if (Keys[65])   camera.ProcessKeyboard(LEFT, dt);
    if (Keys[68])   camera.ProcessKeyboard(RIGHT, dt);

    if (firstMouse)
    {
        mouseLastXPos = mouseXPos;
        mouseLastYPos = mouseYPos;
        firstMouse = false;
    }

    float xoffset = mouseXPos - mouseLastXPos;
    float yoffset = mouseLastYPos - mouseYPos; // reversed since y-coordinates go from bottom to top

    mouseLastXPos = mouseXPos;
    mouseLastYPos = mouseYPos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void Game::Render()
{
    // don't forget to enable shader before setting uniforms
    ResourceManager::GetShader("Shader").Use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)Width / (float)Height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    ResourceManager::GetShader("Shader").SetMatrix4("projection", projection);
    ResourceManager::GetShader("Shader").SetMatrix4("view", view);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
    ResourceManager::GetShader("Shader").SetMatrix4("model", model);
    Shader ourShader = ResourceManager::GetShader("Shader");
    ourModel->Draw(ourShader);
}