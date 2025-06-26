#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
};

VS_OUTPUT VS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// PosScreen
	output.PosScreen = CalculateScreenPosition(Pos, World, gView, gProjection);
	return output;
}

cbuffer DebugColor : register(b6)
{
	float4 DebugColor;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return float4(DebugColor.r,DebugColor.g,DebugColor.b,1);
}
