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

struct Deferred_PS_OUT
{
	float4 Color       : SV_Target;
	float4 Normal      : SV_Target1;
	float4 Position    : SV_Target2;
	float4 Emissive    : SV_Target3;
	// metallic, specular, roughness, AO
	float4 PBRData      : SV_Target4;
};


Deferred_PS_OUT PBR_PS_Default(PBR_PS_INPUT input) : SV_TARGET
{
	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.UV).rgba : DefaultAlbedo;
	float DefaultMetallic = 0.2f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.UV).r : DefaultMetallic;

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_0_3.w;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.UV).r : DefaultSpecular;

	float DefaultRoughness = 0.5f;
	int RoughnessTexBind = bTexBind_4_7.x;
	float roughness = RoughnessTexBind ? RoughnessTexture.Sample(DefaultSampler, input.UV).r : DefaultRoughness;
	
	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.UV).r : DefaultAO;

	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = albedo;
	output.PBRData = float4(metallic, specular, roughness, ao);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);    
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}


Deferred_PS_OUT PBR_PS_Well(PBR_PS_INPUT input) : SV_TARGET
{
	// Sample textures (기존 코드 그대로)
	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.UV).rgba : DefaultAlbedo;
	float DefaultMetallic = 0.5f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.UV).g : DefaultMetallic;

	float DefaultRoughness = 0.5f;
	float roughness = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.UV).b : DefaultRoughness;

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_4_7.x;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.UV) : DefaultSpecular;

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.UV).r : DefaultAO;

	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = albedo;
	output.PBRData = float4(metallic, specular, roughness, ao);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);    
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0,0,0,1);

	return output;
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

Deferred_PS_OUT PBR_PS_Test(PBR_PS_INPUT input) : SV_TARGET
{
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = float4(AlbedoR, AlbedoG, AlbedoB, 1.0f);
	output.PBRData = float4(ObjectMetallic, ObjectSpecular, ObjectRoughness, ObjectAO);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);    
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PBR_PS_Brick(PBR_PS_INPUT input) : SV_TARGET
{
	float4 albedo = float4(AlbedoR, AlbedoG, AlbedoB, 1.0f);
	float metallic = ObjectMetallic;
	float roughness = max(ObjectRoughness, 0.001);
	float specular = ObjectSpecular;
	float ao = ObjectAO;

	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = albedo;
	output.PBRData = float4(metallic,specular,roughness,ao);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);    
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PBR_PS_UE_Chair(PBR_PS_INPUT input) : SV_TARGET
{
	float3 MetallicTexVal = AlbedoTexture.Sample(DefaultSampler, input.UV);
	float3 NormalTexVal = MetallicTexture.Sample(DefaultSampler, input.UV);

	const float4 ColorBase = float4(0.12f, 0.103f, 0.0918f,1);
	const float4 ColorSeats = float4(0.9741f, 0.3378f, 0.0344f,1);
	const float4 ColorMetal = float4(0.914, 0.865, 0.719,1);
	float3 albedo = (lerp(lerp(ColorBase, ColorSeats, MetallicTexVal.b), ColorMetal, MetallicTexVal.g) * MetallicTexVal.r).rgb;

	
	float metallic = lerp(0.0f,1.0f, MetallicTexVal.g);

	const float RoughnessBase = 0.5f;
	const float RoughnessSeats = 0.2f;
	const float RoughnessMetal = 0.4f;
	float roughness = lerp(lerp(RoughnessBase, RoughnessSeats, MetallicTexVal.b),RoughnessMetal,MetallicTexVal.g);
	float specular = 0.5;
	float ao = 1.0f;


	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = float4(albedo,1.0f);
	output.PBRData = float4(metallic, specular, roughness, ao);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);    
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PBR_PS_UE4_Character(PBR_PS_INPUT Input) : SV_TARGET
{
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;
	float3 BodyColor = float3(0.451f, 0.404f, 0.361f);
	
	output.PBRData = float4(0.8f, 0.5f, 0.5f, 1.0f);
	output.Color = float4(BodyColor, 1.0f);
	float3 N = GetNormalFromMap(Input);
	output.Normal = float4(N,1);
	output.Position = float4(Input.ViewPosition, 1.0f);
	output.Emissive = float4(0.0f,0.0f,0.0f,1.0f);

	return output;
}

Deferred_PS_OUT PBR_PS_Brick_Ground(PBR_PS_INPUT input) : SV_TARGET
{
	float2 UV = input.UV * 20.0f;
	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, UV).rgba : DefaultAlbedo;
	float DefaultMetallic = 0.2f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, UV).r : DefaultMetallic;

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_0_3.w;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, UV).r : DefaultSpecular;

	float DefaultRoughness = 0.5f;
	int RoughnessTexBind = bTexBind_4_7.x;
	float roughness = RoughnessTexBind ? RoughnessTexture.Sample(DefaultSampler, UV).r : DefaultRoughness;

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, UV).r : DefaultAO;

	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = albedo;
	output.PBRData = float4(metallic, specular, roughness, ao);
	float3 N = GetNormalFromMap(input, UV);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}


#endif