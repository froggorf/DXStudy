#ifndef __PBR_LIGHTHELPER_FX__
#define __PBR_LIGHTHELPER_FX__

#include "../Global.fx"
#include "../LightHelper.hlsl"
#include "PBRHelpers.fx"

// =======================
// PBR Helper Functions
// =======================

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

// Geometry Function (Smith's method)
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel Equation (Schlick's approximation)
float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fresnel with roughness for IBL
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * 
		pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Get Normal from Normal Map
float3 GetNormalFromMap(PBR_PS_INPUT input)
{
	int bNormalTexBind = bTexBind_0_3.g;
	if (!bNormalTexBind)
	{
		return normalize(input.ViewNormal);
	}

	float3 tangentNormal = NormalTexture.Sample(DefaultSampler, input.TexCoord).xyz * 2.0 - 1.0;

	float3 N = normalize(input.ViewNormal);
	float3 T = normalize(input.ViewTangent);
	float3 B = normalize(input.ViewBinormal);
	float3x3 TBN = float3x3(T, B, N);

	return normalize(mul(tangentNormal, TBN));
}

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

float3 CalcPBRLight(float3 viewPos, float3 N, float3 V, float3 albedo, 
	float metallic, float roughness, float3 F0, int lightIndex)
{
	FLightInfo info = g_LightBuffer[lightIndex];

	float3 L = float3(0, 0, 0);
	float3 radiance = float3(0, 0, 0);

	// Directional Light
	if (0 == info.LightType)
	{
		// World Dir을 View Space로 변환
		L = normalize(mul(float4(normalize(info.WorldDir), 0.f), gView).xyz);
		L = -L; // 광원에서 표면으로 향하는 방향
		radiance = info.LightColor;
	}
	// Point Light
	else if (1 == info.LightType)
	{
		// 광원 위치를 View Space로 변환
		float3 lightViewPos = mul(float4(info.WorldPos, 1.f), gView).xyz;

		// 광원에서 표면으로 향하는 방향벡터
		L = normalize(lightViewPos - viewPos);

		// 거리 계산 및 감쇄
		float distance = length(lightViewPos - viewPos);
		float attenuation = saturate(1.0 - (distance / info.Radius));
		attenuation *= attenuation; 
		radiance = info.LightColor * attenuation;
	}
	// Spot Light
	else if (2 == info.LightType)
	{
		//// 광원 위치를 View Space로 변환
		//float3 lightViewPos = mul(float4(info.WorldPos, 1.f), gView).xyz;
		//
		//L = normalize(lightViewPos - viewPos);
		//float distance = length(lightViewPos - viewPos);
		//
		//// 거리 감쇄
		//float attenuation = saturate(1.0 - (distance / info.Radius));
		//attenuation *= attenuation;
		//
		//// 스팟라이트 방향을 View Space로 변환
		//float3 spotViewDir = normalize(mul(float4(info.WorldDir, 0.f), gView).xyz);
		//float theta = dot(-L, spotViewDir); // L은 표면으로 향하는 방향이므로 -L 사용
		//float epsilon = info.InnerCone - info.OuterCone;
		//float intensity = saturate((theta - info.OuterCone) / epsilon);
		//
		//radiance = info.LightColor * attenuation * intensity;
	}

	// Cook-Torrance BRDF 계산
	return CalcBRDF(N, V, L, albedo, metallic, roughness, F0, radiance);
}


float3 CalcAmbientPBR(float3 N, float3 V, float3 albedo, 
	float metallic, float roughness, float3 F0, float ao)
{
	// View Space -> World Space 변환
	float3 worldN = normalize(mul(N, (float3x3)gViewInv));
	float3 worldV = normalize(mul(V, (float3x3)gViewInv));
	float3 worldR = reflect(-worldV, worldN);

	float NdotV = max(dot(N, V), 0.0);
	float3 F_ambient = FresnelSchlickRoughness(NdotV, F0, roughness);

	float3 kS = F_ambient;
	float3 kD = (1.0 - kS) * (1.0 - metallic);

	// Environment map sampling
	float3 irradiance = EnvironmentMap.Sample(CubeSampler, worldN).rgb;
	float mipLevel = roughness * roughness * 6.0;
	float3 prefilteredColor = EnvironmentMap.SampleLevel(CubeSampler, worldR, mipLevel).rgb;

	float3 diffuse = irradiance * albedo;
	float3 specular = prefilteredColor * kS;

	// 🔥 극단적 해결책: metallic < 0.5일 때 IBL 대폭 감소
	float iblMask = smoothstep(0.0, 0.5, metallic);
	float3 ambient = (kD * diffuse + specular * iblMask) * ao;

	// 🔥 Non-metallic일 때 전체 IBL을 거의 끔
	float overallIBL = lerp(0.1, 1.0, metallic);

	return ambient * overallIBL * 0.3;
}

float3 ACESFilm(float3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

#endif