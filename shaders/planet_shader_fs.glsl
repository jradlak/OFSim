#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec3 Pos;

in float vFragDepth;
uniform float logDepthBufFC;

uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

// make islands-like color pattern
int computeColor() 
{
    highp int x = int(FragPos.x / 100);   
    highp int y = int(FragPos.y / 100);
    highp int z = int(FragPos.z / 100);

    return (x - 2 * (x / 2)) * (y - 2 * (y / 2)) * (z - 2 * (z / 2));
}

int computeColor2()
{
    highp int x = int(FragPos.x / 10) + 87;
    highp int y = int(FragPos.y / 10) + 47;
    highp int z = int(FragPos.z / 10) + 47;

    return (x - 3 * (x / 3)) * (y - 3 * (y / 3)) * (z - 3 * (z / 3));
}

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
    
    vec3 currColor = objectColor;
    if (computeColor() == 0)
    {
        currColor = vec3(0.0, 0.0, 0.6);
    }
    else
    {
        if (computeColor2() != 0)
        {
            currColor = vec3(0.6, 0.6, 0.6);
        }
    }

    vec3 result = (ambient + diffuse) * currColor;
    FragColor = vec4(result, 1.0);

    gl_FragDepth = log2(vFragDepth) * logDepthBufFC * 0.5;
}