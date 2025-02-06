#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "TransformHelpers.hlsl"

cbuffer cbLightMatrix : register(b0)
{
    matrix LightView;
    matrix LightProj;
}

cbuffer cbPerObject : register(b1)
{
    matrix World;
}

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos: POSITION, float3 Normal:NORMAL, float2 Tex : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    //Pos = mul(Pos, World);
    //output.PosScreen = mul(Pos, LightViewProj);
    output.PosScreen = CalculateScreenPosition(Pos, World, LightView, LightProj);

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return float4(0.0f,0.0f,0.0f,0.0f);
}