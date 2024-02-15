#version 410 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
in float vFragDepth;
uniform float logDepthBufFC;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{   
    FragColor = vec4(objectColor, 1.0);
    
    gl_FragDepth = log2(vFragDepth) * logDepthBufFC * 0.5;
} 