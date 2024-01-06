#pragma once

#include "../../external_libraries/glad/glad.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

//based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;    
    void setMat4(const std::string& name, glm::mat4 value) const;    
    void setVec3(const std::string& name, const glm::vec3& value) const;    
    void setVec3(const std::string& name, float x, float y, float z) const;

private:    
    void checkCompileErrors(unsigned int shader, std::string type);
};
