#include "game.h"


float morphFactor = 0.0f;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>


const int SUBDIVISIONS = 64;

std::vector<glm::vec3> cubeVertices;
std::vector<glm::vec3> originalCubeVertices;
std::vector<glm::vec3> colors;
std::vector<unsigned int> indices;

void generateSubdividedCube(int subdivisions) {
    cubeVertices.clear();
    originalCubeVertices.clear();
    colors.clear();
    indices.clear();

    for (int face = 0; face < 6; ++face) {
        glm::vec3 u, v, origin;
        switch (face) {
        case 0: origin = { -1, -1, -1 }; u = { 2, 0, 0 }; v = { 0, 2, 0 }; break;
        case 1: origin = { 1, -1, -1 }; u = { 0, 0, 2 }; v = { 0, 2, 0 }; break;
        case 2: origin = { 1, -1, 1 }; u = { -2, 0, 0 }; v = { 0, 2, 0 }; break;
        case 3: origin = { -1, -1, 1 }; u = { 0, 0, -2 }; v = { 0, 2, 0 }; break;
        case 4: origin = { -1, 1, -1 }; u = { 2, 0, 0 }; v = { 0, 0, 2 }; break;
        case 5: origin = { -1, -1, 1 }; u = { 2, 0, 0 }; v = { 0, 0, -2 }; break;
        }

        for (int i = 0; i <= subdivisions; ++i) {
            for (int j = 0; j <= subdivisions; ++j) {
                glm::vec3 point = origin + (u * (float(i) / subdivisions)) + (v * (float(j) / subdivisions));
                cubeVertices.push_back(point);
                originalCubeVertices.push_back(point);
                colors.push_back(glm::vec3((point.x + 1) * 0.5f, (point.y + 1) * 0.5f, (point.z + 1) * 0.5f));
            }
        }

        int rowSize = subdivisions + 1;
        int startIdx = face * rowSize * rowSize;
        for (int i = 0; i < subdivisions; ++i) {
            for (int j = 0; j < subdivisions; ++j) {
                int idx0 = startIdx + i * rowSize + j;
                int idx1 = idx0 + 1;
                int idx2 = idx0 + rowSize;
                int idx3 = idx2 + 1;
                indices.push_back(idx0); indices.push_back(idx1); indices.push_back(idx2);
                indices.push_back(idx1); indices.push_back(idx3); indices.push_back(idx2);
            }
        }
    }
}

// Function to morph cube to sphere
void morphToSphere(std::vector<glm::vec3>& vertices, float factor) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3 normalized = glm::normalize(originalCubeVertices[i]);
        vertices[i] = glm::mix(originalCubeVertices[i], normalized, factor);
    }
}

unsigned int VAO, VBO, CBO, EBO;

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

    cubePrefab = new Prefab(PrefabType::CUBE);

    generateSubdividedCube(SUBDIVISIONS);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(glm::vec3), cubeVertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

}

void Game::Update(float dt)
{
    morphToSphere(cubeVertices, morphFactor);
}

void Game::ProcessInput(float dt)
{
    if (Keys[265]) morphFactor = std::min(morphFactor + 0.01f, 1.0f);
    if (Keys[264]) morphFactor = std::max(morphFactor - 0.01f, 0.0f);

    if(Keys[256]) glfwSetWindowShouldClose(Window, true);

    if (Keys[87])   camera.ProcessKeyboard(FORWARD, dt);
    if (Keys[83])   camera.ProcessKeyboard(BACKWARD, dt);
    if (Keys[65])   camera.ProcessKeyboard(LEFT, dt);
    if (Keys[68])   camera.ProcessKeyboard(RIGHT, dt);
    if (Keys[32])   camera.ProcessKeyboard(UP, dt);
    if (Keys[340])   camera.ProcessKeyboard(DOWN, dt);

    if (Keys[86] && !OldKeys[86])
    {
        wireFrameDrawMode = !wireFrameDrawMode;
        glPolygonMode(GL_FRONT_AND_BACK, wireFrameDrawMode ? GL_LINE : GL_FILL);
    }

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

    for (int i = 0; i < 1024; i++) OldKeys[i] = Keys[i];
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
    //ourModel->Draw(ourShader);



    

    

    // Matrices (Simple Projection)
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	// it's a bit too big for our scene, so scale it down

    ResourceManager::GetShader("Prefab").Use();
    ResourceManager::GetShader("Prefab").SetMatrix4("projection", projection);
    ResourceManager::GetShader("Prefab").SetMatrix4("view", view);
    ResourceManager::GetShader("Prefab").SetMatrix4("model", model);
    Shader prefabShader = ResourceManager::GetShader("Prefab");

    //cubePrefab->Draw(prefabShader);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, cubeVertices.size() * sizeof(glm::vec3), cubeVertices.data());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}