#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec3 Pos;
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

// make islands-like color pattern
int computeColor() 
{
    highp int x = int(FragPos.x * 10);   
    highp int y = int(FragPos.y * 10);
    highp int z = int(FragPos.z * 10);

    return (x - 2 * (x / 2)) * (y - 2 * (y / 2)) * (z - 2 * (z / 2));
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
        currColor = vec3(0.25, 0.55, 0.75);
    }

    vec3 result = (ambient + diffuse) * currColor;
    FragColor = vec4(result, 1.0);
}