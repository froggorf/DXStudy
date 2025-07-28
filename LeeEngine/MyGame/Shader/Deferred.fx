#include "Global.fx"
#include "TransformHelpers.hlsl"

Texture2D    txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

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

	float3 ViewPosition : POSITION;

	float3 ViewTangent : TANGENT;
	float3 ViewNormal : NORMAL;
	float3 ViewBinormal : BINORMAL;
};


struct PS_OUT
{
    float4 Color       : SV_Target;
    float4 Normal      : SV_Target1;
    float4 Position    : SV_Target2;
    float4 Emissive    : SV_Target3;
    float4 Custom      : SV_Target4;
};

VS_OUTPUT VS_Std3D_Deferred(VS_INPUT Input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0.f;

	// PosScreen
	output.PosScreen = CalculateScreenPosition(Input.Pos, World, gView, gProjection);
	output.Tex = Input.TexCoord;

	float4x4 MatWV = mul(World, gView);

	// 노멀 변환 행렬 (WorldView의 3x3만 추출)
	float3x3 MatWV3x3 = (float3x3)MatWV;
	float3x3 NormalMatrix = transpose((MatWV3x3));

	output.ViewNormal   = normalize(mul(Input.Normal,   NormalMatrix));
	output.ViewTangent  = normalize(mul(Input.Tangent,  NormalMatrix));
	output.ViewBinormal = normalize(mul(Input.Binormal, NormalMatrix));
}

PS_OUT PS_Std3D_Deferred(VS_OUTPUT Input)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    output.Color = txDiffuse.Sample(samLinear, Input.Tex);
        
    output.Normal = float4(Input.ViewNormal, 1.f);    
    output.Position = float4(Input.ViewPosition, 1.f);
    output.Emissive = float4(0.f, 0.f, 0.f, 1.f);
    output.Custom = float4(0.f, 0.f, 0.f, 1.f);
           
    return output;
}
