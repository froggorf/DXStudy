#ifndef _MONOCHROME_FX_
#define _MONOCHROME_FX_

#include "Global.fx"
Texture2D    SceneTexture : register( t0 );
Texture2D	BeforeMonochromeSceneTexture : register(t10);
Texture2D   ViewPosTarget : register(t11);
SamplerState DefaultSampler : register( s0 );

// 데이터를 아끼기 위해
cbuffer cbMonochromeData : register(b8)
{
	float3 CenterViewPos;
	float MonochromeDistance;
}

struct VS_OUT
{
	float4 Position : SV_Position;
	float2 UV : TEXCOORD;
};

float4 PS_Monochrome(VS_OUT Input) : SV_TARGET
{
	float2 ScreenUV = Input.Position / gResolution;

	float3 ObjectViewPos = ViewPosTarget.Sample(DefaultSampler, ScreenUV).rgb;

	float Distance = distance(ObjectViewPos, CenterViewPos);
	// 0 / 1 / 2 / ... / @
	float A = floor(Distance / MonochromeDistance);
	// 1 이상이면 1로 만들고, 1 미만이면 0으로 만들기
	A = saturate(A);
	float4 Color = float4(SceneTexture.Sample(DefaultSampler, ScreenUV).rgb, 1.0f);
	float Gray = dot(Color.rgb, float3(0.299f, 0.587f, 0.114f));
	// 좀 더 어둡게 보이게 하기 위해서
	Gray*= 0.5f;

	Color.rgb = lerp(Color.rgb, float3(Gray,Gray,Gray), A);
	Color.a = 1.0f;
	return Color;
}

float4 PS_NotMonochrome(VS_OUT Input) : SV_TARGET
{
	float2 ScreenUV = Input.Position / gResolution;
	float4 Color = float4(SceneTexture.Sample(DefaultSampler, ScreenUV).rgb, 1.0f);
	Color.a = 1.0f;
	return Color;
}


#endif
