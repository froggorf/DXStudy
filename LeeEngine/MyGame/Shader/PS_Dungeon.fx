#ifndef __PS_DUNGEON_FX__
#define __PS_DUNGEON_FX__

#include "PBRLightHelper.fx"
#include "MaterialNode.fx"

#define Texture0 AlbedoTexture
#define Texture1 NormalTexture
#define Texture2 MetallicTexture
#define Texture3 RoughnessTexture
#define Texture4 AOTexture

struct Deferred_PS_OUT
{
	float4 Color : SV_Target;
	float4 Normal : SV_Target1;
	float4 Position : SV_Target2;
	float4 Emissive : SV_Target3;
	// metallic, specular, roughness, AO
	float4 PBRData : SV_Target4;
};


Deferred_PS_OUT PBR_Dungeon_00(PBR_PS_INPUT input) : SV_TARGET
{
	float3 Color1 = Texture0.Sample(DefaultSampler, input.UV).rgb;
	float3 Color2 = Texture1.Sample(DefaultSampler, input.UV).rgb;
	float3 Color3 = Texture2.Sample(DefaultSampler, input.UV).rgb;
	float3 Normal1 = Texture3.Sample(DefaultSampler, input.UV).rgb;
	float3 Normal2 = Texture4.Sample(DefaultSampler, input.UV).rgb;
	
	float roughness = lerp(lerp(0.85f, 0.88f, Color1.r), lerp(0.98f, 0.99f, Color2.r), 0.5f);
	
	
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;
	output.Color = float4(lerp(Color3, lerp(Color1, Color2, 0.5f), 0.5f), 1.0f);
	output.PBRData = float4(0.0f, 0.5f, roughness, 1.0f);
	float3 N = CalculateNormal(input, lerp(lerp(Normal1, Normal2, 0.5f), Normal1, 0.5f));
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

cbuffer cbDungeonUserParam : register(b4)
{
	float AlbedoR;
	float AlbedoG;
	float2 pad;
}


Deferred_PS_OUT PBR_Dungeon_Base(PBR_PS_INPUT input) : SV_TARGET
{
	float3 Color1 = Texture0.Sample(DefaultSampler, input.UV).rgb;
	float3 Normal1 = Texture1.Sample(DefaultSampler, input.UV).rgb;
	
	float roughness = lerp(0.8f, 0.98f, Color1.r);
	
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;
	output.Color = float4(Color1, 1.0f);
	output.PBRData = float4(0.0f, 0.5f, roughness, 1.0f);
	float3 N = CalculateNormal(input, Normal1);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PBR_Dungeon_Grass_Base(PBR_PS_INPUT input) : SV_TARGET
{
	float4 Color1 = Texture0.Sample(DefaultSampler, input.UV).rgba;
	float3 Normal1 = Texture1.Sample(DefaultSampler, input.UV).rgb;
	
	float roughness = lerp(0.8f, 0.98f, Color1.r);
	
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;
	output.Color = Color1;
	output.PBRData = float4(0.0f, 0.5f, roughness, 1.0f);
	float3 N = CalculateNormal(input, Normal1);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PBR_Dungeon_12(PBR_PS_INPUT input) : SV_TARGET
{
	float3 Color1 = Texture0.Sample(DefaultSampler, input.UV).rgb;
	float3 Normal1 = Texture1.Sample(DefaultSampler, input.UV).rgb;
	
	float roughness = lerp(0.75f, 0.98f, Color1.r);
	
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;
	output.Color = float4(Color1 * 3, 1.0f);
	output.PBRData = float4(0.7f, 0.5f, roughness, 1.0f);
	float3 N = CalculateNormal(input, Normal1);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PBR_Dungeon_14(PBR_PS_INPUT input) : SV_TARGET
{
	float4 Color1 = Texture0.Sample(DefaultSampler, input.UV).rgba;
	if (Color1.a < 0.5f)
	{
		discard;
	}
	float3 Normal1 = Texture1.Sample(DefaultSampler, input.UV).rgb;
	
	float roughness = lerp(0.4f, 0.98f, Color1.r);
	
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;
	output.Color = Color1;
	output.PBRData = float4(0.0f, 0.5f, roughness, 1.0f);
	float3 N = CalculateNormal(input, Normal1);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

#endif