#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 FragColor;

in vec2 texCoordinates;
in vec3 fragNormal;
in vec3 fragPosition;

uniform Material surfaceMat;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform bool useBlinnPhong;

void main()
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Ambient component
    ambient = surfaceMat.ambient;

    // Diffuse component
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightPosition - fragPosition);
    diffuse = surfaceMat.diffuse * max(dot(normal, lightDirection), 0.0);

    // Specular component
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    if(useBlinnPhong)
    {
        vec3 halfwayVector = normalize(lightDirection + viewDirection);
        specular = surfaceMat.specular * pow(dot(halfwayVector, normal), surfaceMat.shininess);
    }
    else
    {
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        specular = surfaceMat.specular * pow(max(dot(viewDirection, reflectionDirection), 0.0), surfaceMat.shininess);
    }

    vec3 result = (ambient + diffuse + specular) * lightColor;
    FragColor = vec4(result, 1.0);
}