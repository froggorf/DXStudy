#pragma target 4.0
#pragma enable_d3d11_debug_symbols

#include "Global.fx"

Texture2D gTex0 : register( t0 );
Texture2D gTex1 : register( t1 );
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

float4 PS_RibbonMaskTrail(VS_OUTPUT input) : SV_Target
{
    float4 ParticleColor = input.ParticleColor;
    
    // Trail Texture로 궤적 모양 바꾸기
    float2 TrailUV = float2(gTime*3 + input.Tex.x, input.Tex.y);
    float4 TrailSample = gTex0.Sample(samLinear, TrailUV);
    float2 TrailUV2 = float2(gTime*3 + 0.7 * input.Tex.x, input.Tex.y);
    float4 TrailSample2 = gTex0.Sample(samLinear, TrailUV2);
    float Alpha = TrailSample.r + TrailSample2.r;

    float2 MaskUV = input.Tex;
    MaskUV.r *= -1;
    float Mask = gTex1.Sample(samLinear, MaskUV).r;
    Alpha *= Mask;

    float4 color =  float4(ParticleColor.r,ParticleColor.g,ParticleColor.b, ParticleColor.a * Alpha);
    return color;
}