#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"
#include "AnimationHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

Texture2D    gShadowMap : register(t1);
SamplerState gShadowSampler : register(s1);



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

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
	float2 Tex : TEXCOORD;

	float3 ViewPos : POSITION;

	float3 ViewTangent : TANGENT;
	float3 ViewNormal : NORMAL;  
	float3 ViewBinormal : BINORMAL;

};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// Pos, Normal 본의 가중치에 맞게 위치 조정
	{
		float4 skinnedPosition;
		float3 skinnedNormal;
		float3 skinnedTangent;
		float3 skinnedBinormal;
		CalculateSkinnedPosition(input.Pos, input.Normal, input.Tangent, input.Binormal, input.boneIDs, input.boneWeights, gBoneFinalTransforms, skinnedPosition, skinnedNormal, skinnedTangent, skinnedBinormal);

		input.Pos    = skinnedPosition;
		input.Normal = skinnedNormal;
		input.Tangent = skinnedTangent;
		input.Binormal = skinnedBinormal;
	}

	output.PosScreen = CalculateScreenPosition(input.Pos, World, gView, gProjection);
	

	output.ViewPos = mul(float4(input.Pos.xyz, 1.f), gMatWV);

	output.ViewTangent = normalize(mul(float4(input.Tangent, 0.f), gMatWV)).xyz;
	output.ViewNormal = normalize(mul(float4(input.Normal, 0.f), gMatWV)).xyz;
	output.ViewBinormal = normalize(mul(float4(input.Binormal, 0.f), gMatWV)).xyz;
	

	// light source에서 버텍스로의 position
	//output.PosLightSpace = CalculateScreenPosition(input.Pos, World, gLightView, gLightProj);

	output.Tex = input.TexCoord;

	return output;
}
