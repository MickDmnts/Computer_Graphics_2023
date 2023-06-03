#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 textCoord;

out vec4 vertexColor;
out vec2 texCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoordinates = textCoord;

    vertexColor = vec4(color, 1.0);

    vec4 finalPos = projection * view * model * vec4(position, 1.0); 

    gl_Position = finalPos;
}