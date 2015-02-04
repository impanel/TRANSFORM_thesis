#version 150

in vec3 LightIntensity;

out vec4 fragColor;

void main()
{
    fragColor = vec4(LightIntensity, 1.0);
}
