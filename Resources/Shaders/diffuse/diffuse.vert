#version 450
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 appNormal;
layout(set = 0, binding = 0) uniform uniformBuffer{
	mat4 modelViewMatrix;
	mat4 projectionMatrix;
};
layout(location = 0) out float vertexColor;

void main()
{
	gl_Position = projectionMatrix * modelViewMatrix * position;
	vec3 normal = mat3(modelViewMatrix) * appNormal;	
	vertexColor = max(0.0, dot(normal, vec3(0.58, 0.58, 0.58)))+0.1;
}
