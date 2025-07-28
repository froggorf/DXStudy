#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"
#include "AnimationHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

Texture2D    gShadowMap : register(t1);
SamplerState gShadowSampler : register(s1);

cbuffer cbSkeletalMeshBoneFinalTransforms : register(b3)
{
	matrix gBoneFinalTransforms[MAX_BONES];
}

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
	float3 PosWorld : POSITION;
	float4 PosLightSpace : POSITION1;
	float3 NormalW : TEXCOORD1;
	float2 Tex : TEXCOORD;
	float  Depth : TEXCOORD2;
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

		CalculateSkinnedPosition(input.Pos, input.Normal, input.boneIDs, input.boneWeights, gBoneFinalTransforms, skinnedPosition, skinnedNormal);

		input.Pos    = skinnedPosition;
		input.Normal = skinnedNormal;
	}

	// output.PosWorld ( // 픽셀 셰이더 내에서 라이팅을 위해 )
	{
		output.PosWorld = mul(input.Pos, World).xyz;
	}

	// PosScreen
	{
		output.PosScreen = CalculateScreenPosition(input.Pos, World, gView, gProjection);
		output.Depth     = output.PosScreen.z;
	}

	// 노말벡터를 월드좌표계로
	{
		output.NormalW = mul(input.Normal, (float3x3)WorldInvTranspose);
		output.NormalW = normalize(output.NormalW);
	}

	// light source에서 버텍스로의 position
	output.PosLightSpace = CalculateScreenPosition(input.Pos, World, gLightView, gLightProj);

	output.Tex = input.TexCoord;

	return output;
}
