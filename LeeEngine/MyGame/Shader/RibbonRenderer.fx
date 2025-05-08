#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct VS_OUTPUT
{
    float4 PosScreen : SV_POSITION;
    float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS_RibbonParticle( float4 Pos : POSITION, float2 TexCoord : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    // PosScreen
    output.PosScreen = mul(mul(Pos, gView),gProjection);

    output.Tex = TexCoord;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_RibbonParticle( VS_OUTPUT input ) : SV_Target
{
    //float4 color = txDiffuse.Sample( samLinear, input.Tex );
    //color.rgb *= color.a;

    float4 color =  float4(1.0f,0.0f,1.0f,1.0f);
	return color;
}
