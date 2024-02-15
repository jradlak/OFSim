#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out float vFragDepth;

uniform mat4 model;
uniform mat4 transformation;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
         
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = transformation * vec4(aPos, 1.0);
    
    //TexCoord = vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);

    vFragDepth = 1.0 + gl_Position.w;
}