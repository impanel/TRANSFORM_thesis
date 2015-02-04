#version 150

//uniform vec3 uLight, uColor;
//
//in vec3 vNormal;
//in vec3 vPosition;
//
//out vec4 fragColor;
//
//void main() {
//  vec3 tolight = normalize(uLight - vPosition);
//  vec3 normal = normalize(vNormal);
//
//  float diffuse = max(0.0, dot(normal, tolight));
//  vec3 intensity = uColor * diffuse;
//
//  fragColor = vec4(intensity, 1.0);
//}

in vec3 LightIntensity;

out vec4 fragColor;

void main()
{
    fragColor = vec4(LightIntensity, 1.0);
}
