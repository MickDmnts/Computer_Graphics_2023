#version 330 core

out vec4 FragColor;

in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 viewPosition;
uniform samplerCube skybox;

//Refraction indexes
uniform float entMediumIdx; //refraction index of the medium we are exiting
uniform float extMediumIdx; //refraction index of the medium we are entering

vec3 calculateReflection()
{
    vec3 viewDirection = normalize(fragPosition - viewPosition);
    vec3 normal = normalize(fragNormal);

    vec3 reflectionDirection = reflect(viewDirection, normal);

    return texture(skybox, reflectionDirection).rgb;
}

vec3 calculateRefraction()
{
    vec3 viewDirection = normalize(fragPosition - viewPosition);
    vec3 normal = normalize(fragNormal);

    float ratio = extMediumIdx/entMediumIdx;
    vec3 refractionDir = refract(viewDirection, normal, ratio);

    return texture(skybox, refractionDir).rgb;
}

void main()
{
    vec3 color = calculateRefraction();

    FragColor = vec4(color, 1.0);
}