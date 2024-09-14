#version 460 core
layout (location = 0)  out vec4 FragColor;
in vec3 Normal;
in float AO;
in vec2 ID;

uniform vec3 up;
uniform float renderDistance;

uniform sampler2D texture_atlas;

in vec2 UV;

vec3 fogColor = vec3(185 / 255.f, 233 / 255.f, 250 / 255.f);

float near = 0.1; 
float far  = 1000.0; 
  
float linearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float logisticDepth(float depth, float steepness, float offset)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
    vec2 subTextureSize = vec2(1.0 / 16.0, 1.0 / 16.0); 
    vec2 offset = vec2(ID.x * subTextureSize.x,  (ID.y + 1)* -subTextureSize.y); 
    
    vec2 atlasUV = UV * subTextureSize + offset;
    
    vec4 full_color = texture(texture_atlas, atlasUV);
    if (full_color.a < 0.1) discard;
    vec3 color = full_color.rgb;
	float depth = logisticDepth(gl_FragCoord.z, 0.05, renderDistance * 10);

	vec3 ambient = 0.2 * color;
	vec3 diffuse = max(dot(Normal, up),0) * color * 0.8;    // TODO: use face values for lighting

    vec4 colorGradient = (1.0f - depth) * vec4((diffuse + ambient) * AO, 1);
    vec4 fogGradient = (depth * vec4(fogColor,1));

	FragColor = vec4((colorGradient).rgb, full_color.a) + fogGradient;

}