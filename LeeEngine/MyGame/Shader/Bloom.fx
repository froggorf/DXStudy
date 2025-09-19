#ifndef __BLOOM_FX_
#define __BLOOM_FX_
#include "PostProcess.fx"
// -> In PostProcess.fx
//Texture2D    SceneTexture : register( t0 );
//SamplerState DefaultSampler : register( s0 );
SamplerState ClampSampler : register(s2);
Texture2D	   EmissiveTexture : register(t1);
Texture2D	   LastDownSamplingTexture : register(t2);

cbuffer cbTextureResolution : register(b8)
{
	float2 TexSize;
	float BloomIntensity;
	float Pad;
}

float4 PS_Blur_DownSampling(VS_OUT Input) : SV_TARGET
{
	float2 TexelSize = 1.0f / TexSize;
	
	// { -TexelSize.x, -TexelSize.y, TexelSize.x, TexelSize.y }
	float4 o = TexelSize.xyxy * float2(-1,1).xxyy;
	float3 s =
		EmissiveTexture.Sample(ClampSampler, Input.UV + o.xy).rgb +
		EmissiveTexture.Sample(ClampSampler, Input.UV + o.zy).rgb +
		EmissiveTexture.Sample(ClampSampler, Input.UV + o.xw).rgb +
		EmissiveTexture.Sample(ClampSampler, Input.UV + o.zw).rgb;
	return float4(s * 0.25f, 1.0f);
}

float4 PS_Blur_UpSampling(VS_OUT Input) : SV_TARGET
{
	float2 TexelSize = 1.0f / TexSize;

	// { -TexelSize.x/2, -TexelSize.y/2, TexelSize.x/2, TexelSize.y/2 }
	float4 o = TexelSize.xyxy * float2(-0.5f, 0.5f).xxyy;
	float3 s =
	EmissiveTexture.Sample(ClampSampler, Input.UV + o.xy).rgb +
	EmissiveTexture.Sample(ClampSampler, Input.UV + o.zy).rgb +
	EmissiveTexture.Sample(ClampSampler, Input.UV + o.xw).rgb +
	EmissiveTexture.Sample(ClampSampler, Input.UV + o.zw).rgb;

	float3 LastDownSampleColor = LastDownSamplingTexture.Sample(ClampSampler, Input.UV).rgb;
	
	return float4(s * 0.25f + LastDownSampleColor, 1.0f);
}

float4 PS_Bloom(VS_OUT Input) : SV_TARGET
{
	float3 SceneColor = SceneTexture.Sample(DefaultSampler, Input.UV).rgb;
	float3 BloomColor = EmissiveTexture.Sample(ClampSampler,Input.UV).rgb;
	
	BloomColor *= BloomIntensity;
	return float4(SceneColor + BloomColor, 1.0f);
}

#endif