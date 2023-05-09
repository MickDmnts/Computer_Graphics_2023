#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec4 vertexColor;

uniform vec3 displacement;

void main()
{
    vertexColor = vec4(color, 1.0);

    vec3 finalPos = position + displacement;

    gl_Position = vec4(finalPos, 1.0);
}