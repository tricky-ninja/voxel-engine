#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 attributes; // Face(x), ambient occlusion(y)

out vec3 Normal;
out vec3 FragPos;
out float AO;

out vec4 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{

    gl_Position = projection * view * model * vec4(pos, 1.0);

    vec3 normal = vec3(0.0, 1.0, 0.0);
    
    int face = int(attributes.x);
    switch (face)
    {
    case 0: // FACE_LEFT
        normal = vec3(-1.0, 0.0, 0.0);
        break;
    case 1: // FACE_RIGHT
        normal = vec3(1.0, 0.0, 0.0);
        break;
    case 2: // FACE_FRONT
        normal = vec3(0.0, 0.0, 1.0);
        break;
    case 3: // FACE_BACK
        normal = vec3(0.0, 0.0, -1.0);
        break;
    case 4: // FACE_TOP
        normal = vec3(0.0, 1.0, 0.0);
        break;
    case 5: // FACE_BOTTOM
        normal = vec3(0.0, -1.0, 0.0);
        break;
    }

    Normal = normalMatrix * normal;
    FragPos = vec3(view * model * vec4(pos, 1.0));

    AO = attributes.y / 3.f;
}
