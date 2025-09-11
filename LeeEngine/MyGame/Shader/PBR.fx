#ifndef __PBR_FX__
#define __PBR_FX__

#include "Global.fx"

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;

	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct PBR_PS_INPUT
{
	float4 PosScreen : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 ViewPosition : POSITION;
	float3 ViewTangent : TANGENT;
	float3 ViewNormal : NORMAL;
	float3 ViewBinormal : BINORMAL;
};

PBR_PS_INPUT VS(VS_INPUT Input)
{
	PBR_PS_INPUT output = (PBR_PS_INPUT)0;
	
	output.PosScreen = mul(mul(Input.Pos, gMatWV), gProjection);
	output.TexCoord = Input.TexCoord;

	output.ViewPosition = mul(float4(Input.Pos.xyz, 1.f), gMatWV);
	output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), gMatWV)).xyz;
	output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), gMatWV)).xyz;
	output.ViewBinormal = normalize(mul(float4(Input.Binormal, 0.f), gMatWV)).xyz;

	return output;
}

#endif