#version 330 core
out vec4 FragColor;
in vec3 Normal;
uniform vec3 up;
void main()
{
	vec3 color = vec3(0.4f, 1.0f, 0.0f);
	vec3 ambient = 0.3 * color;
	vec3 diffuse = max(dot(Normal, up),0) * color;

	FragColor = vec4(diffuse + ambient,1);
}