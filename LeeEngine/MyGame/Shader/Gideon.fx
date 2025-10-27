#include "PS_PBR.fx"


Deferred_PS_OUT PS_Gideon_Head(PBR_PS_INPUT input) : SV_TARGET
{
	int AlbedoTexBind = bTexBind_0_3.x;
	float4 DefaultAlbedo = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float4 albedo = AlbedoTexBind ? AlbedoTexture.Sample(DefaultSampler, input.UV).rgba : DefaultAlbedo;
	albedo.rgb *= 1.2f;
	float Roughness = albedo.a;

	float4 Tex2Data = MetallicTexture.Sample(DefaultSampler, input.UV);
	float Specular = (Tex2Data.r * Tex2Data.g) * 0.4f;

	
	Deferred_PS_OUT output = (Deferred_PS_OUT) 0.f;

	output.Color = albedo;
	output.PBRData = float4(0.0f, Specular, Roughness, 1.0f);
	float3 N = GetNormalFromMap(input);
	output.Normal = float4(N, 1.f);
	output.Position = float4(input.ViewPosition, 1.f);
	output.Emissive = float4(0.f, 0.f, 0.f, 1.f);

	return output;
}
