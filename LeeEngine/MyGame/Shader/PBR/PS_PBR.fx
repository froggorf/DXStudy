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
	float ObjectMetallic;
	float ObjectRoughness;
	float ObjectAO;
	float2 Pad;
	float4 pad2;
}

float4 PBR_PS_Test(PBR_PS_INPUT input) : SV_TARGET
{
	float3 albedo = float3(AlbedoR, AlbedoG, AlbedoB);
    float metallic = ObjectMetallic;
    float roughness = max(ObjectRoughness, 0.01);
    float ao = 1.0;

    // Get normal from normal map
    float3 N = GetNormalFromMap(input);
    
    // Camera position and view direction
    float3 V = normalize(-input.ViewPosition);
    
    // Calculate reflectance at normal incidence
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);
    
    // Direct lighting calculation
    float3 Lo = float3(0.0, 0.0, 0.0);
    
    for (int i = 0; i < gLightCount; ++i)
    {
        Lo += CalcPBRLight(input.ViewPosition, N, V, albedo, metallic, roughness, F0, i);
    }
    
	// Ambient lighting (IBL)
	float3 ambient = CalcAmbientPBR(N, V, albedo, metallic, roughness, F0, ao);

	// 🔥 조명 합성 - metallic에 따라 IBL 강도 조절
	float iblStrength = lerp(0.2, 0.8, metallic); // metallic=0일 때 20%만
	float3 color = ambient * iblStrength + Lo * 1.0;

	// 톤맵핑 및 감마 보정
	color = color / (color + float3(1.0, 1.0, 1.0));
	color = pow(color, 1.0 / 2.2);

	return float4(color, 1.0);
}


#endif