#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 attributes; // Face(x), ambient occlusion(y)
layout(location = 2) in float uv;
layout(location = 3) in float textureID;

out vec3 Normal;
out vec3 FragPos;
out float AO;
out vec2 UV;
out vec2 ID;

out vec4 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform float time;

void main()
{

    vec3 pos = aPos;
    pos.y -= .1;
    pos.y += sin((pos.x * 3.1415 + time)) * .05;
    gl_Position = projection * view * model * vec4(pos, 1.0);
   

    vec3 objectNormal = normalize(vec3(-0.05 * 1 * cos((pos.x * 3.1415 + time)), 1.0, 0.0));

    Normal = normalMatrix * objectNormal;
    FragPos = vec3(view * model * vec4(pos, 1.0));

    AO = attributes.y / 3.f;

    if (uv == float(3)) UV = vec2(1, 1);
    if (uv == float(2)) UV = vec2(1, 0);
    if (uv == float(1)) UV = vec2(0, 1);
    if (uv == float(0)) UV = vec2(0, 0);
    ID = vec2(mod(textureID, 16), floor(textureID / 16.f));
}
