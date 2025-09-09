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

float4 PBR_PS_Default(PBR_PS_INPUT input) : SV_TARGET
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

	float DefaultSpecular = 0.5f;
	int SpecularTexBind = bTexBind_4_7.x;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.TexCoord) : DefaultSpecular;

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
    float3 albedo = float3(AlbedoR, AlbedoG, AlbedoB);
    float metallic = ObjectMetallic;
    float roughness = max(ObjectRoughness, 0.001);
	float specular = ObjectSpecular;
	float ao = ObjectAO;
    float3 color = MyPBR(input, albedo, metallic, specular, roughness, ao);

    return float4(color, 1.0);
}


#endif