#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"
#include "TransformHelpers.hlsl"

TextureCube  CubeTexture : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD;

};

struct VS_OUTPUT
{
	float4 PosScreen : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 LocalPos : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0.f;

	Input.Pos *= 2.f;

	float4 ProjPos = mul(float4(Input.Pos.xyz, 0.f), gView);

	ProjPos = mul(ProjPos, gProjection);
	ProjPos.z = ProjPos.w - 0.00005f;

	output.PosScreen = ProjPos;
	output.Tex = Input.TexCoord;    
	output.LocalPos = Input.Pos;    

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float3 UV = normalize(input.LocalPos);
	float4 ObjectColor = CubeTexture.Sample(samLinear, UV);
	return ObjectColor;
}
