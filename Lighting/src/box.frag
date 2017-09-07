#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 fragColor;

struct Material
{
    sampler2D diffuse;
	sampler2D specular;
	float shininess;
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
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
    vec3 position;
	vec3 direction;
	//cutoff angle
	float cutoff;
	float outCutoff;

	//the attenuation factor
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

const int PointLightNum = 4;

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[PointLightNum];
uniform SpotLight torch;

uniform vec3 viewPos;

float diff(vec3 lightDir, vec3 normal);
float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess);

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewPos);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos);

void main()
{
    vec3 result;
    result += calcDirLight(dirLight, Normal, viewPos);
	for(int i = 0; i < PointLightNum; ++i)
	    result += calcPointLight(pointLights[i], Normal, FragPos, viewPos);
	//result += calcSpotLight(torch, Normal, FragPos, viewPos);

    fragColor = vec4(result, 1.0f);
}

float diff(vec3 lightDir, vec3 normal)
{
	return max(dot(lightDir, normal), 0.0f);
}

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float shininess)
{
	vec3 rd = reflect(-lightDir, normal);
	return pow(max(dot(rd, viewDir), 0.0f), shininess);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewPos)
{
    vec3 ld = normalize(-light.direction);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	vec3 diffuse = light.diffuse * diff(ld, n) * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * vec3(texture(material.specular, TexCoord));

	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 ld = normalize(light.position - fragPos);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);

	//calculate attenuation
	float d = length(light.position - fragPos);
	float atten = 1.0f / (light.constant + light.linear * d + light.quadratic * d * d);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	vec3 diffuse = light.diffuse * diff(ld, n) * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * vec3(texture(material.specular, TexCoord));

	return (ambient + diffuse + specular) * atten;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    vec3 ld = normalize(light.position - fragPos);
	vec3 n = normalize(normal);
	vec3 vd = normalize(viewPos - FragPos);

	//calculate cutoff
	float theta = dot(-ld, light.direction);
	float epsilon = light.cutoff - light.outCutoff;
	float iten = clamp((theta - light.outCutoff) / epsilon, 0.0f, 1.0f);

	//calculate attenuation
	float d = length(light.position - fragPos);
	float atten = 1.0f / (light.constant + light.linear * d + light.quadratic * d * d);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	vec3 diffuse = light.diffuse * diff(ld, n) * vec3(texture(material.diffuse, TexCoord));
	vec3 specular = light.specular * spec(ld, n, vd, material.shininess) * vec3(texture(material.specular, TexCoord));

	return (ambient + (diffuse + specular) * iten) * atten;
}