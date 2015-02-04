#version 150

uniform mat4 modelViewProjectionMatrix; // Projection * ModelView (provided in OF)
in vec4 position; //provided in OF
in vec3 normal; //provided in OF

uniform vec4 uColor;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;

out     vec3 LightIntensity;
uniform vec4 LightPosition;// Light position in eye coords.
uniform vec3 Kd;           // Diffuse reflectivity
uniform vec3 Ld;           // Light source intensity



void main() {
    // convert normal and position to eye coordinates
    vec3 tnorm = normalize(NormalMatrix * normal);
    vec4 eyeCoords = ModelViewMatrix * position;
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    
    //The diffuse shading equation
    LightIntensity = Ld * Kd * max(dot(s, tnorm), 0.0);
    
    //gl_FrontColor = color;
    //gl_Position = ftransform();
    gl_Position = modelViewProjectionMatrix * position;
}