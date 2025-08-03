#ifndef _DECAL
#define _DECAL

#include "Global.fx"


SamplerState samLinear : register( s0 );

Texture2D    POSITION_TARGET : register( t0 );
Texture2D    DecalTexture : register( t1 );


struct VS_IN
{
	float3 vPos : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float3 vLocalPos : POSITION;
};

VS_OUT VS_Decal(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = mul(mul(mul(float4(_in.vPos, 1.f), World),gView),gProjection);
	output.vLocalPos = _in.vPos;


	return output;
}

struct PS_OUT
{
	float4 vColor : SV_Target0;
	float4 vEmissive : SV_Target1;
};

//PS_OUT PS_Decal(VS_OUT _in)
//{
//	PS_OUT output = (PS_OUT) 0.f;
//
//	float2 vScreenUV = _in.vPosition.xy / gResolution;
//	float4 vViewPos = POSITION_TARGET.Sample(samLinear, vScreenUV);
//
//	// 해당 영역에 존재하는 물체가 없다
//	if (vViewPos.x == 0.f && vViewPos.y == 0.f && vViewPos.z == 0.f)
//	{
//		discard;
//	}
//
//	float2 vUV = float2(_in.vLocalPos.x + 0.5f, 1.f - (_in.vLocalPos.z + 0.5f));
//
//	float3 vLocalPos = mul(mul(float4(vViewPos.xyz, 1.f), ), g_matWorldInv).xyz;
//	/*if (vLocalPos.x < -0.5f || 0.5f < vLocalPos.x 
//	|| vLocalPos.y < -0.5f || 0.5f < vLocalPos.y 
//	|| vLocalPos.z < -0.5f || 0.5f < vLocalPos.z)
//	{
//	discard;
//	}*/
//
//	vUV = float2(vLocalPos.x + 0.5f, 1.f - (vLocalPos.z + 0.5f));
//
//	float4 vDecalColor = float4(0.f, 1.f, 0.f, 1.f);
//
//	if(g_btex_1)
//	{
//		vDecalColor = DECAL_TEX.Sample(g_sam_0, vUV);
//	}
//
//	if (AsLight)
//		output.vEmissive = vDecalColor;
//	else
//		output.vColor = vDecalColor;
//
//	return output;
//}

#endif