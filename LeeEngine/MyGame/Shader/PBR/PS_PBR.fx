#ifndef __PS_PBR_FX__
#define __PS_PBR_FX__

#include "PBRLightHelper.fx"

/* // Textures
 Texture2D AlbedoTexture     : register(t0);
 Texture2D NormalTexture     : register(t1);
 Texture2D MetallicTexture   : register(t2);
 Texture2D RoughnessTexture  : register(t3);
 Texture2D AOTexture         : register(t4);
 TextureCube EnvironmentMap  : register(t5);
*/

float4 PBR_PS(PBR_PS_INPUT input) : SV_TARGET
{
	// Sample textures (기존 코드 그대로)
	int AlbedoTexBind = bTexBind_0_3.x;
	float3 DefaultAlbedo = float3(1.0f, 0.0f, 1.0f);
	float3 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.TexCoord).rgb : DefaultAlbedo;
	
	float DefaultMetallic = 0.5f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultMetallic;
	
	float DefaultRoughness = 0.5f;
	int RoughnessTexBind = bTexBind_0_3.w;
	float roughness = RoughnessTexBind ? RoughnessTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultRoughness;
	
	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.x;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultAO;
	
	// Get normal from normal map
	float3 N = GetNormalFromMap(input);
	
	// Camera position and view direction
	float3 CameraPosition = gViewInv[3].xyz;
	float3 V = normalize(CameraPosition - input.WorldPos);
	
	// Calculate reflectance at normal incidence
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
	
	// Direct lighting calculation (여러 라이트 지원)
	float3 Lo = float3(0.0, 0.0, 0.0);
	
	for (int i = 0; i < gLightCount; ++i)
	{
		Lo += CalcPBRLight(input.WorldPos, N, V, albedo, metallic, roughness, F0, i);
	}
	
	// Ambient lighting (IBL)
	float3 ambient = CalcAmbientPBR(N, V, albedo, metallic, roughness, F0, ao);
	
	float3 color = ambient + Lo;
	
	// HDR tonemapping & Gamma correction
	color = color / (color + float3(1.0, 1.0, 1.0));
	color = pow(color, 1.0 / 2.2);
	
	return float4(color, 1.0);
}


float4 PBR_PS_Well(PBR_PS_INPUT input) : SV_TARGET
{
	// Sample textures (기존 코드 그대로)
	int AlbedoTexBind = bTexBind_0_3.x;
	float3 DefaultAlbedo = float3(1.0f, 0.0f, 1.0f);
	float3 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.TexCoord).rgb : DefaultAlbedo;
	
	float DefaultMetallic = 0.5f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.TexCoord).g : DefaultMetallic;
	
	float DefaultRoughness = 0.5f;
	float roughness = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.TexCoord).b : DefaultRoughness;
	
	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.x;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultAO;
	
	// Get normal from normal map
	float3 N = GetNormalFromMap(input);
	
	// Camera position and view direction
	float3 CameraPosition = gViewInv[3].xyz;
	float3 V = normalize(CameraPosition - input.WorldPos);
	
	// Calculate reflectance at normal incidence
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
	
	// Direct lighting calculation (여러 라이트 지원)
	float3 Lo = float3(0.0, 0.0, 0.0);
	
	for (int i = 0; i < gLightCount; ++i)
	{
		Lo += CalcPBRLight(input.WorldPos, N, V, albedo, metallic, roughness, F0, i);
	}
	
	// Ambient lighting (IBL)
	float3 ambient = CalcAmbientPBR(N, V, albedo, metallic, roughness, F0, ao);
	
	float3 color = ambient + Lo;
	
	// HDR tonemapping & Gamma correction
	color = color / (color + float3(1.0, 1.0, 1.0));
	color = pow(color, 1.0 / 2.2);
	
	return float4(color, 1.0);
}

cbuffer cbTest : register(b4)
{
	float AlbedoR;
	float AlbedoG;
	float AlbedoB;
	float ObjectSpecular;
	float ObjectMetallic;
	float ObjectRoughness;
	float ObjectAO;
	float2 Pad;
	float4 pad2;
}

float3 ACESFilm(float3 x)
{
	float a = 2.51;
	float b = 0.03;
	float c = 2.43;
	float d = 0.59;
	float e = 0.14;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PBR_PS_Test(PBR_PS_INPUT input) : SV_TARGET
{
    float3 albedo = float3(AlbedoR, AlbedoG, AlbedoB);
    float metallic = ObjectMetallic;
    float roughness = max(ObjectRoughness, 0.001);

    float3 N = GetNormalFromMap(input);
    float3 V = normalize(-input.ViewPosition);

	const float DefaultSpecular = 0.04;
	float3 SpecularFact = lerp(albedo, float3(DefaultSpecular,DefaultSpecular,DefaultSpecular), metallic);
    float3 F0 = lerp(float3(DefaultSpecular, DefaultSpecular, DefaultSpecular), albedo, metallic);
    
    // 다이렉트 라이팅
    float3 Lo = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < gLightCount; ++i)
    {
        Lo += CalcPBRLight(input.ViewPosition, N, V, albedo, metallic, roughness, SpecularFact, i);
    }
	
    // 🔥 IBL 계산 (통합 버전)
    float3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
    float3 kD = 1.0 - kS;
    
    // Diffuse IBL
    float3 irradiance = FastDiffuseIrradiance(N);
    float3 diffuse = irradiance * albedo;
    
    // Specular IBL  
    float3 R = reflect(-V, N);
    float3 worldR = normalize(mul(R, (float3x3)gViewInv));
	float3 EnvironmentColor = EnvironmentMap.SampleLevel(CubeSampler, worldR, roughness * 15).rgb * albedo;
    float3 prefilteredColor = lerp(albedo, EnvironmentColor ,metallic*1.1);
    
    // 🚀 핵심: Metal일 때는 더 강한 반사!
    float3 specular = prefilteredColor * lerp(kS,F0, metallic * 1.5);
    
    float3 ambient = kD * diffuse + specular;
    float3 color = ambient + Lo;

    // 톤맵핑
	color = ACESFilm(color);
    //color = color / (color + float3(1.0, 1.0, 1.0));
    //color = pow(color, float3(1.0/2.2, 1.0/2.2, 1.0/2.2));

    return float4(color, 1.0);
}


#endif