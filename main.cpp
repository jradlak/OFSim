#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Window.h"
#include "Sphere.h"

#include <iostream>
#include <vector>

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
    Window mainWindow(camera, SCR_WIDTH, SCR_HEIGHT);
    int result = mainWindow.initialize();
    if (result != 0)
    {
        return result;
    }

    // build and compile our shader programs
    // ------------------------------------
    Shader sphereShader("shader_vs.glsl", "shader_fs.glsl");
    Shader lightSourceShader("light_source_vs.glsl", "light_source_fs.glsl");

    Sphere sphere(0.5f, 32, 16);

    std::vector<float> vert = sphere.getVertices();
    float vertices[3366];
    std::copy(vert.begin(), vert.end(), vertices);

    std::vector<int> ind = sphere.getIndices();
    int indices[2880];
    int indCount = ind.size();
    std::copy(ind.begin(), ind.end(), indices);

    // world space positions of our spheres
    glm::vec3 spherePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
    };

    // lighting source position:
    glm::vec3 lightPos(2.0f,  5.0f, -15.0f);

    //sphere VAO:
    unsigned int VBO, sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //position attribute:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //normal attribute:
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //light source VAO:
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // draw only lines
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!mainWindow.shouldClose())
    {
        // input
        // -----
        mainWindow.processInput();

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        sphereShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        sphereShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        sphereShader.setMat4("view", view);

        sphereShader.use();
        sphereShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        sphereShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        sphereShader.setVec3("lightPos", lightPos);

        // render spheres
        glBindVertexArray(sphereVAO);
        for (unsigned int i = 0; i < 2; i++)
        {           
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, spherePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            sphereShader.setMat4("model", model);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // draw light source:
        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        //model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightSourceShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        //glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        mainWindow.swapBuffers();
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    return 0;
}
