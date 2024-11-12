#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool inWater;
uniform float time;

void main()
{
    vec4 TexColor = texture(screenTexture, TexCoords);

    if (inWater) {
        vec2 waveOffset = vec2(
            sin(TexCoords.y * 10.0 + time * 1.2) * 0.006,
            cos(TexCoords.x * 10.0 + time * 1.2) * 0.006 
        );
        vec2 distortedTexCoords = TexCoords + waveOffset;
        vec4 distortedColor = texture(screenTexture, distortedTexCoords);

        float depthFactor = TexCoords.y * 0.5 + 0.3;  
        vec4 depthTint = vec4(0.2, 0.4, 0.8, 1.0) * depthFactor * 1.4;

        vec4 underwaterColor = mix(distortedColor, depthTint, 0.3);

        FragColor = vec4(
            underwaterColor.r * 0.6,
            underwaterColor.g * 0.7,
            underwaterColor.b * 1.1 + depthFactor * 0.2,
            underwaterColor.a
        );
    } 
    else {
        FragColor = TexColor;
    }
}
