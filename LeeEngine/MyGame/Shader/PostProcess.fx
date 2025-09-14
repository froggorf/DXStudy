#ifndef __POSTPROCESS_FX_
#define __POSTPROCESS_FX_
#include "Global.fx"
Texture2D    SceneTexture : register( t0 );
SamplerState DefaultSampler : register( s0 );

struct VS_IN
{
	float3 Pos : POSITION;
	float2 UV : TEXCOORD;
};

struct VS_OUT
{
	float4 Position : SV_Position;
	float2 UV : TEXCOORD;
};

VS_OUT VS_PostProcess(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = float4(_in.Pos * 2.f, 1.f);
	output.UV = _in.UV;

	return output;
}

float4 PS_PostProcess(VS_OUT Input) : SV_TARGET
{
	float2 ScreenUV = Input.Position / gResolution;

	float4 Color = float4(SceneTexture.Sample(DefaultSampler, ScreenUV).rgb, 1.0f);
	Color.rgb = ACESFilm(Color.rgb);
	Color.a = 1.0f;
	return Color;
}

#endif
