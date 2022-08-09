#pragma once

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

// based on: https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/2.text_rendering/text_rendering.cpp

struct Character {
    unsigned int textureID; 
    glm::ivec2   size;      
    glm::ivec2   bearing;   
    unsigned int advance;
};

class TextRenderer
{
public:
    TextRenderer(void);

    int init(unsigned int scrWidth, unsigned int scrHeight);

    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

    ~TextRenderer();

private:
    Shader* shader;

    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;

    void clear();
};