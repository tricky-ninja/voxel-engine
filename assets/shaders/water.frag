#version 460 core
layout(location = 0) out vec4 FragColor;
in vec3 Normal;
in float AO;
in vec2 ID;

uniform vec3 up;
uniform float renderDistance;

uniform sampler2D texture_atlas;
uniform float time;              // Uniform time for animated waves

in vec2 UV;

vec3 fogColor = vec3(185 / 255.0, 233 / 255.0, 250 / 255.0);

float near = 0.1;
float far = 1000.0;

// Depth functions for fog
float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float logisticDepth(float depth, float steepness, float offset) {
    float zVal = linearizeDepth(depth);
    return (1.0 / (1.0 + exp(-steepness * (zVal - offset))));
}

void main() {
    vec2 subTextureSize = vec2(1.0 / 16.0 - 0.001, 1.0 / 16.0 - 0.001); // Small offset to avoid edges
    vec2 offset = vec2(ID.x * subTextureSize.x, (ID.y + 1) * -subTextureSize.y);

    // Multi-layered wave distortion
    vec2 waveDistortion1 = vec2(sin(UV.y * 12.0 + time * 0.2), cos(UV.x * 12.0 + time * 0.25)) * 0.02;
    vec2 waveDistortion2 = vec2(sin(UV.y * 20.0 + time * 0.4), cos(UV.x * 20.0 + time * 0.45)) * 0.01;
    vec2 waveDistortion = waveDistortion1 + waveDistortion2;

    // Apply wave distortion to UV coordinates
    vec2 atlasUV = (UV + waveDistortion) * (subTextureSize * 0.98) + offset + vec2(0.001, -0.001);

    vec4 full_color = texture(texture_atlas, atlasUV);
    if (full_color.a < 0.1) discard;
    vec3 color = full_color.rgb;
    
    // Calculate depth-based fog
    float depth = logisticDepth(gl_FragCoord.z, 0.05, renderDistance * 10);

    // Enhanced Fresnel effect for realistic reflectivity
    float fresnelFactor = pow(1.0 - dot(normalize(Normal), vec3(0.0, 1.0, 0.0)), 3.0);
    vec3 fresnelColor = mix(color, fogColor, fresnelFactor * 0.5);

    vec3 ambient = 0.2 * color;
    vec3 diffuse = max(dot(Normal, up), 0.0) * color * 0.5;

    // Combining lighting components
    vec4 lighting = vec4((ambient + diffuse ) * AO, 1.0);

    // Blending color with fog and Fresnel for realistic water shading
    vec4 colorGradient = (1.0 - depth) * vec4((fresnelColor * AO), .5 );
    vec4 fogGradient = depth * vec4(fogColor, 1.0);

    FragColor = mix(colorGradient, fogGradient, depth);
}
