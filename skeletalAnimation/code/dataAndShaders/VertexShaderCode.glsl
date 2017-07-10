#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 vertexNormal;
in layout(location=3) vec2 texCoord2D;
in layout(location=4) vec3 vertexTangent;
in layout(location=5) vec3 vertexBitangent;
in layout(location=6) vec3 boneIDs;
in layout(location=7) vec3 boneWeights;

const int NR_BONES = 17;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;
uniform vec3 aditionalProperties;
uniform mat4 mBones[NR_BONES];

out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec3 theColor;
out vec2 texCoord;
out vec3 tangentWorld;
out vec3 bitangentWorld;

void main()
{
	mat4 BoneTransform = mBones[int(boneIDs[0])] * boneWeights[0];
    BoneTransform += mBones[int(boneIDs[1])] * boneWeights[1];
    BoneTransform += mBones[int(boneIDs[2])] * boneWeights[2];
	
	vec4 animationPos = BoneTransform * vec4(position, 1.0);
	vec4 animationNormal = BoneTransform * vec4(vertexNormal, 0);
	vec4 animationTangent = BoneTransform * vec4(vertexTangent, 0);
	vec4 animationBitangent = BoneTransform * vec4(vertexBitangent, 0);
	
	gl_Position = modelToProjectionMatrix * animationPos;
	normalWorld = normalize(vec3(modelToWorldMatrix * animationNormal));
	tangentWorld = vec3(modelToWorldMatrix * animationTangent);
	bitangentWorld = vec3(modelToWorldMatrix * animationBitangent);
	vertexPositionWorld = vec3(modelToWorldMatrix * animationPos);
	theColor = vertexColor;
	float ps = aditionalProperties[0];
	gl_PointSize = ps;
	//gl_PointSize = 10.f;
	texCoord = texCoord2D;
}