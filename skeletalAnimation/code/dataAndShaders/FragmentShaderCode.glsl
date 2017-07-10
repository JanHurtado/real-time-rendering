#version 430

out vec4 outColor;

in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 theColor;
in vec2 texCoord;
in vec3 tangentWorld;
in vec3 bitangentWorld;

uniform vec3 lightPositionVector;
uniform vec3 cameraPositionVector;
uniform vec4 lightVector;
layout(binding=0) uniform sampler2D mainTexture;
layout(binding=2) uniform sampler2D mainBumpMap;


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
	
	vec3 texColor = texture(mainTexture, texCoord).rgb;
	vec3 bumpMapNormal = texture(mainBumpMap, texCoord).rgb;
	bumpMapNormal = normalize(expand(bumpMapNormal));

	//normal = normalWorld;
	vec3 normal = bumpMapNormal;
	
	vec3 lightVectorWorld = normalize(lightPositionVector - vertexPositionWorld);
	lightVectorWorld = TBN * lightVectorWorld;
	float brightness = dot(lightVectorWorld, normal);
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	// Specular
	//vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, bumpMapNormal);
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normal);
	vec3 eyeVectorWorld = normalize(cameraPositionVector - vertexPositionWorld);
	eyeVectorWorld = TBN * eyeVectorWorld;
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s, 25);
	vec4 specularLight = vec4(s, s, s, 1);

	outColor = lightVector + clamp(diffuseLight, 0, 1) + specularLight;
	//outColor = theColor * outColor;
	outColor = vec4(texColor,1.0)*outColor;
	
	
	//outColor = vec4(theColor,1.0);
}