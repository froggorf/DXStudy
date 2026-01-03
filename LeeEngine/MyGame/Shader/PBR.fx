#ifndef __PBR_FX__
#define __PBR_FX__

#include "Global.fx"
#include "AnimationHelpers.hlsl"
#include "MaterialNode.fx"

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	int4   boneIDs : BONEIDS;
	float4 boneWeights : BONEWEIGHTS;
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
	float3 PosWorld : POSITION1;
};

PBR_PS_INPUT VS(VS_INPUT Input)
{
	PBR_PS_INPUT output = (PBR_PS_INPUT)0;

	output.PosWorld = mul(Input.Pos, World);
	output.PosScreen = mul(mul(Input.Pos, gMatWV), gProjection);
	output.TexCoord = Input.TexCoord;

	output.ViewPosition = mul(float4(Input.Pos.xyz, 1.f), gMatWV);
	output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), gMatWV)).xyz;
	output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), gMatWV)).xyz;
	output.ViewBinormal = normalize(cross(output.ViewNormal, output.ViewTangent));
	return output;
}

PBR_PS_INPUT VS_Grass(VS_INPUT Input)
{
	PBR_PS_INPUT output = (PBR_PS_INPUT) 0;
	
	float4 worldPos = mul(Input.Pos, World);
    
	float3 windOffset = SimpleGrassWind(worldPos.xyz,Input.Normal,Input.TexCoord,gTime);
	windOffset *= 2.0f;
	worldPos.xyz += windOffset;
    
    output.PosWorld = worldPos;
	output.PosScreen = mul(mul(worldPos, gView), gProjection);
	output.TexCoord = Input.TexCoord;
	output.ViewPosition = mul(worldPos, gView);
	output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), gMatWV)).xyz;
	output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), gMatWV)).xyz;
	output.ViewBinormal = normalize(cross(output.ViewNormal, output.ViewTangent));
    
	return output;
}


PBR_PS_INPUT VS_Skeletal(VS_INPUT Input)
{
	PBR_PS_INPUT output = (PBR_PS_INPUT)0;

	{
		float4 skinnedPosition;
		float3 skinnedNormal;
		float3 skinnedTangent;
		float3 skinnedBinormal;
		CalculateSkinnedPosition(Input.Pos, Input.Normal,Input.Tangent, Input.Binormal, Input.boneIDs, Input.boneWeights, gBoneFinalTransforms, skinnedPosition, skinnedNormal,skinnedTangent,skinnedBinormal);

		Input.Pos = skinnedPosition;
		Input.Normal = skinnedNormal;
		Input.Tangent = skinnedTangent;
	}

	output.PosScreen = mul(mul(Input.Pos, gMatWV), gProjection);
	output.TexCoord = Input.TexCoord;

	output.ViewPosition = mul(float4(Input.Pos.xyz, 1.f), gMatWV);
	output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), gMatWV)).xyz;
	output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), gMatWV)).xyz;
	output.ViewBinormal = normalize(cross(output.ViewNormal, output.ViewTangent));

	return output;
}


#endif