#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 vertexNormal;
in layout(location=3) vec2 texCoord2D;
in layout(location=4) vec3 vertexTangent;
in layout(location=5) vec3 vertexBitangent;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;
uniform vec3 aditionalProperties;

out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec3 theColor;
out vec2 texCoord;
out vec3 tangentWorld;
out vec3 bitangentWorld;

void main()
{
	gl_Position = modelToProjectionMatrix * vec4(position, 1);
	normalWorld = vec3(modelToWorldMatrix * vec4(vertexNormal, 0));
	tangentWorld = vec3(modelToWorldMatrix * vec4(vertexTangent, 1));
	bitangentWorld = vec3(modelToWorldMatrix * vec4(vertexBitangent, 1));
	vertexPositionWorld = vec3(modelToWorldMatrix * vec4(position,1));
	theColor = vertexColor;
	float ps = aditionalProperties[0];
	gl_PointSize = ps;
	//gl_PointSize = 10.f;
	texCoord = texCoord2D;
}