#ifndef __PBR_LIGHTHELPER_FX__
#define __PBR_LIGHTHELPER_FX__

#include "Global.fx"
#include "LightHelper.hlsl"
#include "PBRHelpers.fx"

// =======================
// PBR Helper Functions
// =======================

// Normal Distribution Function (GGX/Trowbridge-Reitz)
// 미세면의 분포를 계산하는 함수

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a      = roughness*roughness;
	float a2     = a*a;
	float NdotH  = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float num   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

// Geometry Function (Smith's method)
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

// 미세면의 그림자를 계산하는 함수
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2  = GeometrySchlickGGX(NdotV, roughness);
	float ggx1  = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel Equation (Schlick's approximation)
// 각도에 따른 반사율을 계산
float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}
// 얘는 거칠기까지 포함해서 계산
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	float3 r = max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0);
	return F0 + (r - F0) * pow(1.0 - cosTheta, 5.0);
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
	roughness = max(roughness, 0.01);
	float3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);

	if (NdotL <= 0.0) return float3(0, 0, 0);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	float3 kS = F;
	float3 kD = float3(1,1,1) - kS;
	kD *= 1.0 - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0 * NdotV * NdotL;
	float3 specular = metallic < 0.01f? 0.0f : numerator / max(denominator,0.001);

	float3 Diffuse = kD * albedo / PI;
	
	return (Diffuse + specular) * radiance * NdotL;
}

void GetLightRadiance(in FLightInfo info, in float3 viewPos, out float3 L, out float3 radiance)
{
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
}

float3 CalcPBRLight(float3 viewPos, float3 N, float3 V, float3 albedo, 
	float metallic, float roughness, float3 F0, int lightIndex)
{
	FLightInfo info = g_LightBuffer[lightIndex];

	float3 L = float3(0, 0, 0);
	float3 radiance = float3(0, 0, 0);

	GetLightRadiance(info, viewPos, L, radiance);

	// Cook-Torrance BRDF 계산
	return CalcBRDF(N, V, L, albedo, metallic, roughness, F0, radiance);
}

// 빛의 기여도만 계산하는 함수
void CalcLightContribution(float3 N, float3 V, float3 L, 
	float metallic, float roughness, float3 F0, float3 radiance,
	out float3 Diffuse, out float3 Specular)
{
	roughness = max(roughness, 0.01);
	float3 H = normalize(V + L);
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);

	if (NdotL <= 0.0)
	{
		Diffuse = float3(0,0,0);
		Specular = float3(0,0,0);
		return;
	}

	// Cook-Torrance BRDF (albedo 제외)
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

	float3 kS = F;
	float3 kD = float3(1,1,1) - kS;
	kD *= 1.0 - metallic;

	// Specular 기여도 (albedo 없음)
	float3 numerator = NDF * G * F;
	float denominator = 4.0 * NdotV * NdotL;
	float3 specular = metallic < 0.01f ? 0.0f : numerator / max(denominator, 0.001);

	// Diffuse 기여도 (albedo 제외, 단순히 kD만)
	float3 diffuseContribution = kD / PI;

	Diffuse = diffuseContribution * radiance * NdotL;
	Specular = specular * radiance * NdotL;
}
// Deferred Light 에서 라이트가 얼만큼의 영향을 주는지 계산하는 함수
void CalcPBRLightContribution(float3 viewPos, float3 N, float3 V, 
	float metallic, float roughness, float3 F0, int lightIndex
	, out float3 Diffuse, out float3 Specular)
{
	FLightInfo info = g_LightBuffer[lightIndex];

	float3 L = float3(0, 0, 0);
	float3 radiance = float3(0, 0, 0);

	GetLightRadiance(info, viewPos, L, radiance);

	CalcLightContribution(N, V, L, metallic, roughness, F0, radiance, Diffuse, Specular);
}

