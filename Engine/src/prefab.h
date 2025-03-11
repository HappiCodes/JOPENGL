#ifndef PREFAB_H
#define PREFAB_H

#include <glad/glad.h> 
#include"shader.h"

enum PrefabType
{
	CUBE
};

class Prefab
{
public:
    float *verts;
    glm::vec4 color = glm::vec4(1,1,1,1);
    GLuint VAO, VBO;
    int vertSize = 0;

    float cubeVertices[108] = {
        // Front Face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // Back Face
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,

         // Left Face
         -0.5f, -0.5f, -0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,
         -0.5f, -0.5f, -0.5f,
         -0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f, -0.5f,

         // Right Face
          0.5f, -0.5f, -0.5f,
          0.5f,  0.5f, -0.5f,
          0.5f,  0.5f,  0.5f,
          0.5f, -0.5f, -0.5f,
          0.5f,  0.5f,  0.5f,
          0.5f, -0.5f,  0.5f,

          // Top Face
          -0.5f,  0.5f, -0.5f,
          -0.5f,  0.5f,  0.5f,
           0.5f,  0.5f,  0.5f,
          -0.5f,  0.5f, -0.5f,
           0.5f,  0.5f,  0.5f,
           0.5f,  0.5f, -0.5f,

           // Bottom Face
           -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
           -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
           -0.5f, -0.5f,  0.5f
    };

	Prefab(PrefabType type)
	{
		switch (type)
		{
		case CUBE:
            verts = cubeVertices;
            vertSize = 36;
			break;
        default:
            verts = new float[0];
		}

        GenerateVAO();
	}

    // draws the prefab
    void Draw(Shader& shader)
    {
        shader.SetVector4f("color", color);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertSize);
    }

    void GenerateVAO()
    {
        // Delete previous VAO & VBO if they exist
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);

        // Create VAO & VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
};

#endif