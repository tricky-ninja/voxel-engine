#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform bool inWater;
uniform float time;

uniform uint width;
uniform uint height;

void main()
{

    vec4 TexColor = texture(screenTexture, TexCoords);

    // Convert coords to range from -0.5 to 0.5
    vec2 uv = gl_FragCoord.xy / vec2(width, height);
    uv -= 0.5;

    float crosshairSize = 0.01;  // halflength
    float thickness = 0.001;      

    bool inVerticalBar = abs(uv.x) < thickness && abs(uv.y) < crosshairSize;
    bool inHorizontalBar = abs(uv.y) < thickness && abs(uv.x) < crosshairSize;

    if (inWater) {
        vec2 waveOffset = vec2(
            sin(TexCoords.y * 10.0 + time * 1.2) * 0.006,
            cos(TexCoords.x * 10.0 + time * 1.2) * 0.004 
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



    if (inVerticalBar || inHorizontalBar) {
        FragColor = vec4(1 - FragColor.rgb, 1);  // Set color (white crosshair)
    }
}
