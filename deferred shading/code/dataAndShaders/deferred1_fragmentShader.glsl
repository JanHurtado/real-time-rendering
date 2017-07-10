#version 430

out vec4 outColor;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 normalWorld;
in vec3 positionWorld;
in vec3 theColor;
in vec2 texCoord;
in vec3 tangentWorld;
in vec3 bitangentWorld;

uniform vec3 cameraPositionVector;
layout(binding=3) uniform sampler2D mainTexture;
layout(binding=4) uniform sampler2D mainBumpMap;


vec3 expand (vec3 v)
{
	return (v-0.5)*2;
}

void main()
{
	//float3x3 objToTangentSpace = float3x3( IN.Tangent, IN.Bitangent, IN.Normal );
	mat3 TBN = transpose(mat3(
        tangentWorld,
        bitangentWorld,
        normalWorld
    ));
	
	mat3 iTBN = transpose(TBN);
	
	vec3 texColor = texture(mainTexture, texCoord).rgb;
	vec3 bumpMapNormal = texture(mainBumpMap, texCoord).rgb;
	bumpMapNormal = normalize(expand(bumpMapNormal));

	gPosition = positionWorld;

    gNormal = normalize(iTBN*bumpMapNormal);

    gAlbedoSpec.rgb = texColor;

    gAlbedoSpec.a = 1.0;
	
	outColor = vec4(1.0,1.0,1.0,1.0);

}