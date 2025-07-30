#ifndef _VOLUME
#define _VOLUME

#include "Global.fx"

struct VS_IN
{
	float4 Pos : POSITION;
};

struct VS_OUT
{
	float4 Position : SV_Position;
};

VS_OUT VS_Volume(VS_IN _in)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = mul(mul(mul(float4(_in.Pos.xyz, 1.f), World),gView),gProjection);

	return output;
}



#endif
