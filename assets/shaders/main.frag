#version 330 core
out vec4 FragColor;
in vec3 Normal;
in float AO;

uniform vec3 up;
uniform float renderDistance;

vec4 fogColor = vec4(185 / 255.f, 233 / 255.f, 250 / 255.f, 1.f);

float near = 0.1; 
float far  = 1000.0; 
  
float linearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float logisticDepth(float depth, float steepness, float offset)
{
	float zVal = linearizeDepth(depth);
	return (1 / (1 + exp(-steepness * (zVal - offset))));
}


void main()
{
	vec3 color = vec3(0.4f, 1.0f, 0.3f);
	float depth = logisticDepth(gl_FragCoord.z, 0.05, renderDistance * 10);
	vec3 ambient = 0.3 * color;
	vec3 diffuse = max(dot(Normal, up),0) * color * 0.8;

	FragColor = ((1 - depth) * fogColor) * vec4(vec3(diffuse + ambient) * AO, 1);
}