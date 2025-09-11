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

struct Deffered_PS_OUT
{
	float4 Color       : SV_Target;
	float4 Normal      : SV_Target1;
	float4 Position    : SV_Target2;
	float4 Emissive    : SV_Target3;
	// metallic, specular, roughness, AO
	float4 PBRData      : SV_Target4;
};

Deffered_PS_OUT PS_PBR_Deferred_Default(PBR_PS_INPUT Input)
{
	Deffered_PS_OUT output = (Deffered_PS_OUT) 0.f;

	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f,1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, Input.TexCoord).rgba : DefaultAlbedo;
	float DefaultMetallic = 0.2f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, Input.TexCoord).r : DefaultMetallic;

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_0_3.w;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, Input.TexCoord).r : DefaultSpecular;

	float DefaultRoughness = 0.5f;
	int RoughnessTexBind = bTexBind_4_7.x;
	float roughness = RoughnessTexBind ? RoughnessTexture.Sample(DefaultSampler, Input.TexCoord).r : DefaultRoughness;

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, Input.TexCoord).r : DefaultAO;

	output.Color = albedo;
	output.PBRData = float4(metallic, specular, roughness, ao);
	output.Normal = float4(Input.ViewNormal, 1.f);    
	output.Position = float4(Input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}


float4 PBR_PS_Default(PBR_PS_INPUT input) : SV_TARGET
{
	// Sample textures (기존 코드 그대로)
	int AlbedoTexBind = bTexBind_0_3.x;
	float3 DefaultAlbedo = float3(1.0f, 0.0f, 1.0f);
	float3 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.TexCoord).rgb : DefaultAlbedo;
	float DefaultMetallic = 0.2f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultMetallic;

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_0_3.w;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultSpecular;

	float DefaultRoughness = 0.5f;
	int RoughnessTexBind = bTexBind_4_7.x;
	float roughness = RoughnessTexBind ? RoughnessTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultRoughness;

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultAO;

	float3 color = MyPBR(input, albedo, metallic, specular, roughness, ao);
	return float4(color,1.0f);

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

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_4_7.x;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.TexCoord) : DefaultSpecular;

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.TexCoord).r : DefaultAO;

	float3 color = MyPBR(input, albedo, metallic, specular, roughness, ao);

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

float4 PBR_PS_Test(PBR_PS_INPUT input) : SV_TARGET
{
	
	
	float3 color = MyPBR(input, float3(AlbedoR,AlbedoG,AlbedoB), ObjectMetallic, ObjectSpecular, ObjectRoughness, ObjectAO);
	
	return float4(color, 1.0);
}

float4 PBR_PS_Brick(PBR_PS_INPUT input) : SV_TARGET
{
	float3 albedo = float3(AlbedoR, AlbedoG, AlbedoB);
	float metallic = ObjectMetallic;
	float roughness = max(ObjectRoughness, 0.001);
	float specular = ObjectSpecular;
	float ao = ObjectAO;
	float3 color = MyPBR(input, albedo, metallic, specular, roughness, ao);

	return float4(color, 1.0);
}

float4 PBR_PS_UE_Chair(PBR_PS_INPUT input) : SV_TARGET
{
	float3 MetallicTexVal = AlbedoTexture.Sample(DefaultSampler, input.TexCoord);
	float3 NormalTexVal = MetallicTexture.Sample(DefaultSampler, input.TexCoord);

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
	float ao = ObjectAO;

	float3 color = MyPBR(input, albedo, metallic, specular, roughness, ao);
	
	return float4(color, 1.0);
}

#endif