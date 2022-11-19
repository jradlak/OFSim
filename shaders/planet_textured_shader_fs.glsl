#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoord;  
in float vFragDepth;

uniform float logDepthBufFC;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform sampler2D texture1;

void main()
{
    // ambient
    float ambientStrength = 0.1; 
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * texture2D(texture1, TexCoord).rgb; // objectColor;
    FragColor = vec4(result, 1.0);
    
    gl_FragDepth = log2(vFragDepth) * logDepthBufFC * 0.5;
} 