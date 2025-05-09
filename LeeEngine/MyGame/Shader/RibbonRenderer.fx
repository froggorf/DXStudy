#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
    float2 Tex : TEXCOORD;
    float4 ParticleColor : POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_RibbonParticle( float4 Pos : POSITION, float2 TexCoord : TEXCOORD, float4 ParticleColor : BONEWEIGHTS)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // PosScreen
    output.PosScreen = mul(mul(Pos, gView),gProjection);

    output.Tex = TexCoord;

    output.ParticleColor = ParticleColor;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_RibbonParticle( VS_OUTPUT input ) : SV_Target
{
    float4 ParticleColor = input.ParticleColor;
    float4 color =  float4(ParticleColor.r,ParticleColor.g,ParticleColor.b, input.Tex.x);
	return color;
}