float3 CalcAmbientPBR(float3 N, float3 V, float3 albedo, 
	float metallic, float roughness, float3 F0, float ao)
{
	// View Space -> World Space 변환
	float3 worldN = normalize(mul(N, (float3x3)gViewInv));
	float3 worldV = normalize(mul(V, (float3x3)gViewInv));
	float3 worldR = reflect(-worldV, worldN);

	// Environment map sampling
	float mipLevel = roughness * roughness * 6.0;
	float3 prefilteredColor = EnvironmentMap.SampleLevel(CubeSampler, worldR, mipLevel).rgb;

	// metallic에 따른 색상 블렌딩
	float3 nonMetalColor = albedo*0.8; // 비금속: 원래 색상 유지
	float3 metalColor = prefilteredColor * albedo; // 금속: 환경맵 반사

	float3 result = lerp(nonMetalColor, metalColor, metallic);
	

	return result; 
}

float3 FastDiffuseIrradiance(float3 Normal)
{
	// View Space -> World Space 변환
	float3 worldN = normalize(mul(Normal, (float3x3)gViewInv));

	// 탄젠트 공간 구성
	float3 up = abs(worldN.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 right = normalize(cross(up, worldN));
	up = normalize(cross(worldN, right));

	const int SAMPLE_COUNT = 32;
	float3 totalIrradiance = float3(0, 0, 0);

	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		float u1 = float(i) / float(SAMPLE_COUNT);
		float u2 = frac(float(i) * 0.618033988749895); // 골든 레이시오로 균등 분포

		float cosTheta = sqrt(u1);                    // 코사인 가중
		float sinTheta = sqrt(1.0 - u1);
		float phi = 2.0 * PI * u2;

		float3 sampleVec = float3(
			sinTheta * cos(phi),
			sinTheta * sin(phi),
			cosTheta
		);

		// 월드 공간으로 변환
		sampleVec = sampleVec.x * right + sampleVec.y * up + sampleVec.z * worldN;

		// 샘플링 및 누적 (코사인 가중치 이미 적용됨)
		totalIrradiance += EnvironmentMap.SampleLevel(CubeSampler, sampleVec, 0).rgb;
	}

	// ✅ 올바른 정규화 (π는 이미 코사인 가중에 포함됨)
	return totalIrradiance / float(SAMPLE_COUNT);
}

float3 IBLAmbient(float3 N, float3 V, float3 F0, float3 albedo, float metallic, float roughness, float ao)
{
	// 🔥 IBL 계산 (통합 버전)
	float3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
	float3 kD = 1.0 - kS;

	// Diffuse IBL
	//float3 irradiance = FastDiffuseIrradiance(N);
	//irradiance = lerp(0.0f, irradiance, metallic);
	float3 diffuse = albedo;	

	// Specular IBL  
	float3 R = reflect(-V, N);
	float3 worldR = normalize(mul(R, (float3x3)gViewInv));
	float3 EnvironmentColor = EnvironmentMap.SampleLevel(CubeSampler, worldR, roughness * 15).rgb * albedo;
	float3 prefilteredColor = lerp(albedo, EnvironmentColor ,metallic*1.1);
	float3 specular = prefilteredColor * lerp(kS,F0, metallic * 1.5);

	// 기본적으로 Ambient의 색상은 낮추고, 빛에의해 밝아지는 세기를 좀 더 키워주도록 변경 (0.4f)
	// 근데 메탈릭은 값이 더 크게 1에 가깝게 되도록 하는게 맞는것같음.
	float3 ambient = (kD * diffuse  + specular) * ao * (0.16f + lerp(0.0f,0.84f,metallic));
	return ambient;
}

// Forward Rendering 전용
float3 MyPBR(PBR_PS_INPUT input, float3 albedo, float metallic, float ObjectSpecular, float roughness, float ao)
{
	float3 N = GetNormalFromMap(input);
	float3 V = normalize(-input.ViewPosition);

	const float DefaultSpecular = 0.04 * ObjectSpecular;
	float3 F0 = lerp(float3(DefaultSpecular, DefaultSpecular, DefaultSpecular), albedo, metallic);

	// 다이렉트 라이팅
	float3 Lo = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < gLightCount; ++i)
	{
		Lo += CalcPBRLight(input.ViewPosition, N, V, albedo, metallic, roughness, F0, i);
	}

	float3 ambient = IBLAmbient(N,V,F0,albedo,metallic,roughness,ao);

	float3 color = Lo +  ambient;

	color = ACESFilm(color);

	return color;
}
#endif
//
//float3 diffuse = lerp(float3(0,0,0), albedo, 1-metallic);
//float3 ambient = (kD * diffuse + specular) * ao;