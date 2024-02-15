#version 410 core
out vec4 FragColor;

in float vFragDepth;
uniform float logDepthBufFC;

void main()
{
    FragColor = vec4(1.0);
    gl_FragDepth = log2(vFragDepth) * logDepthBufFC * 0.5;
}