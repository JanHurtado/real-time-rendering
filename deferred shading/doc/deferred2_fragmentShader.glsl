#version 430

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 4;
uniform Light lights[NR_LIGHTS];
uniform vec3 cameraPositionVector;

void main()
{             
    // retrieve data from gbuffer
	vec3 viewPos = cameraPositionVector;
	
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
	vec3 eyeVectorWorld = normalize(cameraPositionVector - FragPos);
	
	vec4 light = vec4(0.0,0.0,0.0,1.0);
	
	for(int i = 0; i < NR_LIGHTS; ++i)
    {
		vec3 lightDir = normalize(lights[i].Position - FragPos);
		float brightness = dot(lightDir, Normal);
		vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);
		vec3 reflectedLightVectorWorld = reflect(-lightDir, Normal);
		float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
		s = pow(s, 24);
		vec4 specularLight = vec4(s, s, s, 1);
		light = light +  clamp(diffuseLight, 0, 1) + specularLight;
	}
	FragColor = light * vec4(Diffuse,1.0);
	//FragColor = vec4(Normal*1.5,1.0);
	

}