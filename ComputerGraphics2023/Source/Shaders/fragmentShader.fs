#version 330 core

in vec4 vertexColor;
in vec2 texCoordinates;
out vec4 fragColor;

uniform sampler2D textureObj[2];

void main()
{
    vec4 textureResult1 = texture(textureObj[0], texCoordinates); //JPG
    vec4 textureResult2 = texture(textureObj[1], texCoordinates); //PNG

    fragColor = mix(textureResult1, textureResult2, 0.5);

    if(textureResult2.a == 0.0f)
    {
        fragColor = textureResult1;
    }
}