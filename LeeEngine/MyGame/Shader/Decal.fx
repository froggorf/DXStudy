#ifndef _DECAL
#define _DECAL

#include "Global.fx"


SamplerState samLinear : register( s0 );

Texture2D    POSITION_TARGET : register( t10 );
Texture2D    DecalTexture : register( t0 );

cbuffer cbDecal : register( b7 )
{
	float4x4 Dummy1;

	int gDecalIsLight;
	float3 Pad;
};

struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 Position : SV_Position;
};

VS_OUT VS_Decal(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = mul(mul(mul(float4(_in.vPos, 1.f), World),gView),gProjection);

	return output;
}

struct PS_OUT
{
	float4 Color : SV_Target0;
	float4 Emissive : SV_Target1;
};

PS_OUT PS_Decal(VS_OUT _in)
{
	PS_OUT output = (PS_OUT) 0.f;

	float2 vScreenUV = _in.Position.xy / gResolution;
	float4 vViewPos = POSITION_TARGET.Sample(samLinear, vScreenUV);

	// 해당 영역에 존재하는 물체가 없다
	if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
	{
		discard;
	}

	float3 vLocalPos = mul(mul(float4(vViewPos.xyz, 1.f), gViewInv), WorldInv).xyz;
	if (vLocalPos.x < -0.5f || 0.5f < vLocalPos.x 
	|| vLocalPos.y < -0.5f || 0.5f < vLocalPos.y 
	|| vLocalPos.z < -0.5f || 0.5f < vLocalPos.z)
	{
		discard;
	}

	float2 UV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.z + 0.5f));

	float4 DecalColor = DecalTexture.Sample(samLinear, UV);

	if (gDecalIsLight)
	{
		DecalColor.rgb *= DecalColor.a;
		output.Emissive = DecalColor;
	}
	else
	{
		output.Color = DecalColor;
		output.Emissive = float4(0.0f,0.0f,0.0f,0.0f);
	}

	return output;
}

#endif