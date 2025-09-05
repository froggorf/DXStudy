#include "../Global.fx"
#include "PBRHelpers.fx"

float3 CalcBRDF(float3 N, float3 V, float3 L, float3 albedo, 
	float metallic, float roughness, float3 F0, float3 radiance)
{
	float3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);

	if (NdotL <= 0.0) return float3(0, 0, 0);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	float3 kS = F;
	float3 kD = float3(1.0, 1.0, 1.0) - kS;
	kD *= 1.0 - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001;
	float3 specular = numerator / denominator;

	return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 CalcPBRLight(float3 worldPos, float3 N, float3 V, float3 albedo, 
	float metallic, float roughness, float3 F0, int lightIndex)
{
	FLightInfo info = g_LightBuffer[lightIndex];

	float3 L = float3(0, 0, 0);
	float3 radiance = float3(0, 0, 0);

	// Directional Light
	if (0 == info.LightType)
	{
		L = normalize(-info.WorldDir);
		radiance = info.LightColor;
	}
	// Point Light
	else if (1 == info.LightType)
	{
		L = normalize(info.WorldPos - worldPos);
		float distance = length(info.WorldPos - worldPos);
		float attenuation = saturate(1.0 - (distance / info.Radius));
		attenuation *= attenuation; 
		radiance = info.LightColor * attenuation;
	}
	// Spot Light
	else if (2 == info.LightType)
	{
		//L = normalize(info.WorldPos - worldPos);
		//float distance = length(info.WorldPos - worldPos);

		//// 거리 감쇄
		//float attenuation = saturate(1.0 - (distance / info.Radius));
		//attenuation *= attenuation;

		//// 스팟라이트 각도 감쇄
		//float3 spotDir = normalize(info.WorldDir);
		//float theta = dot(L, -spotDir);
		//float epsilon = info.InnerCone - info.OuterCone;
		//float intensity = saturate((theta - info.OuterCone) / epsilon);

		//radiance = info.LightColor * info.LightIntensity * attenuation * intensity;
	}

	// Cook-Torrance BRDF 계산
	return CalcBRDF(N, V, L, albedo, metallic, roughness, F0, radiance);
}


float3 CalcAmbientPBR(float3 N, float3 V, float3 albedo, 
	float metallic, float roughness, float3 F0, float ao)
{
	// Fresnel for ambient
	float3 F_ambient = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float3 kS_ambient = F_ambient;
	float3 kD_ambient = 1.0 - kS_ambient;
	kD_ambient *= 1.0 - metallic;

	// Sample environment map
	float3 R = reflect(-V, N);
	float3 irradiance = EnvironmentMap.Sample(CubeSampler, N).rgb;
	float3 prefilteredColor = EnvironmentMap.SampleLevel(CubeSampler, R, roughness * 4.0).rgb;

	float3 diffuse = irradiance * albedo;
	return (kD_ambient * diffuse + prefilteredColor * F_ambient) * ao;
}