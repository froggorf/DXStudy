#ifndef _SHADOWMAP
#define _SHADOWMAP

#include "Global.fx"
#include "AnimationHelpers.hlsl"

cbuffer cbLightInfo : register( b7 )
{
	row_major matrix LightVP;

	int gLightIndex;
	float3 Pad;
};

struct VS_IN
{
	float3 Pos : POSITION;

	int4   boneIDs : BONEIDS;
	float4 boneWeights : BONEWEIGHTS;
};

struct VS_OUT
{
	float4 Position : SV_Position;
	float4 ProjPos : POSITION;
};



VS_OUT VS_ShadowMap_Static(VS_IN Input)
{
	VS_OUT output = (VS_OUT) 0.f;

	output.Position = mul(mul(float4(Input.Pos, 1.f), World),LightVP);    
	output.ProjPos = output.Position;

	return output;
}

VS_OUT VS_ShadowMap_Skeletal(VS_IN Input)
{
	VS_OUT output = (VS_OUT)0;

	// Pos, Normal 본의 가중치에 맞게 위치 조정
	{
		float4 skinnedPosition;
		float3 skinnedNormal;

		CalculateSkinnedPosition(float4(Input.Pos,1.0f), float4(1.0f,1.0f,1.0f,1.0f), Input.boneIDs, Input.boneWeights, gBoneFinalTransforms, skinnedPosition, skinnedNormal);

		output.Position = mul(mul(skinnedPosition, World),LightVP);
		output.ProjPos = output.Position;
	}

	return output;
}

float PS_ShadowMap(VS_OUT Input) : SV_Target
{
	return Input.ProjPos.z / Input.ProjPos.w;
}


// ==================
// Point Light
// ==================
struct GS_INPUT
{
	float4 Pos : POSITION;
};

struct GS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 WorldPos : POSITION;
	uint   RTIndex : SV_RenderTargetArrayIndex;
};

GS_INPUT VS_PointShadowMap_Static(VS_IN Input)
{
	GS_INPUT Output = (GS_INPUT) 0.0f;
	Output.Pos = mul(float4(Input.Pos,1.0f), World);
	return Output;
}

GS_INPUT VS_PointShadowMap_Skeletal(VS_IN Input)
{
	GS_INPUT output = (GS_INPUT)0;

	// Pos, Normal 본의 가중치에 맞게 위치 조정
	{
		float4 skinnedPosition;
		float3 skinnedNormal;

		CalculateSkinnedPosition(float4(Input.Pos,1.0f), float4(1.0f,1.0f,1.0f,1.0f), Input.boneIDs, Input.boneWeights, gBoneFinalTransforms, skinnedPosition, skinnedNormal);

		output.Pos = mul(skinnedPosition, World);
	}

	return output;
}



// 지오메트리 셰이더
[maxvertexcount(18)] // 삼각형 1개 x 6면 = 최대 18개
void GS_PointShadowMap(triangle GS_INPUT Input[3], inout TriangleStream<GS_OUTPUT> TriStream)
{
	FLightInfo LightInfo = g_LightBuffer[gLightIndex];

	for (uint Face = 0; Face < 6; ++Face)
	{

		float4x4 ViewProj = CalcCubeViewProjMatrix(Face, LightInfo.WorldPos, 0.001, LightInfo.Radius, radians(90.0f), 1.0f); // 각 면의 뷰-프로젝션 행렬

		GS_OUTPUT output[3];
		[unroll]
		for(int i = 0; i < 3; ++i)
		{
			output[i].Pos = mul(Input[i].Pos, ViewProj);
			output[i].WorldPos = Input[i].Pos;
			output[i].RTIndex = Face;
		}
		TriStream.Append(output[0]);
		TriStream.Append(output[1]);
		TriStream.Append(output[2]);
		TriStream.RestartStrip();
	}
}

float PS_PointShadowMap(GS_OUTPUT Input) : SV_Target
{
	float3 FragWorldPos = Input.WorldPos;
	float3 LightPos = g_LightBuffer[gLightIndex].WorldPos;
	float Distance = length (FragWorldPos - LightPos);
	return Distance;
}

#endif