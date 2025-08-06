#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "Global.fx"

cbuffer cbLightInfo : register( b7 )
{
	row_major matrix LightVP;

	int gLightIndex;
	float3 Pad;
};

struct VS_IN
{
	float3 Pos : POSITION;    
};

struct VS_OUT
{
	float4 Position : SV_Position;
	float4 ProjPos : POSITION;
};



VS_OUT VS_ShadowMap(VS_IN Input)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = mul(mul(float4(Input.Pos, 1.f), World),LightVP);    
	output.ProjPos = output.Position;

	return output;
}

float PS_ShadowMap(VS_OUT Input) : SV_Target
{
	return Input.ProjPos.z / Input.ProjPos.w;
}


#endif