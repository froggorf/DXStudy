#ifndef __PBRHELPER_FX__
#define __PBRHELPER_FX__

struct PBR_PS_INPUT
{
	float4 PosScreen : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 ViewPosition : POSITION;
	float3 ViewTangent : TANGENT;
	float3 ViewNormal : NORMAL;
	float3 ViewBinormal : BINORMAL;
};
// Textures
Texture2D AlbedoTexture     : register(t0);
Texture2D NormalTexture     : register(t1);
Texture2D MetallicTexture   : register(t2);
Texture2D SpecularTexture	: register(t3);
Texture2D RoughnessTexture  : register(t4);
Texture2D AOTexture         : register(t5);
TextureCube EnvironmentMap  : register(t50);
Texture2D BRDF_LUT			: register(t51);

// Sampler
SamplerState DefaultSampler : register( s0 );
SamplerState CubeSampler : register(s2);


#endif