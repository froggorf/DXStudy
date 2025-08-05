#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "Global.fx"

struct VS_IN
{
	float3 vPos : POSITION;    
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float4 vProjPos : POSITION;
};

VS_OUT VS_ShadowMap(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.vPosition = mul(mul(mul(float4(_in.vPos, 1.f), World),gView),gProjection);    
	output.vProjPos = output.vPosition;

	return output;
}

float4 PS_ShadowMap(VS_OUT _in) : SV_Target
{    
	return float4(_in.vProjPos.z /= _in.vProjPos.w, 0.f, 0.f, 0.f);
}


#endif