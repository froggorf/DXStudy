#ifndef _LIGHT
#define _LIGHT

#include "Global.fx"


SamplerState samLinear : register( s0 );

Texture2D    POSITION_TARGET : register( t0 );
Texture2D    NORMAL_TARGET : register( t1 );

cbuffer cbLightIndex : register( b7 )
{
	int gLightIndex;
	float3 Pad;
};

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

VS_OUT VS_DirLight(VS_IN Input)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = float4(Input.Pos * 2.f, 1.f);
	output.UV = Input.UV;

	return output;
}


struct PS_OUT
{
	float4 Diffuse : SV_Target;
	float4 Specular : SV_Target1;
};

PS_OUT PS_DirLight(VS_OUT Input)
{
	PS_OUT output = (PS_OUT) 0.f;

	// Position Target 에서 호출된 픽셀 자리에 해당하는 곳에 기록된 물체의 좌표를 확인한다.
	float4 ViewPos = POSITION_TARGET.Sample(samLinear, Input.UV);

	// 빛을 받을 물체가 존재하지 않는다.
	if (ViewPos.x == 0.f && ViewPos.y == 0.f && ViewPos.z == 0.f)
	{
		discard;
	}   


	// 호출된 픽셀 자리에 해당하는 곳에 기록된 물체의 Normal 벡터를 가져온다.
	float3 ViewNormal = NORMAL_TARGET.Sample(samLinear, Input.UV).xyz;

	CalcLight(gView, ViewPos.xyz, ViewNormal, gLightIndex, output.Diffuse.xyz, output.Specular.xyz);

	output.Diffuse.a = 1.0f;
	output.Specular.a = 1.0f;
	//output.Specular.xyz *= ViewPos.w;

	return output;
}


#endif