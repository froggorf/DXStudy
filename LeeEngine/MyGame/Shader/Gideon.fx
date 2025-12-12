#include "PS_PBR.fx"

// 0 -> BaseColor
// 1 -> Normal
// 2 -> Metallic
// 3 -> Specular
// 4 -> Roughness
// 5 -> AO
// 6 -> Alpha
// 7 ->

Deferred_PS_OUT PS_Gideon_Default(PBR_PS_INPUT input) : SV_TARGET
{
	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.UV).rgba : DefaultAlbedo;

	float DefaultMetallic = 0.0f;
	int MetallicTexBind = bTexBind_0_3.z;
	float metallic = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.UV).r : DefaultMetallic;

	float DefaultSpecular = 1.0f;
	int SpecularTexBind = bTexBind_0_3.w;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.UV).r : DefaultSpecular;

	float DefaultRoughness = 0.0f;
	int RoughnessTexBind = bTexBind_4_7.x;
	float roughness = RoughnessTexBind ? RoughnessTexture.Sample(DefaultSampler, input.UV).r : DefaultRoughness;

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.UV).r : DefaultAO;

	float DefaultAlpha = 1.0f;
	int AlphaTexBind = bTexBind_4_7.z;
	float Alpha = AlphaTexBind ? Texture6.Sample(DefaultSampler, input.UV).r : DefaultAlpha;

	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = float4(albedo.rgb, Alpha);
	output.PBRData = float4(metallic, specular, roughness, ao);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}

Deferred_PS_OUT PS_Sanhwa_Default(PBR_PS_INPUT input) : SV_TARGET
{
	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.UV).rgba : DefaultAlbedo;

	int MetallicTexBind = bTexBind_0_3.z;
	float3 MetallicRoughness = MetallicTexBind ? MetallicTexture.Sample(DefaultSampler, input.UV).rgb : float3(0.0f, 0.5f, 0.5f);
	float metallic = MetallicRoughness.g;
	float roughness = MetallicRoughness.b;

	float DefaultSpecular = 1.0f;
	int SpecularTexBind = bTexBind_0_3.w;
	float specular = SpecularTexBind ? SpecularTexture.Sample(DefaultSampler, input.UV).r : DefaultSpecular;

	int EmissiveTexBind = bTexBind_4_7.x;
	float3 Emissive = EmissiveTexBind ? RoughnessTexture.Sample(DefaultSampler, input.UV).rgb : float3(0.0f, 0.0f, 0.0f);

	float DefaultAO = 1.0f;
	int AOTexBind = bTexBind_4_7.y;
	float ao = AOTexBind ? AOTexture.Sample(DefaultSampler, input.UV).r : DefaultAO;

	

	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = float4(albedo.rgb, 1.0f);
	output.PBRData = float4(metallic, specular, roughness, ao);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(Emissive, 1.f);

	return output;
}


