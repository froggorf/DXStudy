#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"

Texture2D    txDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	int4 boneIDs : BONEIDS;
	float4 boneWeights : BONEWEIGHTS;
};

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.PosScreen = CalculateScreenPosition(Input.Pos, World, gView, gProjection);
	output.Tex = Input.TexCoord;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float2 centered = input.Tex * 2.0f - 1.0f;
	centered.y *= 1.35f;

	float radius = length(centered);
	float angle = atan2(centered.y, centered.x);

	float innerRadius = 0.73f;
	float outerRadius = 0.80f;
	float softEdge = 0.02f;

	float waveA = sin(angle * 6.0f + gTime * 3.2f) * 0.03f;
	float waveB = sin(angle * 12.0f - gTime * 1.7f) * 0.015f;
	float wave = waveA + waveB;

	float outer = outerRadius + wave;
	float inner = innerRadius + wave * 0.6f;

	float outerMask = 1.0f - smoothstep(outer - softEdge, outer, radius);
	float innerMask = smoothstep(inner - softEdge, inner, radius);
	float ringMask = saturate(outerMask * innerMask);

	float rimGlow = smoothstep(outer - 0.05f, outer - 0.01f, radius);
	float innerGlow = 1.0f - smoothstep(inner + 0.01f, inner + 0.05f, radius);

	float3 base = float3(0.12f, 0.6f, 1.2f);
	float3 highlight = float3(1.0f, 0.8f, 0.4f);
	float3 ringColor = base + highlight * (rimGlow + innerGlow);

	float alpha = ringMask;
	clip(alpha - 0.005f);

	float4 tex = txDiffuse.Sample(samLinear, input.Tex);
	return float4(ringColor * tex.rgb, alpha);
}
