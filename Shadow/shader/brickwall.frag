#version 420 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in vec4 FragPosDirLightSpace;
in vec4 FragPosSpotLightSpace;

out vec4 fragColor;

struct Material
{
    sampler2D texture_diffuse1;
	sampler2D texture_normal1;
	float shininess;
};

struct Attenuation
{
    float constant;
	float linear;
	float quadratic;
};

struct Cutoff
{
    float cutoff;
	float outCutoff;
};

struct DirectionalLight
{
    vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
    vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//the attenuation factor
	Attenuation atten;
};

struct SpotLight
{
    vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//the attenuation factor
	Attenuation atten;

	//cutoff angle
	Cutoff co;
};

const int PointLightNum = 4;
const int SpotLightNum = 1;

uniform Material material;

layout(std140, binding = 1) uniform Lights
{
    DirectionalLight dirLight;
    PointLight pointLights[PointLightNum];
    SpotLight torch[SpotLightNum];
};

uniform vec3 viewPos;

uniform sampler2D dirShadowMap;
uniform sampler2D spotShadowMap;

float diff(vec3 lightDir, vec3 normal);
float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess);

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewPos, float shadow);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos, float shadow);

float calcShadow(sampler2D shadowMap, vec4 fpls, vec3 lightDir, vec3 normal);

void main()
{
    vec3 mapNormal = texture(material.texture_normal1, TexCoord).rgb;
	mapNormal = mapNormal * 2.0f - vec3(1.0f);

    float dirShadow = calcShadow(dirShadowMap, FragPosDirLightSpace, dirLight.direction, Normal);

    vec3 result;
    result += calcDirLight(dirLight, mapNormal, viewPos, dirShadow);
	for(int i = 0; i < PointLightNum; ++i)
	    result += calcPointLight(pointLights[i], mapNormal, FragPos, viewPos);
	for(int i = 0; i < SpotLightNum; ++i)
	{
	    float spotShadow = calcShadow(spotShadowMap, FragPosSpotLightSpace, torch[i].direction, Normal);
	    result += calcSpotLight(torch[i], mapNormal, FragPos, viewPos, spotShadow);
	}
	    
    
    fragColor = vec4(result, 1.0f);
}

float diff(vec3 lightDir, vec3 normal)
{
	return max(dot(lightDir, normal), 0.0f);
}

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess)
{
	vec3 hd = normalize(lightDir + viewDir);
	return pow(max(dot(hd, normal), 0.0f), shininess);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewPos, float shadow)
{
    vec3 ld = normalize(-light.direction);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 diffuse = light.diffuse * diff(ld, n) * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * vec3(0.2f);

	return ambient + (1.0f - shadow) * (diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 ld = normalize(light.position - fragPos);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);

	//calculate attenuation
	float d = length(light.position - fragPos);
	float atten = 1.0f / (light.atten.constant + light.atten.linear * d + light.atten.quadratic * d * d);

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 diffuse = light.diffuse * diff(ld, n) * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * vec3(0.2f);

	return (ambient + diffuse + specular) * atten;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos, float shadow)
{
    vec3 ld = normalize(light.position - fragPos);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);

	//calculate cutoff
	float theta = dot(-ld, light.direction);
	float epsilon = light.co.cutoff - light.co.outCutoff;
	float iten = clamp((theta - light.co.outCutoff) / epsilon, 0.0f, 1.0f);

	//calculate attenuation
	float d = length(light.position - fragPos);
	float atten = 1.0f / (light.atten.constant + light.atten.linear * d + light.atten.quadratic * d * d);

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 diffuse = light.diffuse * diff(ld, n) * vec3(texture(material.texture_diffuse1, TexCoord));
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * vec3(0.2f);

	return (ambient + (1.0f - shadow) * (diffuse + specular) * iten) * atten;
}

float calcShadow(sampler2D shadowMap, vec4 fpls, vec3 lightDir, vec3 normal)
{
    vec3 projCoords = fpls.xyz / fpls.w;
	projCoords = projCoords * 0.5f + 0.5f;

	float currDepth = projCoords.z;

	if(currDepth > 1.0f) return 0.0f;

	normal = normalize(normal);

	//float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0.005f;

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

	return shadow;
}