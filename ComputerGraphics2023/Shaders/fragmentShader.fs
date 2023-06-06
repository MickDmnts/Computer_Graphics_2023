#version 330 core

out vec4 FragColor;

in vec2 texCoordinates;

uniform sampler2D textureObj[2];

void main()
{
    vec4 textureResult1 = texture(textureObj[0], texCoordinates); //JPG
    vec4 textureResult2 = texture(textureObj[1], texCoordinates); //PNG

    FragColor = mix(textureResult1, textureResult2, 0.5);

    if(textureResult2.a == 0.0f)
    {
        FragColor = textureResult1;
    }
}