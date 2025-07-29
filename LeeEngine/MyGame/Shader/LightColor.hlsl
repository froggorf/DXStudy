#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

Texture2D    gShadowMap : register(t1);
SamplerState gShadowSampler : register(s1);

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;

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
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.PosScreen = CalculateScreenPosition(Input.Pos, World, gView, gProjection);
	output.Tex = Input.TexCoord;

	output.ViewPos = mul(float4(Input.Pos.xyz, 1.f), gMatWV);

	output.ViewTangent = normalize(mul(float4(Input.Tangent, 0.f), gMatWV)).xyz;
	output.ViewNormal = normalize(mul(float4(Input.Normal, 0.f), gMatWV)).xyz;
	output.ViewBinormal = normalize(mul(float4(Input.Binormal, 0.f), gMatWV)).xyz;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 OutColor = (float4) 0.f;
	float4 ObjectColor = txDiffuse.Sample(samLinear, input.Tex);

	float3 LightColor = (float3)0.f;
	float3 Specular = (float3) 0.f;

	for (int i = 0; i < gLightCount; ++i)
	{
		CalcLight(gView, input.ViewPos, input.ViewNormal, i, LightColor, Specular);
	}

	OutColor.rgb = (ObjectColor.rgb) * LightColor.rgb + Specular;
	OutColor.a = ObjectColor.a;
	return OutColor;
}

cbuffer cbTest : register(b4)
{
	float  TestSpeedX;
	float  TestSpeedY;
	float2 Pad;
}

float4 TestWater(VS_OUTPUT input) : SV_Target
{
	float2 NewWaterUV = input.Tex;
	NewWaterUV.x      = NewWaterUV.x + gTime * TestSpeedX;
	NewWaterUV.y      = NewWaterUV.y + gTime * TestSpeedY;
	float4 color      = txDiffuse.Sample(samLinear, NewWaterUV);

	//float4 color = txDiffuse.Sample( samLinear, input.Tex );

	

	return color;
}
